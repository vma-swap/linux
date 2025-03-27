#undef TRACE_SYSTEM
#define TRACE_SYSTEM swap

#if !defined(_TRACE_SWAP_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_SWAP_H

#include <linux/tracepoint.h>
#include <linux/types.h>
#include <linux/swap.h>
#include <linux/stacktrace.h>

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
TRACE_EVENT(swap_free_cluster,
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
           /* Capture the stack trace */
    ),

    TP_printk("ci_count=%d ci_flags=%x ci_order=%d si_flags=%x offset=%x",
    __entry->ci_count, __entry->ci_flags, __entry->ci_order)
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
#endif /* _TRACE_SWAP_H */

/* This part must be outside protection */
#include <trace/define_trace.h>