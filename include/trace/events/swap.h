#undef TRACE_SYSTEM
#define TRACE_SYSTEM swap

#if !defined(_TRACE_SWAP_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SWAP_H

#include <linux/tracepoint.h>
#include <linux/types.h>
#include <linux/swap.h>
#include <linux/stacktrace.h>
#include <linux/swapops.h>
#include <linux/mm.h>
#include "../../../mm/vma.h"
TRACE_EVENT(swap_entry_alloc_from_cache,
    TP_PROTO(swp_entry_t swp_entry, swp_entry_t	*slots, 
             int cur, int nr),
    
    TP_ARGS(swp_entry,slots,cur,nr),
    
    TP_STRUCT__entry(
        __field(unsigned long, entry_val)
        __field(swp_entry_t *, slots)
        __field(int, cur)
        __field(int, nr)
    ),
    
    TP_fast_assign(
        __entry->entry_val = swp_entry.val;
        __entry->slots = slots;
        __entry->cur = cur;
        __entry->nr = nr;
    ),
    
    TP_printk("entry=%lx slots=%p cur=%d nr=%d",
              __entry->entry_val, __entry->slots, __entry->cur, __entry->nr)
);
TRACE_EVENT(refill_swap_slots_cache,
    TP_PROTO(swp_entry_t *slots, int nr),
    
    TP_ARGS(slots, nr),
    
    TP_STRUCT__entry(
        __field(swp_entry_t *, slots)
        __field(int, nr)
    ),
    
    TP_fast_assign(
        __entry->slots = slots;
        __entry->nr = nr;
    ),
    
    TP_printk("slots=%p nr=%d", __entry->slots, __entry->nr)
);
TRACE_EVENT(swap_cluster_alloc_current_cluster,
    TP_PROTO(struct swap_info_struct *si, struct swap_cluster_info *ci, unsigned int offset),

    TP_ARGS(si, ci, offset),
    
    TP_STRUCT__entry(
        __field(unsigned int, ci_count)
        __field(unsigned int, ci_flags)
        __field(unsigned int, ci_order)
        __field(unsigned int, si_flags)
        __field(unsigned int, offset)
    ),

    TP_fast_assign(
        __entry->ci_count = ci->count;
        __entry->ci_flags = ci->flags;
        __entry->ci_order = ci->order;
        __entry->si_flags = si->flags;
        __entry->offset = offset;
    ),

    TP_printk("ci_count=%d ci_flags=%x ci_order=%d si_flags=%x offset=%x",
              __entry->ci_count, __entry->ci_flags, __entry->ci_order,
              __entry->si_flags, __entry->offset)
);

