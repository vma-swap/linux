/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM vmscan

#if !defined(_TRACE_VMSCAN_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_VMSCAN_H

#include <linux/types.h>
#include <linux/tracepoint.h>
#include <linux/mm.h>
#include <linux/memcontrol.h>
#include <trace/events/mmflags.h>

#define RECLAIM_WB_ANON		0x0001u
#define RECLAIM_WB_FILE		0x0002u
#define RECLAIM_WB_MIXED	0x0010u
#define RECLAIM_WB_SYNC		0x0004u /* Unused, all reclaim async */
#define RECLAIM_WB_ASYNC	0x0008u
#define RECLAIM_WB_LRU		(RECLAIM_WB_ANON|RECLAIM_WB_FILE)

#define show_reclaim_flags(flags)				\
	(flags) ? __print_flags(flags, "|",			\
		{RECLAIM_WB_ANON,	"RECLAIM_WB_ANON"},	\
		{RECLAIM_WB_FILE,	"RECLAIM_WB_FILE"},	\
		{RECLAIM_WB_MIXED,	"RECLAIM_WB_MIXED"},	\
		{RECLAIM_WB_SYNC,	"RECLAIM_WB_SYNC"},	\
		{RECLAIM_WB_ASYNC,	"RECLAIM_WB_ASYNC"}	\
		) : "RECLAIM_WB_NONE"

#define _VMSCAN_THROTTLE_WRITEBACK	(1 << VMSCAN_THROTTLE_WRITEBACK)
#define _VMSCAN_THROTTLE_ISOLATED	(1 << VMSCAN_THROTTLE_ISOLATED)
#define _VMSCAN_THROTTLE_NOPROGRESS	(1 << VMSCAN_THROTTLE_NOPROGRESS)
#define _VMSCAN_THROTTLE_CONGESTED	(1 << VMSCAN_THROTTLE_CONGESTED)

#define show_throttle_flags(flags)						\
	(flags) ? __print_flags(flags, "|",					\
		{_VMSCAN_THROTTLE_WRITEBACK,	"VMSCAN_THROTTLE_WRITEBACK"},	\
		{_VMSCAN_THROTTLE_ISOLATED,	"VMSCAN_THROTTLE_ISOLATED"},	\
		{_VMSCAN_THROTTLE_NOPROGRESS,	"VMSCAN_THROTTLE_NOPROGRESS"},	\
		{_VMSCAN_THROTTLE_CONGESTED,	"VMSCAN_THROTTLE_CONGESTED"}	\
		) : "VMSCAN_THROTTLE_NONE"


#define trace_reclaim_flags(file) ( \
	(file ? RECLAIM_WB_FILE : RECLAIM_WB_ANON) | \
	(RECLAIM_WB_ASYNC) \
	)

TRACE_EVENT(mm_vmscan_kswapd_sleep,

	TP_PROTO(int nid),

	TP_ARGS(nid),

	TP_STRUCT__entry(
		__field(	int,	nid	)
	),

	TP_fast_assign(
		__entry->nid	= nid;
	),

	TP_printk("nid=%d", __entry->nid)
);

TRACE_EVENT(mm_vmscan_kswapd_wake,

	TP_PROTO(int nid, int zid, int order),

	TP_ARGS(nid, zid, order),

	TP_STRUCT__entry(
		__field(	int,	nid	)
		__field(	int,	zid	)
		__field(	int,	order	)
	),

	TP_fast_assign(
		__entry->nid	= nid;
		__entry->zid    = zid;
		__entry->order	= order;
	),

	TP_printk("nid=%d order=%d",
		__entry->nid,
		__entry->order)
);

TRACE_EVENT(mm_vmscan_wakeup_kswapd,

	TP_PROTO(int nid, int zid, int order, gfp_t gfp_flags),

	TP_ARGS(nid, zid, order, gfp_flags),

	TP_STRUCT__entry(
		__field(	int,	nid		)
		__field(	int,	zid		)
		__field(	int,	order		)
		__field(	unsigned long,	gfp_flags	)
	),

	TP_fast_assign(
		__entry->nid		= nid;
		__entry->zid		= zid;
		__entry->order		= order;
		__entry->gfp_flags	= (__force unsigned long)gfp_flags;
	),

	TP_printk("nid=%d order=%d gfp_flags=%s",
		__entry->nid,
		__entry->order,
		show_gfp_flags(__entry->gfp_flags))
);

DECLARE_EVENT_CLASS(mm_vmscan_direct_reclaim_begin_template,

	TP_PROTO(int order, gfp_t gfp_flags),

	TP_ARGS(order, gfp_flags),

	TP_STRUCT__entry(
		__field(	int,	order		)
		__field(	unsigned long,	gfp_flags	)
	),

	TP_fast_assign(
		__entry->order		= order;
		__entry->gfp_flags	= (__force unsigned long)gfp_flags;
	),

	TP_printk("order=%d gfp_flags=%s",
		__entry->order,
		show_gfp_flags(__entry->gfp_flags))
);

