
#ifndef	_E2K_CPU_REGS_TYPES_H_
#define	_E2K_CPU_REGS_TYPES_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef __ASSEMBLER__

/* E2K physical address definitions */

/* E2K physical address size (bits number) */
#define	MAX_PA_SIZE	CONFIG_E2K_PA_BITS
/* The number of the most significant bit of E2K physical address */
#define	MAX_PA_MSB	(MAX_PA_SIZE - 1)
#define	MAX_PA_MASK	((1UL << MAX_PA_SIZE) - 1)
#define	MAX_PM_SIZE	(1UL << MAX_PA_SIZE)

/* E2K virtual address definitions */
#define	MAX_VA_SIZE	59			/* Virtual address maximum */
						/* size (bits number) */
#define	MAX_VA_MSB	(MAX_VA_SIZE -1)	/* The maximum number of the */
						/* most significant bit of */
						/* virtual address */
#define	MAX_VA_MASK	((1UL << MAX_VA_SIZE) - 1)

#define	E2K_VA_SIZE	48			/* E2K Virtual address size */
						/* (bits number) */
#define	E2K_VA_MSB	(E2K_VA_SIZE - 1)	/* The number of the most */
						/* significant bit of E2K */
						/* virtual address */
#define	E2K_VA_MASK	((1UL << E2K_VA_SIZE) - 1)

#define	E2K_VA_PAGE_MASK	(E2K_VA_MASK & PAGE_MASK)


/*
 * Read/Write Pointer (RWP) (64 bits)
 */
typedef	struct e2k_rwp_fields {		/* Structure of Read/write pointer */
	uint64_t	base	: E2K_VA_SIZE;		/* [47: 0] */
	uint64_t	unused2	: 53 - E2K_VA_MSB;	/* [53:48] */
	uint64_t	stub5	:  1;			/* [54] */
	uint64_t	stub4	:  1;			/* [55] */
	uint64_t	stub3	:  1;			/* [56] */
	uint64_t	stub2	:  1;			/* [57] */
	uint64_t	stub1	:  1;			/* [58] */
	uint64_t	unused	:  5;			/* [63:59] */
} e2k_rwp_fields_t;
typedef	union e2k_rwp_struct {		/* Structure of lower word */
	e2k_rwp_fields_t	fields;	/* as fields */
	uint64_t			word;	/* as entire register */
} e2k_rwp_struct_t;
#define	E2K_RWP_stub1		fields.stub1
#define	E2K_RWP_stub2		fields.stub2
#define	E2K_RWP_stub3		fields.stub3
#define	E2K_RWP_stub4		fields.stub4
#define	E2K_RWP_stub5		fields.stub5
#define	E2K_RWP_base		fields.base
#define	E2K_RWP_reg		word

/*
 * Read/Write Array Pointer (RWAP)
 */
typedef	struct e2k_rwap_lo_fields {	/* Fields of lower word */
	uint64_t	base	: E2K_VA_SIZE;		/* [47: 0] */
	uint64_t	unused2	: 55 - E2K_VA_MSB;	/* [55:48] */
	uint64_t	stub3	:  1;			/* [56] */
	uint64_t	stub2	:  1;			/* [57] */
	uint64_t	stub1	:  1;			/* [58] */
	uint64_t	rw	:  2;			/* [60:59] */
	uint64_t	itag	:  3;			/* [63:61] */
} e2k_rwap_lo_fields_t;
typedef	struct e2k_rusd_lo_fields {	/* Fields of lower word */
	uint64_t	base	: E2K_VA_SIZE;		/* [47: 0] */
	uint64_t	unused2	: 57 - E2K_VA_MSB;	/* [57:48] */
	uint64_t	p	:  1;			/* [58] */
	uint64_t	rw	:  2;			/* [60:59] */
	uint64_t	unused	:  3;			/* [63:61] */
} e2k_rusd_lo_fields_t;
typedef	union e2k_rwap_lo_struct {	/* Structure of lower word */
	e2k_rwap_lo_fields_t	ap_fields;	/* as AP fields */
	e2k_rusd_lo_fields_t	fields;		/* as USD fields */
	uint64_t			word;	/* as entire register */
} e2k_rwap_lo_struct_t;
#define	E2K_RWAP_lo_itag	ap_fields.itag
#define	E2K_RWAP_lo_rw		ap_fields.rw
#define	E2K_RWAP_lo_stub1	ap_fields.stub1
#define	E2K_RWAP_lo_stub2	ap_fields.stub2
#define	E2K_RWAP_lo_stub3	ap_fields.stub3
#define	E2K_RWAP_lo_base	ap_fields.base
#define	E2K_RUSD_lo_rw		fields.rw
#define	E2K_RUSD_lo_p		fields.p
#define	E2K_RUSD_lo_p_bit	58	/* do not forget to modify if changed */
#define	E2K_RUSD_lo_base	fields.base
#define	E2K_RWAP_lo_half	word
#define	E2K_RUSD_lo_half	word

typedef	struct e2k_rwap_hi_fields {	/* Fields of high word */
	uint64_t	curptr	: 32;			/* [31: 0] */
	uint64_t	size	: 32;			/* [63:32] */
} e2k_rwap_hi_fields_t;
typedef	struct e2k_rpsp_hi_fields {	/* Fields of high word */
	uint64_t	ind	: 32;			/* [31: 0] */
	uint64_t	size	: 32;			/* [63:32] */
} e2k_rpsp_hi_fields_t;
typedef	union e2k_rwap_hi_struct {	/* Structure of high word */
	e2k_rwap_hi_fields_t	ap_fields;	/* as AP fields */
	e2k_rpsp_hi_fields_t	fields;		/* as PSP fields */
	uint64_t			word;	/* as entire register */
} e2k_rwap_hi_struct_t;
#define	E2K_RWAP_hi_size	ap_fields.size
#define	E2K_RWAP_hi_curptr	ap_fields.curptr
#define	E2K_RWAP_hi_half	word
#define	E2K_RPSP_hi_size	fields.size
#define	E2K_RPSP_hi_ind		fields.ind
#define	E2K_RPSP_hi_half	word

typedef	struct e2k_rwap_struct {	/* quad-word register */
	e2k_rwap_lo_struct_t	lo;
	e2k_rwap_hi_struct_t	hi;
} e2k_rwap_struct_t;
#define	E2K_RWAP_lo_struct	lo
#define	E2K_RUSD_lo_struct	lo
#define	E2K_RWAP_hi_struct	hi
#define	E2K_RPSP_hi_struct	hi
#define	E2K_RWAP_itag		lo.E2K_RWAP_lo_itag
#define	E2K_RWAP_rw		lo.E2K_RWAP_lo_rw
#define	E2K_RWAP_stub1		lo.E2K_RWAP_lo_stub1
#define	E2K_RWAP_stub2		lo.E2K_RWAP_lo_stub2
#define	E2K_RWAP_stub3		lo.E2K_RWAP_lo_stub3
#define	E2K_RWAP_base		lo.E2K_RWAP_lo_base
#define	E2K_RUSD_rw		lo.E2K_RUSD_lo_rw
#define	E2K_RUSD_p		lo.E2K_RUSD_lo_p
#define	E2K_RUSD_p_bit		E2K_RUSD_lo_p_bit	/* protected flag */
#define	E2K_RUSD_p_flag		(1 << E2K_RUSD_p_bit)	/* as value */
#define	E2K_RUSD_base		lo.E2K_RUSD_lo_base
#define	E2K_RWAP_size		hi.E2K_RWAP_hi_size
#define	E2K_RWAP_curptr		hi.E2K_RWAP_hi_curptr
#define	E2K_RPSP_size		hi.E2K_RPSP_hi_size
#define	E2K_RPSP_ind		hi.E2K_RPSP_hi_ind
#define	E2K_RWAP_lo_reg		lo.E2K_RWAP_lo_half
#define	E2K_RUSD_lo_reg		lo.E2K_RUSD_lo_half
#define	E2K_RWAP_hi_reg		hi.E2K_RWAP_hi_half
#define	E2K_RPSP_hi_reg		hi.E2K_RPSP_hi_half

#define	E2_RWAR_R_ENABLE	0x1UL
#define	E2_RWAR_W_ENABLE	0x2UL
#define	E2_RWAR_RW_ENABLE	(E2_RWAR_R_ENABLE | E2_RWAR_W_ENABLE)
#define	E2_RWAR_C_TRUE		0x1

#define	R_ENABLE		0x1
#define	W_ENABLE		0x2
#define	RW_ENABLE		0x3

/*
 * Read/Write Stack Array Pointer (RWSAP)
 */
typedef	struct e2k_rwsap_lo_fields {	/* Fields of lower word */
	uint64_t	base	: 32;			/* [31: 0] */
	uint64_t	psl	: 16;			/* [47:32] */
	uint64_t	unused2	:  8;			/* [55:48] */
	uint64_t	stub3	:  1;			/* [56] */
	uint64_t	stub2	:  1;			/* [57] */
	uint64_t	stub1	:  1;			/* [58] */
	uint64_t	rw	:  2;			/* [60:59] */
	uint64_t	itag	:  3;			/* [63:61] */
} e2k_rwsap_lo_fields_t;
typedef	struct e2k_rpusd_lo_fields {	/* Fields of lower word */
	uint64_t	base	: 32;			/* [31: 0] */
	uint64_t	psl	: 16;			/* [47:32] */
	uint64_t	unused2	: 10;			/* [57:48] */
	uint64_t	p	:  1;			/* [58] */
	uint64_t	rw	:  2;			/* [60:59] */
	uint64_t	unused	:  3;			/* [63:61] */
} e2k_rpusd_lo_fields_t;
typedef	union e2k_rwsap_lo_struct {	/* Structure of lower word */
	e2k_rwsap_lo_fields_t	sap_fields;	/* as SAP fields */
	e2k_rpusd_lo_fields_t	fields;		/* as PUSD fields */
	uint64_t			word;	/* as entire register */
} e2k_rwsap_lo_struct_t;
#define	E2K_RWSAP_lo_itag	sap_fields.itag
#define	E2K_RWSAP_lo_rw		sap_fields.rw
#define	E2K_RWSAP_lo_stub1	sap_fields.stub1
#define	E2K_RWSAP_lo_stub2	sap_fields.stub2
#define	E2K_RWSAP_lo_stub3	sap_fields.stub3
#define	E2K_RWSAP_lo_psl	sap_fields.psl
#define	E2K_RWSAP_lo_base	sap_fields.base
#define	E2K_RPUSD_lo_rw		fields.rw
#define	E2K_RPUSD_lo_p		fields.p
#define	E2K_RPUSD_lo_psl	fields.psl
#define	E2K_RPUSD_lo_base	fields.base
#define	E2K_RWSAP_lo_half	word
#define	E2K_RPUSD_lo_half	word

typedef	struct e2k_rwsap_hi_fields {	/* Fields of high word */
	uint64_t	curptr	: 32;			/* [31: 0] */
	uint64_t	size	: 32;			/* [63:32] */
} e2k_rwsap_hi_fields_t;
typedef	union e2k_rwsap_hi_struct {	/* Structure of high word */
	e2k_rwsap_hi_fields_t	fields;	/* as fields */
	uint64_t			word;	/* as entire register */
} e2k_rwsap_hi_struct_t;
#define	E2K_RWSAP_hi_size	fields.size
#define	E2K_RWSAP_hi_curptr	fields.curptr
#define	E2K_RWSAP_hi_half	word

typedef	struct e2k_rwsap_struct {	/* quad-word register */
	e2k_rwsap_lo_struct_t	lo;
	e2k_rwsap_hi_struct_t	hi;
} e2k_rwsap_struct_t;
#define	E2K_RWSAP_lo_struct	lo
#define	E2K_RPUSD_lo_struct	lo
#define	E2K_RWSAP_hi_struct	hi
#define	E2K_RWSAP_itag		lo.E2K_RWSAP_lo_itag
#define	E2K_RWSAP_rw		lo.E2K_RWSAP_lo_rw
#define	E2K_RWSAP_stub1		lo.E2K_RWSAP_lo_stub1
#define	E2K_RWSAP_stub2		lo.E2K_RWSAP_lo_stub2
#define	E2K_RWSAP_stub3		lo.E2K_RWSAP_lo_stub3
#define	E2K_RWSAP_psl		lo.E2K_RWSAP_lo_psl
#define	E2K_RWSAP_base		lo.E2K_RWSAP_lo_base
#define	E2K_RPUSD_rw		lo.E2K_RPUSD_lo_rw
#define	E2K_RPUSD_p		lo.E2K_RPUSD_lo_p
#define	E2K_RPUSD_psl		lo.E2K_RPUSD_lo_psl
#define	E2K_RPUSD_base		lo.E2K_RPUSD_lo_base
#define	E2K_RWSAP_size		hi.E2K_RWSAP_hi_size
#define	E2K_RWSAP_curptr	hi.E2K_RWSAP_hi_curptr
#define	E2K_RWSAP_lo_reg	lo.E2K_RWSAP_lo_half
#define	E2K_RPUSD_lo_reg	lo.E2K_RPUSD_lo_half
#define	E2K_RWSAP_hi_reg	hi.E2K_RWSAP_hi_half

/*
 * Compilation Unit Descriptor (CUD)
 * describes the memory containing codes of the current compilation unit
 */

	/*
	 * Structure of lower word
	 * access CUD.lo.CUD_lo_xxx or CUD -> lo.CUD_lo_xxx
	 *	or CUD_lo.CUD_lo_xxx or CUD_lo -> CUD_lo_xxx
	 */
typedef	e2k_rwap_lo_struct_t	e2k_cud_lo_t;
#define	_CUD_lo_rw	E2K_RWAP_lo_rw		/* [60:59] - read/write flags */
						/* should be "R" */
#define	E2K_CUD_RW_PROTECTIONS			E2_RWAR_R_ENABLE
#define	CUD_lo_c	E2K_RWAP_lo_stub1	/* [58] - checked flag, */
						/* if set then literal CT */
						/* is correct */
#define	E2K_CUD_CHECKED_FLAG			E2_RWAR_C_TRUE
#define	CUD_lo_base	E2K_RWAP_lo_base	/* [47: 0] - base address */
#define	CUD_lo_half	E2K_RWAP_lo_half	/* [63: 0] - entire lower */
						/* double-word of register */
	/*
	 * Structure of high word
	 * access CUD.hi.CUD_hi_xxx or CUD -> hi.CUD_hi_xxx
	 *	or CUD_hi.CUD_hi_xxx or CUD_hi -> CUD_hi_xxx
	 */
