/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : eta_memman_fibonachi.c
 ***********************************************************************/

#include <stdbool.h>
#include "eta_memman.h"
#include "eta_dbg.h"

//#define MEMMAN_FIBONACHI
//#define MEMMAN_SIMPLE

#if defined(MEMMAN_SIMPLE) || defined(MEMMAN_FIBONACHI)

typedef struct sMemManDLLRoot sMemManDLLRoot;
typedef struct sMemManDLLEntity sMemManDLLEntity;
typedef struct sMemManBlock sMemManBlock;

struct sMemManDLLRoot {
	sMemManBlock *FirstBlock;
	sMemManBlock *LastBlock;
};
struct sMemManDLLEntity {
	sMemManBlock *PrevBlock;
	sMemManBlock *NextBlock;
};

#if defined(MEMMAN_FIBONACHI)

struct sMemManBlock {
    uint32_t Magic;
	sMemManDLLEntity Entity;
	struct {
		memman_size_t LeftBuddyCounter;
		uint8_t BuddySizeIndex;
		bool IsBusy;
	} __attribute((packed));
	union {
		struct {
			sMemManDLLEntity CongruousDllEntity;
            // here might be some fields of free block for special purposes
		} Free;
		__extension__ struct {
			// here might be some fields of busy block for special purposes
		} Busy;
	};
};

#elif defined(MEMMAN_SIMPLE)

struct sMemManBlock {
    uint32_t Magic;
    sMemManDLLEntity Entity;
    bool IsBusy;
    union {
        __extension__ struct {
            // here might be some fields of free block for special purposes
        } Free;
        __extension__ struct {
            // here might be some fields of busy block for special purposes
        } Busy;
    };
};

#endif

#define MEMMAN_BLOCKMAGIC 0x12F5AC72

#define MEMMAN_BLOCKPTR(ptr) ((sMemManBlock *)(ptr))
#define MEMMAN_BYTESPTR(ptr) ((uint8_t *)(ptr))
#define MEMMAN_BLOCKOVERHEAD_FREE ((memman_size_t)((uint32_t)(&MEMMAN_BLOCKPTR(0)->Free) + (uint32_t)(sizeof(MEMMAN_BLOCKPTR(0)->Free))))	// overhead of free block
#define MEMMAN_BLOCKOVERHEAD_BUSY ((memman_size_t)((uint32_t)(&MEMMAN_BLOCKPTR(0)->Busy) + (uint32_t)(sizeof(MEMMAN_BLOCKPTR(0)->Busy))))	// overhead of busy block

static sMemManDLLRoot MemManHeapDll;				// main heap double linked list
static memman_size_t MemManHeapFree, MemManMinimumHeapFree;

// Makes the block free
__inline static void bm_makefree(sMemManBlock *block) {
    block->IsBusy = false;
}
// Makes the block busy
__inline static void bm_makebusy(sMemManBlock *block, memman_size_t size) {
    block->IsBusy = true;
    memset(((uint8_t *)block) + MEMMAN_BLOCKOVERHEAD_BUSY, 0, size);
}
// Checks block for magic
__inline static void bm_checkmagic(sMemManBlock const *block) {
    if(block && block->Magic != MEMMAN_BLOCKMAGIC) {
        DBG_PutString("!!!!! memory integrity failure !!!!!"NL);
        //while (true);
    }
}
// Appends a new block as the last in heap block table
__inline static void bm_dllappendlast_heap(sMemManBlock *block_new) {
    if (MemManHeapDll.LastBlock) {
        block_new->Entity.PrevBlock = MemManHeapDll.LastBlock;
        block_new->Entity.NextBlock = nullptr;
        MemManHeapDll.LastBlock->Entity.NextBlock = block_new;
        MemManHeapDll.LastBlock = block_new;
    }
    else {
        block_new->Entity.PrevBlock = block_new->Entity.NextBlock = nullptr;
        MemManHeapDll.FirstBlock = MemManHeapDll.LastBlock = block_new;
    }
}
// Appends a new block after the specified one in heap block table
__inline static void bm_dllappendafter_heap(sMemManBlock *block, sMemManBlock *block_new) {
    block_new->Entity.PrevBlock = block;
    block_new->Entity.NextBlock = block->Entity.NextBlock;
    if (block->Entity.NextBlock) block->Entity.NextBlock->Entity.PrevBlock = block_new;
    block->Entity.NextBlock = block_new;
}

