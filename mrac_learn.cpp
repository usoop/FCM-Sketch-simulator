#include "./src/MRAC/MRAC.h"
using namespace std;

int main()
{
    printf("Start accuracy measurement of MRAC\n");
    uint32_t totnum_packet = ReadTraces();
    /* NOTE: init pointer */
    MRAC *mrac = NULL;

    for(int iter_data = 0; iter_data < NUM_TRACE; ++iter_data){
        printf("[INFO] %3d-th trace starts to be processed..\n", iter_data+1);
        unordered_map<uint32_t, uint32_t> true_freq;
        vector<int> true_dist(1000000);
        /* NOTE: init instance */
        mrac = new MRAC(MRAC_BYTES);
        int num_pkt = (int)traces[iter_data].size();

        for (int i = 0; i < num_pkt; ++i){
            /* NOTE: insert */
            mrac->insert((uint8_t*)(traces[iter_data][i].key));
            true_freq[*((uint32_t*)(traces[iter_data][i].key))]++;
        }
        printf("[INFO] End of insertion process...\n");
        /*-*-*-* End of packet insertion *-*-*-*/

        for(unordered_map<uint32_t, uint32_t>::iterator it = true_freq.begin(); it != true_freq.end(); ++it){
            if (it->second >= true_dist.size()) {
                true_dist.resize(it->second + 1, 0);
            }
            /*!
             * `true_freq` count the `<key, key_occurance_frequency>`,
             * and the `true_dist` counts the frequency of `key_occurance_frequency`,
             * i.e., the frequency of element that appear `key_occurance_frequency` times.
             * Note that, the `key_occurance_frequency` is the index of the `true_dist`,
             * that is, `true_dist[x]` means the frequency of the element that appear `x` times in the data stream;
             */
            true_dist[it->second] += 1;
        }

        vector<double> dist;
        /* NOTE: get distribution */
        mrac->get_distribution(dist);   

        // compute WMRD
        double WMRD = 0;
        double WMRD_nom = 0;
        double WMRD_denom = 0;
        printf("estimated dist");
        for (int i=0;i<dist.size() and i<10;i++) {
            printf("i: %f \n",dist[i]);
        }
        printf("true dist");
        for (int i=0;i<true_dist.size() and i<10;i++) {
            printf("i: %d \n",true_dist[i]);
        }
        for (int i = 1; i < true_dist.size(); ++i)
        {
            if (true_dist[i] == 0){
                continue;
            }
            WMRD_nom += std::abs(true_dist[i] - dist[i]);
            WMRD_denom += double(true_dist[i] + dist[i])/2;
        }
        WMRD = WMRD_nom / WMRD_denom;
        printf("WMRD : %3.5f\n", WMRD);


        // entropy initialization
        double entropy_err = 0;
        double entropy_est = 0;
        double entropy_true = 0;

        double tot_est = 0;
        double entr_est = 0;

        for (int i = 1; i < dist.size(); ++i)
        {
            if (dist[i] == 0)
                continue;
            tot_est += i * dist[i];
            entr_est += i * dist[i] * log2(i);
        }
        entropy_est = - entr_est / tot_est + log2(tot_est);

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
        printf("Entropy Relative Error (RE) = %f (true : %f, est : %f)\n", entropy_err, entropy_true, entropy_est);
        /*-*-*-* End of flow size distribution and entropy *-*-*-*/

        /* NOTE: remove */
        delete mrac;
    }
    printf("END\n");
    printf("\n\n\n");
}