typedef	e2k_rwap_hi_struct_t	e2k_cud_hi_t;
#define	CUD_hi_size	E2K_RWAP_hi_size	/* [63:32] - size */
#define	_CUD_hi_curptr	E2K_RWAP_hi_curptr	/* [31: 0] - should be 0 */
#define	CUD_hi_half	E2K_RWAP_hi_half	/* [63: 0] - entire high */
						/* double-word of register */

	/*
	 * Structure of quad-word register
	 * access CUD.CUD_xxx or CUD -> CUD_xxx
	 */
typedef	e2k_rwap_struct_t	cud_struct_t;
#define	_CUD_rw		E2K_RWAP_rw		/* [60:59] - read/write flags */
						/* should be "R" */
#define	CUD_c		E2K_RWAP_stub1		/* [58] - checked flag, */
						/* if set then literal CT */
						/* is correct */
#define	CUD_base	E2K_RWAP_base		/* [47: 0] - base address */
#define	CUD_size	E2K_RWAP_size		/* [63:32] - size */
#define	_CUD_curptr	E2K_RWAP_curptr		/* [31: 0] - should be 0 */
#define	CUD_lo_reg	E2K_RWAP_lo_reg		/* [63: 0] - entire lower */
						/* double-word of register */
#define	CUD_hi_reg	E2K_RWAP_hi_reg		/* [63: 0] - entire high */
						/* double-word of register */
#define	CUD_lo_struct	E2K_RWAP_lo_struct	/* low register structure */
#define	CUD_hi_struct	E2K_RWAP_hi_struct	/* high register structure */
#endif /* !(__ASSEMBLER__) */

#define	E2K_ALIGN_CODES		12		/* Codes area boundaries */
						/* alignment (2's exponent */
						/* value */
#ifndef __ASSEMBLER__
#define	E2K_ALIGN_CODES_MASK	((1UL << E2K_ALIGN_CODES) - 1)
#else	/* __ASSEMBLER__ */
#define	E2K_ALIGN_CODES_MASK	((1 << E2K_ALIGN_CODES) - 1)
#endif /* !(__ASSEMBLER__) */

#ifndef __ASSEMBLER__
/*
 * Compilation Unit Globals Descriptor (GD)
 * describes the global variables memory of the current compilation unit
 */

	/*
	 * Structure of lower word
	 * access GD.lo.GD_lo_xxx or GD -> lo.GD_lo_xxx
	 *	or GD_lo.GD_lo_xxx or GD_lo -> GD_lo_xxx
	 */
typedef	e2k_rwap_lo_struct_t	e2k_gd_lo_t;
#define	_GD_lo_rw	E2K_RWAP_lo_rw		/* [60:59] - read/write flags */
						/* should be "RW" */
#define	E2K_GD_RW_PROTECTIONS			E2_RWAR_RW_ENABLE;
#define	GD_lo_base	E2K_RWAP_lo_base	/* [47: 0] - base address */
#define	GD_lo_half	E2K_RWAP_lo_half	/* [63: 0] - entire lower */
						/* double-word of register */

	/*
	 * Structure of high word
	 * access GD.hi.GD_hi_xxx or GD -> hi.GD_hi_xxx
	 *	or GD_hi.GD_hi_xxx or GD_hi -> GD_hi_xxx
	 */
typedef	e2k_rwap_hi_struct_t	e2k_gd_hi_t;
#define	GD_hi_size	E2K_RWAP_hi_size	/* [63:32] - size */
#define	_GD_hi_curptr	E2K_RWAP_hi_curptr	/* [31: 0] - should be 0 */
#define	GD_hi_half	E2K_RWAP_hi_half	/* [63: 0] - entire high */
						/* double-word of register */

	/*
	 * Structure of quad-word register
	 * access GD.GD_xxx or GD -> GD_xxx
	 */
typedef	e2k_rwap_struct_t	gd_struct_t;
#define	_GD_rw		E2K_RWAP_rw		/* [60:59] - read/write flags */
						/* should be "RW" */
#define	GD_base		E2K_RWAP_base		/* [47: 0] - base address */
#define	GD_size		E2K_RWAP_size		/* [63:32] - size */
#define	_GD_curptr	E2K_RWAP_curptr		/* [31: 0] - should be 0 */
#define	GD_lo_reg	E2K_RWAP_lo_reg		/* [63: 0] - entire lower */
						/* double-word of register */
#define	GD_hi_reg	E2K_RWAP_hi_reg		/* [63: 0] - entire high */
						/* double-word of register */
#define	GD_lo_struct	E2K_RWAP_lo_struct	/* low register structure */
#define	GD_hi_struct	E2K_RWAP_hi_struct	/* high register structure */
#endif /* !(__ASSEMBLER__) */

#define	E2K_ALIGN_GLOBALS	12		/* Globals area boundaries */
						/* alignment (2's exponent */
						/* value */
#define E2K_ALIGN_GLOBALS_SZ	_BITUL(E2K_ALIGN_GLOBALS)
#define	E2K_ALIGN_GLOBALS_MASK	(_BITUL(E2K_ALIGN_GLOBALS) - 1)

#ifndef __ASSEMBLER__
/*
 * OS Compilation Unit Descriptor (OSCUD)
 * describes the global variables memory containing interface codes of the OS
 */

	/*
	 * Structure of lower word
	 * access OSCUD.lo.OSCUD_xxx or OSCUD -> lo.OSCUD_xxx
	 *	or OSCUD_lo.OSCUD_xxx or OSCUD_lo -> OSCUD_xxx
	 */
typedef	e2k_rwap_lo_struct_t	e2k_oscud_lo_t;
#define	_OSCUD_lo_rw	E2K_RWAP_lo_rw		/* [60:59] - read/write flags */
						/* should be "R" */
#define	E2K_OSCUD_RW_PROTECTIONS		E2_RWAR_R_ENABLE;
#define	OSCUD_lo_c	E2K_RWAP_lo_stub1	/* [58] - checked flag, */
						/* if set then literal CT */
						/* is correct */
#define	OSCUD_lo_base	E2K_RWAP_lo_base	/* [47: 0] - base address */
#define	OSCUD_lo_half	E2K_RWAP_lo_half	/* [63: 0] - entire lower */
						/* double-word of register */

#define	OSCUD_lo_base_mask	E2K_VA_MASK

	/*
	 * Structure of high word
	 * access OSCUD.hi.OSCUD_xxx or OSCUD -> hi.OSCUD_xxx
	 *	or OSCUD_hi.OSCUD_xxx or OSCUD_hi -> OSCUD_xxx
	 */
typedef	e2k_rwap_hi_struct_t	e2k_oscud_hi_t;
#define	OSCUD_hi_size	E2K_RWAP_hi_size	/* [63:32] - size */
#define	_OSCUD_hi_curptr \
			E2K_RWAP_hi_curptr	/* [31: 0] - should be 0 */
#define	OSCUD_hi_half	E2K_RWAP_hi_half	/* [63: 0] - entire high */
						/* double-word of register */

	/*
	 * Structure of quad-word register
	 * access OSCUD.OSCUD_xxx or OSCUD -> OSCUD_xxx
	 */
typedef	e2k_rwap_struct_t	oscud_struct_t;
#define	_OSCUD_rw	E2K_RWAP_rw		/* [60:59] - read/write flags */
						/* should be "R" */
#define	OSCUD_c		E2K_RWAP_stub1		/* [58] - checked flag, */
						/* if set then literal CT */
						/* is correct */
#define	OSCUD_base	E2K_RWAP_base		/* [47: 0] - base address */
#define	OSCUD_size	E2K_RWAP_size		/* [63:32] - size */
#define	_OSCUD_curptr	E2K_RWAP_curptr		/* [31: 0] - should be 0 */
#define	OSCUD_lo_reg	E2K_RWAP_lo_reg		/* [63: 0] - entire lower */
						/* double-word of register */
#define	OSCUD_hi_reg	E2K_RWAP_hi_reg		/* [63: 0] - entire high */
						/* double-word of register */
#define	OSCUD_lo_struct	E2K_RWAP_lo_struct	/* low register structure */
#define	OSCUD_hi_struct	E2K_RWAP_hi_struct	/* high register structure */
#endif /* !(__ASSEMBLER__) */

#define	E2K_ALIGN_OSCU		12		/* OS codes area boundaries */
						/* alignment (2's exponent */
						/* value */
#ifndef __ASSEMBLER__
#define	E2K_ALIGN_OSCU_MASK	((1UL << E2K_ALIGN_OSCU) - 1)
#else	/* __ASSEMBLER__ */
#define	E2K_ALIGN_OSCU_MASK	((1 << E2K_ALIGN_OSCU) - 1)
#endif /* !(__ASSEMBLER__) */

#ifndef __ASSEMBLER__
/*
 * OS Compilation Unit Globals Descriptor (OSGD)
 * describes the OS global variables memory
 */

	/*
	 * Structure of lower word
	 * access OSGD.lo.OSGD_lo_xxx or OSGD -> lo.OSGD_lo_xxx
	 *	or OSGD_lo.OSGD_lo_xxx or OSGD_lo -> OSGD_lo_xxx
	 */
typedef	e2k_rwap_lo_struct_t	e2k_osgd_lo_t;
#define	_OSGD_lo_rw	E2K_RWAP_lo_rw		/* [60:59] - read/write flags */
						/* should be "RW" */
#define	E2K_OSGD_RW_PROTECTIONS			E2_RWAR_RW_ENABLE;
#define	OSGD_lo_base	E2K_RWAP_lo_base	/* [47: 0] - base address */
#define	OSGD_lo_half	E2K_RWAP_lo_half	/* [63: 0] - entire lower */
						/* double-word of register */

	/*
	 * Structure of high word
	 * access OSGD.hi.OSGD_hi_xxx or OSGD -> hi.OSGD_hi_xxx
	 *	or OSGD_hi.OSGD_hi_xxx or OSGD_hi -> OSGD_hi_xxx
	 */
typedef	e2k_rwap_hi_struct_t	e2k_osgd_hi_t;
#define	OSGD_hi_size	E2K_RWAP_hi_size	/* [63:32] - size */
#define	_OSGD_hi_curptr	E2K_RWAP_hi_curptr	/* [31: 0] - should be 0 */
#define	OSGD_hi_half	E2K_RWAP_hi_half	/* [63: 0] - entire high */
						/* double-word of register */

	/*
	 * Structure of quad-word register
	 * access OSGD.OSGD_xxx or OSGD -> OSGD_xxx
	 */
typedef	e2k_rwap_struct_t	osgd_struct_t;
#define	_OSGD_rw	E2K_RWAP_rw		/* [60:59] - read/write flags */
						/* should be "RW" */
#define	OSGD_base	E2K_RWAP_base		/* [47: 0] - base address */
#define	OSGD_size	E2K_RWAP_size		/* [63:32] - size */
#define	_OSGD_curptr	E2K_RWAP_curptr		/* [31: 0] - should be 0 */
#define	OSGD_lo_reg	E2K_RWAP_lo_reg		/* [63: 0] - entire lower */
						/* double-word of register */
#define	OSGD_hi_reg	E2K_RWAP_hi_reg		/* [63: 0] - entire high */
						/* double-word of register */
#define	OSGD_lo_struct	E2K_RWAP_lo_struct	/* low register structure */
#define	OSGD_hi_struct	E2K_RWAP_hi_struct	/* high register structure */
#endif /* !(__ASSEMBLER__) */

#define	E2K_ALIGN_OS_GLOBALS	12		/* OS Globals area boundaries */
						/* alignment (2's exponent */
						/* value */
#ifndef __ASSEMBLER__
#define	E2K_ALIGN_OS_GLOBALS_MASK	((1UL << E2K_ALIGN_OS_GLOBALS) - 1)
#else	/* __ASSEMBLER__ */
#define	E2K_ALIGN_OS_GLOBALS_MASK	((1 << E2K_ALIGN_OS_GLOBALS) - 1)
#endif /* !(__ASSEMBLER__) */

#ifndef __ASSEMBLER__
/*
 * Procedure Stack Pointer (PSP)
 * describes the full procedure stack memory as well as the current pointer
 * to the top of a procedure stack memory part.
 */

	/*
	 * Structure of lower word
	 * access PSP.lo.PSP_lo_xxx or PSP -> lo.PSP_lo_xxx
	 *	or PSP_lo.PSP_lo_xxx or PSP_lo -> PSP_lo_xxx
	 */
typedef	e2k_rwap_lo_struct_t	e2k_psp_lo_t;
#define	_PSP_lo_rw	E2K_RWAP_lo_rw		/* [60:59] - read/write flags */
						/* should be "RW" */
#define	E2K_PSP_RW_PROTECTIONS			E2_RWAR_RW_ENABLE;
#define	PSP_lo_base	E2K_RWAP_lo_base	/* [47: 0] - base address */
#define	PSP_lo_half	E2K_RWAP_lo_half	/* [63: 0] - entire lower */
						/* double-word of register */

	/*
	 * Structure of high word
	 * access PSP.hi.PSP_hi_xxx or PSP -> hi.PSP_hi_xxx
	 *	or PSP_hi.PSP_hi_xxx or PSP_hi -> PSP_hi_xxx
	 */
typedef	e2k_rwap_hi_struct_t	e2k_psp_hi_t;
#define	PSP_hi_size	E2K_RPSP_hi_size	/* [63:32] - size */
#define	PSP_hi_ind	E2K_RPSP_hi_ind		/* [31: 0] - index for SPILL */
						/*		and FILL */
#define	PSP_hi_half	E2K_RPSP_hi_half	/* [63: 0] - entire high */
						/* double-word of register */
	/*
	 * Structure of LSR -Loop status register
	*/

typedef struct  e2k_lsr_fields {
	uint64_t	lcnt	: 32;	/* [31: 0] (loop counter) */
	uint64_t	ecnt    :  5;	/* [36:32] (epilogue counter)*/
	uint64_t	vlc	:  1;	/* [37] (loop counter valid bit) */
	uint64_t	over	:  1;	/* [38] */
	uint64_t	ldmc	:  1;	/* [39] (loads manual control)*/
	uint64_t	ldovl	:  8;	/* [47:40] (load overlap)*/
	uint64_t	pcnt	:  5;	/* [52:48] (prologue counter)*/
	uint64_t	strmd	:  7;	/* [59:53] (store remainder counter)*/
	uint64_t	semc	:  1;	/* [60] (side effects manual control */
	uint64_t	unused	:  3;	/* [63:61] */
} e2k_lsr_fields_t;

typedef	union e2k_lsr_struct_t {	/* quad-word register */
	e2k_lsr_fields_t	fields;		/* as fields */
	uint64_t			word;		/* as entire register */
} e2k_lsr_t;