// Deletes the specified block from the heap block table
__inline static void bm_dllremove_heap(sMemManBlock *block) {
    if (block->Entity.PrevBlock) block->Entity.PrevBlock->Entity.NextBlock = block->Entity.NextBlock;
    else MemManHeapDll.FirstBlock = block->Entity.NextBlock;
    if (block->Entity.NextBlock) block->Entity.NextBlock->Entity.PrevBlock = block->Entity.PrevBlock;
    else MemManHeapDll.LastBlock = block->Entity.PrevBlock;
}

#if defined(MEMMAN_SIMPLE)

static uint8_t *MemManHeapEnd;

// Inits block as free
static void bm_initfree(sMemManBlock *block) {
    block->Entity.PrevBlock = nullptr;
    block->Entity.NextBlock = nullptr;
    block->Magic = MEMMAN_BLOCKMAGIC;
    block->IsBusy = false;
}

// Allocates a block in memory heap
static void *memman_alloc(memman_size_t size, bool is_clear_block) {
    // Search for the best suited block of free memory
    sMemManBlock *_best_block = nullptr;
    memman_size_t _best_size = (memman_size_t)-1;
    sMemManBlock *_current_block = MemManHeapDll.FirstBlock;
    bm_checkmagic(_current_block);
    while (_current_block) {
        bm_checkmagic(_current_block->Entity.NextBlock);
        if(!_current_block->IsBusy) {
            memman_size_t _current_size = (memman_size_t)((_current_block->Entity.NextBlock) ? MEMMAN_BYTESPTR(_current_block->Entity.NextBlock) - MEMMAN_BYTESPTR(_current_block) : MemManHeapEnd - MEMMAN_BYTESPTR(_current_block)) - MEMMAN_BLOCKOVERHEAD_BUSY;
            if(_current_size == size) {
                _best_block = _current_block; _best_size = _current_size;
                break;
            }
            else if(_current_size > size && _best_size > _current_size) {
                _best_block = _current_block; _best_size = _current_size;
            }
        }
        _current_block = _current_block->Entity.NextBlock;
    }

    if(_best_block) {
        memman_size_t _remains_size = _best_size - size;
        bm_makebusy(_best_block, is_clear_block ? size : 0);
        void *_block = MEMMAN_BYTESPTR(_best_block) + MEMMAN_BLOCKOVERHEAD_BUSY;
        // Is there some space remains after this block?
        if(_remains_size > max(MEMMAN_BLOCKOVERHEAD_FREE, MEMMAN_BLOCKOVERHEAD_BUSY)) {
            sMemManBlock *_remains_block = MEMMAN_BLOCKPTR(MEMMAN_BYTESPTR(_block) + size);
            bm_initfree(_remains_block);
            bm_dllappendafter_heap(_best_block, _remains_block);
            MemManHeapFree -= size;
        }
        else MemManHeapFree -= _best_size;
        if (MemManMinimumHeapFree > MemManHeapFree) MemManMinimumHeapFree = MemManHeapFree;
        return _block;
    }
    else return nullptr;
}
// Frees a block previously allocated by memman_alloc
static void memman_free(void *block) {
    if (block) {
        sMemManBlock *_block = MEMMAN_BLOCKPTR(MEMMAN_BYTESPTR(((uint32_t)block & -4) - MEMMAN_BLOCKOVERHEAD_BUSY));
        // Check block magic
        bm_checkmagic(_block);
        // Skip if the block is already free
        if (_block->IsBusy) {
            // Make the freeing block free
            bm_makefree(_block);
            memman_size_t _block_size = (memman_size_t)((_block->Entity.NextBlock) ? MEMMAN_BYTESPTR(_block->Entity.NextBlock) - MEMMAN_BYTESPTR(_block) : MemManHeapEnd - MEMMAN_BYTESPTR(_block)) - MEMMAN_BLOCKOVERHEAD_BUSY;
            MemManHeapFree += _block_size;
            // Try to merge with neighbour blocks if they are free
            sMemManBlock *_block_prev = _block->Entity.PrevBlock; if(_block_prev && !_block_prev->IsBusy) bm_dllremove_heap(_block);
            sMemManBlock *_block_next = _block->Entity.NextBlock; if(_block_next && !_block_next->IsBusy) bm_dllremove_heap(_block_next);
        }
    }
    else { DBG_PutString("!!!!! memory block is not busy !!!!!"NL); }
}

