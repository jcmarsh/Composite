/* Definitions */
	

/* offsets into the thd_invocation_frame structure (thread.h) */
#ifdef X86_64
/* the offset on the stack to the fn return address for trust cases */
//#define IPRETURN 12 // saving ecx & edx??
#define IPRETURN 8 // TODO: This needs to be tested -jcm

#define SFRAMEUSR 8 // 2x because all the pointers are 8 instead of 4.
#define SFRAMESP 16
#define SFRAMEIP 24

/* user capability structure offsets (cos_types.h usr_inv_cap)*/
/* really 16, see below for use (mult index reg by 2) */
#define UCAP_SZ 8 		/* # of longs */
#define UCAP_SZ_STR "8"
#define SIZEOFUSERCAP (UCAP_SZ * 4) // We need 32 (4 quads) bytes, but they are reserved as .longs. 
#define INVFN 0
#define ENTRYFN 8
#define INVOCATIONCNT 16
#define CAPNUM 24
#else /* x86_32 implementation */
/* the offset on the stack to the fn return address for trust cases */
//#define IPRETURN 12 // saving ecx & edx??
#define IPRETURN 4

#define SFRAMEUSR 4
#define SFRAMESP 8
#define SFRAMEIP 12

/* user capability structure offsets (cos_types.h usr_inv_cap)*/
/* really 16, see below for use (mult index reg by 2) */
#define UCAP_SZ 4 		/* # of longs */
#define UCAP_SZ_STR "4"
#define SIZEOFUSERCAP (UCAP_SZ*4)
#define INVFN 0
#define ENTRYFN 4
#define INVOCATIONCNT 8
#define CAPNUM 12
#endif /* X86_64 */

/* offsets into syscall integer */
#define COS_CAPABILITY_OFFSET 20 /* bits 20->32 */
#define COS_SYSCALL_OFFSET 15	 /* bits 15->19 */

//#define RET_CAP (INV_CAP_OFFSET-1)