DEFINE_EVENT(mm_vmscan_direct_reclaim_begin_template, mm_vmscan_direct_reclaim_begin,

	TP_PROTO(int order, gfp_t gfp_flags),

	TP_ARGS(order, gfp_flags)
);

#ifdef CONFIG_MEMCG
DEFINE_EVENT(mm_vmscan_direct_reclaim_begin_template, mm_vmscan_memcg_reclaim_begin,

	TP_PROTO(int order, gfp_t gfp_flags),

	TP_ARGS(order, gfp_flags)
);

DEFINE_EVENT(mm_vmscan_direct_reclaim_begin_template, mm_vmscan_memcg_softlimit_reclaim_begin,

	TP_PROTO(int order, gfp_t gfp_flags),

	TP_ARGS(order, gfp_flags)
);
#endif /* CONFIG_MEMCG */

DECLARE_EVENT_CLASS(mm_vmscan_direct_reclaim_end_template,

	TP_PROTO(unsigned long nr_reclaimed),

	TP_ARGS(nr_reclaimed),

	TP_STRUCT__entry(
		__field(	unsigned long,	nr_reclaimed	)
	),

	TP_fast_assign(
		__entry->nr_reclaimed	= nr_reclaimed;
	),

	TP_printk("nr_reclaimed=%lu", __entry->nr_reclaimed)
);

DEFINE_EVENT(mm_vmscan_direct_reclaim_end_template, mm_vmscan_direct_reclaim_end,

	TP_PROTO(unsigned long nr_reclaimed),

	TP_ARGS(nr_reclaimed)
);

#ifdef CONFIG_MEMCG
DEFINE_EVENT(mm_vmscan_direct_reclaim_end_template, mm_vmscan_memcg_reclaim_end,

	TP_PROTO(unsigned long nr_reclaimed),

	TP_ARGS(nr_reclaimed)
);

DEFINE_EVENT(mm_vmscan_direct_reclaim_end_template, mm_vmscan_memcg_softlimit_reclaim_end,

	TP_PROTO(unsigned long nr_reclaimed),

	TP_ARGS(nr_reclaimed)
);
#endif /* CONFIG_MEMCG */

TRACE_EVENT(mm_shrink_slab_start,
	TP_PROTO(struct shrinker *shr, struct shrink_control *sc,
		long nr_objects_to_shrink, unsigned long cache_items,
		unsigned long long delta, unsigned long total_scan,
		int priority),

	TP_ARGS(shr, sc, nr_objects_to_shrink, cache_items, delta, total_scan,
		priority),

	TP_STRUCT__entry(
		__field(struct shrinker *, shr)
		__field(void *, shrink)
		__field(int, nid)
		__field(long, nr_objects_to_shrink)
		__field(unsigned long, gfp_flags)
		__field(unsigned long, cache_items)
		__field(unsigned long long, delta)
		__field(unsigned long, total_scan)
		__field(int, priority)
	),

	TP_fast_assign(
		__entry->shr = shr;
		__entry->shrink = shr->scan_objects;
		__entry->nid = sc->nid;
		__entry->nr_objects_to_shrink = nr_objects_to_shrink;
		__entry->gfp_flags = (__force unsigned long)sc->gfp_mask;
		__entry->cache_items = cache_items;
		__entry->delta = delta;
		__entry->total_scan = total_scan;
		__entry->priority = priority;
	),

	TP_printk("%pS %p: nid: %d objects to shrink %ld gfp_flags %s cache items %ld delta %lld total_scan %ld priority %d",
		__entry->shrink,
		__entry->shr,
		__entry->nid,
		__entry->nr_objects_to_shrink,
		show_gfp_flags(__entry->gfp_flags),
		__entry->cache_items,
		__entry->delta,
		__entry->total_scan,
		__entry->priority)
);

TRACE_EVENT(mm_shrink_slab_end,
	TP_PROTO(struct shrinker *shr, int nid, int shrinker_retval,
		long unused_scan_cnt, long new_scan_cnt, long total_scan),

	TP_ARGS(shr, nid, shrinker_retval, unused_scan_cnt, new_scan_cnt,
		total_scan),

	TP_STRUCT__entry(
		__field(struct shrinker *, shr)
		__field(int, nid)
		__field(void *, shrink)
		__field(long, unused_scan)
		__field(long, new_scan)
		__field(int, retval)
		__field(long, total_scan)
	),

	TP_fast_assign(
		__entry->shr = shr;
		__entry->nid = nid;
		__entry->shrink = shr->scan_objects;
		__entry->unused_scan = unused_scan_cnt;
		__entry->new_scan = new_scan_cnt;
		__entry->retval = shrinker_retval;
		__entry->total_scan = total_scan;
	),

	TP_printk("%pS %p: nid: %d unused scan count %ld new scan count %ld total_scan %ld last shrinker return val %d",
		__entry->shrink,
		__entry->shr,
		__entry->nid,
		__entry->unused_scan,
		__entry->new_scan,
		__entry->total_scan,
		__entry->retval)
);