// Initializes buddymem heap and creates initial blocks
void memman_initialize(void *heap_start, memman_size_t heap_size) {
    DBG_PutString("Buddy memory manager initialization"NL);
    DBG_PutFormat(" - sizeof(sMemManBlockFree): %7d"NL, MEMMAN_BLOCKOVERHEAD_FREE);
    DBG_PutFormat(" - sizeof(sMemManBlockBusy): %7d"NL, MEMMAN_BLOCKOVERHEAD_BUSY);
    DBG_PutFormat(" - heap start:      0x%08X (%7d)"NL, heap_start, heap_start);
    DBG_PutFormat(" - heap size:       0x%08X (%7d)"NL, heap_size, heap_size);
    MemManHeapEnd = ((uint8_t *)heap_start) + heap_size;
    heap_size -= MEMMAN_BLOCKOVERHEAD_BUSY;
    MemManHeapFree = heap_size;
    MemManMinimumHeapFree = MemManHeapFree;
    sMemManBlock *_block_new = (sMemManBlock *)heap_start;
    bm_initfree(_block_new);
    bm_dllappendlast_heap(_block_new);
}

#elif defined(MEMMAN_FIBONACHI)

static memman_size_t const MemManBlockSizeTable[] = {
	24,			//  0
	36,			//  1
	60,			//  2
	96,			//  3
	156,		//  4
	252,		//  5
	408,		//  6
	660,		//  7
	1068,		//  8
	1728,		//  9
	2796,		// 10
	4524,		// 11
	7320,		// 12
	11844,		// 13
	19164,		// 14
	31008,		// 15
	50172,		// 16
    //	81180,		// 17
    //	131352,		// 18
    //	212532,		// 19
    //	343884,		// 20
    //	556416,		// 21
    //	900300,		// 22
    //	1456716,	// 23
    //	2357016,	// 24
    //	3813732,	// 25
    //	6170748,	// 26
};
#define MemMan_ROWS sizeof_array(MemManBlockSizeTable)										// number of rows in free block DLL table

static sMemManDLLRoot MemManFreeDllTable[MemMan_ROWS];  // double linked lists of congruous size block table

// Inits block as free
static void bm_initfree(sMemManBlock *block, memman_size_t lbc, uint8_t bsi) {
	block->LeftBuddyCounter = lbc;
	block->BuddySizeIndex = bsi;
	block->IsBusy = false;
}
// Searches the nearest small size that is bigger or equal to param 'size'
static uint8_t bm_find_bsi(memman_size_t size, uint8_t *lowbsi) {
	uint8_t _lowbsi = 0;
	uint8_t _highbsi = MemMan_ROWS - 1;
	if (size <= MemManBlockSizeTable[0]) {
		if (lowbsi) *lowbsi = 0;
		return 0;
	}

	// Binary search for the nearest low and high BSI of the requested size
	while (_lowbsi <= _highbsi) {
		uint8_t _midbsi = (_lowbsi + _highbsi) >> 1;
		if (MemManBlockSizeTable[_midbsi] > size) _highbsi = _midbsi - 1;
		else _lowbsi = _midbsi + 1;
	}
	_lowbsi = _highbsi;
	if (MemManBlockSizeTable[_lowbsi] < size) _highbsi++;
	if (lowbsi) *lowbsi = _lowbsi;

	return _highbsi;
}

// Appends a new block as the last in congruous size block table
__inline static void bm_dllappendlast_cs(sMemManBlock *block_new) {
	sMemManDLLRoot *_dll_cs = &MemManFreeDllTable[block_new->BuddySizeIndex];
	if (_dll_cs->LastBlock) {
		block_new->Free.CongruousDllEntity.PrevBlock = _dll_cs->LastBlock;
		block_new->Free.CongruousDllEntity.NextBlock = nullptr;
		_dll_cs->LastBlock->Free.CongruousDllEntity.NextBlock = block_new;
		_dll_cs->LastBlock = block_new;
	}
	else {
		block_new->Free.CongruousDllEntity.PrevBlock = block_new->Free.CongruousDllEntity.NextBlock = nullptr;
		_dll_cs->FirstBlock = _dll_cs->LastBlock = block_new;
	}
}