#define	LSR_lcnt		fields.lcnt
#define	LSR_ecnt		fields.ecnt
#define	LSR_vlc			fields.vlc
#define	LSR_over		fields.over
#define	LSR_ldmc		fields.ldmc
#define	LSR_ldovl		fields.ldovl
#define	LSR_pcnt		fields.pcnt
#define	LSR_strmd		fields.strmd
#define	LSR_semc		fields.semc
#define	LSR_reg			word

/*   see C.19.1. */
#define ls_prlg(x)              ((x).LSR_pcnt != 0)
#define ls_lst_itr(x)           ((x).LSR_vlc && ((x).LSR_lcnt < 2))
#define ls_loop_end(x)          (ls_lst_itr(x) && ((x).LSR_ecnt == 0))

#define E2K_LSR_VLC (1UL << 37)

	/*
	 * Structure of ILCR - Initial loop counters register
	 */

typedef struct  e2k_ilcr_fields {
	uint64_t       lcnt	: 32;   /* [31: 0] (loop counter) */
	uint64_t	ecnt    :  5;	/* [36:32] (epilogue counter)*/
	uint64_t	unused1	: 11;	/* [47:37] unused */
	uint64_t	pcnt	:  5;	/* [52:48] (prologue counter)*/
	uint64_t	unused2	: 11;	/* [63:53] unused */
} e2k_ilcr_fields_t;

typedef	union e2k_ilcr {	/* quad-word register */
	e2k_ilcr_fields_t	fields;		/* as fields */
	uint64_t			word;		/* as entire register */
} e2k_ilcr_t;

#define	ILCR_lcnt		fields.lcnt
#define	ILCR_ecnt		fields.ecnt
#define	ILCR_pcnt		fields.pcnt
#define	ILCR_reg		word

/* see C.17.1.2. */
typedef struct  e2k_ct_op_fields {
	uint64_t	psrc	:  5;	/* [4:0] (pointer to condition)*/
	uint64_t	ct	:  4;	/* [8:5] (condition type) */
} e2k_ct_op_fields_t;

typedef	union e2k_ct_struct_t {
	e2k_ct_op_fields_t	fields;	/* as fields */
	uint64_t			word;	/* as entire register */
} e2k_ct_t;

#define	CT_reg(x)		((x).word)
#define CT_PSRC(x)		((x).fields.psrc)
#define CT_CT(x)		((x).fields.ct)

	/*
	 * Structure of quad-word register
	 * access PSP.PSP_xxx or PSP -> PSP_xxx
	 */
typedef	e2k_rwap_struct_t	psp_struct_t;
#define	_PSP_rw		E2K_RWAP_rw		/* [60:59] - read/write flags */
						/* should be "RW" */
#define	PSP_base	E2K_RWAP_base		/* [47: 0] - base address */
#define	PSP_size	E2K_RPSP_size		/* [63:32] - size */
#define	PSP_ind		E2K_RPSP_ind		/* [31: 0] - index for SPILL */
						/*		and FILL */
#define	PSP_lo_reg	E2K_RWAP_lo_reg		/* [63: 0] - entire lower */
						/* double-word of register */
#define	PSP_hi_reg	E2K_RPSP_hi_reg		/* [63: 0] - entire high */
						/* double-word of register */
#define	PSP_lo_struct	E2K_RWAP_lo_struct	/* low register structure */
#define	PSP_hi_struct	E2K_RPSP_hi_struct	/* high register structure */
#endif /* !(__ASSEMBLER__) */

#define	E2K_ALIGN_PSTACK	12		/* Procedure stack boundaries */
						/* alignment (2's exponent */
						/* value) */
#define	E2K_ALIGN_PSTACK_TOP	5		/* Procedure stack top */
						/* boundaries alignment */
						/* (2's exponent value) */
#ifndef __ASSEMBLER__
# define ALIGN_PSTACK_SIZE	(1ULL << E2K_ALIGN_PSTACK)
# define ALIGN_PSTACK_TOP_SIZE	(1ULL << E2K_ALIGN_PSTACK_TOP)
#else	/* __ASSEMBLER__ */
# define ALIGN_PSTACK_SIZE	(1 << E2K_ALIGN_PSTACK)
# define ALIGN_PSTACK_TOP_SIZE	(1 << E2K_ALIGN_PSTACK_TOP)
#endif /* !(__ASSEMBLER__) */

#define	E2K_ALIGN_PSTACK_MASK		(ALIGN_PSTACK_SIZE - 1)
#define	E2K_ALIGN_PSTACK_TOP_MASK	(ALIGN_PSTACK_TOP_SIZE - 1)

#ifndef __ASSEMBLER__
/*
 * Procedure Chain Stack Pointer (PCSP)
 * describes the full procedure chain stack memory as well as the current
 * pointer to the top of a procedure chain stack memory part.
 */
typedef	union {
	struct {
		uint64_t base : E2K_VA_SIZE;
		uint64_t      : 58 - E2K_VA_SIZE;
		uint64_t p    : 1;
		uint64_t rw   : 2;
		uint64_t      : 3;
	};
	e2k_rwap_lo_fields_t	fields;
	uint64_t			word;
} e2k_pcsp_lo_t;
#define	_PCSP_lo_rw	rw
#define	 E2K_PCSR_RW_PROTECTIONS	E2_RWAR_RW_ENABLE;
#define	PCSP_lo_base	base
#define	PCSP_lo_half	word

typedef	union {
	struct {
		uint64_t ind  : 32;
		uint64_t size : 32;
	};
	e2k_rpsp_hi_fields_t	fields;
	uint64_t			word;
} e2k_pcsp_hi_t;
#define	PCSP_hi_size	size
#define	PCSP_hi_ind	ind
#define	PCSP_hi_half	word

	/*
	 * Structure of quad-word register
	 * access PCSP.PCSP_xxx or PCSP -> PCSP_xxx
	 */
typedef	struct {
	union {
		struct {
			uint64_t base : E2K_VA_SIZE;
			uint64_t      : 58 - E2K_VA_SIZE;
			uint64_t p    : 1;
			uint64_t rw   : 2;
			uint64_t      : 3;
		};
		e2k_pcsp_lo_t lo;
	};
	union {
		struct {
			uint64_t ind  : 32;
			uint64_t size : 32;
		};
		e2k_pcsp_hi_t hi;
	};
} pcsp_struct_t;
#define	PCSP_rw		rw
#define	PCSP_base	base
#define	PCSP_size	size
#define	PCSP_ind	ind
#define	PCSP_lo_reg	lo.word
#define	PCSP_hi_reg	hi.word
#define	PCSP_lo_struct	lo
#define	PCSP_hi_struct	hi
#endif /* !(__ASSEMBLER__) */

#define	E2K_ALIGN_PCSTACK	12		/* Procedure chain stack */
						/* boundaries alignment */
						/* (2's exponent value) */
#define	E2K_ALIGN_PCSTACK_TOP	5		/* Procedure chain stack top */
						/* boundaries alignment */
						/* (2's exponent value) */

#ifndef __ASSEMBLER__
# define ALIGN_PCSTACK_SIZE (1ULL << E2K_ALIGN_PCSTACK)
# define ALIGN_PCSTACK_TOP_SIZE (1ULL << E2K_ALIGN_PCSTACK_TOP)
#else
# define ALIGN_PCSTACK_SIZE (1 << E2K_ALIGN_PCSTACK)
# define ALIGN_PCSTACK_TOP_SIZE (1 << E2K_ALIGN_PCSTACK_TOP)
#endif

#define	E2K_ALIGN_PCSTACK_MASK		(ALIGN_PCSTACK_SIZE - 1)
#define	E2K_ALIGN_PCSTACK_TOP_MASK	(ALIGN_PCSTACK_TOP_SIZE - 1)


/*
 * ==========   numeric registers (register file)  ===========
 */

#define	E2K_MAXCR	64			/* The total number of */
						/* chain registers */
#define	E2K_MAXCR_q	E2K_MAXCR		/* The total number of */
						/* chain quad-registers */
#define	E2K_ALIGN_CHAIN_WINDOW	5		/* Chain registers Window */
						/* boundaries alignment */
#define	E2K_CWD_MSB	9			/* The number of the */
						/* most significant bit */
						/* of CWD_base */
#define	E2K_CWD_SIZE	(E2K_CWD_MSB + 1)	/* The number of bits in */
						/* CWD_base field */
#define	E2K_PCSHTP_MSB	(E2K_CWD_MSB + 1)	/* The number of the */
						/* most significant bit */
						/* of PCSHTP */
#define	E2K_PCSHTP_SIZE	(E2K_PCSHTP_MSB + 1)	/* The number of bits in */
						/* PCSHTP */

/* Maximum size to be filled by hardware */
#define E2K_CF_MAX_FILL_FILLC_q (E2K_MAXCR_q - 6)


#ifndef __ASSEMBLER__

/* Current chain registers window descriptor (CWD) */

typedef	unsigned int		e2k_cwd_t;

/*
 * Structure of procedure chain stack hardare top register PCSHTP
 * Register is signed value, so read from register get signed value
 * and write to put signed value.
 */

typedef	unsigned int	e2k_pcshtp_t;

#define	PCSHTP_SIGN_EXTEND(pcshtp) \
		(((s64) (pcshtp) << (s64) (64 - E2K_PCSHTP_SIZE)) \
				 >> (s64) (64 - E2K_PCSHTP_SIZE))

#endif /* !(__ASSEMBLER__) */

#ifndef __ASSEMBLER__
/*
 * User Stack Base Register (USBR/SBR)
 * USBR - contains the base virtual address of the current User Stack area.
 * SBR  - contains the base virtual address of an area dedicated for all user
 * stacks of the current task
 */
typedef	e2k_rwp_struct_t	e2k_usbr_t;
typedef	e2k_rwp_struct_t	e2k_sbr_t;

	/*
	 * Structure of double-word register
	 * access USBR.USBR_xxx or USBR -> USBR_xxx
	 * access SBR.SBR_xxx or SBR -> SBR_xxx
	 */
#define	USBR_base	E2K_RWP_base		/* [47: 0] - base address */
#define	USBR_reg	E2K_RWP_reg		/* [63: 0] - entire */
						/* double-word register */
#define	SBR_base	USBR_base		/* [47: 0] - base address */
#define	SBR_reg		USBR_reg		/* [63: 0] - entire */
						/* double-word register */
#endif /* !(__ASSEMBLER__) */

#define	E2K_ALIGN_STACKS_BASE		12	/* User stacks boundaries */
						/* alignment */
						/* (2's exponent value) */
#define	E2K_ALIGN_ALL_STACKS_BASE	37	/* All User stacks area */
						/* boundaries alignment */
						/* (2's exponent value) */
#define E2K_PROTECTED_STACK_BASE_BITS	32	/* Protected mode stack */
						/* does not cross 4 Gb	*/
						/* boundary.		*/

#define	E2K_ALIGN_STACK_BASE_REG	(1UL << E2K_ALIGN_STACKS_BASE)
#define	E2K_ALIGN_STACKS_BASE_MASK	((1UL << E2K_ALIGN_STACKS_BASE) - 1)
#define	E2K_ALL_STACKS_MAX_SIZE		(1UL << E2K_ALIGN_ALL_STACKS_BASE)
#define	E2K_PROTECTED_STACK_BASE_MASK \
	((1UL << E2K_PROTECTED_STACK_BASE_BITS) - 1)

#ifndef __ASSEMBLER__

/*
 * Non-Protected User Stack Descriptor (USD)
 * contains free memory space dedicated for user stack data and
 * is supposed to grow from higher memory addresses to lower ones
 */

	/*
	 * Structure of lower word
	 * access USD.lo.USD_lo_xxx or USD -> lo.USD_lo_xxx
	 *	or USD.USD_lo_xxx or USD -> USD_lo_xxx
	 */
typedef	e2k_rwap_lo_struct_t	e2k_usd_lo_t;
#define	_USD_lo_rw	E2K_RUSD_lo_rw		/* [60:59] - read/write flags */
						/* should be "RW" */
#define	USD_lo_p	E2K_RUSD_lo_p		/* [58] - flag of "protected" */
						/* mode: should be */
						/* 0 - non-protected */
#define	USD_lo_p_bit	E2K_RUSD_lo_p_bit	/* protected flag as value */
#define	USD_lo_p_flag	(1UL << USD_lo_p_bit)

#define	USD_lo_base	E2K_RUSD_lo_base	/* [47: 0] - base address */
#define	USD_lo_half	E2K_RUSD_lo_half	/* [63: 0] - entire lower */
						/* double-word of register */

	/*
	 * Structure of high word
	 * access USD.hi.USD_hi_xxx or USD -> hi.USD_hi_xxx
	 *	or USD_hi.USD_hi_xxx or USD_hi -> USD_hi_xxx
	 */
typedef	e2k_rwap_hi_struct_t	e2k_usd_hi_t;
#define	USD_hi_size	E2K_RWAP_hi_size	/* [63:32] - size */
#define	_USD_hi_curptr	E2K_RWAP_hi_curptr	/* [31: 0] - should be 0 */
#define	USD_hi_half	E2K_RWAP_hi_half	/* [63: 0] - entire high */
						/* double-word of register */

#define MAX_USD_HI_SIZE	(4ULL * 1024 * 1024 * 1024 - 1ULL)

	/*
	 * Structure of quad-word register
	 * access USD.USD_xxx or USD -> USD_xxx
	 */
typedef	e2k_rwap_struct_t	usd_struct_t;
#define	_USD_rw		E2K_RUSD_rw		/* [60:59] - read/write flags */
						/* should be "RW" */
#define	USD_p		E2K_RUSD_p		/* [58] - flag of "protected" */
						/* mode: 1 - protected */
#define	USD_base	E2K_RUSD_base		/* [31: 0] - base address */
#define	USD_size	E2K_RWAP_size		/* [63:32] - size */
#define	_USD_curptr	E2K_RWAP_curptr		/* [31: 0] - should be 0 */
#define	USD_lo_reg	E2K_RUSD_lo_reg		/* [63: 0] - entire lower */
						/* double-word of register */
#define	USD_hi_reg	E2K_RWAP_hi_reg		/* [63: 0] - entire high */
						/* double-word of register */
#define	USD_lo_struct	E2K_RUSD_lo_struct	/* low register structure */
#define	USD_hi_struct	E2K_RWAP_hi_struct	/* high register structure */

/*
 * Protected User Stack Descriptor (PUSD)
 * contains free memory space dedicated for user stack data and
 * is supposed to grow from higher memory addresses to lower ones
 */

	/*
	 * Structure of lower word
	 * access PUSD.lo.PUSD_lo_xxx or PUSD -> lo.PUSD_lo_xxx
	 *	or PUSD.PUSD_lo_xxx or PUSD -> PUSD_lo_xxx
	 */
typedef	e2k_rwsap_lo_struct_t	e2k_pusd_lo_t;
#define	_PUSD_lo_rw	E2K_RPUSD_lo_rw		/* [60:59] - read/write flags */
						/* should be "RW" */
