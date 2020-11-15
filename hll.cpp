#include "./src/Hll/hll.h"

using namespace std;
using namespace hll;

int main()
{
	printf("Start accuracy measurement of HyperLogLog\n");
	uint32_t totnum_packet = ReadTraces();

	HyperLogLog * hll = NULL;

	for(int iter_data = 0; iter_data < NUM_TRACE; ++iter_data){
		printf("[INFO] %3d-th trace starts to be processed..\n", iter_data+1);
		unordered_map<uint32_t, uint32_t> true_freq;
		vector<int> true_dist(1000000);
		hll = new HyperLogLog(HLL_B, HLL_REG_SIZE);
		int num_pkt = (int)traces[iter_data].size();

		for (int i = 0; i < num_pkt; ++i){
			hll->add((char*)(traces[iter_data][i].key), 4); // 4 = key_len
			true_freq[*((uint32_t*)(traces[iter_data][i].key))]++;
		}
        printf("[INFO] End of insertion process...\n");
		/*-*-*-* End of packet insertion *-*-*-*/

		int card = hll->estimate();
		double card_err = abs(card - int(true_freq.size())) / double(true_freq.size());
		printf("RE of cardinality : %2.6f (est=%8.0d, true=%8.0d)\n", card_err, card, (int)true_freq.size());
		/*-*-*-* End of cardinality estimation *-*-*-*/

        delete hll;
	}
    printf("END\n");
	printf("\n\n\n");

}