// Deletes the specified block from the congruous size block table
__inline static void bm_dllremove_cs(sMemManBlock *block) {
	sMemManDLLRoot *_dll_cs = &MemManFreeDllTable[block->BuddySizeIndex];
	if (block->Free.CongruousDllEntity.PrevBlock) block->Free.CongruousDllEntity.PrevBlock->Free.CongruousDllEntity.NextBlock = block->Free.CongruousDllEntity.NextBlock;
	else _dll_cs->FirstBlock = block->Free.CongruousDllEntity.NextBlock;
	if (block->Free.CongruousDllEntity.NextBlock) block->Free.CongruousDllEntity.NextBlock->Free.CongruousDllEntity.PrevBlock = block->Free.CongruousDllEntity.PrevBlock;
	else _dll_cs->LastBlock = block->Free.CongruousDllEntity.PrevBlock;
}

// Allocates a block in memory heap
static void *memman_alloc(memman_size_t size, bool is_clear_block) {
	// find minimum BSI for the size + overhead
	size += MEMMAN_BLOCKOVERHEAD_BUSY;
	uint8_t _bsi_need = bm_find_bsi(size, nullptr);

	// Table lookup
	sMemManBlock *_block = MemManFreeDllTable[_bsi_need].FirstBlock;
	if (!_block) {
		// No free block with requested BSI. Try to find a free one with higher BSI and divide it into buddies
		uint8_t _bsi_higher = _bsi_need;
		while (++_bsi_higher < MemMan_ROWS && !_block) {
			_block = MemManFreeDllTable[_bsi_higher].FirstBlock;
		}

		if (_block) {
			// Divide larger block into buddies until a block with appropriate BSI is found
			bm_dllremove_cs(_block);
			while (_block->BuddySizeIndex > _bsi_need && _block->BuddySizeIndex > 1) {
				_block->BuddySizeIndex--;
				_block->LeftBuddyCounter++;
				// Create right buddy
				uint8_t _bsi_right = _block->BuddySizeIndex;
				sMemManBlock *_block_right = MEMMAN_BLOCKPTR(MEMMAN_BYTESPTR(_block) + MemManBlockSizeTable[_bsi_right--]);
				bm_initfree(_block_right, 0, _bsi_right);
				bm_dllappendafter_heap(_block, _block_right);

				// Choose the smallest suitable block
				if (_bsi_right < _bsi_need) bm_dllappendlast_cs(_block_right);
				else {
					bm_dllappendlast_cs(_block);
					_block = _block_right;
				}
			}
		}
		// else -- out of memory
	}
	else bm_dllremove_cs(_block);

	if (_block) {
		// We've found required block with appropriate BSI
		bm_makebusy(_block, is_clear_block ? (MemManBlockSizeTable[_block->BuddySizeIndex] - MEMMAN_BLOCKOVERHEAD_BUSY) : 0);
		MemManHeapFree -= MemManBlockSizeTable[_block->BuddySizeIndex];
		if (MemManMinimumHeapFree > MemManHeapFree) MemManMinimumHeapFree = MemManHeapFree;
		return _block + MEMMAN_BLOCKOVERHEAD_BUSY;
	}
	else return nullptr;
}
// Frees a block previously allocated by memman_alloc
static void memman_free(void *block) {
	if (block) {
		sMemManBlock *_block = MEMMAN_BLOCKPTR(MEMMAN_BYTESPTR(((uint32_t)block & -4) - MEMMAN_BLOCKOVERHEAD_BUSY));
		// Skip if the block is already free
		if (_block->IsBusy) {
			// Make the freeing block free
			bm_makefree(_block);
			MemManHeapFree += MemManBlockSizeTable[_block->BuddySizeIndex];

			// Trying recursively to merge blocks if they are buddies
			while (1) {
				uint8_t _bsi = _block->BuddySizeIndex;
				if (_block->LeftBuddyCounter) {
					// We've got left buddy block
					//  check weather the next buddy is right, free and its BSI is 1 less than BSI of the left buddy
					sMemManBlock *_block_right = _block->Entity.NextBlock;
					if (!_block_right || _block_right->IsBusy || _block_right->LeftBuddyCounter || _block_right->BuddySizeIndex != (_bsi - 1)) break;
					bm_dllremove_heap(_block_right);
					bm_dllremove_cs(_block_right);
					bm_initfree(_block, _block->LeftBuddyCounter - 1, _bsi + 1);
				}
				else {
					// We've got right buddy block
					//  check weather the previous buddy is left, free and its BSI is 1 greater than BSI of the right buddy
					sMemManBlock *_block_left = _block->Entity.PrevBlock;
					if (!_block_left || _block_left->IsBusy || !_block_left->LeftBuddyCounter || _block_left->BuddySizeIndex != (_bsi + 1)) break;
					bm_dllremove_heap(_block);
					bm_dllremove_cs(_block_left);
					bm_initfree(_block_left, _block_left->LeftBuddyCounter - 1, _bsi + 2);
					_block = _block_left;
				}
			}

			// Add free block to congruous size block table
			bm_dllappendlast_cs(_block);
		}
	}
}

