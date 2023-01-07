#include <stdlib.h>
#include <time.h>

#ifndef CACHE_H
#define CACHE_H
//cache line 是cache和主存之间传输的最小单位
#define L1_BLOCK_SZ_BYTES 64
//512行 cache line
#define L1_SETS 512

#define L1_WAYS 2 
#define Num 1024

#define L1_SZ_BYTES (L1_SETS*L1_WAYS*L1_BLOCK_SZ_BYTES)



volatile uint8_t dummyMem1[2 * L1_SZ_BYTES];
uint8_t unusedarr1[L1_SZ_BYTES];
volatile uint8_t dummyMem2[4 * L1_SZ_BYTES];
uint8_t unusedarr2[L1_SZ_BYTES];
volatile uint8_t dummyMem3[8 * L1_SZ_BYTES];
uint8_t unusedarr3[L1_SZ_BYTES];
volatile uint8_t dummyMem4[16 * L1_SZ_BYTES];

void flushCache(){
    volatile uint8_t dummy = 0;
    int tmpNum = 2*1024;
    for(int idx=0;idx<tmpNum;idx++){
        dummy = dummyMem1[rand()%tmpNum];
    }
    tmpNum = 4*1024;
    for(int idx=0;idx<tmpNum;idx++){
        dummy = dummyMem2[rand()%tmpNum];
    }
    tmpNum = 8*1024;
    for(int idx=0;idx<tmpNum;idx++){
        dummy = dummyMem3[rand()%tmpNum];
    }
    tmpNum = 16*1024;
    for(int idx=0;idx<tmpNum;idx++){
        dummy = dummyMem3[rand()%tmpNum];
    }
   // printf("dummy = %d\n",dummy);       
}

#endif