#define	PUSD_lo_p	E2K_RPUSD_lo_p		/* [58] - flag of "protected" */
						/* mode: should be */
						/* 1 - protected */
#define	PUSD_lo_psl	E2K_RPUSD_lo_psl	/* {47:32} - dynamic level of */
						/* the current procedure in a */
						/* stack of called procedures */
#define	PUSD_lo_base	E2K_RPUSD_lo_base	/* [31: 0] - base address */
#define	PUSD_lo_half	E2K_RPUSD_lo_half	/* [63: 0] - entire lower */
						/* double-word of register */

	/*
	 * Structure of high word
	 * access PUSD.hi.PUSD_hi_xxx or PUSD -> hi.PUSD_hi_xxx
	 *	or PUSD_hi.PUSD_hi_xxx or PUSD_hi -> PUSD_hi_xxx
	 */
typedef	e2k_rwsap_hi_struct_t	e2k_pusd_hi_t;
#define	PUSD_hi_size	E2K_RWSAP_hi_size	/* [63:32] - size */
#define	_PUSD_hi_curptr	E2K_RWSAP_hi_curptr	/* [31: 0] - should be 0 */
#define	PUSD_hi_half	E2K_RWSAP_hi_half	/* [63: 0] - entire high */
						/* double-word of register */

	/*
	 * Structure of quad-word register
	 * access PUSD.PUSD_xxx or PUSD -> PUSD_xxx
	 */
typedef	e2k_rwsap_struct_t	pusd_struct_t;
#define	_PUSD_rw	E2K_RPUSD_rw		/* [60:59] - read/write flags */
						/* should be "RW" */
#define	PUSD_p		E2K_RPUSD_p		/* [58] - flag of "protected" */
						/* mode: should be */
						/* 1 - protected */
#define	PUSD_psl	E2K_RPUSD_psl		/* {47:32} - dynamic level of */
						/* the current procedure in a */
						/* stack of called procedures */
#define	PUSD_base	E2K_RUSD_base		/* [31: 0] - base address */
#define	PUSD_size	E2K_RWSAP_size		/* [63:32] - size */
#define	_PUSD_curptr	E2K_RWSAP_curptr	/* [31: 0] - should be 0 */
#define	PUSD_lo_reg	E2K_RPUSD_lo_reg	/* [63: 0] - entire lower */
						/* double-word of register */
#define	PUSD_hi_reg	E2K_RWSAP_hi_reg	/* [63: 0] - entire high */
						/* double-word of register */
#define	PUSD_lo_struct	E2K_RUSD_lo_struct	/* low register structure */
#define	PUSD_hi_struct	E2K_RWSAP_hi_struct	/* high register structure */


#endif /* !(__ASSEMBLER__) */

#define	E2K_ALIGN_USTACK	4		/* Non-Protected User Stack */
						/* boundaries alignment */
						/* (2's exponent value) */
#define	E2K_ALIGN_PUSTACK	5		/* Protected User Stack */
						/* boundaries alignment */
						/* (2's exponent value) */

#define E2K_ALIGN_USTACK_SIZE	(1UL << E2K_ALIGN_USTACK)
#define E2K_ALIGN_PUSTACK_SIZE	(1UL << E2K_ALIGN_PUSTACK)

/*
 * This should be
 *	max(E2K_ALIGN_USTACK_SIZE, E2K_ALIGN_PUSTACK_SIZE)
 * but we want it to be constant
 */
#define E2K_ALIGN_STACK		32UL

#ifndef __ASSEMBLER__
#define	E2K_ALIGN_USTACK_MASK	((1UL << E2K_ALIGN_USTACK) - 1)
#define E2K_ALIGN_PUSTACK_MASK   ((1UL << E2K_ALIGN_PUSTACK) - 1)
#else	/* __ASSEMBLER__ */
#define	E2K_ALIGN_USTACK_MASK	((1 << E2K_ALIGN_USTACK) - 1)
#define E2K_ALIGN_PUSTACK_MASK   ((1 << E2K_ALIGN_PUSTACK) - 1)
#endif /* !(__ASSEMBLER__) */

#ifndef __ASSEMBLER__

/*
 * Instruction structure
 */

typedef	uint64_t		instr_item_t;	/* min. item of instruction */
					/* is double-word */

#define	E2K_INSTR_MAX_SYLLABLES_NUM	8	/* max length of instruction */
						/* in terms of min item of */
						/* instruction */
#define	E2K_INSTR_MAX_SIZE		(E2K_INSTR_MAX_SYLLABLES_NUM * \
						sizeof(instr_item_t))

/* Asynchronous program instruction 'fapb' is always 16 bytes long */
#define E2K_ASYNC_INSTR_SIZE		16
/* Asynchronous program can contain maximum 32 instructions */
#define MAX_ASYNC_PROGRAM_INSTRUCTIONS	32

typedef	uint16_t		instr_semisyl_t; /* instruction semi-syllable */
					/* is short */

typedef	uint32_t		instr_syl_t;	/* instruction syllable */
					/* is word */
/*
 * Order of fixed syllables of instruction
 */
#define	E2K_INSTR_HS_NO		0	/* header syllable */
#define E2K_INSTR_SS_NO		1	/* stubs syllable (if present) */

#define	E2K_GET_INSTR_SEMISYL(instr_addr, semisyl_no)			\
		(((instr_semisyl_t *)(instr_addr))			\
			[((semisyl_no) & 0x1) ? ((semisyl_no) - 1) :	\
						((semisyl_no) + 1)])
#define	E2K_GET_INSTR_SYL(instr_addr, syl_no)	\
		(((instr_syl_t *)(instr_addr))[syl_no])

#define	E2K_GET_INSTR_HS(instr_addr)	E2K_GET_INSTR_SYL(instr_addr, \
							E2K_INSTR_HS_NO)
#define	E2K_GET_INSTR_SS(instr_addr)	E2K_GET_INSTR_SYL(instr_addr, \
							E2K_INSTR_SS_NO)
#define E2K_GET_INSTR_ALS0(instr_addr, ss_flag)				\
		E2K_GET_INSTR_SYL(instr_addr,				\
					(ss_flag) ? E2K_INSTR_SS_NO + 1 \
							:		\
							E2K_INSTR_SS_NO)
#define E2K_GET_INSTR_ALES0(instr_addr, mdl)				\
		E2K_GET_INSTR_SEMISYL(instr_addr, ((mdl) + 1) * 2)

/*
 * Header syllable structure
 */

typedef	union instr_hs {
	struct {
		uint32_t mdl	: 4;	/* [ 3: 0] middle pointer in terms of */
				/*	   syllables - 1 */
		uint32_t lng	: 3;	/* [ 6: 4] length of instruction in */
				/*	   terms of double-words - 1 */
		uint32_t nop	: 3;	/* [ 9: 7] no operation code */
		uint32_t lm	: 1;	/*    [10] loop mode flag */
		uint32_t x	: 1;	/*    [11] unused field */
		uint32_t s	: 1;	/*    [12] Stubs syllable presence bit */
		uint32_t sw	: 1;	/*    [13] bit used by software */
		uint32_t c	: 2;	/* [15:14] Control syllables presence */
				/*	   mask */
		uint32_t cd	: 2;	/* [17:16] Conditional execution */
				/*	   syllables number */
		uint32_t pl	: 2;	/* [19:18] Predicate logic channel */
				/*	   syllables number */
		uint32_t ale	: 6;	/* [25:20] Arithmetic-logic channel */
				/*	   syllable extensions */
				/*	   presence mask */
		uint32_t al	: 6;	/* [31:26] Arithmetic-logic channel */
				/*	   syllables presence mask */
	};
	struct {
		uint32_t __pad	: 14;
		uint32_t c0		: 1;	/* CS0 */
		uint32_t c1		: 1;	/* CS1 */
		uint32_t __pad2	: 4;
		uint32_t ale0	: 1;
		uint32_t ale1	: 1;
		uint32_t ale2	: 1;
		uint32_t ale3	: 1;
		uint32_t ale4	: 1;
		uint32_t ale5	: 1;
		uint32_t al0		: 1;
		uint32_t al1		: 1;
		uint32_t al2		: 1;
		uint32_t al3		: 1;
		uint32_t al4		: 1;
		uint32_t al5		: 1;
	};
	instr_syl_t word;		/* as entire syllable	*/
} instr_hs_t;

#define E2K_INSTR_HS_LNG_MASK	0x70

#define	E2K_GET_INSTR_SIZE(hs)	((hs.lng + 1) * sizeof(instr_item_t))

/*
 * Stubs sullable structure
 */

typedef	union instr_ss {
	struct {
		uint32_t	ctcond	:  9;	/* [ 8: 0] control transfer condition */
		uint32_t	x	:  1;	/* [    9] unused field */
		uint32_t	ctop	:  2;	/* [11:10] control transfer opcode */
		uint32_t	aa	:  4;	/* [15:12] mask of AAS */
		uint32_t	alc	:  2;	/* [17:16] advance loop counters */
		uint32_t	abp	:  2;	/* [19:18] advance predicate base */
		uint32_t	xx	:  1;	/*    [20] unused field */
		uint32_t	abn	:  2;	/* [22:21] advance numeric base */
		uint32_t	abg	:  2;	/* [24:23] advance global base */
		uint32_t	xxx	:  1;	/*    [25] unused field */
		uint32_t	vfdi	:  1;	/*    [26] verify deferred interrupt */
		uint32_t	srp	:  1;	/*    [27] store recovery point */
		uint32_t	bap	:  1;	/*    [28] begin array prefetch */
		uint32_t	eap	:  1;	/*    [29] end array prefetch */
		uint32_t	ipd	:  2;	/* [31:30] instruction prefetch depth */
	};
	instr_syl_t	word;		/* as entire syllable	*/
} instr_ss_t;

/*
 * ALU syllables structure
 */

typedef	union {
	union {
		struct {
			uint32_t dst  : 8; /* [ 7: 0] destination */
			uint32_t src2 : 8; /* [15: 8] source register #2 */
			uint32_t opce : 8; /* [23:16] opcode extension */
			uint32_t cop  : 7; /* [30:24] code of operation */
			uint32_t spec : 1; /*    [31] speculative mode */
		};
		struct {
			uint32_t     : 24;
			uint32_t opc : 8;
		};
	} alf2;
	instr_syl_t word;		/* as entire syllable	*/
} instr_als_t;

typedef	union instr_ales {
	struct {
		uint16_t src3 :  8;
		uint16_t opc2 :  8;
	} alef1;
	struct {
		uint16_t opce :  8;
		uint16_t opc2 :  8;
	} alef2;
	instr_semisyl_t word;		/* as entire syllable	*/
} instr_ales_t;

typedef union {
	struct {
		uint8_t __pad : 5;
		uint8_t rt5 : 1;
		uint8_t rt6 : 1;
		uint8_t rt7 : 1;
	};
	uint8_t word;
} instr_src_t;

#define INSTR_SRC2_GREG_VALUE		0xe0
#define INSTR_SRC2_GREG_MASK		0xe0
#define INSTR_SRC2_GREG_NUM_MASK	0x1f
#define INSTR_SRC2_16BIT_VALUE		0xd0
#define INSTR_SRC2_32BIT_VALUE		0xd8
#define INSTR_SRC2_BIT_MASK		0xf8
#define INSTR_SRC2_LTS_NUM_MASK		0x03
#define INSTR_SRC2_LTS_SHIFT_MASK	0x04
#define INSTR_LTS_32BIT_SHIFT		0
#define INSTR_LTS_16BIT_SHIFT		16
#define INSTR_LTS_16BIT_NOSHIFT		0
#define INSTR_LTS_32BIT_MASK		0xffffffff
#define INSTR_LTS_16BIT_SHIFT_MASK	0xffff0000
#define INSTR_LTS_16BIT_NOSHIFT_MASK	0x0000ffff

/*
 * ALU syllable code of operations and opcode extentions
 */
#define	DRTOAP_ALS_COP		0x62		/* DRTOAP */
#define	GETSP_ALS_COP		0x58		/* GETSP */
#define	GETSOD_ALS_COP		0x5a		/* GETSOP */
#define	EXT_ALES_OPC2		0x01		/* EXTension  */
#define	USD_ALS_OPCE		0xec		/* USD  */

/*
 * CS0 syllable structure
 */

typedef	union {
	struct {
		uint32_t prefr	: 3;
		uint32_t ipd		: 1;
		uint32_t pdisp	: 24;
		uint32_t __pad	: 4;
	} pref;
	struct {
		uint32_t param	: 28;
		uint32_t ctp_opc	: 2;
		uint32_t ctpr	: 2;
	} cof1;
	struct {
		uint32_t disp	: 28;
		uint32_t ctp_opc	: 2;
		uint32_t ctpr	: 2;
	} cof2;
	struct {
		uint32_t __pad1	: 28;
		uint32_t opc		: 4;
	};
	struct {
		uint32_t __pad2	: 28;
		uint32_t ctp_opc	: 2;
		uint32_t ctpr	: 2;
	};
	instr_syl_t word;
} instr_cs0_t;

#define CS0_CTP_OPC_IBRANCH	0
#define CS0_CTP_OPC_DISP	0
#define CS0_CTP_OPC_LDISP	1
#define CS0_CTP_OPC_PREF	1
#define CS0_CTP_OPC_PUTTSD	2


/*
 * CS1 syllable structure
 */

typedef	union {
	struct {
		uint32_t __pad1	: 27;
		uint32_t sft		: 1;
		uint32_t __pad2	: 4;
	};
	struct {
		uint32_t param	: 28;
		uint32_t opc		: 4;
	};
	instr_syl_t word;
} instr_cs1_t;

#define CS1_OPC_SETEI	2
#define CS1_OPC_WAIT	3
#define CS1_OPC_CALL	5


/*
 * ==========   numeric registers (register file)  ===========
 */

#define	E2K_MAXNR	128			/* The total number of */
						/* quad-NRs */
#define	E2K_MAXGR	16			/* The total number of global */
						/* quad-NRs */
#define	E2K_MAXSR	(E2K_MAXNR - E2K_MAXGR)	/* The total number of stack */
						/* quad-NRs */
#define	E2K_MAXNR_d	(E2K_MAXNR * 2)		/* The total number of */
						/* double-NRs */
#define	E2K_MAXGR_d	(E2K_MAXGR * 2)		/* The total number of global */
						/* double-NRs */
#define	E2K_MAXSR_d	(E2K_MAXSR * 2)		/* The total number of stack */
						/* double-NRs */
#define	E2K_ALIGN_WINDOW	4		/* Window boundaries */
						/* alignment */