TRACE_EVENT(swap_cluster_alloc_new_cluster,
    TP_PROTO(struct swap_info_struct *si, struct swap_cluster_info *ci, unsigned int offset),

    TP_ARGS(si, ci, offset),
    
    TP_STRUCT__entry(
        __field(unsigned int, ci_count)
        __field(unsigned int, ci_flags)
        __field(unsigned int, ci_order)
        __field(unsigned int, si_flags)
        __field(unsigned int, offset)
    ),

    TP_fast_assign(
        __entry->ci_count = ci->count;
        __entry->ci_flags = ci->flags;
        __entry->ci_order = ci->order;
        __entry->si_flags = si->flags;
        __entry->offset = offset;
    ),

    TP_printk("ci_count=%d ci_flags=%x ci_order=%d si_flags=%x offset=%x",
              __entry->ci_count, __entry->ci_flags, __entry->ci_order,
              __entry->si_flags, __entry->offset)
);
TRACE_EVENT(swap_cluster_alloc_nonfull_cluster,
    TP_PROTO(struct swap_info_struct *si, struct swap_cluster_info *ci, unsigned int offset),

    TP_ARGS(si, ci, offset),
    
    TP_STRUCT__entry(
        __field(unsigned int, ci_count)
        __field(unsigned int, ci_flags)
        __field(unsigned int, ci_order)
        __field(unsigned int, si_flags)
        __field(unsigned int, offset)
    ),

    TP_fast_assign(
        __entry->ci_count = ci->count;
        __entry->ci_flags = ci->flags;
        __entry->ci_order = ci->order;
        __entry->si_flags = si->flags;
        __entry->offset = offset;
    ),

    TP_printk("ci_count=%d ci_flags=%x ci_order=%d si_flags=%x offset=%x",
              __entry->ci_count, __entry->ci_flags, __entry->ci_order,
              __entry->si_flags, __entry->offset)
);
TRACE_EVENT(swap_cluster_alloc_frag_cluster,
    TP_PROTO(struct swap_info_struct *si, struct swap_cluster_info *ci, unsigned int offset),

    TP_ARGS(si, ci, offset),
    
    TP_STRUCT__entry(
        __field(unsigned int, ci_count)
        __field(unsigned int, ci_flags)
        __field(unsigned int, ci_order)
        __field(unsigned int, si_flags)
        __field(unsigned int, offset)
    ),

    TP_fast_assign(
        __entry->ci_count = ci->count;
        __entry->ci_flags = ci->flags;
        __entry->ci_order = ci->order;
        __entry->si_flags = si->flags;
        __entry->offset = offset;
    ),

    TP_printk("ci_count=%d ci_flags=%x ci_order=%d si_flags=%x offset=%x",
              __entry->ci_count, __entry->ci_flags, __entry->ci_order,
              __entry->si_flags, __entry->offset)
);
TRACE_EVENT(swap_range_free,
    TP_PROTO(struct swap_info_struct *si, unsigned int offset, unsigned int end),

    TP_ARGS(si, offset, end),
    
    TP_STRUCT__entry(
        __field(unsigned int, si_flags)
        __field(unsigned int, offset)
        __field(unsigned int, end)
        __array(unsigned long, stack_entries, 10)
    ),

    TP_fast_assign(
        __entry->si_flags = si->flags;
        __entry->offset = offset;
        __entry->end = end;

    ),

    TP_printk("si_flags=%x offset=%x end=%x",
              __entry->si_flags, __entry->offset, __entry->end)
);
TRACE_EVENT(swap_partial_free_cluster,
    TP_PROTO(struct swap_info_struct *si, struct swap_cluster_info *ci, unsigned int offset),

    TP_ARGS(si, ci, offset),
    
    TP_STRUCT__entry(
        __field(unsigned int, ci_count)
        __field(unsigned int, ci_flags)
        __field(unsigned int, ci_order)
        __field(unsigned int, si_flags)
        __field(unsigned int, offset)
    ),

    TP_fast_assign(
        __entry->ci_count = ci->count;
        __entry->ci_flags = ci->flags;
        __entry->ci_order = ci->order;
        __entry->si_flags = si->flags;
        __entry->offset = offset;
    ),

    TP_printk("ci_count=%d ci_flags=%x ci_order=%d si_flags=%x offset=%x",
              __entry->ci_count, __entry->ci_flags, __entry->ci_order,
              __entry->si_flags, __entry->offset)
);
TRACE_EVENT(init_swap_cluster_add_to_free,
    TP_PROTO(unsigned int col, unsigned int row, unsigned int offset),

    TP_ARGS(col, row, offset),
    
    TP_STRUCT__entry(
        __field(unsigned int, col)
        __field(unsigned int, row)
        __field(unsigned int, offset)
    ),

    TP_fast_assign(
        __entry->col = col;
        __entry->row = row;
        __entry->offset = offset;
    ),
    TP_printk("col=%d row=%d offset=%x", __entry->col, __entry->row, __entry->offset)

);
TRACE_EVENT(init_swap_cluster,
    TP_PROTO(unsigned int total_clusters, unsigned int columns, unsigned int rows),

    TP_ARGS(total_clusters, columns, rows),
    TP_STRUCT__entry(
        __field(unsigned int, total_clusters)
        __field(unsigned int, columns)
        __field(unsigned int, rows)
    ),
    TP_fast_assign(
        __entry->total_clusters = total_clusters;
        __entry->columns = columns;
        __entry->rows = rows;
    ),
    TP_printk("total_clusters=%d columns=%d rows=%d",
              __entry->total_clusters, __entry->columns, __entry->rows)
);
TRACE_EVENT(should_try_to_free_swap,
    TP_PROTO(bool not_folio_test_swapcache,bool mem_cgroup_full,bool vm_locked,bool folio_test_mlocked,bool fault_flag_write, bool not_folio_test_ksm,bool folio_ref_count_1),
    TP_ARGS(not_folio_test_swapcache,mem_cgroup_full,vm_locked,folio_test_mlocked,fault_flag_write,not_folio_test_ksm,folio_ref_count_1),
    TP_STRUCT__entry(
        __field(bool, not_folio_test_swapcache)
        __field(bool, mem_cgroup_full)
        __field(bool, vm_locked)
        __field(bool, folio_test_mlocked)
        __field(bool, fault_flag_write)
        __field(bool, not_folio_test_ksm)
        __field(bool, folio_ref_count_1)
    ),
    TP_fast_assign(
        __entry->not_folio_test_swapcache = not_folio_test_swapcache;
        __entry->mem_cgroup_full = mem_cgroup_full;
        __entry->vm_locked = vm_locked;
        __entry->folio_test_mlocked = folio_test_mlocked;
        __entry->fault_flag_write = fault_flag_write;
        __entry->not_folio_test_ksm = not_folio_test_ksm;
        __entry->folio_ref_count_1 = folio_ref_count_1;
    ),
    TP_printk("not_folio_test_swapcache=%d mem_cgroup_full=%d vm_locked=%d folio_test_mlocked=%d fault_flag_write=%d not_folio_test_ksm=%d folio_ref_count_1=%d",
              __entry->not_folio_test_swapcache, __entry->mem_cgroup_full, __entry->vm_locked, __entry->folio_test_mlocked, __entry->fault_flag_write, __entry->not_folio_test_ksm, __entry->folio_ref_count_1)
);
TRACE_EVENT(mem_cgroup_swap_full,
TP_PROTO(bool vm_swap_full, unsigned long usage, unsigned long high, unsigned long max),
TP_ARGS(vm_swap_full, usage, high, max),
TP_STRUCT__entry(
    __field(bool, vm_swap_full)
    __field(unsigned long, usage)
    __field(unsigned long, high)
    __field(unsigned long, max)
),
TP_fast_assign(
    __entry->vm_swap_full = vm_swap_full;
    __entry->usage = usage;
    __entry->high = high;
    __entry->max = max;
),
TP_printk("vm_swap_full=%d usage=%lu high=%lu max=%lu",
          __entry->vm_swap_full, __entry->usage, __entry->high, __entry->max)
);
TRACE_EVENT(folio_alloc_swap,
TP_PROTO( struct folio *folio, pgoff_t offset, unsigned long type),
TP_ARGS(folio, offset, type),
TP_STRUCT__entry(
    __field(struct folio *, folio)
    __field(pgoff_t, offset)
    __field(unsigned long, type)
),
TP_fast_assign(
    __entry->folio = folio;
    __entry->offset = offset;
    __entry->type = type;
),
TP_printk("entry for folio folio=%p swp_offset=%ld swp_type=%lx.",
          __entry->folio, __entry->offset, __entry->type)
);
TRACE_EVENT(get_swap_pages,
TP_PROTO( int n_ret, struct swap_info_struct* si, pgoff_t offset, unsigned long type),
TP_ARGS(n_ret, si, offset, type),
TP_STRUCT__entry(
    __field(int, n_ret)
    __field(struct swap_info_struct*, si)
    __field(pgoff_t, offset)    
    __field(unsigned long, type)
),
TP_fast_assign(
    __entry->n_ret = n_ret;
    __entry->si = si;
    __entry->offset = offset;
    __entry->type = type;
),
TP_printk("n_ret=%d si=%p swap_offset=%ld swap_type=%lx.",
          __entry->n_ret, __entry->si, __entry->offset, __entry->type)
);
TRACE_EVENT(get_swap_info_from_folio,
TP_PROTO( struct folio *folio, struct swap_info_struct* si, int index),
TP_ARGS(folio, si, index),
TP_STRUCT__entry(
    __field(struct folio *, folio)
    __field(struct swap_info_struct*, si)
    __field(int, index)
),
TP_fast_assign(
    __entry->folio = folio;
    __entry->si = si;
    __entry->index = index;
),
TP_printk("folio=%p si=%p index=%d.",
          __entry->folio, __entry->si, __entry->index)
);
TRACE_EVENT(vma_get_swap_info,
TP_PROTO(struct vm_area_struct *vma, struct swap_info_struct* si, struct folio *folio, int index, unsigned long address),
TP_ARGS(vma, si, folio, index, address),
TP_STRUCT__entry(
    __field(struct vm_area_struct *, vma)
    __field(struct swap_info_struct*, si)
    __field(struct folio *, folio)
    __field(int, index)
    __field(unsigned long, address)
),
TP_fast_assign(
    __entry->vma = vma;
    __entry->si = si;
    __entry->folio = folio;
    __entry->index = index;
    __entry->address = address;
),
TP_printk("vma=%p si=%p folio=%p index=%d address=%lx.",
          __entry->vma, __entry->si, __entry->folio, __entry->index, __entry->address)
);
TRACE_EVENT(get_swap_index_for_folio,
TP_PROTO(struct vm_area_struct* vma, struct folio *folio, int index, unsigned long address),
TP_ARGS(vma, folio, index, address),
TP_STRUCT__entry(
    __field(struct vm_area_struct*, vma)
    __field(struct folio *, folio)
    __field(int, index)
    __field(unsigned long, address)
),
TP_fast_assign(
    __entry->vma = vma;
    __entry->folio = folio;
    __entry->index = index;
    __entry->address = address;
),
TP_printk("vma=%p folio=%p index=%d address=%lx",
          __entry->vma, __entry->folio, __entry->index, __entry->address)
);
TRACE_EVENT(vma_set_swap_info,
TP_PROTO( struct vm_area_struct *vma, struct swap_info_struct* actual_si,struct swap_info_struct* attempt_si, struct folio *folio, int index, unsigned long address),
TP_ARGS(vma, actual_si, attempt_si, folio, index, address),
TP_STRUCT__entry(
    __field(struct vm_area_struct *, vma)
    __field(struct swap_info_struct*, actual_si)
    __field(struct swap_info_struct*, attempt_si)
    __field(struct folio *, folio)
    __field(int, index)
    __field(unsigned long, address)
),
TP_fast_assign(
    __entry->vma = vma;
    __entry->actual_si = actual_si;
    __entry->attempt_si = attempt_si;
    __entry->folio = folio;
    __entry->index = index;
    __entry->address = address;
),
TP_printk("vma=%p actual_si=%p attempt_si=%p folio=%p index=%d address=%lx.",
          __entry->vma, __entry->actual_si, __entry->attempt_si, __entry->folio, __entry->index, __entry->address)
);
TRACE_EVENT(vma_alloc_range,
TP_PROTO( struct swap_info_struct* si, unsigned long start, unsigned char usage, unsigned int order),
TP_ARGS(si, start, usage, order),
TP_STRUCT__entry(
    __field(struct swap_info_struct*, si)
    __field(unsigned long, start)
    __field(unsigned char, usage)
    __field(unsigned int, order)
),
TP_fast_assign(
    __entry->si = si;
    __entry->start = start;
    __entry->usage = usage;
    __entry->order = order;
),
TP_printk("si=%p start=%lx usage=%x order=%x.",
          __entry->si, __entry->start, __entry->usage, __entry->order)
);
TRACE_EVENT(do_swap_page,
TP_PROTO(struct vm_area_struct *vma, unsigned long address, unsigned long type, pgoff_t offset, struct folio *folio, char is_sync),
TP_ARGS(vma, address, type, offset, folio, is_sync),
TP_STRUCT__entry(
    __field(struct vm_area_struct*, vma)
    __field(unsigned long, address)
    __field(unsigned long, type)
    __field(pgoff_t, offset)
    __field(struct folio *, folio)
    __field(char, is_sync)
),
TP_fast_assign(
    __entry->vma = vma;
    __entry->address = address;
    __entry->type = type;
    __entry->offset = offset;
    __entry->folio = folio;
    __entry->is_sync = is_sync;
),
TP_printk("vma=%p address=%lx type=%lx offset=%ld folio=%p is_sync=%d (0=not sync, 1=sync, 2=cached).",
          __entry->vma, __entry->address, __entry->type, __entry->offset, __entry->folio, __entry->is_sync)
);
TRACE_EVENT(read_swap_cache_async,
TP_PROTO(struct swap_info_struct *si, unsigned long type, unsigned long offset, int is_cached),
TP_ARGS(si, type, offset, is_cached),
TP_STRUCT__entry(
    __field(struct swap_info_struct*, si)
    __field(unsigned long, type)
    __field(unsigned long, offset)
    __field(int, is_cached)
),
TP_fast_assign(
    __entry->si = si;
    __entry->type = type;
    __entry->offset = offset;
    __entry->is_cached = is_cached;

),
TP_printk("si=%p type=%lx offset=%lx is_cached=%d.",
          __entry->si, __entry->type, __entry->offset, __entry->is_cached)
);
TRACE_EVENT(swapin_nr_pages,
TP_PROTO(unsigned int pages, unsigned long offset, unsigned int hits, unsigned int max_pages),
TP_ARGS(pages, offset, hits, max_pages),
TP_STRUCT__entry(
    __field(unsigned int, pages)
    __field(unsigned long, offset)
    __field(unsigned int, hits)
    __field(unsigned int, max_pages)
),
TP_fast_assign(
    __entry->pages = pages;
    __entry->offset = offset;
    __entry->hits = hits;
    __entry->max_pages = max_pages;
),
TP_printk("pages=%d offset=%lx hits=%d max_pages=%d.",
          __entry->pages, __entry->offset, __entry->hits, __entry->max_pages)
);
TRACE_EVENT(swap_cluster_readahead,
TP_PROTO(struct swap_info_struct *si, unsigned long type, unsigned long offset),
TP_ARGS(si, type, offset),
TP_STRUCT__entry(
    __field(struct swap_info_struct*, si)
    __field(unsigned long, type)
    __field(unsigned long, offset)
),
TP_fast_assign(
    __entry->si = si;
    __entry->type = type;
    __entry->offset = offset;
),
TP_printk("si=%p type=%lx offset=%lx.",
          __entry->si, __entry->type, __entry->offset)
);
TRACE_EVENT(swap_vma_readahead,
TP_PROTO(unsigned long type, unsigned long offset, struct vm_fault *vmf),
TP_ARGS(type, offset, vmf),
TP_STRUCT__entry(
    __field(unsigned long, type)
    __field(unsigned long, offset)
    __field(struct vm_fault *, vmf)
),
TP_fast_assign(
    __entry->type = type;
    __entry->offset = offset;
    __entry->vmf = vmf;
),
TP_printk("type=%lx offset=%lx addr=%lx.",
          __entry->type, __entry->offset, __entry->vmf->address)
);
TRACE_EVENT(swap_vma_ra_win,
TP_PROTO(struct vm_fault *vmf, unsigned long start, unsigned long end, unsigned long faddr, unsigned long prev_faddr, unsigned int hits, unsigned int win),
TP_ARGS(vmf, start, end, faddr, prev_faddr, hits, win),
TP_STRUCT__entry(
    __field(struct vm_fault *, vmf)
    __field(unsigned long, start)
    __field(unsigned long, end)
    __field(unsigned long, faddr)
    __field(unsigned long, prev_faddr)
    __field(unsigned int, hits)
    __field(unsigned int, win)
),
TP_fast_assign(
    __entry->vmf = vmf;
    __entry->start = start;
    __entry->end = end;
    __entry->faddr = faddr;
    __entry->prev_faddr = prev_faddr;
    __entry->hits = hits;
    __entry->win = win;
),
TP_printk("vmf=%p start=%lx end=%lx faddr=%lx prev_faddr=%lx hits=%d win=%d.",
          __entry->vmf, __entry->start, __entry->end, __entry->faddr, __entry->prev_faddr, __entry->hits, __entry->win)
);
TRACE_EVENT(commit_merge,
    TP_PROTO(struct vma_merge_struct *vmg),
    TP_ARGS(vmg),
    TP_STRUCT__entry(
        __field(unsigned long, vmg_ptr)
        __field(unsigned long, vma_ptr)
        __field(unsigned long, vma_si_ptr)
        __field(unsigned long, prev_ptr)
        __field(unsigned long, prev_si_ptr)
        __field(unsigned long, next_ptr)
        __field(unsigned long, next_si_ptr)
        __field(unsigned long, start)
        __field(unsigned long, end)
    ),
    TP_fast_assign(
        __entry->vmg_ptr = (unsigned long)vmg;
        __entry->vma_ptr = (unsigned long)(vmg ? vmg->vma : NULL);
        #ifdef CONFIG_SWAP_VMA
        __entry->vma_si_ptr = (unsigned long)(vmg && vmg->vma ? vmg->vma->si : NULL);
        __entry->prev_si_ptr = (unsigned long)(vmg && vmg->prev ? vmg->prev->si : NULL);
        __entry->next_si_ptr = (unsigned long)(vmg && vmg->next ? vmg->next->si : NULL);
        #endif
        __entry->prev_ptr = (unsigned long)(vmg ? vmg->prev : NULL);
        __entry->next_ptr = (unsigned long)(vmg ? vmg->next : NULL);
        __entry->start = vmg ? vmg->start : 0;
        __entry->end = vmg ? vmg->end : 0;
    ),
    #ifdef CONFIG_SWAP_VMA
    TP_printk("vmg=%p vma=%p vma_si=%p prev=%p prev_si=%p next=%p next_si=%p start=%lx end=%lx",
        (void *)__entry->vmg_ptr, (void *)__entry->vma_ptr, (void *)__entry->vma_si_ptr,
        (void *)__entry->prev_ptr, (void *)__entry->prev_si_ptr,
        (void *)__entry->next_ptr, (void *)__entry->next_si_ptr,
        __entry->start, __entry->end)
    #else
    TP_printk("vmg=%p vma=%p prev=%p next=%p start=%lx end=%lx",
        (void *)__entry->vmg_ptr, (void *)__entry->vma_ptr,
        (void *)__entry->prev_ptr, (void *)__entry->next_ptr,
        __entry->start, __entry->end)
    #endif /* CONFIG_SWAP_VMA */
);
TRACE_EVENT(vma_fault, 
    TP_PROTO(struct vm_area_struct *vma, unsigned long faddr, unsigned long old_fadrr, int new_flag, pgoff_t fault_off, pgoff_t window_start, pgoff_t window_end, size_t swap_ahead_size,struct folio* folio, int folio_ref_count),
    TP_ARGS(vma, faddr, old_fadrr, new_flag, fault_off, window_start, window_end, swap_ahead_size, folio, folio_ref_count),
    TP_STRUCT__entry(
        __field(unsigned long, vma)
        __field(unsigned long, faddr)
        __field(unsigned long, old_fadrr)
        __field(int, new_flag)
        __field(pgoff_t, fault_off)
        __field(pgoff_t, window_start)
        __field(pgoff_t, window_end)
        __field(size_t, swap_ahead_size)
        __field(struct folio*, folio)
        __field(int, folio_ref_count)
    ),
    TP_fast_assign(
        __entry->vma = (unsigned long)vma;
        __entry->faddr = faddr;
        __entry->old_fadrr = old_fadrr;
        __entry->new_flag = new_flag;
        __entry->fault_off = fault_off;
        __entry->window_start = window_start;
        __entry->window_end = window_end;
        __entry->swap_ahead_size = swap_ahead_size;
        __entry->folio = folio;
        __entry->folio_ref_count = folio_ref_count;
    ),
    TP_printk("vma=%lx faddr=%lx old_fadrr=%lx new_flag=%d fault_off=%ld window_start=%ld window_end=%ld swap_ahead_size=%zu folio=%p folio_ref_count=%d",
        __entry->vma, __entry->faddr, __entry->old_fadrr, __entry->new_flag, __entry->fault_off, __entry->window_start, __entry->window_end, __entry->swap_ahead_size, __entry->folio, __entry->folio_ref_count)
);
TRACE_EVENT(add_to_swap,
    TP_PROTO(struct folio *folio, unsigned long type, unsigned long offset, int ref_count),
    TP_ARGS(folio, type, offset, ref_count),
    TP_STRUCT__entry(
        __field(struct folio *, folio)  
        __field(unsigned long, type)
        __field(unsigned long, offset)
        __field(int, ref_count)
    ),
    TP_fast_assign(
        __entry->folio = folio;     
        __entry->type = type;
        __entry->offset = offset;
        __entry->ref_count = ref_count;
    ),
    TP_printk("folio=%p type=%lx offset=%lx ref_count=%d.",
        __entry->folio, __entry->type, __entry->offset, __entry->ref_count)
);
TRACE_EVENT(add_to_swap_entry,
    TP_PROTO(struct folio *folio, int ref_count),
    TP_ARGS(folio, ref_count),
    TP_STRUCT__entry(
        __field(struct folio *, folio)  
        __field(int, ref_count)
    ),
    TP_fast_assign(
        __entry->folio = folio;     
        __entry->ref_count = ref_count;
    ),
    TP_printk("folio=%p ref_count=%d.",
        __entry->folio, __entry->ref_count)
);
TRACE_EVENT(vma_fault_early_return,
    TP_PROTO(struct vm_area_struct *vma, unsigned long faddr, char* reason),
    TP_ARGS(vma, faddr, reason),
    TP_STRUCT__entry(
        __field(unsigned long, vma)
        __field(unsigned long, faddr)
        __string(reason, reason)
    ),
    TP_fast_assign(
        __entry->vma = (unsigned long)vma;
        __entry->faddr = faddr;
        __assign_str(reason);
    ),
    TP_printk("vma=%lx faddr=%lx reason=%s",
        __entry->vma, __entry->faddr, __get_str(reason))
);
#endif /* _TRACE_SWAP_H */

/* This part must be outside protection */
#include <trace/define_trace.h>