TRACE_EVENT(mm_vmscan_lru_isolate,
	TP_PROTO(int highest_zoneidx,
		int order,
		unsigned long nr_requested,
		unsigned long nr_scanned,
		unsigned long nr_skipped,
		unsigned long nr_taken,
		int lru),

	TP_ARGS(highest_zoneidx, order, nr_requested, nr_scanned, nr_skipped, nr_taken, lru),

	TP_STRUCT__entry(
		__field(int, highest_zoneidx)
		__field(int, order)
		__field(unsigned long, nr_requested)
		__field(unsigned long, nr_scanned)
		__field(unsigned long, nr_skipped)
		__field(unsigned long, nr_taken)
		__field(int, lru)
	),

	TP_fast_assign(
		__entry->highest_zoneidx = highest_zoneidx;
		__entry->order = order;
		__entry->nr_requested = nr_requested;
		__entry->nr_scanned = nr_scanned;
		__entry->nr_skipped = nr_skipped;
		__entry->nr_taken = nr_taken;
		__entry->lru = lru;
	),

	/*
	 * classzone is previous name of the highest_zoneidx.
	 * Reason not to change it is the ABI requirement of the tracepoint.
	 */
	TP_printk("classzone=%d order=%d nr_requested=%lu nr_scanned=%lu nr_skipped=%lu nr_taken=%lu lru=%s",
		__entry->highest_zoneidx,
		__entry->order,
		__entry->nr_requested,
		__entry->nr_scanned,
		__entry->nr_skipped,
		__entry->nr_taken,
		__print_symbolic(__entry->lru, LRU_NAMES))
);

TRACE_EVENT(mm_vmscan_write_folio,

	TP_PROTO(struct folio *folio),

	TP_ARGS(folio),

	TP_STRUCT__entry(
		__field(unsigned long, pfn)
		__field(int, reclaim_flags)
	),

	TP_fast_assign(
		__entry->pfn = folio_pfn(folio);
		__entry->reclaim_flags = trace_reclaim_flags(
						folio_is_file_lru(folio));
	),

	TP_printk("page=%p pfn=0x%lx flags=%s",
		pfn_to_page(__entry->pfn),
		__entry->pfn,
		show_reclaim_flags(__entry->reclaim_flags))
);

TRACE_EVENT(mm_vmscan_reclaim_pages,

	TP_PROTO(int nid,
		unsigned long nr_scanned, unsigned long nr_reclaimed,
		struct reclaim_stat *stat),

	TP_ARGS(nid, nr_scanned, nr_reclaimed, stat),

	TP_STRUCT__entry(
		__field(int, nid)
		__field(unsigned long, nr_scanned)
		__field(unsigned long, nr_reclaimed)
		__field(unsigned long, nr_dirty)
		__field(unsigned long, nr_writeback)
		__field(unsigned long, nr_congested)
		__field(unsigned long, nr_immediate)
		__field(unsigned int, nr_activate0)
		__field(unsigned int, nr_activate1)
		__field(unsigned long, nr_ref_keep)
		__field(unsigned long, nr_unmap_fail)
	),

	TP_fast_assign(
		__entry->nid = nid;
		__entry->nr_scanned = nr_scanned;
		__entry->nr_reclaimed = nr_reclaimed;
		__entry->nr_dirty = stat->nr_dirty;
		__entry->nr_writeback = stat->nr_writeback;
		__entry->nr_congested = stat->nr_congested;
		__entry->nr_immediate = stat->nr_immediate;
		__entry->nr_activate0 = stat->nr_activate[0];
		__entry->nr_activate1 = stat->nr_activate[1];
		__entry->nr_ref_keep = stat->nr_ref_keep;
		__entry->nr_unmap_fail = stat->nr_unmap_fail;
	),

	TP_printk("nid=%d nr_scanned=%ld nr_reclaimed=%ld nr_dirty=%ld nr_writeback=%ld nr_congested=%ld nr_immediate=%ld nr_activate_anon=%d nr_activate_file=%d nr_ref_keep=%ld nr_unmap_fail=%ld",
		__entry->nid,
		__entry->nr_scanned, __entry->nr_reclaimed,
		__entry->nr_dirty, __entry->nr_writeback,
		__entry->nr_congested, __entry->nr_immediate,
		__entry->nr_activate0, __entry->nr_activate1,
		__entry->nr_ref_keep, __entry->nr_unmap_fail)
);

