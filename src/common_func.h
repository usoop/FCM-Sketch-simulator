#ifndef _COMMON_FUNC_H_
#define _COMMON_FUNC_H_

#include <iostream>
#include <utility>
#include <unistd.h>
#include <stdint.h>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <arpa/inet.h>
#include <cstring>
#include <random>
#include <stdexcept>

using std::map;
using std::vector;
using std::set;
using std::unordered_map;
using std::pair;
using std::make_pair;
using std::string;


/************************** Loading Traces ******************************/

#define NUM_TRACE 1 // Number of traces in DATA directory 
#define DATA_ROOT_15s "/data/data" // NUM_TRACE = 1, CAIDA

struct SRCIP_TUPLE{ char key[4]; };
struct REST_TUPLE{ char key[9]; };

typedef vector<SRCIP_TUPLE> TRACE;

TRACE traces[NUM_TRACE];

uint32_t ReadTraces()
{
    uint32_t total_pck_num = 0;
    for (int iter_data = 0; iter_data < NUM_TRACE; ++iter_data)
    {
        char filename[100];
        sprintf(filename, "%s%s%d.dat", get_current_dir_name(), DATA_ROOT_15s, iter_data+1);
        traces[iter_data].clear();

        // std::cout << filename << std::endl;
        FILE *fin;
      
        char data_command[200];
        sprintf(data_command, "%s %s%s", "wget https://www.dropbox.com/s/bajuqha884cxgwl/data1.dat -P", get_current_dir_name(), "/data/");

        if ((fin = fopen(filename, "rb")))
            std::cout << filename << " is successfully loaded..." << std::endl;
        else{
            std::cout << "\033[1;31m A sample data (at data/) does not exist, so start to download (~260 MB)...\033[0m" << std::endl;
            int wget_result = std::system(data_command);
            if (!(fin = fopen(filename, "rb")))
                std::cout << "\033[1;31m Download is failed. Please see the src/common_func.h\033[0m" << std::endl;
        }

        fseek(fin, 0L, SEEK_END);
        uint32_t sz = ftell(fin);
        fseek(fin, 0L, SEEK_SET);
        std::cout << "Size of File : " << sz << std::endl;

        if (sz < 270000000){
            throw std::invalid_argument("\033[1;31m File is either corrupted or partly downloaded. Please re-download the sample file data/data1.dat\033[0m");
        }


        SRCIP_TUPLE data_srcip;
        REST_TUPLE data_residual;

        if (fin != NULL) {
            while(fread(&data_srcip, 1, 4, fin) == 4 and fread(&data_residual, 1, 9, fin) == 9) {
                traces[iter_data].push_back(data_srcip);
            }
        }
        fclose(fin);
        total_pck_num += traces[iter_data].size();

        printf("Successfully read in %s, %ld packets\n", filename, traces[iter_data].size());
    }
    printf("\n");
    return total_pck_num;
}
/********************************************************************/

/************************** COMMON FUNCTIONS*************************/
#define ROUND_2_INT(f) ((int)(f >= 0.0 ? (f + 0.5) : (f - 0.5)))
#define HH_THRESHOLD 10000 // 20,000,000 * 0.0005 (0.05%)


/******************** Data Structures ****************************/
// FCM-Sketch (8-ary)
// FCM-Sketch is fully implementable on hardware switch.

#define FCMSK_DEPTH 2 // number of trees
#define FCMSK_LEVEL 3 // number of layer in trees
#define FCMSK_K_ARY 8 // k-ary tree

#if FCMSK_K_ARY == 2
  #define FCMSK_K_POW 1 // 2^1 = 2
#elif FCMSK_K_ARY == 4
  #define FCMSK_K_POW 2 // 2^2 = 4
#elif FCMSK_K_ARY == 8
  #define FCMSK_K_POW 3 // 2^3 = 8
#elif FCMSK_K_ARY == 16
  #define FCMSK_K_POW 4 // 2^4 = 16
#elif FCMSK_K_ARY == 32
  #define FCMSK_K_POW 5 // 2^5 = 32
#endif

/* Config using about 1.3MB */
#define FCMSK_WL1 524288 // width of layer 1 (number of registers)
#define FCMSK_WL2 65536 // width of layer 2 (number of registers)
#define FCMSK_WL3 8192  // width of layer 3 (number of registers)

typedef uint8_t FCMSK_C1; // 8-bit
#define FCMSK_THL1 254
typedef uint16_t FCMSK_C2; // 16-bit
#define FCMSK_THL2 65534
typedef uint32_t FCMSK_C3; // 32-bit
#define FCMSK_EM_ITER 15 // Number of iteration of EM-Algorithm. You can control.
/********************************************************************/


