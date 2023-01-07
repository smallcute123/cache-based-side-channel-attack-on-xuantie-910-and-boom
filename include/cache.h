#ifndef CACHE_H
#define CACHE_H

// cache values
// TODO: check that these parameters are right

//cache line 是cache和主存之间传输的最小单位
#define L1_BLOCK_SZ_BYTES 64
#define L1_BLOCK_BITS 6 // note: this is log2Ceil(L1_BLOCK_SZ_BYTES)

//512行 cache line
#define L1_SETS 512
#define L1_SET_BITS 9 // note: this is log2Ceil(L1_SETS)

//路（Ways）
#define L1_WAYS 2 


#define L1_SZ_BYTES (L1_SETS*L1_WAYS*L1_BLOCK_SZ_BYTES)
#define FULL_MASK 0xFFFFFFFFFFFFFFFF
#define OFF_MASK (~(FULL_MASK << L1_BLOCK_BITS))
#define TAG_MASK (FULL_MASK << (L1_SET_BITS + L1_BLOCK_BITS))
#define SET_MASK (~(TAG_MASK | OFF_MASK))

/* ----------------------------------
 * |                  Cache address |
 * ----------------------------------
 * |       tag |      idx |  offset |
 * ----------------------------------
 * | 63 <-> 15 | 14 <-> 6 | 5 <-> 0 |
 * ----------------------------------
 */

// setup array size of cache to "put" in the cache on $ flush
// guarantees contiguous set of addrs that is at least the sz of cache
// 5 so that you can hit more
volatile uint8_t dummyMem[10 * L1_SZ_BYTES];

/**
 * Flush the cache of the address given since RV64 does not have a
 * clflush type of instruction. Clears any set that has the same idx bits
 * as the address input range.
 *
 * Note: This does not work if you are trying to flush dummyMem out of the
 * cache.
 *
 * @param addr starting address to clear the cache
 * @param sz size of the data to remove in bytes
 */
void flushCache(uint64_t addr, uint64_t sz){
    //printf("L1_SZ_BYTES = %luKB,SETS= %d, Block = %dBytes\n",L1_SZ_BYTES/(1024),L1_SETS,L1_BLOCK_SZ_BYTES);
    //printf("Flushed addr(0x%x) tag(0x%x) set(0x%x) off(0x%x) sz(%d)\n", addr, (addr & TAG_MASK) >> (L1_SET_BITS + L1_BLOCK_BITS), (addr & SET_MASK) >> L1_BLOCK_BITS, addr & OFF_MASK, sz);

    // // find out the amount of blocks you want to clear
    //计算需要清除的缓存行数
    volatile uint64_t numSetsClear = sz >> L1_BLOCK_BITS;
    if ((sz & OFF_MASK) != 0){
        numSetsClear += 1;
    }
    if (numSetsClear > L1_SETS){
        //flush entire cache with no rollover (makes the function finish faster) 
        numSetsClear = L1_SETS;
    }
    
    //volatile uint64_t numSetsClear = L1_SETS*2;
    //printf("numSetsClear(%d)\n", numSetsClear);

    // temp variable used for nothing
    volatile uint8_t dummy = 0;

    // this mem address is the start of a contiguous set of memory that will fit inside of the  cache
    // 是连续的一组地址，将会填充cache ,计算存放的位置
    // thus it has the following properties
    // 1. dummyMem <= alignedMem < dummyMem + sizeof(dummyMem)
    // 2. alignedMem has idx = 0 and offset = 0 
    
    volatile uint64_t alignedMem = (((uint64_t)&dummyMem) + L1_SZ_BYTES) & TAG_MASK;

    //printf("alignedMem(0x%x)\n", alignedMem);
        
    for (volatile uint64_t i = 0; i < numSetsClear; ++i){
        // offset to move across the sets that you want to flush
        volatile uint64_t setOffset = (((addr & SET_MASK) >> L1_BLOCK_BITS) + i) << L1_BLOCK_BITS;
        //printf("setOffset(0x%x)\n", setOffset);

        // since there are L1_WAYS you need to flush the entire set
        //cache默认采用的是随机替换策略从而需要更多次的驱逐
        for(volatile uint64_t j = 0; j < 4*L1_WAYS; ++j){
            // offset to reaccess the set
            volatile uint64_t wayOffset = j << (L1_BLOCK_BITS + L1_SET_BITS);
            //printf("wayOffset(0x%x)\n", wayOffset);

            // evict the previous cache block and put in the dummy mem
            // 来加载指定地址的垃圾数据到dummy数组中
            // 同时CPU会把这些垃圾数据更新到data cache
            // 从而实现对data cache指定空间的刷新操作
            dummy = *((uint8_t*)(alignedMem + setOffset + wayOffset));
            //printf("evict read(0x%x)\n", alignedMem + setOffset + wayOffset);
        }
    }
    //printf("%d ",dummy);
}

#endif
