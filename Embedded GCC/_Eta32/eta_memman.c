/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : eta_memman_fibonachi.c
 ***********************************************************************/

#include "eta_memman.h"
#include "eta_dbg.h"

#ifdef MEMMAN_FIBONACHI

typedef struct _sBMDLLRoot sBMDLLRoot;
typedef struct _sBMDLLEntity sBMDLLEntity;
typedef struct _sBMBlock sBMBlock;

struct _sBMDLLRoot {
	sBMBlock *FirstBlock;
	sBMBlock *LastBlock;
};
struct _sBMDLLEntity {
	sBMBlock *PrevBlock;
	sBMBlock *NextBlock;
};
struct _sBMBlock {
	sBMDLLEntity HeapDllEntity;
	struct {
		uint16_t LeftBuddyCounter;
		uint8_t BuddySizeIndex;
		bool IsBusy;
	} __attribute((packed));
	union {
		struct {
			sBMDLLEntity CongruousDllEntity;
		} Free;
		__extension__ struct {
			// here might be some fields of busy block for special purposes
		} Busy;
	};
};

static uint16_t const BMBlockSizeTable[] = {
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
};

#define BM_ROWS sizeof_array(BMBlockSizeTable)										// number of rows in free block DLL table
#define BM_BMBLOCKPTR(ptr) ((sBMBlock *)ptr)
#define BM_BLOCKOVERHEAD_FREE ((uint16_t)((uint32_t)(&BM_BMBLOCKPTR(0)->Free) + (uint32_t)(sizeof(BM_BMBLOCKPTR(0)->Free))))	// overhead of free block
#define BM_BLOCKOVERHEAD_BUSY ((uint16_t)((uint32_t)(&BM_BMBLOCKPTR(0)->Busy) + (uint32_t)(sizeof(BM_BMBLOCKPTR(0)->Busy))))	// overhead of busy block

static sBMDLLRoot BMHeapDll;				// main heap double linked list
static sBMDLLRoot BMFreeDllTable[BM_ROWS];// double linked lists of congruous size block table
static uint16_t BMHeapFree, BMMinimumHeapFree;

// Inits block as free
static void bm_initfree(sBMBlock *block, uint16_t lbc, uint8_t bsi) {
	block->LeftBuddyCounter = lbc;
	block->BuddySizeIndex = bsi;
	block->IsBusy = false;
}
// Makes the block free
static void bm_makefree(sBMBlock *block) {
	block->IsBusy = false;
}
// Makes the block busy
static void bm_makebusy(sBMBlock *block) {
	block->IsBusy = true;
	memset(((uint8_t *)block) + BM_BLOCKOVERHEAD_BUSY, 0, BMBlockSizeTable[block->BuddySizeIndex] - BM_BLOCKOVERHEAD_BUSY);
}
// Searches the nearest small size that is bigger or equal to param 'size'
static unsigned char bm_find_bsi(uint16_t size, uint8_t *lowbsi) {
	uint8_t _lowbsi = 0;
	uint8_t _highbsi = BM_ROWS - 1;
	if (size <= BMBlockSizeTable[0]) {
		if (lowbsi) *lowbsi = 0;
		return 0;
	}

	// Binary search for the nearest low and high BSI of the requested size
	while (_lowbsi <= _highbsi) {
		uint8_t _midbsi = (_lowbsi + _highbsi) >> 1;
		if (BMBlockSizeTable[_midbsi] > size) {
			_highbsi = _midbsi - 1;
		}
		else {
			_lowbsi = _midbsi + 1;
		}
	}
	_lowbsi = _highbsi;
	if (BMBlockSizeTable[_lowbsi] < size) _highbsi++;
	if (lowbsi) *lowbsi = _lowbsi;

	return _highbsi;
}