#define	E2K_WD_MSB	10			/* The number of bits in WD */
						/* fields */
#define	E2K_WD_SIZE	(E2K_WD_MSB + 1)	/* The number of bits in WD */
						/* fields */
#define	E2K_NR_SIZE	16			/* Byte size of quad-NR */

/* Total size of registers file (local stack registers + global registers) */
#define	MAX_NRF_SIZE	(E2K_MAXNR * E2K_NR_SIZE)
/* Size of local stack registers file */
#define	MAX_SRF_SIZE	(E2K_MAXSR * E2K_NR_SIZE)

struct e2k_wd_fields {
	uint64_t base    : E2K_WD_SIZE;	/* [10: 0] window base: */
					/* %r0 physical address */
	uint64_t unused1 : 16 - E2K_WD_SIZE;	/* [15:11] */
	uint64_t size    : E2K_WD_SIZE;	/* [26:16] window size */
	uint64_t unused2 : 16 - E2K_WD_SIZE;	/* [31:27] */
	uint64_t psize   : E2K_WD_SIZE;	/* [42:32] parameters area */
					/* size */
	uint64_t unused3 : 16 - E2K_WD_SIZE;	/* [47:43] */
	uint64_t fx      : 1;		/* [48]    spill/fill */
					/* extended flag; indicates */
					/* that the current procedure */
					/* has variables of FX type */
	uint64_t unused4 : 15;		/* [63:49] unused field */
};

/* Current window descriptor (WD) */
typedef	union e2k_wd {
	struct {
		uint64_t		: 3;
		uint64_t base_d	: E2K_WD_SIZE - 3;
		uint64_t		: 16 - E2K_WD_SIZE + 3;
		uint64_t size_d	: E2K_WD_SIZE - 3;
		uint64_t		: 16 - E2K_WD_SIZE + 3;
		uint64_t psize_d	: E2K_WD_SIZE - 3;
		uint64_t		: 32 - E2K_WD_SIZE;
	};
	struct {
		uint64_t base    : E2K_WD_SIZE;	/* [10: 0] window base: */
						/* %r0 physical address */
		uint64_t unused1 : 16 - E2K_WD_SIZE;	/* [15:11] */
		uint64_t size    : E2K_WD_SIZE;	/* [26:16] window size */
		uint64_t unused2 : 16 - E2K_WD_SIZE;	/* [31:27] */
		uint64_t psize   : E2K_WD_SIZE;	/* [42:32] parameters area */
						/* size */
		uint64_t unused3 : 16 - E2K_WD_SIZE;	/* [47:43] */
		uint64_t fx      : 1;		/* [48]    spill/fill */
						/* extended flag; indicates */
						/* that the current procedure */
						/* has variables of FX type */
		uint64_t dbl     : 1;		/* [49]  */
		uint64_t unused4 : 14;		/* [63:50] unused field */
	};
	struct e2k_wd_fields fields;
	uint64_t word;		/* as entire opcode	*/
} e2k_wd_t;

#define	WD_base			base
#define	WD_size			size
#define	WD_psize		psize
#define	WD_fx			fx
#define	WD_reg			word

/* Structure of dword register PSHTP */
typedef	struct e2k_pshtp_fields {		/* PSHTP fields */
	uint64_t	ind	: E2K_WD_SIZE + 1;	/* [WD_MSB + 1 : 0] */
	uint64_t	unused1	: 16 - E2K_WD_SIZE - 1;	/* [15: WD_MSB + 2] */
	uint64_t	fxind	: E2K_WD_SIZE;		/* [16 + WD_MSB : 16] */
	uint64_t	unused2	: 32 - E2K_WD_SIZE - 16;/* [31: 16+ WD_MSB + 1] */
	uint64_t	tind	: E2K_WD_SIZE;		/* [32 + WD_MSB : 32] */
	uint64_t	unused3	: 48 - E2K_WD_SIZE - 32;/* [47: 32+ WD_MSB + 1] */
	uint64_t	fx	:  1;			/* [48 : 48] */
	uint64_t	unused4	: 15;			/* [63 : 49] */
} e2k_pshtp_fields_t;

typedef	union e2k_pshtp_struct {		/* Register */
	e2k_pshtp_fields_t	fields;		/* as fields */
	uint64_t			word;		/* as entire register */
} e2k_pshtp_t;

#define	PSHTP_ind		fields.ind
#define	PSHTP_tind		fields.tind
#define	PSHTP_fxind		fields.fxind
#define	PSHTP_fx		fields.fx
#define	PSHTP_reg		word

/*
 * PSHTP register contains index in terms of double-numeric registers
 * PSP register contains index in terms of extended double-numeric
 * registers spilled into memory - each double-numeric register occupy
 * two double words: one for main part and second for extension.
 * So it need some conversion to operate with PSP_ind and PSHTP_ind in
 * common terms.
 */
#define	PSHTP_IND_TO_PSP(nr_nums)	((nr_nums) * 2)
#define	PSP_IND_TO_PSHTP(mem_ind)	((mem_ind) / 2)
#define	PSHTP_IND_SIGN_EXTEND(pshtp) \
		(((s64) (pshtp) << (s64) (64 - (E2K_WD_SIZE + 1))) \
				>> (s64) (64 - (E2K_WD_SIZE + 1)))
#define	PSHTP_MEM_SIGN_EXTEND(pshtp) \
		(((s64) (pshtp) << (s64) (64 - (E2K_WD_SIZE + 1))) \
				>> (s64) (64 - (E2K_WD_SIZE + 1) - 1))
#define	PSHTP_Q_SIGN_EXTEND(pshtp) \
		(((s64) (pshtp) << (s64) (64 - (E2K_WD_SIZE + 1))) \
				>> (s64) (64 - (E2K_WD_SIZE + 1) + 4))
#define	GET_PSHTP_NR_INDEX(pshtp) ((uint64_t) PSHTP_IND_SIGN_EXTEND(AW(pshtp)))
#define	SET_PSHTP_NR_INDEX(pshtp, signed_nr_nums)			\
		((pshtp).PSHTP_ind = (signed_nr_nums))
#define	GET_PSHTP_MEM_INDEX(pshtp) ((uint64_t) PSHTP_MEM_SIGN_EXTEND(AW(pshtp)))
#define	SET_PSHTP_MEM_INDEX(pshtp, mem_ind)				\
		SET_PSHTP_NR_INDEX(pshtp, PSP_IND_TO_PSHTP(mem_ind))
#define	GET_PSHTP_Q_INDEX(pshtp) ((uint64_t) PSHTP_Q_SIGN_EXTEND(AW(pshtp)))


/* Numeric Register in a rotatable area: %br# or %dbr# (OPCODE) */
typedef	struct e2k_nbr_fields {
	uint8_t	index	: 7;			/* [ 6: 0] NR index in a */
						/*	   rotatable area */
	uint8_t	rt7	: 1;			/* [ 7]	   should be 0 */
} e2k_nbr_fields_t;
typedef	union e2k_nbr {
	e2k_nbr_fields_t	fields;		/* as fields		*/
	uint8_t			word;		/* as entire opcode	*/
} e2k_nbr_t;

/* Numeric Register in a window: %r# or %dr# (OPCODE) */
typedef	struct e2k_nr_fields {
	uint8_t	index	: 6;			/* [ 5: 0] NR index in a */
						/*	   window */
	uint8_t	rt6	: 1;			/* [ 6]	   should be 0 */
	uint8_t	rt7	: 1;			/* [ 7]	   should be 1 */
} e2k_nr_fields_t;
typedef	union e2k_nr {
	e2k_nr_fields_t		fields;		/* as fields		*/
	uint8_t			word;		/* as entire opcode	*/
} e2k_nr_t;

/* Numeric results */
/* Result destination (destination(ALS.dst)) is encoded in dst fields */
/* of ALS or AAS syllables as follows: */

typedef	union e2k_dst {
	e2k_nbr_t		nbr;		/* as rotatable register */
	e2k_nr_t		nr;		/* as window register */
	uint8_t			word;		/* as entire opcode	*/
} e2k_dst_t;

#define	DST_IS_NBR(dst)		(AS_STRUCT(dst.nbr).rt7 == 0)
#define	DST_IS_NR(dst)		(AS_STRUCT(dst.nr).rt7 == 1 && \
					AS_STRUCT(dst.nr).rt6 == 0)
#define	DST_NBR_INDEX(dst)	(AS_STRUCT(dst.nbr).index)
#define	DST_NR_INDEX(dst)	(AS_STRUCT(dst.nr).index)
#define	DST_NBR_RNUM_d(dst)	DST_NBR_INDEX(dst)
#define	DST_NR_RNUM_d(dst)	DST_NR_INDEX(dst)

/* The effective address of NR in a rotatable area (in terms of double-NR) */
#define	NBR_IND_d(BR, rnum_d)	(AS_STRUCT(BR).rbs * 2 + \
					(AS_STRUCT(BR).rcur * 2 + rnum_d) % \
						(AS_STRUCT(BR).rsz * 2 + 2))
#define	NBR_REA_d(WD, ind_d)	((AS_STRUCT(WD).base / 8 + ind_d) % \
					E2K_MAXSR_d)

/* The effective address of NR in a window (in terms of double-NR) */
#define	NR_REA_d(WD, rnum_d)	((AS_STRUCT(WD).base / 8 + rnum_d) % \
					E2K_MAXSR_d)


/*
 * ==========   chain regs & usd regs    ===========
 * To work with reg as with word use AS_WORD
 * To work with reg as with struct use AS_STRUCT
 */


#define AS_WORD(x)		((x).word)
#define AS_STRUCT(x)		((x).fields)
#define AS_V3_STRUCT(x)		((x).v3_fields)
#define AS_V6_STRUCT(x)		((x).v6_fields)
#define AS_SAP_STRUCT(x)	((x).sap_fields)
#define AS_AP_STRUCT(x)		((x).ap_fields)
#define AS_WORD_P(xp)		((xp)->word)
#define AS_STRUCT_P(xp)		((xp)->fields)
#define AS_SAP_STRUCT_P(xp)	((xp)->sap_fields)
#define AS_AP_STRUCT_P(xp)	((xp)->ap_fields)

#define AW(x)	AS_WORD(x)
#define AS(x)	AS_STRUCT(x)
#define AWP(xp)	AS_WORD_P(xp)
#define ASP(xp)	AS_STRUCT_P(xp)

/* BR */
typedef	struct e2k_br_fields {	/* Structure of br reg */
	uint32_t	rbs	: 6;		/* [ 5: 0]	*/
	uint32_t	rsz	: 6;		/* [11: 6]	*/
	uint32_t	rcur	: 6;		/* [17:12]	*/
	uint32_t	psz	: 5;		/* [22:18]	*/
	uint32_t	pcur	: 5;		/* [27:23]	*/
} e2k_br_fields_t;
typedef	union e2k_br {
	struct {
		uint32_t rbs  : 6;
		uint32_t rsz  : 6;
		uint32_t rcur : 6;
		uint32_t psz  : 5;
		uint32_t pcur : 5;
	};
	e2k_br_fields_t	fields;		/* as fields		*/
	uint32_t		word;		/* as entire register	*/
} e2k_br_t;
#define	BR_rbs		fields.rbs
#define	BR_rsz		fields.rsz
#define	BR_rcur		fields.rcur
#define	BR_psz		fields.psz
#define	BR_pcur		fields.pcur
#define	BR_reg		word

static inline int br_rsz_full_d(e2k_br_t br)
{
	return 2 * (br.rsz + 1);
}

/* see 5.25.1. */

typedef	union e2k_rpr_lo_struct {
	e2k_rwp_fields_t	fields;	/* as fields */
	uint64_t			word;	/* as entire register */
} e2k_rpr_lo_t;

#define	RPR_lo_reg(rpr_lo)	((rpr_lo).word)
#define	RPR_lo_ip	E2K_RWP_base		/* [47: 0] - IP of trap */
#define	RPR_lo_stp	E2K_RWP_stub1		/* [58] - store pointer */
#define	RPR_lo_half	BR_reg

typedef union e2k_rpr_hi_struct {
	e2k_br_fields_t		fields;	/* as fields */
	uint64_t			word;	/* as entire register */
} e2k_rpr_hi_t;

#define	RPR_hi_reg(rpr_hi)	((rpr_hi).word)
#define	RPR_hi_rbs		BR_rbs
#define	RPR_hi_rsz		BR_rsz
#define	RPR_hi_rcur		BR_rcur
#define	RPR_hi_psz		BR_psz
#define	RPR_hi_pcur		BR_pcur
#define	RPR_hi_half		BR_reg

#define RPR_IP(x)		((x).RPR_lo_ip)
#define RPR_STP(x)		((x).RPR_lo_stp)
#define RPR_BR_CUR(x)		((x).RPR_hi_rcur)
#define RPR_BR_PCUR(x)		((x).RPR_hi_pcur)

/*
 * BGR. Rotation base of global registers.
 * 11 bits wide. Rounded to 32-bit, because 16-bit memory & sysreg access
 * makes no sense in this case
 */
typedef	struct e2k_bgr_fields {	/* Structure of bgr reg */
	uint32_t	val	: 8;		/* [ 7: 0]	*/
	uint32_t	cur	: 3;		/* [10: 8]	*/
} e2k_bgr_fields_t;
typedef	union e2k_bgr {
	e2k_bgr_fields_t	fields;	/* as fields		*/
	uint32_t			word;	/* as entire register	*/
} e2k_bgr_t;

#define	BGR_val		fields.val
#define	BGR_cur		fields.cur
#define	BGR_reg		word

#define	E2K_INITIAL_BGR_VAL	0xff
#define	E2K_INITIAL_BGR		((e2k_bgr_t) { {cur : 0, val : 0xff} })


#define	E2K_GB_START_REG_NO_d	24
#define	E2K_GB_REGS_NUM_d	(E2K_MAXGR_d - E2K_GB_START_REG_NO_d)


/* CR0 */

typedef	struct {	/* Structure of cr0_hi chain reg */
	uint64_t	unused	: 3;		/* [ 2: 0]	*/
	uint64_t	ip	: 61;		/* [63: 3]	*/
} e2k_cr0_hi_fields_t;
typedef	union {
	struct {
		uint64_t    : 3;
		uint64_t ip : 61;
	};
	e2k_cr0_hi_fields_t	fields;	/* as fields		*/
	uint64_t			word;	/* as entire register	*/
} e2k_cr0_hi_t;
#define	CR0_hi_ip	fields.ip	/* [63: 3] - IP >> 3 */
#define	CR0_hi_half	word		/* [63: 0] - entire high */
#define	CR0_hi_IP	CR0_hi_half	/* [63: 0] - IP */

