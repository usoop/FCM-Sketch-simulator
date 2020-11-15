#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <set>
#include <cmath>
#include "./src/UnivMon/UnivMon.h"
using namespace std;

int main()
{
    printf("Start accuracy measurement of UnivMon with level: %d, k: %d, row: %d\n", UNIV_LEVEL, UNIV_K, UNIV_ROW);
    uint32_t totnum_packet = ReadTraces();

    UnivMon<4> *um = NULL;

    for(int iter_data = 0; iter_data < NUM_TRACE; ++iter_data){
        printf("[INFO] %3d-th trace starts to be processed..\n", iter_data+1);
        unordered_map<string, int> true_freq;
        unordered_map<string, vector<uint8_t> > mapping_str_srcip;
        vector<int> true_dist(1000000);
        um = new UnivMon<4>(UNIV_BYTES);
        int num_pkt = (int)traces[iter_data].size();

        for (int i = 0; i < num_pkt; ++i){
            um->insert((uint8_t*)(traces[iter_data][i].key));
            string str((const char*)(traces[iter_data][i].key), 4);
            true_freq[str]++;

            uint8_t key[4];
            memcpy(key, str.c_str(), 4);
            vector<uint8_t> key_vec;
            key_vec.assign(key, key + 4);
            mapping_str_srcip.insert({str, key_vec});
        }
        printf("[INFO] End of insertion process...\n");
        /*-*-*-* End of packet insertion *-*-*-*/

        set<vector<uint8_t> > HH_true;
        for(unordered_map<string, int>::iterator it = true_freq.begin(); it != true_freq.end(); ++it)
        {
            uint8_t key[4]; // 32 bits
            memcpy(key, (it->first).c_str(), 4); // flow ID
            if (it->second >= true_dist.size())
                true_dist.resize(it->second + 1, 0);
            true_dist[it->second] += 1;
            if (it->second > HH_THRESHOLD)
            {
                vector<uint8_t> temp_key;
                temp_key.assign(key, key + 4);
                HH_true.insert(temp_key);
            }
        }
        printf("END\n\n");


        /****************** HEAVY HITTER & CARDINALITY ***************/
        std::vector< std::pair< std::string, int> > heavy_hitters;
        um->get_heavy_hitters(HH_THRESHOLD, heavy_hitters);
        set<vector<uint8_t> > HH_estimate;
        for (int i = 0; i < heavy_hitters.size(); ++i)
        {
            if (heavy_hitters[i].second > HH_THRESHOLD)
                HH_estimate.insert(mapping_str_srcip[heavy_hitters[i].first]);
        }


        double HH_precision = 0;
        int HH_PR = 0;
        int HH_PR_denom = 0;
        int HH_RR = 0;
        int HH_RR_denom = 0;
        set<vector<uint8_t> >::iterator itr;
        for (itr = HH_true.begin(); itr != HH_true.end(); ++itr)
        {
            HH_PR_denom += 1;
            HH_PR += HH_estimate.find(*itr) != HH_estimate.end();
        }
        for (itr = HH_estimate.begin(); itr != HH_estimate.end(); ++itr)
        {
            HH_RR_denom += 1;
            HH_RR += HH_true.find(*itr) != HH_true.end();
        }
        HH_precision = (2 * (double(HH_PR) / double(HH_PR_denom)) * (double(HH_RR) / double(HH_RR_denom))) / ((double(HH_PR) / double(HH_PR_denom)) + (double(HH_RR) / double(HH_RR_denom)));
        printf("HH_precision : %3.5f\n", HH_precision);
        printf("HH_PR : %d, HH_PR_denom : %d, HH_RR : %d, HH_RR_denom : %d\n", HH_PR, HH_PR_denom, HH_RR, HH_RR_denom);

        /***************************************************************/
        double card = um->get_cardinality();
        double card_error = abs(card - int(true_freq.size())) / double(true_freq.size());
        printf("Real cardinality: %d\n", int(true_freq.size()));
        printf("Est_cardinality : %f, RE : %f\n", card, card_error);
        /************************************************/
        
        double entropy_err = 0;
        double entropy_est = um->get_entropy();
        double entropy_true = 0;

        double tot_true = 0;
        double entr_true = 0;
        for (int i = 0; i < true_dist.size(); ++i)
        {
            if (true_dist[i] == 0)
                continue;
            tot_true += i * true_dist[i];
            entr_true += i * true_dist[i] * log2(i);
        }
        entropy_true = - entr_true / tot_true + log2(tot_true);

        entropy_err = std::abs(entropy_est - entropy_true) / entropy_true;
        printf("ENTROPY ERROR ( true : %f, est : %f) = %f\n", entropy_true, entropy_est, entropy_err);
        /***************************************************************/

        true_freq.clear();
        delete um;
    }
    printf("End\n");
    printf("\n\n\n");
}