// Appends a new block as the last in heap block table
__inline static void bm_dllappendlast_heap(sBMBlock *block_new) {
	if (BMHeapDll.LastBlock) {
		block_new->HeapDllEntity.PrevBlock = BMHeapDll.LastBlock;
		block_new->HeapDllEntity.NextBlock = nullptr;
		BMHeapDll.LastBlock->HeapDllEntity.NextBlock = block_new;
		BMHeapDll.LastBlock = block_new;
	}
	else {
		block_new->HeapDllEntity.PrevBlock = block_new->HeapDllEntity.NextBlock = nullptr;
		BMHeapDll.FirstBlock = BMHeapDll.LastBlock = block_new;
	}
}
// Appends a new block as the last in congruous size block table
__inline static void bm_dllappendlast_cs(sBMBlock *block_new) {
	sBMDLLRoot *_dll_cs = &BMFreeDllTable[block_new->BuddySizeIndex];
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

// Appends a new block after the specified one in heap block table
__inline static void bm_dllappendafter_heap(sBMBlock *block, sBMBlock *block_new) {
	block_new->HeapDllEntity.PrevBlock = block;
	block_new->HeapDllEntity.NextBlock = block->HeapDllEntity.NextBlock;
	if (block->HeapDllEntity.NextBlock) {
		block->HeapDllEntity.NextBlock->HeapDllEntity.PrevBlock = block_new;
	}
	block->HeapDllEntity.NextBlock = block_new;
}

// Deletes the specified block from the heap block table
__inline static void bm_dllremove_heap(sBMBlock *block) {
	if (block->HeapDllEntity.PrevBlock) {
		block->HeapDllEntity.PrevBlock->HeapDllEntity.NextBlock = block->HeapDllEntity.NextBlock;
	}
	else BMHeapDll.FirstBlock = block->HeapDllEntity.NextBlock;
	if (block->HeapDllEntity.NextBlock) {
		block->HeapDllEntity.NextBlock->HeapDllEntity.PrevBlock = block->HeapDllEntity.PrevBlock;
	}
	else BMHeapDll.LastBlock = block->HeapDllEntity.PrevBlock;
}
// Deletes the specified block from the congruous size block table
__inline static void bm_dllremove_cs(sBMBlock *block) {
	sBMDLLRoot *_dll_cs = &BMFreeDllTable[block->BuddySizeIndex];
	if (block->Free.CongruousDllEntity.PrevBlock) {
		block->Free.CongruousDllEntity.PrevBlock->Free.CongruousDllEntity.NextBlock = block->Free.CongruousDllEntity.NextBlock;
	}
	else _dll_cs->FirstBlock = block->Free.CongruousDllEntity.NextBlock;

	if (block->Free.CongruousDllEntity.NextBlock) {
		block->Free.CongruousDllEntity.NextBlock->Free.CongruousDllEntity.PrevBlock = block->Free.CongruousDllEntity.PrevBlock;
	}
	else _dll_cs->LastBlock = block->Free.CongruousDllEntity.PrevBlock;
}

// Allocates a block in memory heap
static void *memman_alloc(uint16_t size) {
	void *_block_ptr = nullptr;
	// find minimum BSI for the size + overhead
	size += BM_BLOCKOVERHEAD_BUSY;
	uint8_t _bsi_need = bm_find_bsi(size, nullptr);

	// Table lookup
	sBMBlock *_block = BMFreeDllTable[_bsi_need].FirstBlock;
	if (!_block) {
		// No free block with requested BSI. Try to find a free one with higher BSI and divide it into buddies
		uint8_t _bsi_higher = _bsi_need;
		while (++_bsi_higher < BM_ROWS && !_block) {
			_block = BMFreeDllTable[_bsi_higher].FirstBlock;
		}

		if (_block) {
			// Divide larger block into buddies until a block with appropriate BSI is found
			bm_dllremove_cs(_block);
			while (_block->BuddySizeIndex > _bsi_need && _block->BuddySizeIndex > 1) {
				_block->BuddySizeIndex--;
				_block->LeftBuddyCounter++;
				// Create right buddy
				uint8_t _bsi_right = _block->BuddySizeIndex;
				sBMBlock *_block_right = (sBMBlock *) ((void *) (((uint8_t *) _block) + BMBlockSizeTable[_bsi_right--]));
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
		bm_makebusy(_block);
		_block_ptr = (uint8_t *) _block + BM_BLOCKOVERHEAD_BUSY;
		BMHeapFree -= BMBlockSizeTable[_block->BuddySizeIndex];
		if (BMMinimumHeapFree > BMHeapFree) BMMinimumHeapFree = BMHeapFree;
	}

	return _block_ptr;
}
// Frees a block previously allocated by memman_alloc
static void memman_free(void *block) {
	if (block) {
		sBMBlock *_block = (sBMBlock *) ((void *) (((int32_t) block & -4) - BM_BLOCKOVERHEAD_BUSY));
		// Skip if the block is already free
		if (_block->IsBusy) {
			// Make the freeing block free
			bm_makefree(_block);
			BMHeapFree += BMBlockSizeTable[_block->BuddySizeIndex];

			// Trying recursively to merge blocks if they are buddies
			while (1) {
				uint8_t _bsi = _block->BuddySizeIndex;
				if (_block->LeftBuddyCounter) {
					// We've got left buddy block
					//  check weather the next buddy is right, free and its BSI is 1 less than BSI of the left buddy
					sBMBlock *_block_right = _block->HeapDllEntity.NextBlock;
					if (!_block_right || _block_right->IsBusy || _block_right->LeftBuddyCounter || _block_right->BuddySizeIndex != (_bsi - 1)) break;
					bm_dllremove_heap(_block_right);
					bm_dllremove_cs(_block_right);
					bm_initfree(_block, _block->LeftBuddyCounter - 1, _bsi + 1);
				}
				else {
					// We've got right buddy block
					//  check weather the previous buddy is left, free and its BSI is 1 greater than BSI of the right buddy
					sBMBlock *_block_left = _block->HeapDllEntity.PrevBlock;
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
void memman_initialize(void *heap_start, uint16_t heap_size) {
	DBG_PutString("Buddy memory manager initialization"NL);
	DBG_PutFormat(" - sizeof(sBMBlockFree): %7d"NL, BM_BLOCKOVERHEAD_FREE);
	DBG_PutFormat(" - sizeof(sBMBlockBusy): %7d"NL, BM_BLOCKOVERHEAD_BUSY);
	DBG_PutFormat(" - heap start:      0x%08X (%7d)"NL, heap_start, heap_start);
	DBG_PutFormat(" - heap size:       0x%08X (%7d)"NL, heap_size, heap_size);
	uint16_t _sz = BM_BLOCKOVERHEAD_BUSY;
	heap_size -= _sz;
	while (heap_size >= BMBlockSizeTable[0]) {
		sBMBlock *_block_new = (sBMBlock *) heap_start;
		uint16_t _block_new_size = heap_size;
		uint8_t _bsi_low;
		bm_find_bsi(_block_new_size, &_bsi_low);
		_block_new_size = BMBlockSizeTable[_bsi_low];
		DBG_PutFormat("  - new block:  0x%08X (%7d)"NL, _block_new_size, _block_new_size);
		bm_initfree(_block_new, 0, _bsi_low);
		bm_dllappendlast_heap(_block_new);
		bm_dllappendlast_cs(_block_new);
		heap_start = ((uint8_t *)heap_start) + _block_new_size;
		heap_size -= _block_new_size;
		BMHeapFree += _block_new_size;
	}
	BMMinimumHeapFree = BMHeapFree;
}
// Returns amount of free bytes available for allocation
uint16_t memman_get_free() { return BMHeapFree; }
// Returns amount of minimum free bytes ever remaining since last system boot
uint16_t memman_get_minimum_free() { return BMMinimumHeapFree; }
// Returns amount of maximum busy bytes ever allocated since last system boot
uint16_t memman_get_maximum_busy() { return BMHeapFree - BMMinimumHeapFree; }
// Returns amount of maximum free bytes that can be allocated as single block
uint16_t memman_get_maximum_free_block() {
	uint8_t _bsi = BM_ROWS - 1;
	do { if(BMFreeDllTable[_bsi].FirstBlock) return BMBlockSizeTable[_bsi]; } while(_bsi--);
	return 0;
}

void *malloc(size_t size) { return memman_alloc((uint16_t) size); }
void free(void *ptr) { memman_free(ptr); }

#else

__attribute__ ((noreturn))
static void memman_error_mes() { DBG_PutString("Memory manager is disabled"NL); while(true); }

void *malloc(size_t size) { memman_error_mes(); }
void free(void *ptr) { memman_error_mes(); }

#endif