typedef	struct {	/* Structure of cr0_lo chain reg */
	uint64_t	pf	: 64;		/* [63: 0]	*/
} e2k_cr0_lo_fields_t;
typedef	union {
	e2k_cr0_lo_fields_t	fields;	/* as fields		*/
	uint64_t			word;	/* as entire register	*/
} e2k_cr0_lo_t;
#define	CR0_lo_pf	fields.pf	/* [63: 0] - predicates file */
#define	CR0_lo_half	word		/* [63: 0] - entire high */

/* CR1 */

typedef	union {	/* Structure of cr1_hi chain reg */
	struct {
		uint64_t br		: 28;	/* [27: 0]	*/
		uint64_t unused	: 7;	/* [34:28]	*/
		uint64_t wdbl	: 1;	/* [35:35]	*/
		uint64_t ussz	: 28;	/* [63:36]	*/
	};
	struct {
		uint64_t rbs		: 6;	/* [5 :0 ]	*/
		uint64_t rsz		: 6;	/* [11:6 ]	*/
		uint64_t rcur	: 6;	/* [17:12]	*/
		uint64_t psz		: 5;	/* [22:18]	*/
		uint64_t pcur	: 5;	/* [27:23]	*/
		uint64_t __x1	: 36;	/* [63:28]	*/
	};
} e2k_cr1_hi_fields_t;
typedef	union {
	struct {
		uint64_t br   : 28;
		uint64_t      : 7;
		uint64_t wdbl : 1;
		uint64_t ussz : 28;
	};
	struct {
		uint64_t rbs  : 6;
		uint64_t rsz  : 6;
		uint64_t rcur : 6;
		uint64_t psz  : 5;
		uint64_t pcur : 5;
		uint64_t      : 36;
	};
	e2k_cr1_hi_fields_t	fields;	/* as fields		*/
	uint64_t			word;	/* as entire register	*/
} e2k_cr1_hi_t;
#define	CR1_hi_br	fields.br	/* [27: 0] - base of rotate regs */
#define	CR1_hi_wdbl	fields.wdbl	/* [63:36] - ??? */
#define	CR1_hi_ussz	fields.ussz	/* [63:36] - user stack size */
#define	CR1_hi_rbs	fields.rbs	/* [ 5: 0] - base of rotate regs */
#define	CR1_hi_rsz	fields.rsz	/* [11: 6] - size of rotate regs */
#define	CR1_hi_rcur	fields.rcur	/* [17:12] - current of rotate regs */
#define	CR1_hi_psz	fields.psz	/* [22:18] - size of rotate preds */
#define	CR1_hi_pcur	fields.pcur	/* [27:23] - current of rotate preds */
#define	CR1_hi_half	word		/* [63: 0] - entire high */

typedef union {	/* Structure of cr1_lo chain reg */
	struct {
		uint64_t unused1	: 16;	/* [15:0]	*/
		uint64_t ein		:  8;	/* [23:16]	*/
		uint64_t ss		:  1;	/* [24]		*/
		uint64_t wfx		:  1;	/* [25]		*/
		uint64_t wpsz	:  7;	/* [32:26]	*/
		uint64_t wbs		:  7;	/* [39:33]	*/
		uint64_t cuir	: 17;	/* [56:40]	*/
		uint64_t psr		:  7;	/* [63:57]	*/
	};
	struct {
		uint64_t __x1	: 40;	/* [39:0]	*/
		uint64_t cui		: 16;	/* [40:55]	*/
		uint64_t ic		: 1;	/* [56]		*/
		uint64_t pm		: 1;	/* [57] privileged mode */
		uint64_t ie		: 1;	/* [58] interrupt enable */
		uint64_t sge		: 1;	/* [59] stack gard control enable */
		uint64_t lw		: 1;	/* [60] last wish */
		uint64_t uie		: 1;	/* [61] user interrupts enable */
		uint64_t nmie	: 1;	/* [62] not masked interrupts enable */
		uint64_t unmie	: 1;	/* [63] user not masked interrupts */
					/*	enable */
	};
} e2k_cr1_lo_fields_t;
typedef	union {
	struct {
		uint64_t      : 16;
		uint64_t ein  :  8;
		uint64_t ss   :  1;
		uint64_t wfx  :  1;
		uint64_t wpsz :  7;
		uint64_t wbs  :  7;
		uint64_t cuir : 17;
		uint64_t psr  :  7;
	};
	struct {
		uint64_t       : 40;
		uint64_t cui   : 16;
		uint64_t ic    : 1; /* iset <= v5 */
		uint64_t pm    : 1;
		uint64_t ie    : 1;
		uint64_t sge   : 1;
		uint64_t lw    : 1;
		uint64_t uie   : 1;
		uint64_t nmie  : 1;
		uint64_t unmie : 1;
	};
	e2k_cr1_lo_fields_t	fields;	/* as fields		*/
	uint64_t			word;	/* as entire register	*/
} e2k_cr1_lo_t;
#define	CR1_lo_tr	fields.tr	/* [14: 0] - ??? */
#define	CR1_lo_ein	fields.ein	/* [23:16] - ??? */
#define	CR1_lo_wfx	fields.wfx	/*    [25] - ??? */
#define	CR1_lo_wpsz	fields.wpsz	/* [32:26] - regs window parametr */
#define	CR1_lo_wbs	fields.wbs	/* [39:33] - regs window base */
#define	CR1_lo_cuir	fields.cuir	/* [56:40] - compilation unit index */
#define	CR1_lo_cui	fields.cui	/* [55:40] - compilation unit index */
					/*		new release field */
#define	CR1_lo_ic	fields.ic	/* [56]    - index checkup flag */
#define	CR1_lo_psr	fields.psr	/* [63:57] - processor state reg */
#define	CR1_lo_pm	fields.pm	/* [57]    - privileged mode */
#define	CR1_lo_ie	fields.ie	/* [58]    - interrupt enable */
#define	CR1_lo_sge	fields.sge	/* [59]    - stack gard control */
					/*		enable */
#define	CR1_lo_lw	fields.lw	/* [60]    - last wish */
#define	CR1_lo_uie	fields.uie	/* [61]    - user interrupts enable */
#define	CR1_lo_nmie	fields.nmie	/* [62]    - not masked interrupts */
					/*		enable */
#define	CR1_lo_unmie	fields.unmie	/* [63]    - user not masked */
					/*		interrupts enable */
#define	CR1_lo_half	word		/* [63: 0] - entire high */
#endif /* !(__ASSEMBLER__) */

#ifdef __ASSEMBLER__
/* for assemly only */
#define	CR1_lo_psr_shift	57	/* [63:57] - processor state reg */
#define	CR1_LO_PSR_PM_BIT	(PSR_PM_BIT + CR1_lo_psr_shift)
#define	CR1_LO_PSR_IE_BIT	(PSR_IE_BIT + CR1_lo_psr_shift)
#define	CR1_LO_PSR_SGE_BIT	(PSR_SGE_BIT + CR1_lo_psr_shift)
#define	CR1_LO_PSR_LW_BIT	(PSR_LW_BIT + CR1_lo_psr_shift)
#define	CR1_LO_PSR_UIE_BIT	(PSR_UIE_BIT + CR1_lo_psr_shift)
#define	CR1_LO_PSR_NMIE_BIT	(PSR_NMIE_BIT + CR1_lo_psr_shift)
#define	CR1_LO_PSR_UNMIE_BIT	(PSR_UNMIE_BIT + CR1_lo_psr_shift)

#define	CR1_LO_PSR_PM_MASK	(0x1UL << CR1_LO_PSR_PM_BIT)
#define	CR1_LO_PSR_IE_MASK	(0x1UL << CR1_LO_PSR_IE_BIT)
#define	CR1_LO_PSR_SGE_MASK	(0x1UL << CR1_LO_PSR_SGE_BIT)
#define	CR1_LO_PSR_LW_MASK	(0x1UL << CR1_LO_PSR_LW_BIT)
#define	CR1_LO_PSR_UIE_MASK	(0x1UL << CR1_LO_PSR_UIE_BIT)
#define	CR1_LO_PSR_NMIE_MASK	(0x1UL << CR1_LO_PSR_NMIE_BIT)
#define	CR1_LO_PSR_UNMIE_MASK	(0x1UL << CR1_LO_PSR_UNMIE_BIT)

#define	CR1_lo_cuir_shift	40	/* [55:40] - CUIR value */
#define	CR1_lo_cuir_mask	(CUIR_mask << CR1_lo_cuir_shift)

#endif /*  __ASSEMBLER__ */

#define	CR1_lo_cuir_size	16	/*	     size in bits */
#define CUIR_mask		((1UL << CR1_lo_cuir_size) - 1)

#ifndef __ASSEMBLER__

#define	E2K_ALIGN_INS		3		/* number of least */
						/* significant  bits of IP */
						/* are zeroed */

/*
 * Control Transfer Preparation Register (CTPR)
 */

	/*
	 * Structure of double-word register
	 * access CTPR.CTPR_xxx or CTPR -> CTPR_xxx
	 */
typedef	union {
	struct {
		uint64_t ta_base : E2K_VA_SIZE;	/* [47: 0] */
		uint64_t __pad1  : 53 - E2K_VA_MSB;	/* [53:48] */
		uint64_t ta_tag  : 3;		/* [56:54] */
		uint64_t opc     : 2;		/* [58:57] */
		uint64_t ipd     : 2;		/* [60:59] */
		uint64_t __pad2  : 3;		/* [63:61] */
	} fields;
	struct {
		uint64_t ta_base : E2K_VA_SIZE;
		uint64_t __pad1  : 53 - E2K_VA_MSB;
		uint64_t ta_tag  : 3;
		uint64_t opc	    : 2;
		uint64_t ipd	    : 2;
		uint64_t __pad2  : 3;
	};
	uint64_t word;
} e2k_ctpr_t;
#define	CTPR_ta_base	ta_base	/* [47: 0] - transfer address */
#define	CTPR_ta_tag	ta_tag	/* [56:54] - tag */
#define	CTPR_opc	opc	/* [58:57] - opcode */
#define	CTPR_ipd	ipd	/* [58:57] - prefetch level */
#define	CTPR_reg	word	/* [63: 0] - entire register */
/* Control Transfer Opcodes */
#define	DISP_CT_OPC	0
#define	LDISP_CT_OPC	1
#define	RETURN_CT_OPC	3

/* Control Transfer Tag */
#define	CTPEW_CT_TAG	0	/* empty word */
#define	CTPDW_CT_TAG	1	/* diagnostic word */
#define	CTPPL_CT_TAG	2	/* procedure label */
#define	CTPLL_CT_TAG	3	/* local label */
#define	CTPNL_CT_TAG	4	/* numeric label */
#define	CTPSL_CT_TAG	5	/* system label */

/* Control Transfer Prefetch Level */
#define	NONE_CT_IPD	0	/* none any prefetching */
#define	ONE_IP_CT_IPD	1	/* only one instruction on 'ta_base' IP */
#define	TWO_IP_CT_IPD	2	/* two instructions on 'ta_base' and next IP */

typedef	union {
	struct {
		uint64_t cui   : 16;
		uint64_t __pad : 48;
	};
	uint64_t word;
} e2k_ctpr_hi_t;


/* PSR */
typedef	struct {
	uint32_t	pm	:  1;		/* [ 0]		*/
	uint32_t	ie	:  1;		/* [ 1]		*/
	uint32_t	sge	:  1;		/* [ 2]		*/
	uint32_t	lw	:  1;		/* [ 3] last wish */
	uint32_t	uie	:  1;		/* [ 4] user interrupts enable */
	uint32_t	nmie	:  1;		/* [ 5] not masked interrupts enable */
	uint32_t	unmie	:  1;		/* [ 6] user not masked interrupts */
					/*	enable */
	uint32_t	unused	: 25;		/* [31: 7]	*/
} e2k_psr_fields_t;
typedef	union {
	struct {
		uint32_t pm    : 1;
		uint32_t ie    : 1;
		uint32_t sge   : 1;
		uint32_t lw    : 1;
		uint32_t uie   : 1;
		uint32_t nmie  : 1;
		uint32_t unmie : 1;
		uint32_t       : 25;
	};
	e2k_psr_fields_t	fields;	/* as fields		*/
	uint32_t			word;	/* as entire register	*/
} e2k_psr_t;

#define	PSR_pm		fields.pm		/* [ 0] */
#define	PSR_ie		fields.ie		/* [ 1] */
#define	PSR_sge		fields.sge		/* [ 2] */
#define	PSR_lw		fields.lw		/* [ 3] */
#define	PSR_uie		fields.uie		/* [ 4] */
#define	PSR_nmie	fields.nmie		/* [ 5] */
#define	PSR_unmie	fields.unmie		/* [ 6] */
#define	PSR_reg		word			/* [31: 0] - entire */
						/* single-word register */

#define	PSR_PM		0x01UL
#define	PSR_IE		0x02UL
#define	PSR_SGE		0x04UL
#define	PSR_LW		0x08UL
#define	PSR_UIE		0x10UL
#define	PSR_NMIE	0x20UL
#define	PSR_UNMIE	0x40UL
#define	PSR_DISABLE	0xff8dUL		/*~(PSR_IE|PSR_NMIE|PSR_UIE|PSR_UNMIE)*/
#define	PSR_PM_DISABLE	0xfffeUL		/* ~PSR_PM_AS */
#endif /* !(__ASSEMBLER__) */

#ifdef __ASSEMBLER__
/* for assemly only */
#define	PSR_PM_BIT	0
#define	PSR_IE_BIT	1
#define	PSR_SGE_BIT	2
#define	PSR_LW_BIT	3
#define	PSR_UIE_BIT	4
#define	PSR_NMIE_BIT	5
#define	PSR_UNMIE_BIT	6

#define	PSR_PM_AS	(0x1 << PSR_PM_BIT)
#define	PSR_IE_AS	(0x1 << PSR_IE_BIT)
#define	PSR_SGE_AS	(0x1 << PSR_SGE_BIT)
#define	PSR_LW_AS	(0x1 << PSR_LW_BIT)
#define	PSR_UIE_AS	(0x1 << PSR_UIE_BIT)
#define	PSR_NMIE_AS	(0x1 << PSR_NMIE_BIT)
#define	PSR_UNMIE_AS	(0x1 << PSR_UNMIE_BIT)
#define	PSR_DISABLE	(~(PSR_IE_AS | PSR_NMIE_AS | PSR_UIE_AS | PSR_UNMIE_AS))
#define	PSR_PM_DISABLE	(~PSR_PM_AS)

#endif /*  __ASSEMBLER__ */

#ifndef __ASSEMBLER__

/* CUT entry */