TRACE_EVENT(mm_vmscan_lru_shrink_inactive,

	TP_PROTO(int nid,
		unsigned long nr_scanned, unsigned long nr_reclaimed,
		struct reclaim_stat *stat, int priority, int file),

	TP_ARGS(nid, nr_scanned, nr_reclaimed, stat, priority, file),

	TP_STRUCT__entry(
		__field(int, nid)
		__field(unsigned long, nr_scanned)
		__field(unsigned long, nr_reclaimed)
		__field(unsigned long, nr_dirty)
		__field(unsigned long, nr_writeback)
		__field(unsigned long, nr_congested)
		__field(unsigned long, nr_immediate)
		__field(unsigned int, nr_activate0)
		__field(unsigned int, nr_activate1)
		__field(unsigned long, nr_ref_keep)
		__field(unsigned long, nr_unmap_fail)
		__field(int, priority)
		__field(int, reclaim_flags)
	),

	TP_fast_assign(
		__entry->nid = nid;
		__entry->nr_scanned = nr_scanned;
		__entry->nr_reclaimed = nr_reclaimed;
		__entry->nr_dirty = stat->nr_dirty;
		__entry->nr_writeback = stat->nr_writeback;
		__entry->nr_congested = stat->nr_congested;
		__entry->nr_immediate = stat->nr_immediate;
		__entry->nr_activate0 = stat->nr_activate[0];
		__entry->nr_activate1 = stat->nr_activate[1];
		__entry->nr_ref_keep = stat->nr_ref_keep;
		__entry->nr_unmap_fail = stat->nr_unmap_fail;
		__entry->priority = priority;
		__entry->reclaim_flags = trace_reclaim_flags(file);
	),

	TP_printk("nid=%d nr_scanned=%ld nr_reclaimed=%ld nr_dirty=%ld nr_writeback=%ld nr_congested=%ld nr_immediate=%ld nr_activate_anon=%d nr_activate_file=%d nr_ref_keep=%ld nr_unmap_fail=%ld priority=%d flags=%s",
		__entry->nid,
		__entry->nr_scanned, __entry->nr_reclaimed,
		__entry->nr_dirty, __entry->nr_writeback,
		__entry->nr_congested, __entry->nr_immediate,
		__entry->nr_activate0, __entry->nr_activate1,
		__entry->nr_ref_keep, __entry->nr_unmap_fail,
		__entry->priority,
		show_reclaim_flags(__entry->reclaim_flags))
);

TRACE_EVENT(mm_vmscan_lru_shrink_active,

	TP_PROTO(int nid, unsigned long nr_taken,
		unsigned long nr_active, unsigned long nr_deactivated,
		unsigned long nr_referenced, int priority, int file),

	TP_ARGS(nid, nr_taken, nr_active, nr_deactivated, nr_referenced, priority, file),

	TP_STRUCT__entry(
		__field(int, nid)
		__field(unsigned long, nr_taken)
		__field(unsigned long, nr_active)
		__field(unsigned long, nr_deactivated)
		__field(unsigned long, nr_referenced)
		__field(int, priority)
		__field(int, reclaim_flags)
	),

	TP_fast_assign(
		__entry->nid = nid;
		__entry->nr_taken = nr_taken;
		__entry->nr_active = nr_active;
		__entry->nr_deactivated = nr_deactivated;
		__entry->nr_referenced = nr_referenced;
		__entry->priority = priority;
		__entry->reclaim_flags = trace_reclaim_flags(file);
	),

	TP_printk("nid=%d nr_taken=%ld nr_active=%ld nr_deactivated=%ld nr_referenced=%ld priority=%d flags=%s",
		__entry->nid,
		__entry->nr_taken,
		__entry->nr_active, __entry->nr_deactivated, __entry->nr_referenced,
		__entry->priority,
		show_reclaim_flags(__entry->reclaim_flags))
);

TRACE_EVENT(mm_vmscan_node_reclaim_begin,

	TP_PROTO(int nid, int order, gfp_t gfp_flags),

	TP_ARGS(nid, order, gfp_flags),

	TP_STRUCT__entry(
		__field(int, nid)
		__field(int, order)
		__field(unsigned long, gfp_flags)
	),

	TP_fast_assign(
		__entry->nid = nid;
		__entry->order = order;
		__entry->gfp_flags = (__force unsigned long)gfp_flags;
	),

	TP_printk("nid=%d order=%d gfp_flags=%s",
		__entry->nid,
		__entry->order,
		show_gfp_flags(__entry->gfp_flags))
);

DEFINE_EVENT(mm_vmscan_direct_reclaim_end_template, mm_vmscan_node_reclaim_end,

	TP_PROTO(unsigned long nr_reclaimed),

	TP_ARGS(nr_reclaimed)
);