// FCM+TopK (16-ary)
// Here, we consider the actual hardware implementation on Tofino. 
// The actual register size of each bucket in each Top-K entry is (8 * 3 + 4) = 28 Byte,
// which is composed of 1 val_all (4B) + 3 key-value pairs (4 + 4) = 28 Byte.

#define JUDGE_IF_SWAP_FCMPLUS_P4(min_val, guard_val) ((guard_val >> 5) > min_val)
#define FCMPLUS_DEPTH 2 // number of trees
#define FCMPLUS_LEVEL 3 // number of layer in trees
#define FCMPLUS_BUCKET 4096 // 2^12, num of entries for key-value pairs
#define FCMPLUS_K_ARY 16 // k-ary tree


#if FCMPLUS_K_ARY == 2
  #define FCMPLUS_K_POW 1 // 2^1 = 2
#elif FCMPLUS_K_ARY == 4
  #define FCMPLUS_K_POW 2 // 2^2 = 4
#elif FCMPLUS_K_ARY == 8
  #define FCMPLUS_K_POW 3 // 2^3 = 8
#elif FCMPLUS_K_ARY == 16
  #define FCMPLUS_K_POW 4 // 2^4 = 16
#elif FCMPLUS_K_ARY == 32
  #define FCMPLUS_K_POW 5 // 2^5 = 32
#endif



 // Config using 1.25MB 
#define FCMPLUS_BUCKET 4096 // 2^12, num of entries for key-value pairs
#define FCMPLUS_WL1 524288 // width of layer 1 (number of registers)
#define FCMPLUS_WL2 32768 // width of layer 2 (number of registers)
#define FCMPLUS_WL3 2048  // width of layer 3 (number of registers)



typedef uint8_t FCMPLUS_C1; // 8-bit
#define FCMPLUS_THL1 254
typedef uint16_t FCMPLUS_C2; // 16-bit
#define FCMPLUS_THL2 65534
typedef uint32_t FCMPLUS_C3; // 32-bit
#define FCMPLUS_EM_ITER 15 // Num.iteration of EM-Algorithm. You can control.
/********************************************************************/


// ElasticSketch
// Here, ElasticSketch is P4-BMV2 (software) version, and doesn't consider the hardware restriction for Top-K algorithm. 
// Actually, its hardware implementation of Top-K algorithm will have a worse performance. 
// In software, each bucket of Top-K entry is of size 12-Byte (4B for val_all, and 4 + 4 for a key-value pair). 
// In Tofino, each bucket is 28 bits by approximated implementation. 

// Config using 1.375MB

#define ELASTIC_WL 1048576
#define ELASTIC_BUCKET 8192

#define ELASTIC_HEAVY_STAGE 4
#define ELASTIC_TOFINO 0 // 1 : Tofino, 0 : Software emulation
#define JUDGE_IF_SWAP_ELASTIC_P4(min_val, guard_val) ((guard_val >> 5) > min_val)
#define ELASTIC_EM_ITER 15 // Num.iteration of EM-Algorithm. You can control.
struct Bucket
{
  uint32_t key;
  uint32_t val;
  uint32_t guard_val;
};
/********************************************************************/


// Count-Min
#define CM_BYTES 1572864 // 1.5 * 1024 * 1024 = 1.5MB
#define CM_DEPTH 3 // depth of CM
/********************************************************************/


// MRAC
#define MRAC_BYTES 1572864 // 1.5 * 1024 * 1024 = 1.5MB
#define MRAC_EM_ITER 15 // Num.iteration of EM-Algorithm. You can control.
/********************************************************************/


// HYPERLOGLOG
#define HLL_B 20 // number of registers, 2^20 * 1 Byte = 1.0MB
#define HLL_REG_SIZE 8 // 8-bit register size
/********************************************************************/

// CUSKETCH (Count-Min + Conservative Update scheme)
#define CU_BYTES 1572864 // 1.5 * 1024 * 1024 = 1.5MB
#define CU_DEPTH 3 // depth of CU
/********************************************************************/

// PyramidSketch + Count-Min (PCMSketch)
#define MAX_HASH_NUM 20
#define LOW_HASH_NUM 4 // depth of PCM
typedef long long lint;
typedef unsigned int uint;
#define PCM_BYTES 1572864 // 1.5 * 1024 * 1024 = 1.5MB
/********************************************************************/

// UnivMon
#define UNIV_LEVEL 16
#define UNIV_K 2048
#define UNIV_ROW 5
#define UNIV_BYTES 1572864 // 1.5 * 1024 * 1024 = 1.5MB
/********************************************************************/

#endif