typedef	struct e2k_cute_dw0_fields {	/* Structure of the first d-word */
					/* of CUT entry */
	uint64_t	cud_base	: E2K_VA_SIZE;		/* [47: 0]	*/
	uint64_t	unused1		: 57 - E2K_VA_MSB;	/* [57:48]	*/
	uint64_t	cud_c		: 1;			/* [58:58]	*/
	uint64_t	unused2		: 5;			/* [63:59]	*/
} e2k_cute_dw0_fields_t;

typedef	union e2k_cute_dw0 {
	e2k_cute_dw0_fields_t	fields;	/* as fields		*/
	uint64_t			word;	/* as entire register	*/
} e2k_cute_dw0_t;


typedef	struct e2k_cute_dw1_fields {	/* Structure of the second d-word */
					/* of CUT entry			*/
	uint64_t	unused1		: 32;			/* [31: 0]	*/
	uint64_t	cud_size	: 32;			/* [63:32]	*/
} e2k_cute_dw1_fields_t;

typedef	union e2k_cute_dw1 {
	e2k_cute_dw1_fields_t	fields;	/* as fields		*/
	uint64_t			word;	/* as entire register	*/
} e2k_cute_dw1_t;

typedef	struct e2k_cute_dw2_fields {	/* Structure of the third d-word */
					/* of CUT entry			*/
	uint64_t	gd_base		: E2K_VA_SIZE;		/* [47: 0]	*/
	uint64_t	unused1		: 63 - E2K_VA_MSB;	/* [63:48]	*/
} e2k_cute_dw2_fields_t;

typedef	union e2k_cute_dw2 {
	e2k_cute_dw2_fields_t	fields;	/* as fields		*/
	uint64_t			word;	/* as entire register	*/
} e2k_cute_dw2_t;

typedef	struct e2k_cute_dw3_fields {	/* Structure of the fourth d-word */
					/* of CUT entry			*/
	uint64_t	tsd_base	: 15;			/* [14: 0]	*/
	uint64_t	unused1		: 1;			/* [15:15]	*/
	uint64_t	tsd_size	: 15;			/* [30:16]	*/
	uint64_t	unused2		: 1;			/* [31:31]	*/
	uint64_t	gd_size		: 32;			/* [63:32]	*/
} e2k_cute_dw3_fields_t;

typedef	union e2k_cute_dw3 {
	e2k_cute_dw3_fields_t	fields;	/* as fields		*/
	uint64_t			word;	/* as entire register	*/
} e2k_cute_dw3_t;

/* Structure of entire CUT entry */
typedef	struct e2k_cute {
	e2k_cute_dw0_t	dw0;
	e2k_cute_dw1_t	dw1;
	e2k_cute_dw2_t	dw2;
	e2k_cute_dw3_t	dw3;
} e2k_cute_t;

#define	CUTE_CUD_BASE(p)	AS_STRUCT(p->dw0).cud_base
#define	CUTE_CUD_SIZE(p)	AS_STRUCT(p->dw1).cud_size
#define	CUTE_CUD_C(p)		AS_STRUCT(p->dw0).cud_c

#define	CUTE_GD_BASE(p)		AS_STRUCT(p->dw2).gd_base
#define	CUTE_GD_SIZE(p)		AS_STRUCT(p->dw3).gd_size

#define	CUTE_TSD_BASE(p)	AS_STRUCT(p->dw3).tsd_base
#define	CUTE_TSD_SIZE(p)	AS_STRUCT(p->dw3).tsd_size

#endif /* !(__ASSEMBLER__) */

#define	E2K_ALIGN_CUT		5		/* Compilation units table */
						/* boundaries alignment */
						/* (2's exponent value */
#ifndef __ASSEMBLER__
#define	E2K_ALIGN_CUT_MASK	((1UL << E2K_ALIGN_CUT) - 1)
#else	/* __ASSEMBLER__ */
#define	E2K_ALIGN_CUT_MASK	((1 << E2K_ALIGN_CUT) - 1)
#endif /* !(__ASSEMBLER__) */

#ifndef __ASSEMBLER__

/* CUTD */

typedef	e2k_rwp_struct_t	e2k_cutd_t;
#define	CUTD_base		E2K_RWP_base	/* [47: 0] - base address */
#define	CUTD_reg		E2K_RWP_reg	/* [63: 0] - entire double- */
						/*           word register */

/* CUIR */

typedef	struct e2k_cuir_fields {	/* Structure of the CUIR reg	*/
	uint32_t	index		: 16;			/* [15: 0]	*/
	uint32_t	checkup		: 1;			/* [16:16]	*/
	uint32_t	unused1		: 15;			/* [31:17]	*/
} e2k_cuir_fields_t;

typedef	union e2k_cuir {
	e2k_cuir_fields_t	fields;	/* as fields		*/
	uint32_t			word;	/* as entire register	*/
} e2k_cuir_t;
#define	CUIR_index		fields.index
#define	CUIR_checkup		fields.checkup
#define	CUIR_reg		word

/* TSD */

typedef	struct e2k_tsd_fields {		/* Structure of the TSD reg	*/
	uint64_t	base		: 15;			/* [14: 0]	*/
	uint64_t	unused1		: 17;			/* [31:15]	*/
	uint64_t	size		: 15;			/* [46:32]	*/
	uint64_t	unused2		: 17;			/* [63:47]	*/
} e2k_tsd_fields_t;

typedef	union e2k_tsd {
	e2k_tsd_fields_t	fields;	/* as fields		*/
	uint64_t			word;	/* as entire register	*/
} e2k_tsd_t;
#define	TSD_base		fields.base
#define	TSD_size		fields.size
#define	TSD_reg			word

#define	CUD_CFLAG_CEARED	0	/* intermodule security verification */
					/* (ISV) have not passed	     */
#define	CUD_CFLAG_SET		1	/* ISV have passed		     */

/* Hardware procedure stack memory mapping (one quad-register record, LE) */
/* Istruction sets from V3 to V4 */
typedef	struct e2k_mem_ps_v3 {
	unsigned long		word_lo;	/* low word value */
	unsigned long		word_hi;	/* high word value */
	unsigned long		ext_lo;		/* extention of low word */
	unsigned long		ext_hi;		/* extention of hagh word */
} e2k_mem_ps_v3_t;
/* Istruction sets from V5 to V6 */
typedef	struct e2k_mem_ps_v5 {
	unsigned long		word_lo;	/* low word value */
	unsigned long		ext_lo;		/* extention of low word */
	unsigned long		word_hi;	/* high word value */
	unsigned long		ext_hi;		/* extention of hagh word */
} e2k_mem_ps_v5_t;
typedef union e2k_mem_ps {
	e2k_mem_ps_v3_t		v3;
	e2k_mem_ps_v5_t		v5;
} e2k_mem_ps_t;

/* interkernel hardware-independent representation */
typedef	struct kernel_mem_ps {
	unsigned long		word_lo;	/* low word value */
	unsigned long		word_hi;	/* high word value */
	unsigned long		ext_lo;		/* extention of low word */
	unsigned long		ext_hi;		/* extention of hagh word */
} kernel_mem_ps_t;

/* Chain stack memory mapping (one record, LE) */

typedef	struct e2k_mem_crstack {
	e2k_cr0_lo_t		cr0_lo;
	e2k_cr0_hi_t		cr0_hi;
	e2k_cr1_lo_t		cr1_lo;
	e2k_cr1_hi_t		cr1_hi;
} e2k_mem_crs_t;

#endif /* !(__ASSEMBLER__) */

/*
 * chain stack items relative offset from cr_ind for pcsp
 */

#define	CR0_LO_I	(0 * 8)
#define	CR0_HI_I	(1 * 8)
#define	CR1_LO_I	(2 * 8)
#define	CR1_HI_I	(3 * 8)

#ifndef	__ASSEMBLER__

/*
 * cr1.lo.wbs is size of prev proc in term of size of 4 32 bit reegs.
 * But in hard stack these regs are in extended format (*2)
 */
#define	EXT_4_NR_SZ	((4 * 4) * 2)
#define	SZ_OF_CR	sizeof(e2k_mem_crs_t)


typedef union {
	struct {
		uint64_t trwm_itag       : 3;
		uint64_t trwm_idata      : 3;
		uint64_t trwm_cf         : 3;
		uint64_t ib_snoop_dsbl   : 1;
		uint64_t bist_cf         : 1;
		uint64_t bist_tu         : 1;
		uint64_t bist_itag       : 1;
		uint64_t bist_itlbtag    : 1;
		uint64_t bist_itlbdata   : 1;
		uint64_t bist_idata_nm   : 4;
		uint64_t bist_idata_cnt  : 10;
		uint64_t pipe_frz_dsbl   : 1; /* Since iset v5 */
		uint64_t rf_clean_dsbl   : 1;
		/* iset v6 */
		uint64_t virt_dsbl       : 1;
		uint64_t upt_sec_ad_shift_dsbl : 1;
		uint64_t pdct_stat_enbl  : 1;
		uint64_t pdct_dyn_enbl   : 1;
		uint64_t pdct_rbr_enbl   : 1;
		uint64_t pdct_ret_enbl   : 1;
		uint64_t pdct_retst_enbl : 1;
		uint64_t pdct_cond_enbl  : 1;
	};
	uint64_t word;
} e2k_cu_hw0_t;


/*
 * Trap Info Registers
 */

typedef	e2k_rwp_struct_t	e2k_tir_lo_t;

typedef	struct tir_hi_fields {	/* Structure of the TIR_hi reg	*/
	uint64_t	exc	: 44;	/* exceptions mask [43: 0]	*/
	uint64_t	al	:  6;	/* ALS mask	   [49:44]	*/
	uint64_t	unused1	:  2;	/* unused bits	   [51:50]	*/
	uint64_t	aa	:  4;	/* MOVA mask	   [55:52]	*/
	uint64_t	j	:  8;	/* # of TIR	   [63:56]	*/
} tir_hi_fields_t;

typedef	union tir_hi_struct {
	struct {	/* Structure of the TIR_hi reg	*/
		uint64_t exc		: 44;	/* exceptions mask [43: 0]	*/
		uint64_t al		:  6;	/* ALS mask	   [49:44]	*/
		uint64_t unused1	:  2;	/* unused bits	   [51:50]	*/
		uint64_t aa		:  4;	/* MOVA mask	   [55:52]	*/
		uint64_t j		:  8;	/* # of TIR	   [63:56]	*/
	};
	tir_hi_fields_t	fields;	/* as fields		*/
	uint64_t		word;	/* as entire register	*/
} e2k_tir_hi_t;

typedef struct e2k_tir_reg {		/* simple TIRj register desc */
	e2k_tir_lo_t	TIR_lo;
	e2k_tir_hi_t	TIR_hi;
} e2k_tir_t;

	/*
	 * Structure of low word of the register
	 * access TIR_lo.TIR_lo_xxx or TIR_lo -> TIR_lo_xxx
	 */
#define	TIR_lo_ip	E2K_RWP_base		/* [47: 0] - IP of trap */
#define	TIR_lo_reg	E2K_RWP_reg		/* [63: 0] - entire */
						/* double-word register */

	/*
	 * Structure of hi word of the register
	 * access TIR_hi.TIR_hi_xxx or TIR_hi -> TIR_hi_xxx
	 */
#define TIR_hi_reg	word			/* [63: 0] - entire */

#define	TIR_hi_exc	fields.exc
#define	TIR_hi_al	fields.al
#define	TIR_hi_aa	fields.aa
#define	TIR_hi_j	fields.j

/* ALS mask structure */
#define	ALS0_mask	0x01
#define	ALS1_mask	0x02
#define	ALS2_mask	0x04
#define	ALS3_mask	0x08
#define	ALS4_mask	0x10
#define	ALS5_mask	0x20

#define	MAX_TIRs_NUM	19

/*
 *  User processor status register (UPSR)
 */
typedef	struct e2k_upsr_fields {
	uint32_t	fe	: 1;	/* float-pointing enable */
	uint32_t	se	: 1;	/* supervisor mode enable (only for Intel) */
	uint32_t	ac	: 1;	/* not-aligned access control */
	uint32_t	di	: 1;	/* delayed interrupt (only for Intel) */
	uint32_t	wp	: 1;	/* write protection (only for Intel) */
	uint32_t	ie	: 1;	/* interrupt enable */
	uint32_t	a20	: 1;	/* emulation of 1 Mb memory (only for Intel) */
				/* should be 0 for Elbrus */
	uint32_t	nmie	: 1;	/* not masked interrupt enable */
	/* next field of register exist only on E2S/E8C/E1C+ CPUs */
	uint32_t	fsm	: 1;	/* floating comparison mode flag */
				/* 1 - compatible with x86/x87 */
	uint32_t	impt	: 1;	/* ignore Memory Protection Table flag */
	uint32_t	iuc	: 1;	/* ignore access right for uncached pages */

} e2k_upsr_fields_t;
typedef	union e2k_upsr {
	e2k_upsr_fields_t	fields;	/* as fields		*/
	uint32_t			word;	/* as entire register	*/
} e2k_upsr_t;
#define	UPSR_fe		fields.fe
#define	UPSR_se		fields.se
#define	UPSR_ac		fields.ac
#define	UPSR_di		fields.di
#define	UPSR_wp		fields.wp
#define	UPSR_ie		fields.ie
#define	UPSR_a20	fields.a20
#define	UPSR_nmie	fields.nmie
#define	UPSR_fsm	fields.fsm
#define	UPSR_impt	fields.impt
#define	UPSR_iuc	fields.iuc
#define	UPSR_reg	word

#endif /* !(__ASSEMBLER__) */

#define	UPSR_FE		0x01UL
#define	UPSR_SE		0x02UL
#define	UPSR_AC		0x04UL
#define	UPSR_DI		0x08UL
#define	UPSR_WP		0x10UL
#define	UPSR_IE		0x20UL
#define	UPSR_A20	0x40UL
#define	UPSR_NMIE	0x80UL
/* next field of register exist only on E2S/E8C/E1C+ CPUs */
#define	UPSR_FSM	0x100UL
#define	UPSR_IMPT	0x200UL
#define	UPSR_IUC	0x400UL
#define	UPSR_DISABLE		(0xff5f)	/* ~(UPSR_IE_AS|UPSR_NMIE_AS) */

/* (IS_UPT_E3S ? 0 : UPSR_SE_AS) */
#ifndef IS_UPT_E3S
  #define KERNEL_UPSR_SE_INIT	0
#else
  #define KERNEL_UPSR_SE_INIT	UPSR_SE
#endif /* IS_UPT_E3S */
#ifndef	CONFIG_ACCESS_CONTROL
  #define KERNEL_UPSR_ALL_INIT	(UPSR_FE | KERNEL_UPSR_SE_INIT)
#else
  #define KERNEL_UPSR_ALL_INIT	(UPSR_FE | KERNEL_UPSR_SE_INIT | UPSR_AC)
