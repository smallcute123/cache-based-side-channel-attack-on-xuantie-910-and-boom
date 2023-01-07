#include <stdio.h>
#include <stdint.h> 
#include "encoding.h"
#include "flushCache.h"

uint64_t array1_sz = 16;
int main(void){
    uint64_t start, diff;
    uint8_t dummy = 2;
 uint8_t    dummy1 = 0;
    static uint64_t results[256];
    start = rdcycle();
    dummy1= dummy;
    diff = (rdcycle() - start);
         printf("test:read from ram, cycles = %lu\n", diff); 

    start = rdcycle();
    dummy1= array1_sz;
    diff = (rdcycle() - start);
     printf("test:read from ram, cycles = %lu\n", diff); 
    start = rdcycle();
    dummy1= array1_sz ;
    diff = (rdcycle() - start);
     printf("test:read from cache, cycles = %lu\n", diff); 
    array1_sz =  array1_sz << 4;
for(int i=0;i<10;i++){
    asm("fcvt.s.lu	fa4, %[in]\n"
        "fcvt.s.lu	fa5, %[inout]\n"
        "fdiv.s	fa5, fa5, fa4\n"
        "fdiv.s	fa5, fa5, fa4\n"
        "fdiv.s	fa5, fa5, fa4\n"
        "fdiv.s	fa5, fa5, fa4\n"
        "fcvt.lu.s	%[out], fa5, rtz\n"
           : [out] "=r" (array1_sz)
           : [inout] "r" (array1_sz), [in] "r" (dummy)
           : "fa4", "fa5");
    }
    start = rdcycle();
    dummy1= array1_sz;
    diff = (rdcycle() - start);
     printf("test:read after div, cycles = %lu\n", diff); 

}