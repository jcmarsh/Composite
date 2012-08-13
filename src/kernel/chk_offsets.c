//#include "./include/debug.h"
#include "./include/asm_ipc_defs.h"
#include "./include/thread.h"
#include "./include/shared/cos_types.h"

#include "chk_offsets.h"

// see asm_ipc_defs.h
void check_offsets() {
  // thd_invocation_frame
  assert(offsetof(struct thd_invocation_frame, spd) == SFRAMEUSR);
  // #error "offsetof(struct thd_invocation_frame, spd) == SFRAMEUSR"

  assert(offsetof(struct thd_invocation_frame, ip) == SFRAMEIP);
  // #error "offsetof(struct thd_invocation_frame, ip) == SFRAMEIP"

  assert(offsetof(struct thd_invocation_frame, sp) == SFRAMESP);
  // #error "offsetof(struct thd_invocation_frame, sp) == SFRAMESP"


  // usr_inv_cap
  assert(offsetof(struct usr_inv_cap, invocation_fn) == INVFN);
  // #error "offsetof(struct usr_inv_cap, invocation_fn) == INVFN"

  assert(offsetof(struct usr_inv_cap, service_entry_inst) == ENTRYFN);
  // #error "offsetof(struct usr_inv_cap, service_entry_inst) == ENTRYFN"

  assert(offsetof(struct usr_inv_cap, invocation_count) == INVOCATIONCNT);
  // #error "offsetof(struct usr_inv_cap, invocation_count) == INVOCATIONCNT"

  assert(offsetof(struct usr_inv_cap, cap_no) == CAPNUM);
  // #error "offsetof(struct usr_inv_cap, cap_no) == CAPNUM"

}
