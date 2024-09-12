/*
 * Copyright (c) 2024 Beijing Institute of Open Source Chip (BOSC)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __EVENTS_H__
#define __EVENTS_H__

// due to the probable performance event change
// it is recommeded to parse_encoding again
// to update this event_encoding

#define noEvent 0
#define Frontend_noEvent 0
#define Frontend_frontendFlush 1
#define Frontend_ifu_req 2
#define Frontend_ifu_miss 3
#define Frontend_ifu_req_cacheline_0 4
#define Frontend_ifu_req_cacheline_1 5
#define Frontend_ifu_req_cacheline_0_hit 6
#define Frontend_ifu_req_cacheline_1_hit 7
#define Frontend_only_0_hit 8
#define Frontend_only_0_miss 9
#define Frontend_hit_0_hit_1 10
#define Frontend_hit_0_miss_1 11
#define Frontend_miss_0_hit_1 12
#define Frontend_miss_0_miss_1 13
#define Frontend_IBuffer_Flushed 14
#define Frontend_IBuffer_hungry 15
#define Frontend_IBuffer_1_4_valid 16
#define Frontend_IBuffer_2_4_valid 17
#define Frontend_IBuffer_3_4_valid 18
#define Frontend_IBuffer_4_4_valid 19
#define Frontend_IBuffer_full 20
#define Frontend_Front_Bubble 21
#define Frontend_icache_miss_cnt 22
#define Frontend_icache_miss_penalty 23
#define Frontend_bpu_s2_redirect 24
#define Frontend_bpu_s3_redirect 25
#define Frontend_bpu_to_ftq_stall 26
#define Frontend_mispredictRedirect 27
#define Frontend_replayRedirect 28
#define Frontend_predecodeRedirect 29
#define Frontend_to_ifu_bubble 30
#define Frontend_from_bpu_real_bubble 31
#define Frontend_BpInstr 32
#define Frontend_BpBInstr 33
#define Frontend_BpRight 34
#define Frontend_BpWrong 35
#define Frontend_BpBRight 36
#define Frontend_BpBWrong 37
#define Frontend_BpJRight 38
#define Frontend_BpJWrong 39
#define Frontend_BpIRight 40
#define Frontend_BpIWrong 41
#define Frontend_BpCRight 42
#define Frontend_BpCWrong 43
#define Frontend_BpRRight 44
#define Frontend_BpRWrong 45
#define Frontend_ftb_false_hit 46
#define Frontend_ftb_hit 47
#define Frontend_tage_tht_hit 48
#define Frontend_sc_update_on_mispred 49
#define Frontend_sc_update_on_unconf 50
#define Frontend_ftb_commit_hits 51
#define Frontend_ftb_commit_misses 52
#define CSR_Hc 0
#define CSR_Hc_1 1
#define CSR_Hc_2 2
#define CSR_Hc_3 3
#define CSR_Hc_4 4
#define CSR_Hc_5 5
#define CSR_Hc_6 6
#define CSR_Hc_7 7
#define CSR_Hc_8 8
#define CSR_Hc_9 9
#define CSR_Hc_10 10
#define CSR_Hc_11 11
#define CSR_Hc_12 12
#define CSR_Hc_13 13
#define CSR_Hc_14 14
#define CSR_Hc_15 15
#define CSR_Hc_16 16
#define CSR_Hc_17 17
#define CSR_Hc_18 18
#define CSR_Hc_19 19
#define CSR_Hc_20 20
#define CSR_Hc_21 21
#define CSR_Hc_22 22
#define CSR_Hc_23 23
#define CSR_Hc_24 24
#define MemBlock_noEvent 0
#define MemBlock_lu0_load_s0_in_fire 1
#define MemBlock_lu0_load_to_load_forward 2
#define MemBlock_lu0_stall_dcache 3
#define MemBlock_lu0_addr_spec_success 4
#define MemBlock_lu0_addr_spec_failed 5
#define MemBlock_lu0_load_s1_in_fire 6
#define MemBlock_lu0_load_s1_tlb_miss 7
#define MemBlock_lu0_load_s2_in_fire 8
#define MemBlock_lu0_load_s2_dcache_miss 9
#define MemBlock_lu0_load_s2_replay 10
#define MemBlock_lu0_load_s2_replay_tlb_miss 11
#define MemBlock_lu0_load_s2_replay_cache 12
#define MemBlock_lu1_load_s0_in_fire 13
#define MemBlock_lu1_load_to_load_forward 14
#define MemBlock_lu1_stall_dcache 15
#define MemBlock_lu1_addr_spec_success 16
#define MemBlock_lu1_addr_spec_failed 17
#define MemBlock_lu1_load_s1_in_fire 18
#define MemBlock_lu1_load_s1_tlb_miss 19
#define MemBlock_lu1_load_s2_in_fire 20
#define MemBlock_lu1_load_s2_dcache_miss 21
#define MemBlock_lu1_load_s2_replay 22
#define MemBlock_lu1_load_s2_replay_tlb_miss 23
#define MemBlock_lu1_load_s2_replay_cache 24
#define MemBlock_lu1_sbuffer_req_valid 25
#define MemBlock_sbuffer_req_fire 26
#define MemBlock_sbuffer_merge 27
#define MemBlock_sbuffer_newline 28
#define MemBlock_dcache_req_valid 29
#define MemBlock_dcache_req_fire 30
#define MemBlock_sbuffer_idle 31
#define MemBlock_sbuffer_flush 32
#define MemBlock_sbuffer_replace 33
#define MemBlock_mpipe_resp_valid 34
#define MemBlock_refill_resp_valid 35
#define MemBlock_replay_resp_valid 36
#define MemBlock_coh_timeout 37
#define MemBlock_sbuffer_1_4_valid 38
#define MemBlock_sbuffer_2_4_valid 39
#define MemBlock_sbuffer_3_4_valid 40
#define MemBlock_sbuffer_full_valid 41
#define MemBlock_lq_rollback 42
#define MemBlock_lq_mmioCycle 43
#define MemBlock_lq_mmio_Cnt 44
#define MemBlock_lq_refill 45
#define MemBlock_lq_writeback_success 46
#define MemBlock_lq_writeback_blocked 47
#define MemBlock_ltq_1_4_valid 48
#define MemBlock_ltq_2_4_valid 49
#define MemBlock_ltq_3_4_valid 50
#define MemBlock_ltq_4_4_valid 51
#define MemBlock_sq_mmioCycle 52
#define MemBlock_sq_mmioCnt 53
#define MemBlock_sq_mmio_wb_success 54
#define MemBlock_sq_mmio_wb_blocked 55
#define MemBlock_stq_1_4_valid 56
#define MemBlock_stq_2_4_valid 57
#define MemBlock_stq_3_4_valid 58
#define MemBlock_stq_4_4_valid 59
#define MemBlock_dcache_wbq_req 60
#define MemBlock_dcache_wbq_1_4_valid 61
#define MemBlock_dcache_wbq_2_4_valid 62
#define MemBlock_dcache_wbq_3_4_valid 63
#define MemBlock_dcache_wbq_4_4_valid 64
#define MemBlock_dcache_mp_req 65
#define MemBlock_dcache_mp_total_penalty 66
#define MemBlock_dcache_missq_req 67
#define MemBlock_dcache_missq_1_4_valid 68
#define MemBlock_dcache_missq_2_4_valid 69
#define MemBlock_dcache_missq_3_4_valid 70
#define MemBlock_dcache_missq_4_4_valid 71
#define MemBlock_dcache_probq_req 72
#define MemBlock_dcache_probq_1_4_valid 73
#define MemBlock_dcache_probq_2_4_valid 74
#define MemBlock_dcache_probq_3_4_valid 75
#define MemBlock_dcache_probq_4_4_valid 76
#define MemBlock_loadpipe0_load_req 77
#define MemBlock_loadpipe0_load_replay 78
#define MemBlock_loadpipe0_load_replay_for_data_nack 79
#define MemBlock_loadpipe0_load_replay_for_no_mshr 80
#define MemBlock_loadpipe0_load_replay_for_conflict 81
#define MemBlock_loadpipe1_load_req 82
#define MemBlock_loadpipe1_load_replay 83
#define MemBlock_loadpipe1_load_replay_for_data_nack 84
#define MemBlock_loadpipe1_load_replay_for_no_mshr 85
#define MemBlock_loadpipe1_load_replay_for_conflict 86
#define MemBlock_perfEventsPTW_0 87
#define MemBlock_perfEventsPTW_1 88
#define MemBlock_perfEventsPTW_2 89
#define MemBlock_perfEventsPTW_3 90
#define MemBlock_perfEventsPTW_4 91
#define MemBlock_perfEventsPTW_5 92
#define MemBlock_perfEventsPTW_6 93
#define MemBlock_perfEventsPTW_7 94
#define MemBlock_perfEventsPTW_8 95
#define MemBlock_perfEventsPTW_9 96
#define MemBlock_perfEventsPTW_10 97
#define MemBlock_perfEventsPTW_11 98
#define MemBlock_perfEventsPTW_12 99
#define MemBlock_perfEventsPTW_13 100
#define MemBlock_perfEventsPTW_14 101
#define MemBlock_perfEventsPTW_15 102
#define MemBlock_perfEventsPTW_16 103
#define MemBlock_perfEventsPTW_17 104
#define MemBlock_perfEventsPTW_18 105
#define MemBlock_ldDeqCount 106
#define MemBlock_stDeqCount 107
#define CtrlBlock_noEvent 0
#define CtrlBlock_decoder_fused_instr 1
#define CtrlBlock_decoder_waitInstr 2
#define CtrlBlock_decoder_stall_cycle 3
#define CtrlBlock_decoder_utilization 4
#define CtrlBlock_rename_in 5
#define CtrlBlock_rename_waitinstr 6
#define CtrlBlock_rename_stall_cycle_dispatch 7
#define CtrlBlock_rename_stall_cycle_fp 8
#define CtrlBlock_rename_stall_cycle_int 9
#define CtrlBlock_rename_stall_cycle_walk 10
#define CtrlBlock_me_freelist_1_4_valid 11
#define CtrlBlock_me_freelist_2_4_valid 12
#define CtrlBlock_me_freelist_3_4_valid 13
#define CtrlBlock_me_freelist_4_4_valid 14
#define CtrlBlock_std_freelist_1_4_valid 15
#define CtrlBlock_std_freelist_2_4_valid 16
#define CtrlBlock_std_freelist_3_4_valid 17
#define CtrlBlock_std_freelist_4_4_valid 18
#define CtrlBlock_dispatch_in 19
#define CtrlBlock_dispatch_empty 20
#define CtrlBlock_dispatch_utili 21
#define CtrlBlock_dispatch_waitinstr 22
#define CtrlBlock_dispatch_stall_cycle_lsq 23
#define CtrlBlock_dispatch_stall_cycle_rob 24
#define CtrlBlock_dispatch_stall_cycle_int_dq 25
#define CtrlBlock_dispatch_stall_cycle_fp_dq 26
#define CtrlBlock_dispatch_stall_cycle_ls_dq 27
#define CtrlBlock_intdq_dispatchq_in 28
#define CtrlBlock_intdq_dispatchq_out 29
#define CtrlBlock_intdq_dispatchq_out_try 30
#define CtrlBlock_intdq_dispatchq_fake_block 31
#define CtrlBlock_intdq_dispatchq_1_4_valid 32
#define CtrlBlock_intdq_dispatchq_2_4_valid 33
#define CtrlBlock_intdq_dispatchq_3_4_valid 34
#define CtrlBlock_intdq_dispatchq_4_4_valid 35
#define CtrlBlock_fpdq_dispatchq_in 36
#define CtrlBlock_fpdq_dispatchq_out 37
#define CtrlBlock_fpdq_dispatchq_out_try 38
#define CtrlBlock_fpdq_dispatchq_fake_block 39
#define CtrlBlock_fpdq_dispatchq_1_4_valid 40
#define CtrlBlock_fpdq_dispatchq_2_4_valid 41
#define CtrlBlock_fpdq_dispatchq_3_4_valid 42
#define CtrlBlock_fpdq_dispatchq_4_4_valid 43
#define CtrlBlock_lsdq_dispatchq_in 44
#define CtrlBlock_lsdq_dispatchq_out 45
#define CtrlBlock_lsdq_dispatchq_out_try 46
#define CtrlBlock_lsdq_dispatchq_fake_block 47
#define CtrlBlock_lsdq_dispatchq_1_4_valid 48
#define CtrlBlock_lsdq_dispatchq_2_4_valid 49
#define CtrlBlock_lsdq_dispatchq_3_4_valid 50
#define CtrlBlock_lsdq_dispatchq_4_4_valid 51
#define CtrlBlock_rob_interrupt_num 52
#define CtrlBlock_rob_exception_num 53
#define CtrlBlock_rob_flush_pipe_num 54
#define CtrlBlock_rob_replay_inst_num 55
#define CtrlBlock_rob_commitUop 56
#define CtrlBlock_rob_commitInstr 57
#define CtrlBlock_rob_commitInstrMove 58
#define CtrlBlock_rob_commitInstrFused 59
#define CtrlBlock_rob_commitInstrLoad 60
#define CtrlBlock_rob_commitInstrLoad_1 61
#define CtrlBlock_rob_commitInstrLoadWait 62
#define CtrlBlock_rob_commitInstrStore 63
#define CtrlBlock_rob_walkInstr 64
#define CtrlBlock_rob_walkCycle 65
#define CtrlBlock_rob_1_4_valid 66
#define CtrlBlock_rob_2_4_valid 67
#define CtrlBlock_rob_3_4_valid 68
#define CtrlBlock_rob_4_4_valid 69
#define CtrlBlock_perfEventsEu0_0 70
#define CtrlBlock_perfEventsEu0_1 71
#define CtrlBlock_perfEventsEu0_2 72
#define CtrlBlock_perfEventsEu0_3 73
#define CtrlBlock_perfEventsEu0_4 74
#define CtrlBlock_perfEventsEu0_5 75
#define CtrlBlock_perfEventsEu1_0 76
#define CtrlBlock_perfEventsEu1_1 77
#define CtrlBlock_perfEventsEu1_2 78
#define CtrlBlock_perfEventsEu1_3 79
#define CtrlBlock_perfEventsEu1_4 80
#define CtrlBlock_perfEventsEu1_5 81
#define CtrlBlock_perfEventsRs_0 82
#define CtrlBlock_perfEventsRs_1 83
#define CtrlBlock_perfEventsRs_2 84
#define CtrlBlock_perfEventsRs_3 85
#define CtrlBlock_perfEventsRs_4 86
#define CtrlBlock_perfEventsRs_5 87
#define CtrlBlock_perfEventsRs_6 88
#define CtrlBlock_perfEventsRs_7 89

#endif // __EVENTS_H__
