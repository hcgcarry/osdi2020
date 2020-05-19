#ifndef __MM_H__
#define __MM_H__

#define KERNEL_VIRT_BASE        0xFFFF000000000000
#define PAGE_SIZE               4096
#define PAGE_FRAMES_NUM         (0x40000000 / PAGE_SIZE)

#ifndef __ASSEMBLY__

#include "typedef.h"
#include "schedule.h"

enum page_status {
    AVAL,
    USED,
};

struct page_t {
    enum page_status used;
};

/* Variables init in mm.c */
extern struct page_t page[PAGE_FRAMES_NUM];

/* Function in mm.c */
void mm_init();
uint64_t virtual_to_physical(uint64_t virt_addr);
uint64_t phy_to_pfn(uint64_t phy_addr);
void* page_alloc();
void* page_alloc_user();
void page_free(void* addr);
void* map_addr_user(uint64_t user_addr);

#endif /* __ASSEMBLY__ */

#endif /* __MM_H__ */