#include "./src/PyramidSketch/PCMSketch.h"
using namespace std;

// PyramidSketch + Count-Min Sketch

int main()
{
    printf("PYRAMID SKETCH + COUNT_MIN --- DEPTH : %d\n", LOW_HASH_NUM);
    uint32_t totnum_packet = ReadTraces();

    double memory = PCM_BYTES * 1.0; // MB
    int word_size = 64;
    int w_p = memory * 8.0 / (word_size * 2); // what is 2?

    PCMSketch *pcmsketch = NULL;

    std::vector<double> pyramid_are;
    std::vector<double> pyramid_aae;

    for(int iter_data = 0; iter_data < NUM_TRACE; ++iter_data){
        printf("[INFO] %3d-th trace starts to be processed..\n", iter_data+1);
        unordered_map<uint32_t, uint32_t> true_freq;
        vector<int> true_dist(1000000);
        pcmsketch = new PCMSketch(w_p, LOW_HASH_NUM, word_size);
        int num_pkt = (int)traces[iter_data].size();

        for (int i = 0; i < num_pkt; ++i){
            pcmsketch->insert((char*)(traces[iter_data][i].key));
            true_freq[*((uint32_t*)(traces[iter_data][i].key))]++;
        }
        printf("[INFO] End of insertion process...\n");
        /*-*-*-* End of packet insertion *-*-*-*/

        double ARE = 0;
        double AAE = 0;
        set<uint32_t> HH_true;
        for(unordered_map<uint32_t, uint32_t>::iterator it = true_freq.begin(); it != true_freq.end(); ++it){
            char key[4] = {0}; // srcIP-flowkey
            uint32_t temp_first = htonl(it->first); // convert uint32_t -> uint8_t * 4 array
            for (int i=0; i<4 ;++i){
                key[i] = ((char*)&temp_first)[3-i];
            }
            if (it->second >= true_dist.size())
                true_dist.resize(it->second + 1, 0);
            true_dist[it->second] += 1;
            uint32_t est_val = pcmsketch->query(key);
            int dist = std::abs((int)it->second - (int)est_val);
            ARE += dist * 1.0 / (it->second);
            AAE += dist * 1.0;
            if (it->second > HH_THRESHOLD)
                HH_true.insert(it->first);
        }
        ARE /= (int)true_freq.size();
        AAE /= (int)true_freq.size();

        printf("ARE : %.8lf, AAE : %.8lf\n", ARE, AAE);
        delete pcmsketch;
    }
    printf("END\n");
    printf("\n\n\n");
}