// Initializes buddymem heap and creates initial blocks
void memman_initialize(void *heap_start, memman_size_t heap_size) {
	DBG_PutString("Buddy memory manager initialization"NL);
	DBG_PutFormat(" - sizeof(sMemManBlockFree): %7d"NL, MEMMAN_BLOCKOVERHEAD_FREE);
	DBG_PutFormat(" - sizeof(sMemManBlockBusy): %7d"NL, MEMMAN_BLOCKOVERHEAD_BUSY);
	DBG_PutFormat(" - heap start:      0x%08X (%7d)"NL, heap_start, heap_start);
	DBG_PutFormat(" - heap size:       0x%08X (%7d)"NL, heap_size, heap_size);
	memman_size_t _sz = MEMMAN_BLOCKOVERHEAD_BUSY;
	heap_size -= _sz;
	while (heap_size >= MemManBlockSizeTable[0]) {
		sMemManBlock *_block_new = MEMMAN_BLOCKPTR(heap_start);
		memman_size_t _block_new_size = heap_size;
		uint8_t _bsi_low;
		bm_find_bsi(_block_new_size, &_bsi_low);
		_block_new_size = MemManBlockSizeTable[_bsi_low];
		DBG_PutFormat("  - new block:  0x%08X (%7d)"NL, _block_new_size, _block_new_size);
		bm_initfree(_block_new, 0, _bsi_low);
		bm_dllappendlast_heap(_block_new);
		bm_dllappendlast_cs(_block_new);
		heap_start = ((uint8_t *)heap_start) + _block_new_size;
		heap_size -= _block_new_size;
		MemManHeapFree += _block_new_size;
	}
	MemManMinimumHeapFree = MemManHeapFree;
}
// Returns amount of free bytes available for allocation
memman_size_t memman_get_free() { return MemManHeapFree; }
// Returns amount of minimum free bytes ever remaining since last system boot
memman_size_t memman_get_minimum_free() { return MemManMinimumHeapFree; }
// Returns amount of maximum busy bytes ever allocated since last system boot
memman_size_t memman_get_maximum_busy() { return MemManHeapFree - MemManMinimumHeapFree; }
// Returns amount of maximum free bytes that can be allocated as single block
memman_size_t memman_get_maximum_free_block() {
	uint8_t _bsi = MemMan_ROWS - 1;
	do { if(MemManFreeDllTable[_bsi].FirstBlock) return MemManBlockSizeTable[_bsi]; } while(_bsi--);
	return 0;
}

#endif

void *malloc(size_t size) { return memman_alloc((memman_size_t)size, false); }
void *calloc(size_t number, size_t size) { return memman_alloc((memman_size_t)number * size, true); }
void free(void *ptr) { memman_free(ptr); }

#else

__attribute__ ((noreturn))
static void memman_error_mes() { DBG_PutString("Memory manager is disabled"NL); while(true); }

void *malloc(size_t size) { memman_error_mes(); }
void *calloc(size_t number, size_t size) { memman_error_mes(); }
void free(void *ptr) { memman_error_mes(); }

#endif