#endif /* KERNEL_UPSR_ALL_INIT */

#ifndef __ASSEMBLER__

/*
 *  Processor Identification Register (IDR)
 */
typedef	union e2k_idr {
	struct {
		uint64_t mdl		:  8;	/* CPU model number */
		uint64_t rev		:  4;	/* revision number */
		uint64_t wbl		:  3;	/* write back length of L2 */
		uint64_t core	:  5;	/* number of the core into node */
		uint64_t pn		:  4;	/* node number from RT_LCFG0.pn */
		uint64_t hw_virt	:  1;	/* hardware virtualization enabled */
		uint64_t hw_virt_ver :  4;	/* hardware virtualization revision */
					/* number */
		uint64_t reserve	: 35;	/* reserved */
	};
	struct {
		uint64_t __pad	: 12;
		uint64_t ms		: 52;	/* model specific info */
	};
	uint64_t			word;		/* as entire register */
} e2k_idr_t;

#define IDR_reg			word		/* [63: 0] - entire */

#define	IDR_mdl			mdl
#define	IDR_rev			rev
#define	IDR_ms			ms
#define	IDR_wbl			wbl
#define	IDR_ms_core		core
#define	IDR_ms_pn		pn
#define	IDR_ms_hw_virt		hw_virt
#define	IDR_ms_hw_virt_ver	hw_virt_ver

/* CPU model numbers */
#define IDR_NONE		0x00	/* No such hardware exists */
#define	IDR_E2S_MDL		0x03	/* Elbrus-4C (Elbrus-2S) */
#define	IDR_E4S_MDL		0x05	/* reserve */
#define IDR_E8C_MDL		0x07	/* Elbrus-8C */
#define IDR_E1CP_MDL		0x08	/* Elbrus-1C+ one processor e2s */
					/* + graphic */
#define IDR_E8C2_MDL		0x09	/* Elbrus-8C2 */
#define IDR_E12C_MDL		0x0a	/* Elbrus-12C */
#define IDR_E16C_MDL		0x0b	/* Elbrus-16C */
#define IDR_E2C3_MDL		0x0c	/* Elbrus-2C3 */

/* Convert IDR register write back length code to number of bytes */
/* using current WBL code presentation */
#define	IDR_WBL_TO_BYTES(wbl)	((wbl) ? (1 << (wbl + 4)) : 1)

/*
 *  Processor Core Mode Register (CORE_MODE)
 */
typedef	union e2k_core_mode {
	struct {
		uint32_t reserve0	   : 1;	/* bit #0 reserved */
		uint32_t no_stack_prot  : 1;	/* no check stack pointers */
		uint32_t sep_virt_space : 1;	/* separate page tables for kernel */
					/* and users */
		uint32_t gmi		   : 1;	/* indicator of guest mode */
					/* actual only in guest mode */
		uint32_t hci		   : 1;	/* indicator of hypercalls enabled */
					/* actual only in guest mode */
		uint32_t pt_v6	   : 1;	/* new Page Tables structures mode */
					/* only for ISET >= V6 */
		uint32_t sp_rollback_en : 1;	/* hardware rollback PSP/PCSP stack */
					/* pointers is enabled */
		uint32_t reserve6	   : 25; /* other bits reserved */
	};
	uint32_t			word;		/* as entire register */
} e2k_core_mode_t;

#define CORE_MODE_reg			word	/* [31: 0] - entire */

#define	CORE_MODE_no_stack_prot		no_stack_prot
#define	CORE_MODE_sep_virt_space	sep_virt_space
#define	CORE_MODE_gmi			gmi
#define	CORE_MODE_hci			hci
#define	CORE_MODE_pt_v6			pt_v6
#define	CORE_MODE_sp_rollback_en	sp_rollback_en

/*
 *  Packed Floating Point Flag Register (PFPFR)
 */
typedef	struct e2k_pfpfr_fields {
	uint32_t	ie	: 1;		/* [0] */
	uint32_t	de	: 1;		/* [1] */
	uint32_t	ze	: 1;		/* [2] */
	uint32_t	oe	: 1;		/* [3] */
	uint32_t	ue	: 1;		/* [4] */
	uint32_t	pe	: 1;		/* [5] */
	uint32_t	zero1	: 1;		/* [6] */
	uint32_t	im	: 1;		/* [7] */
	uint32_t	dm	: 1;		/* [8] */
	uint32_t	zm	: 1;		/* [9] */
	uint32_t	om	: 1;		/* [10] */
	uint32_t	um	: 1;		/* [11] */
	uint32_t	pm	: 1;		/* [12] */
	uint32_t	rc	: 2;		/* [14:13] */
	uint32_t	fz	: 1;		/* [15] */
	uint32_t	zero2	: 10;		/* [25:16] */
	uint32_t	die	: 1;		/* [26] */
	uint32_t	dde	: 1;		/* [27] */
	uint32_t	dze	: 1;		/* [28] */
	uint32_t	doe	: 1;		/* [29] */
	uint32_t	due	: 1;		/* [30] */
	uint32_t	dpe	: 1;		/* [31] */
} e2k_pfpfr_fields_t;
typedef	union e2k_pfpfr {
	e2k_pfpfr_fields_t	fields;	/* as fields		*/
	uint32_t		word;	/* as entire register	*/
} e2k_pfpfr_t;

#define	PFPFR_ie	fields.ie
#define	PFPFR_de	fields.de
#define	PFPFR_ze	fields.ze
#define	PFPFR_oe	fields.oe
#define	PFPFR_ue	fields.ue
#define	PFPFR_pe	fields.pe
#define	PFPFR_zero1	fields.zero1
#define	PFPFR_im	fields.im
#define	PFPFR_dm	fields.dm
#define	PFPFR_zm	fields.zm
#define	PFPFR_om	fields.om
#define	PFPFR_um	fields.um
#define	PFPFR_pm	fields.pm
#define	PFPFR_rc	fields.rc
#define	PFPFR_fz	fields.fz
#define	PFPFR_zero2	fields.zero2
#define	PFPFR_die	fields.die
#define	PFPFR_dde	fields.dde
#define	PFPFR_dze	fields.dze
#define	PFPFR_doe	fields.doe
#define	PFPFR_due	fields.due
#define	PFPFR_dpe	fields.dpe
#define	PFPFR_reg	word

/*
 *  Floating point control register (FPCR)
 */
typedef	struct e2k_fpcr_fields {
	uint32_t	im	: 1;		/* [0] */
	uint32_t	dm	: 1;		/* [1] */
	uint32_t	zm	: 1;		/* [2] */
	uint32_t	om	: 1;		/* [3] */
	uint32_t	um	: 1;		/* [4] */
	uint32_t	pm	: 1;		/* [5] */
	uint32_t	one1	: 1;		/* [6] */
	uint32_t	zero1	: 1;		/* [7] */
	uint32_t	pc	: 2;		/* [9:8] */
	uint32_t	rc	: 2;		/* [11:10] */
	uint32_t	ic	: 1;		/* [12] */
	uint32_t	zero2	: 3;		/* [15:13] */
} e2k_fpcr_fields_t;
typedef	union e2k_fpcr {
	e2k_fpcr_fields_t	fields;	/* as fields		*/
	uint32_t			word;	/* as entire register	*/
} e2k_fpcr_t;

#define	FPCR_im		fields.im
#define	FPCR_dm		fields.dm
#define	FPCR_zm		fields.zm
#define	FPCR_om		fields.om
#define	FPCR_um		fields.um
#define	FPCR_pm		fields.pm
#define	FPCR_one1	fields.one1
#define	FPCR_zero1	fields.zero1
#define	FPCR_pc		fields.pc
#define	FPCR_rc		fields.rc
#define	FPCR_ic		fields.ic
#define	FPCR_zero2	fields.zero2
#define	FPCR_reg	word


/*
 * Floating point status register (FPSR)
 */
typedef	struct e2k_fpsr_fields {
	uint32_t	ie	: 1;		/* [0] */
	uint32_t	de	: 1;		/* [1] */
	uint32_t	ze	: 1;		/* [2] */
	uint32_t	oe	: 1;		/* [3] */
	uint32_t	ue	: 1;		/* [4] */
	uint32_t	pe	: 1;		/* [5] */
	uint32_t	zero1	: 1;		/* [6] */
	uint32_t	es	: 1;		/* [7] */
	uint32_t	zero2	: 1;		/* [8] */
	uint32_t	c1	: 1;		/* [9] */
	uint32_t	zero3	: 5;		/* [14:10] */
	uint32_t	bf	: 1;		/* [15] */
} e2k_fpsr_fields_t;
typedef	union e2k_fpsr {
	e2k_fpsr_fields_t	fields;	/* as fields		*/
	uint32_t			word;	/* as entire register	*/
} e2k_fpsr_t;

#define	FPSR_ie		fields.ie
#define	FPSR_de		fields.de
#define	FPSR_ze		fields.ze
#define	FPSR_oe		fields.oe
#define	FPSR_ue		fields.ue
#define	FPSR_pe		fields.pe
#define	FPSR_zero1	fields.zero1
#define	FPSR_es		fields.es
#define	FPSR_zero2	fields.zero2
#define	FPSR_c1		fields.c1
#define	FPSR_zero3	fields.zero3
#define	FPSR_bf		fields.bf
#define	FPSR_reg	word

typedef	union {
	uint32_t half_word[2];
	struct {
		uint32_t user    : 1;
		uint32_t system  : 1;
		uint32_t trap    : 1;
		uint32_t unused1 : 13;
		uint32_t event   : 7;
		uint32_t unused2 : 9;
	} fields[2];
	struct {
		uint64_t __pad1 : 11;
		uint64_t u_m_en : 1;
		uint64_t mode   : 4;
		uint64_t __pad2 : 48;
	};
	uint64_t word;
} e2k_dimcr_t;
#define	DIMCR_reg	word

static inline bool dimcr_enabled(e2k_dimcr_t dimcr, int monitor)
{
	return (monitor == 0) ? (AS(dimcr)[0].user || AS(dimcr)[0].system)
			      : (AS(dimcr)[1].user || AS(dimcr)[1].system);
}

typedef union {
	struct {
		uint32_t b0    : 1;
		uint32_t b1    : 1;
		uint32_t b2    : 1;
		uint32_t b3    : 1;
		uint32_t bt    : 1;
		uint32_t m0    : 1;
		uint32_t m1    : 1;
		uint32_t ss    : 1;
		uint32_t btf   : 1;
	};
	struct {               /* structure of register */
		uint32_t b0    : 1;       /* [0] */
		uint32_t b1    : 1;       /*     */
		uint32_t b2    : 1;       /*     */
		uint32_t b3    : 1;       /*     */
		uint32_t bt    : 1;       /* [4] */
		uint32_t m0    : 1;       /* [5] */
		uint32_t m1    : 1;       /* [6] */
		uint32_t ss    : 1;       /* [7] */
		uint32_t btf   : 1;       /* [8] */
	} fields;
	uint32_t word;
} e2k_dibsr_t;
#define	DIBSR_reg	word

#define E2K_DIBSR_MASK(cp_num) (0x1ULL << (cp_num))
#define E2K_DIBSR_MASK_ALL_BP 0xfULL

typedef union {
	struct {
		uint32_t v0   : 1;
		uint32_t t0   : 1;
		uint32_t v1   : 1;
		uint32_t t1   : 1;
		uint32_t v2   : 1;
		uint32_t t2   : 1;
		uint32_t v3   : 1;
		uint32_t t3   : 1;
		uint32_t bt   : 1;
		uint32_t stop : 1;
		uint32_t btf  : 1;
		uint32_t gm   : 1;
	};
	uint32_t word;
} e2k_dibcr_t;
#define	DIBCR_reg	word

#define E2K_DIBCR_MASK(cp_num) (0x3ULL << ((cp_num) * 2))

typedef union e2k_dimtp {
	struct {
		struct {
			uint64_t base : E2K_VA_SIZE;
			uint64_t      : 59 - E2K_VA_SIZE;
			uint64_t rw   : 2;
			uint64_t      : 3;
		};
		struct {
			uint64_t ind  : 32;
			uint64_t size : 32;
		};
	};
	struct {
		uint64_t lo;
		uint64_t hi;
	};
} e2k_dimtp_t;

#define E2K_DIMTP_ALIGN 32


/*
 * Global registers (saved state) definition
 */
typedef struct e2k_svd_gregs_struct {
	uint64_t	base;			/* exists any time */
	uint32_t	extension;		/* when holds an FP value */
	uint8_t	tag;			/* any time too */
} e2k_svd_gregs_t;


struct hw_stacks {
	e2k_psp_lo_t	psp_lo;
	e2k_psp_hi_t	psp_hi;
	e2k_pshtp_t	pshtp;
	e2k_pcsp_lo_t	pcsp_lo;
	e2k_pcsp_hi_t	pcsp_hi;
	e2k_pcshtp_t	pcshtp;
};


typedef union {
	struct {
		uint64_t div    : 32;
		uint64_t mdiv   : 1;
		uint64_t mode   : 1;
		uint64_t trn    : 1;
		uint64_t sw     : 1;
		uint64_t wsclkr : 1;
		uint64_t __pad1 : 19;
		uint64_t ver    : 8;
	};
	struct {
		uint64_t __pad2 : 56;
		uint64_t w_sclkr_hi : 1;
		uint64_t sclkm3 : 1;
		uint64_t __pad3 : 6;
	};
	uint64_t word;
} e2k_sclkm1_t;

typedef enum cu_reg_no {
	undef_cu_reg_no = -1,
	SCLKM1_cu_reg_no = 0x70,
	SCLKM2_cu_reg_no = 0x71,
	SCLKM3_cu_reg_no = 0x72,
	IDR_cu_reg_no = 0x8a,
	CLKR_cu_reg_no = 0x90,
	SCLKR_cu_reg_no = 0x92,
	DIBCR_cu_reg_no = 0x40,
	DIMCR_cu_reg_no = 0x41,
	DIBSR_cu_reg_no = 0x42,
	DTCR_cu_reg_no = 0x43,
	DIMTP_hi_cu_reg_no = 0x46,
	DIMTP_lo_cu_reg_no = 0x47,
	DIBAR0_cu_reg_no = 0x48,
	DIBAR1_cu_reg_no = 0x49,
	DIBAR2_cu_reg_no = 0x4a,
	DIBAR3_cu_reg_no = 0x4b,
	DIMAR0_cu_reg_no = 0x4c,
	DIMAR1_cu_reg_no = 0x4d,
	DTARF_cu_reg_no = 0x4e,
	DTART_cu_reg_no = 0x4f,
} cu_reg_no_t;

#endif /* ! __ASSEMBLER__ */

#endif  /* _E2K_CPU_REGS_TYPES_H_ */