TRACE_EVENT(mm_vmscan_throttled,

	TP_PROTO(int nid, int usec_timeout, int usec_delayed, int reason),

	TP_ARGS(nid, usec_timeout, usec_delayed, reason),

	TP_STRUCT__entry(
		__field(int, nid)
		__field(int, usec_timeout)
		__field(int, usec_delayed)
		__field(int, reason)
	),

	TP_fast_assign(
		__entry->nid = nid;
		__entry->usec_timeout = usec_timeout;
		__entry->usec_delayed = usec_delayed;
		__entry->reason = 1U << reason;
	),

	TP_printk("nid=%d usec_timeout=%d usect_delayed=%d reason=%s",
		__entry->nid,
		__entry->usec_timeout,
		__entry->usec_delayed,
		show_throttle_flags(__entry->reason))
);
TRACE_EVENT(mm_vmscan_isolate_folio,

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
	TP_printk("folio=%p ref_count=%d",
		__entry->folio,
		__entry->ref_count
	)
);
#ifdef CONFIG_VMA_RECLAIM
TRACE_EVENT(mm_vmscan_get_next_folio,
	TP_PROTO(struct folio *folio, unsigned long addr, struct vm_area_struct *vma, int is_anon, int is_file, int has_mapping),
	TP_ARGS(folio, addr, vma, is_anon, is_file, has_mapping),
	TP_STRUCT__entry(
		    __field(struct folio *, folio)
		    __field(unsigned long, addr)
		    __field(struct vm_area_struct *, vma)
			__field(int, is_anon)
			__field(int, is_file)
			__field(int, has_mapping)
	),
	TP_fast_assign(
		__entry->folio = folio;
		__entry->addr = addr;
		__entry->vma = vma;
		__entry->is_anon = is_anon;
		__entry->is_file = is_file;
		__entry->has_mapping = has_mapping;
	),
	TP_printk("folio=%p addr=0x%lx vma=%p is_anon=%d is_file=%d has_mapping=%d",
		__entry->folio,
		__entry->addr,
		__entry->vma,
		__entry->is_anon,
		__entry->is_file,
		__entry->has_mapping)
);
TRACE_EVENT(mm_vmscan_get_prev_folio,
	TP_PROTO(struct folio *folio, unsigned long addr, struct vm_area_struct *vma, int is_anon, int is_file, int has_mapping),
	TP_ARGS(folio, addr, vma, is_anon, is_file, has_mapping),
	TP_STRUCT__entry(
		    __field(struct folio *, folio)
		    __field(unsigned long, addr)
		    __field(struct vm_area_struct *, vma)
			__field(int, is_anon)
			__field(int, is_file)
			__field(int, has_mapping)
	),
	TP_fast_assign(
		__entry->folio = folio;
		__entry->addr = addr;
		__entry->vma = vma;
		__entry->is_anon = is_anon;
		__entry->is_file = is_file;
		__entry->has_mapping = has_mapping;
	),
	TP_printk("folio=%p addr=0x%lx vma=%p is_anon=%d is_file=%d has_mapping=%d",
		__entry->folio,
		__entry->addr,
		__entry->vma,
		__entry->is_anon,
		__entry->is_file,
		__entry->has_mapping)
);
TRACE_EVENT(mm_vmscan_get_first_folio_in_seq,
	TP_PROTO(struct folio *folio, unsigned long first_addr, bool do_break, unsigned long go_back_size, unsigned long distance_from_vma_start),
	TP_ARGS(folio, first_addr, do_break, go_back_size, distance_from_vma_start),
	TP_STRUCT__entry(
		    __field(struct folio *, folio)
			__field(unsigned long, first_addr)
			__field(bool, do_break)
			__field(unsigned long, go_back_size)
			__field(unsigned long, distance_from_vma_start)
	),
	TP_fast_assign(
		__entry->folio = folio;
		__entry->first_addr = first_addr;
		__entry->do_break = do_break;
		__entry->go_back_size = go_back_size;
		__entry->distance_from_vma_start = distance_from_vma_start;
	),
	TP_printk("folio=%p first_addr=0x%lx do_break=%d go_back_size=%ld distance_from_vma_start=%ld", __entry->folio, __entry->first_addr, __entry->do_break, __entry->go_back_size, __entry->distance_from_vma_start)
);
TRACE_EVENT(mm_vmscan_follow_address,
	TP_PROTO(struct vm_area_struct *vma, unsigned long addr, unsigned long pgd_val, 
			unsigned long p4d_val, unsigned long pud_val, unsigned long pmd_val, unsigned long pte_val, unsigned long pfn, struct folio *folio, int ref_count),
	TP_ARGS(vma, addr, pgd_val, p4d_val, pud_val, pmd_val, pte_val, pfn, folio, ref_count),
	TP_STRUCT__entry(
		    __field(struct vm_area_struct *, vma)
		    __field(unsigned long, addr)
		    __field(unsigned long, pgd_val)
		    __field(unsigned long, p4d_val)
		    __field(unsigned long, pud_val)
		    __field(unsigned long, pmd_val)
		    __field(unsigned long, pte_val)
		    __field(unsigned long, pfn)
		    __field(struct folio *, folio)
			__field(int, ref_count)
	),	
	TP_fast_assign(
		__entry->vma = vma;
		__entry->addr = addr;
		__entry->pgd_val = pgd_val;
		__entry->p4d_val = p4d_val;
		__entry->pud_val = pud_val;
		__entry->pmd_val = pmd_val;
		__entry->pte_val = pte_val;
		__entry->pfn = pfn;
		__entry->folio = folio;
		__entry->ref_count = ref_count;
	),
	TP_printk("vma=%p addr=0x%lx pgd_val=0x%lx p4d_val=0x%lx pud_val=0x%lx pmd_val=0x%lx pte_val=0x%lx pfn=0x%lx folio=%p order=%d ref_count=%d",
		__entry->vma,
		__entry->addr,
		__entry->pgd_val,
		__entry->p4d_val,
		__entry->pud_val,
		__entry->pmd_val,
		__entry->pte_val,
		__entry->pfn,
		__entry->folio,
		__entry->folio ? folio_order(__entry->folio) : -1,
		__entry->ref_count)

);
TRACE_EVENT(mm_vmscan_get_next_folio_for_folio,
	TP_PROTO(struct folio *cur_folio, struct folio *next_folio, unsigned long addr, struct vm_area_struct *vma, int is_anon, int is_file, int has_mapping),
	TP_ARGS(cur_folio, next_folio, addr, vma, is_anon, is_file, has_mapping),
	TP_STRUCT__entry(
		    __field(struct folio *, cur_folio)
		    __field(struct folio *, next_folio)
		    __field(unsigned long, addr)
		    __field(struct vm_area_struct *, vma)
			__field(int, is_anon)
			__field(int, is_file)
			__field(int, has_mapping)
	),
	TP_fast_assign(
		__entry->cur_folio = cur_folio;
		__entry->next_folio = next_folio;
		__entry->addr = addr;
		__entry->vma = vma;
		__entry->is_anon = is_anon;
		__entry->is_file = is_file;

	),
	TP_printk("cur_folio=%p next_folio=%p next_addr=0x%lx vma=%p is_anon=%d is_file=%d has_mapping=%d",
		__entry->cur_folio,
		__entry->next_folio,
		__entry->addr,
		__entry->vma,
		__entry->is_anon,
		__entry->is_file,
		__entry->has_mapping)
);
TRACE_EVENT(mm_vmscan_get_prev_folio_for_folio,
	TP_PROTO(struct folio *cur_folio, struct folio *prev_folio, unsigned long addr, struct vm_area_struct *vma, int is_anon, int is_file, int has_mapping),
	TP_ARGS(cur_folio, prev_folio, addr, vma, is_anon, is_file, has_mapping),
	TP_STRUCT__entry(
		    __field(struct folio *, cur_folio)
		    __field(struct folio *, prev_folio)
		    __field(unsigned long, addr)
		    __field(struct vm_area_struct *, vma)
			__field(int, is_anon)
			__field(int, is_file)
			__field(int, has_mapping)
	),
	TP_fast_assign(
		__entry->cur_folio = cur_folio;
		__entry->prev_folio = prev_folio;
		__entry->addr = addr;
		__entry->vma = vma;
		__entry->is_anon = is_anon;
		__entry->is_file = is_file;
		__entry->has_mapping = has_mapping;
	),
	TP_printk("cur_folio=%p prev_folio=%p prev_addr=0x%lx vma=%p is_anon=%d is_file=%d has_mapping=%d",
		__entry->cur_folio,
		__entry->prev_folio,
		__entry->addr,
		__entry->vma,
		__entry->is_anon,
		__entry->is_file,
		__entry->has_mapping)
);
TRACE_EVENT(mm_vmscan_reclaim_page,
	TP_PROTO(struct folio *folio, int node_id, int memcg_id),
	TP_ARGS(folio, node_id, memcg_id),
	TP_STRUCT__entry(
		    __field(struct folio *, folio)
		    __field(int, node_id)
		    __field(int, memcg_id)
	),
	TP_fast_assign(
		__entry->folio = folio;
		__entry->node_id = node_id;
		__entry->memcg_id = memcg_id;
	),
	TP_printk("folio=%p node_id=%d memcg_id=%d",
		__entry->folio,
		__entry->node_id,
		__entry->memcg_id)
);
TRACE_EVENT(mm_vmscan_get_vma_for_folio,
	TP_PROTO(struct folio *folio, struct vm_area_struct *vma, unsigned long cur_addr, pgoff_t window_start, pgoff_t window_end,size_t swap_ahead_size, int is_anon, int is_file, int has_mapping),
	TP_ARGS(folio, vma, cur_addr, window_start, window_end,swap_ahead_size, is_anon, is_file, has_mapping),
	TP_STRUCT__entry(
			__field(struct folio *, folio)
			__field(struct vm_area_struct *, vma)
			__field(unsigned long, cur_addr)
			__field(pgoff_t, window_start)
			__field(pgoff_t, window_end)
			__field(size_t, swap_ahead_size)
			__field(int, is_anon)
			__field(int, is_file)
			__field(int, has_mapping)
	),
	TP_fast_assign(
		__entry->folio = folio;
		__entry->vma = vma;
		__entry->cur_addr = cur_addr;
	  	__entry->window_start = window_start;
		__entry->window_end = window_end;
		__entry->swap_ahead_size = swap_ahead_size;
		__entry->is_anon = is_anon;
		__entry->is_file = is_file;
		__entry->has_mapping = has_mapping;
	)
	,
	TP_printk("folio=%p vma=%p cur_addr=0x%lx window_start=%llu window_end=%llu swap_ahead_size=%ld is_anon=%d is_file=%d has_mapping=%d",
		__entry->folio,
		__entry->vma,
		__entry->cur_addr,
	  	(unsigned long long)__entry->window_start,
		(unsigned long long)__entry->window_end,
		__entry->swap_ahead_size,
		__entry->is_anon,
		__entry->is_file,
		__entry->has_mapping
	)
);
TRACE_EVENT(mm_vmscan_shrink_folio_list,
	TP_PROTO(struct folio *folio, int keep_locked, char* reason),
	TP_ARGS(folio, keep_locked, reason),
	TP_STRUCT__entry(
		    __field(struct folio *, folio)
		    __field(int, keep_locked)
		    __string(reason, reason)
	),
	TP_fast_assign(
		__entry->folio = folio;
		__entry->keep_locked = keep_locked;
		__assign_str(reason);
	),
	TP_printk("folio=%p keep_locked=%d reason=%s",
		__entry->folio,
		__entry->keep_locked,
		__get_str(reason))
);
TRACE_EVENT(mm_vmscan_pageout,

	TP_PROTO(struct folio *folio, int action, const char *reason),

	TP_ARGS(folio, action, reason),
	TP_STRUCT__entry(
		__field(struct folio *, folio)
		__field(int, action)
		__string(reason, reason)
	),
		TP_fast_assign(
		__entry->folio = folio;
		__entry->action = action;
		__assign_str(reason);
	),
	TP_printk("folio=%p action=%d reason=%s",
		__entry->folio,
		__entry->action,
		__get_str(reason))
);
TRACE_EVENT(mm_vmscan_is_page_cache_freeable,

	TP_PROTO(struct folio *folio, int ref_count, int is_private, int is_private_2, int nr_pages),

	TP_ARGS(folio, ref_count, is_private, is_private_2, nr_pages),

	TP_STRUCT__entry(
		__field(struct folio *, folio)
		__field(int, ref_count)
		__field(int, is_private)
		__field(int, is_private_2)
		__field(int, nr_pages)
	),

	TP_fast_assign(
		__entry->folio = folio;
		__entry->ref_count = ref_count;
		__entry->is_private = is_private;
		__entry->is_private_2 = is_private_2;
		__entry->nr_pages = nr_pages;
	),

	TP_printk("folio=%p ref_count=%d is_private=%d is_private_2=%d nr_pages=%d",
		  __entry->folio,
		  __entry->ref_count,
		  __entry->is_private,
		  __entry->is_private_2,
		  __entry->nr_pages)
);
TRACE_EVENT(mm_vmscan_folio_refs,
	TP_PROTO(struct folio *folio, int refs, char* reason),
	TP_ARGS(folio, refs, reason),
	TP_STRUCT__entry(
		    __field(struct folio *, folio)
		    __field(int, refs)
			__string(reason, reason)
	),
	TP_fast_assign(
		__entry->folio = folio;
		__entry->refs = refs;
		__assign_str(reason);
	),
	TP_printk("folio=%p refs=%d reason=%s",
		__entry->folio,
		__entry->refs,
		__get_str(reason))
);
TRACE_EVENT(mm_vmscan_update_vma_reclaim_size,
	TP_PROTO(struct vm_area_struct *vma, unsigned int new_size, size_t vma_seq_hits),
	TP_ARGS(vma, new_size, vma_seq_hits),
	TP_STRUCT__entry(
		    __field(struct vm_area_struct *, vma)
		    __field(unsigned int, new_size)
			__field(size_t, vma_seq_hits)
	),
	TP_fast_assign(
		__entry->vma = vma;
		__entry->new_size = new_size;
		__entry->vma_seq_hits = vma_seq_hits;
	),
	TP_printk("vma=%p new_size=%u vma_seq_hits=%ld",
		__entry->vma,
		__entry->new_size,
		__entry->vma_seq_hits)
);
TRACE_EVENT(mm_vmscan_sort_folio,
	TP_PROTO(struct folio *folio, unsigned int ref, char* reason),
	TP_ARGS(folio, ref, reason),
	TP_STRUCT__entry(
		    __field(struct folio *, folio)
			__field(unsigned int, ref)
			__string(reason, reason)
	),
	TP_fast_assign(
		__entry->folio = folio;
		__entry->ref = ref;
		__assign_str(reason);
	),
	TP_printk("folio=%p ref=%u reason=%s",
		__entry->folio,
		__entry->ref,
		__get_str(reason))
);
TRACE_EVENT(mm_vmscan_is_dirty_seq_hit,
	TP_PROTO(struct folio *folio, bool is_swapcache, bool is_anon, bool is_swapbacked, bool is_dirty, bool is_writeback, bool is_reclaim, int refs, int seq_hits),
	TP_ARGS(folio, is_swapcache, is_anon, is_swapbacked, is_dirty, is_writeback, is_reclaim, refs, seq_hits),
	TP_STRUCT__entry(
		    __field(struct folio *, folio)
			__field(bool, is_swapcache)
			__field(bool, is_anon)
			__field(bool, is_swapbacked)
			__field(bool, is_dirty)	
			__field(bool, is_writeback)
			__field(bool, is_reclaim)
			__field(int, refs)
			__field(int, seq_hits)
	),
	TP_fast_assign(
		__entry->folio = folio;
		__entry->is_swapcache = is_swapcache;
		__entry->is_anon = is_anon;	
		__entry->is_swapbacked = is_swapbacked;
		__entry->is_dirty = is_dirty;
		__entry->is_writeback = is_writeback;
		__entry->is_reclaim = is_reclaim;
		__entry->refs = refs;
		__entry->seq_hits = seq_hits;
	),
	TP_printk("folio=%p is_swapcache=%d is_anon=%d is_swapbacked=%d is_dirty=%d is_writeback=%d is_reclaim=%d refs=%d seq_hits=%d",
		__entry->folio,
		__entry->is_swapcache,
		__entry->is_anon,	
		__entry->is_swapbacked,
		__entry->is_dirty,
		__entry->is_writeback,
		__entry->is_reclaim,
		__entry->refs,
		__entry->seq_hits)
);
TRACE_EVENT(mm_vmscan_should_abort_scan,
	TP_PROTO(int nr_reclaimed, int nr_to_reclaim, int order, bool root_reclaim),
	TP_ARGS(nr_reclaimed, nr_to_reclaim, order, root_reclaim),
	TP_STRUCT__entry(
		__field(int, nr_reclaimed)
		__field(int, nr_to_reclaim)
		__field(int, order)
		__field(bool, root_reclaim)
	),
	TP_fast_assign(
		__entry->nr_reclaimed = nr_reclaimed;
		__entry->nr_to_reclaim = nr_to_reclaim;
		__entry->order = order;
		__entry->root_reclaim = root_reclaim;
	),
	TP_printk("nr_reclaimed=%d nr_to_reclaim=%d order=%d root_reclaim=%d",
		__entry->nr_reclaimed,
		__entry->nr_to_reclaim,
		__entry->order,
		__entry->root_reclaim)
);
TRACE_EVENT(mm_vmscan_try_to_shrink_lruvec,
	TP_PROTO(int delta, unsigned long scanned, long nr_to_scan),
	TP_ARGS(delta, scanned, nr_to_scan),
	TP_STRUCT__entry(
		__field(int, delta)
		__field(unsigned long, scanned)
		__field(long, nr_to_scan)
	),
	TP_fast_assign(
		__entry->delta = delta;
		__entry->scanned = scanned;
		__entry->nr_to_scan = nr_to_scan;
	),
	TP_printk("delta=%d scanned=%lu nr_to_scan=%ld",
		__entry->delta,
		__entry->scanned,
		__entry->nr_to_scan)
);
TRACE_EVENT(mm_vmscan_scan_folios,
	TP_PROTO(struct folio *folio, bool needs_release, struct address_space *mapping, bool is_dirty),
	TP_ARGS(folio, needs_release, mapping, is_dirty),
	TP_STRUCT__entry(
		__field(struct folio *, folio)
		__field(bool, needs_release)
		__field(struct address_space *, mapping)
		__field(bool, is_dirty)
	),
	TP_fast_assign(
		__entry->folio = folio;
		__entry->needs_release = needs_release;
		__entry->mapping = mapping;
		__entry->is_dirty = is_dirty;
	),
	TP_printk("folio=%p needs_release=%d mapping=%p is_dirty=%d",
		__entry->folio,
		__entry->needs_release,
		__entry->mapping,
		__entry->is_dirty)
);
TRACE_EVENT(mm_vmscan_is_seq_dirty_hit,
	TP_PROTO(struct folio *folio, bool is_swapcache, bool is_anon, bool is_swapbacked, bool is_dirty, bool is_writeback),
	TP_ARGS(folio, is_swapcache, is_anon, is_swapbacked, is_dirty, is_writeback),
	TP_STRUCT__entry(
		__field(struct folio *, folio)
		__field(bool, is_swapcache)
		__field(bool, is_anon)
		__field(bool, is_swapbacked)
		__field(bool, is_dirty)
		__field(bool, is_writeback)
	),
	TP_fast_assign(
		__entry->folio = folio;
		__entry->is_swapcache = is_swapcache;
		__entry->is_anon = is_anon;
		__entry->is_swapbacked = is_swapbacked;
		__entry->is_dirty = is_dirty;
		__entry->is_writeback = is_writeback;
	),
	TP_printk("folio=%p is_swapcache=%d is_anon=%d is_swapbacked=%d is_dirty=%d is_writeback=%d",
		__entry->folio,
		__entry->is_swapcache,
		__entry->is_anon,
		__entry->is_swapbacked,
		__entry->is_dirty,
		__entry->is_writeback)
);
TRACE_EVENT(mm_vmscan_get_vma,
	TP_PROTO(struct vm_area_struct *vma, unsigned long address, struct folio *folio, int is_anon, int is_file, int has_mapping),
	TP_ARGS(vma, address, folio, is_anon, is_file, has_mapping),
	TP_STRUCT__entry(
		__field(struct vm_area_struct *, vma)
		__field(unsigned long, address)
		__field(struct folio *, folio)
		__field(int, is_anon)
		__field(int, is_file)
		__field(int, has_mapping)
	),
	TP_fast_assign(
		__entry->vma = vma;
		__entry->address = address;
		__entry->folio = folio;
		__entry->is_anon = is_anon;
		__entry->is_file = is_file;
		__entry->has_mapping = has_mapping;
	),
	TP_printk("vma=%p address=0x%lx folio=%p is_anon=%d is_file=%d has_mapping=%d",
		__entry->vma,
		__entry->address,
		__entry->folio,
		__entry->is_anon,	
		__entry->is_file,
		__entry->has_mapping)
	);
#endif
#endif /* _TRACE_VMSCAN_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
