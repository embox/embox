.include "aliases.inc"
.include "R1_pcf.inc"

;
.global PSW.ONIRQS      ;
.equiv  PSW.ONIRQS,       0x0001
.global INTR.PRGE       ;
.equiv  INTR.PRGE,        0x0008

.alias divsl divsl.crt
.alias divl divl.crt

;
.global irq.desc.tbl    ;
.global sizeof.IDT.item ;
.equiv  sizeof.IDT.item,  sizeof.ptr
.global __libc_main     ;
.global main            ;
.global divsl           ;
.global divl            ;
.global movb            ;
.global movl            ;
.global movq            ;
;
.global asm_getl_PSW 		;
.global asm_setl_PSW 		;
.global asm_getl_MSKR 		;
.global asm_setl_MSKR 		;
.global asm_getl_INTR 		;
.global asm_setl_INTR 		;
.global asm_getl_INTNUM  	;
.global asm_getl_PLLCR 		;
.global asm_setl_PLLCR 		;
.global asm_getl_PLLSTR		;
.global asm_setl_PLLSTR		;
.global asm_getl_PLLTMCR 	;
.global asm_setl_PLLTMCR 	;
.global asm_set_PLL         ;

;
.global  set_irq_vector ;

;
.local master.isr       ;
.local tsk.retval       ;

;===============================================================================
; Interrupt Descriptor Table (IDT):
;===============================================================================
.data
.align 4

  irq.desc.tbl:
    .long \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0
	;0, \ ;//ENMI.isr,
	;0 ;//RESERVED.isr,
      ; 0, \ ;//PERE.isr,
      ; 0, \ ;//PPGE.isr,
      ; 0, \;//RESERVED.isr,
      ; 0, \;//RESERVED.isr,
      ; 0, \;//WKUP.isr,
      ; 0, \;//DTC0.isr,
      ; 0, \;//ST0.isr,
	  ; 0, \;//ST1.isr,
	  ; 0, \;//ST2.isr,
      ; 0, \;//ST3.isr,
      ; 0, \;//SW0.isr,
	  ; 0, \;//SW1.isr,
      ; 0, \;//SW2.isr,
      ; 0, \;//SW3.isr,
      ; 0, \;//ADC0.isr,
      ; 0, \;//DAC0.isr,
      ; 0, \;//ADC1.isr,
      ; 0, \;//DAC1.isr,
      ; 0, \;//UART0.isr,
	  ; 0, \;//UART1.isr,
	  ; 0, \;//UART2.isr,
	  ; 0, \;//UART3.isr,
      ; 0, \;//I2C0.isr,
	  ; 0, \;//I2C1.isr,
      ; 0, \;//SPI0.isr,
	  ; 0, \;//SPI1.isr,
      ; 0, \;//I2S0.isr,
      ; 0, \;//GPTIM0.isr,
	  ; 0, \;//GPTIM1.isr,
	  ; 0, \;//GPTIM2.isr,
	  ; 0, \;//GPTIM3.isr,
      ; 0, \;//WDT.isr,
      ; 0, \;//RTC.isr,
      ; 0, \;//GPIOA.isr,
	  ; 0, \;//GPIOB.isr,
	  ; 0, \;//GPIOC.isr,
	  ; 0, \;//GPIOD.isr,
	  ; 0, \;//GPIOE.isr,
	  ; 0, \;//GPIOF.isr,
      ; 0, \;//ETHERNET0.isr,
      ; 0, \;//USB0.isr,
      ; 0, \;//USB0_EPI.isr,
      ; 0, \;//USB0_EPO.isr,
      ; 0, \;//PWM0.isr,
      ; 0 ;//STAT.isr

;===============================================================================
; Interrupt System Timer(IST):
;===============================================================================



/////////////////////////////////////////
.data
  tsk.retval: .long
  .alias sizeof.tsk.retval sizeof.long
  //psw_reset_val: .long

.section .boot, "ax"

__libc_main:
 wdt_off:
     getl 0x00003333
     wrl @1, 0xC00E0004 ; disable watch dog timer
     jmp start;
 complete

start:

    ;
    ;     0           #SP      #BP = sizeof.DM
    ;     |           |        |
    ;     v           v        v
    ; DM |...........|....|....|
    ;                      ^^^^
    ;                 ^^^^ tsk.retval
    ;                 #start.stop
    ; #SP
    ; #BP
    jmp   kernel_start
    getl  sizeof.DM
    getl  0
    getl  start.stop
    subl  @3, sizeof.tsk.retval
    subl  @1, sizeof.ptr
    wrl   @4, @2                  ; __libc_main::retval = 0
    wrl   @4, @2                  ; __libc_main::retaddr = start.stop
    setl  #BP, @7                 ; BP = sizeof (DM)
    setl  #SP, @4                 ; SP = BP - sizeof.tsk.retval - sizeof.ptr

    ;

    setl  #IHOOKADDR, master.isr

	setl #INTR, 0xFFFFFFFF ;
	setl #MSKR, 0 ;

    getq  #PSW
    or    @1, PSW.ONIRQS
    setq  #PSW, @1

  complete                        ; => __libc_main


  start.stop:                     ; <= __libc_main

    ;jmp   start

    ;

    rdl   #SP, sizeof.ptr
    wrl   @1, tsk.retval

    ;

    getl  #SP
    addl  @1, sizeof.ptr + sizeof.tsk.retval
    setl  #SP, @1                 ; SP = BP = sizeof.DM

    ;

    ;setq  #FORCE, INTR.PRGE

  complete

;===============================================================================
; __libc_main
;-------------------------------------------------------------------------------
;===============================================================================
  .alias __libc_main.ret.value  #SP,sizeof.ptr


 ;===============================================================================
; Interrupt Vector Setting (IVS):
; void set_irq_vector(int function, int number_irq);
;===============================================================================
 set_irq_vector:
	rdl #SP + 4 ;function name
	rdl #SP + 8 ;irq number
    getl irq.desc.tbl
    mull @2, sizeof.IDT.item
    addl @1, @2
	wrl @5, @1 ;set irq vector
	;rdl #SP
    ;jmp @1
    jmp [#SP]
 complete


;===============================================================================
; Interrupt Service Routine (ISR):
;===============================================================================


.data

  .local master.isr.retaddr
  master.isr.retaddr: .long
  test_irq_psw: .long

.text

  master.isr:


    getl  #SP
    getl  master.isr.stop
    getl  #INTNUMR
    getl  #IRETADDR
    getl  irq.desc.tbl
    mull @3, sizeof.IDT.item
    addl @1, @2
    rdl   @1
	je @1, master.isr.stop
	jne @2, @2
    subl  @10, sizeof.ptr
    wrl   @10, @1
    setl  #SP, @2


    wrl   @10, master.isr.retaddr

    getl 1
    sllq @1, @13
    setq #INTR, @1
  complete

  master.isr.stop:

    rdl   master.isr.retaddr
    jmp   @1

    getq  #PSW
    or    @1, PSW.ONIRQS
    setq  #PSW, @1

    getl #SP
    addl @1, sizeof.ptr
    setl #SP, @1

  complete

  STUB.isr:
    rdl   #SP
    jmp   @1
  complete


;===============================================================================
;   union Result {
;     uint64_t value;
;     struct {
;       int32_t Quotient;
;       int32_t Remainder;
;     } fields;
;   }
; Result divsl (int32_t Numerator, int32_t Denominator);
;
;===============================================================================

.text

  .alias div.Denominator  #BP, +20
  .alias div.Numerator    #BP, +16
  .alias div.Remainder    #BP, +12
  .alias div.Quotient     #BP, +8
  .alias div.RetAddr      #BP, +4
  .alias div.PrevBP       #BP
  .alias div.Index        #BP, -4

  .alias div.arg.Denominator  #SP, -4
  .alias div.arg.Numerator    #SP, -8
  .alias div.arg.Remainder    #SP, -12
  .alias div.arg.Quotient     #SP, -16
  .alias div.arg.RetAddr      #SP, -20

  .alias div.sizeof.args      4*sizeof.long

  .alias div.res.Remainder    #SP, +8
  .alias div.res.Quotient     #SP, +4

  divsl:
  divsl.prologue:

    jmp   divsl.check.numerator

    getl  #SP
    getl  #BP
    subl  @2, sizeof.ptr
    wrl   @2, #SP,-sizeof.ptr
    setl  #SP, @2
    setl  #BP, @3

  complete
  divsl.check.numerator:               ; if (N < 0)

    rdsl   div.Numerator
    js    @1, divsl.neg.num
    jns   @2, divsl.pos.num
    wrl	  @0,0x7FFFC

  complete
  divsl.neg.num:                       ; if (D < 0)

    rdsl   div.Denominator
    js    @1, divsl.neg.num.neg.denom
    jns   @2, divsl.neg.num.pos.denom
    wrl	  @0,0x7FFFC

  complete
  divsl.neg.num.neg.denom:             ; divl (-N,-D)

    jmp   divl

    getl  #SP

    rdsl  div.Denominator
    rdsl  div.Numerator
    getl  divsl.neg.num.neg.denom.returns

    insubsl @3, 0
    insubsl @3, 0
    subl  @6, div.sizeof.args + sizeof.ptr

    wrsl  @3, div.arg.Denominator
    wrsl  @3, div.arg.Numerator
    wrl   @6, div.arg.RetAddr

    setl  #SP, @4

  complete
  divsl.neg.num.neg.denom.returns:      ; R = -R

    jmp   divsl.epilogue

    getl  #SP

    rdl   div.res.Remainder
    rdl   div.res.Quotient

    insubsl @2, 0
    addl  @4, div.sizeof.args + sizeof.ptr

    wrl   @2, div.Remainder
    wrl   @4, div.Quotient

    setl  #SP, @3

  complete
  divsl.neg.num.pos.denom:              ; divl (-N,D)

    jmp   divl

    getl  #SP

    rdsl  div.Denominator
    rdsl  div.Numerator
    getl  divsl.neg.num.pos.denom.returns

    insubsl @2, 0
    subl  @5, div.sizeof.args + sizeof.ptr

    wrsl  @5, div.arg.Denominator
    wrsl  @3, div.arg.Numerator
    wrl   @5, div.arg.RetAddr

    setl  #SP, @4

  complete
  divsl.neg.num.pos.denom.returns:      ; Q = -Q; R = -R;

    jmp   divsl.epilogue

    getl  #SP

    rdl   div.res.Remainder
    rdl   div.res.Quotient

    insubsl @2, 0
    insubsl @2, 0
    addl  @5, div.sizeof.args + sizeof.ptr

    wrl   @3, div.Remainder
    wrl   @3, div.Quotient

    setl  #SP, @3

  complete
  divsl.pos.num:                        ; if (D < 0)

    rdsl  div.Denominator
    js    @1, divsl.pos.num.neg.denom
    jns   @2, divsl.pos.num.pos.denom
    wrl	  @0,0x7FFFC

  complete
  divsl.pos.num.neg.denom:              ; divl (N,-D)

    jmp   divl

    getl  #SP

    rdsl  div.Denominator
    rdsl  div.Numerator
    getl  divsl.pos.num.neg.denom.returns

    insubsl @3, 0
    subl  @5, div.sizeof.args + sizeof.ptr

    wrsl  @2, div.arg.Denominator
    wrsl  @5, div.arg.Numerator
    wrl   @5, div.arg.RetAddr

    setl  #SP, @4

  complete
  divsl.pos.num.neg.denom.returns:      ; Q = -Q

    jmp   divsl.epilogue

    getl  #SP

    rdl   div.res.Remainder
    rdl   div.res.Quotient

    insubsl @1, 0
    addl  @4, div.sizeof.args + sizeof.ptr

    wrl   @4, div.Remainder
    wrl   @3, div.Quotient

    setl  #SP, @3

  complete
  divsl.pos.num.pos.denom:               ; divl (N,D)

    jmp   divl

    getl  #SP

    rdsl  div.Denominator
    rdsl  div.Numerator
    getl  divsl.pos.num.pos.denom.returns

    subl  @4, div.sizeof.args + sizeof.ptr

    wrsl  @4, div.arg.Denominator
    wrsl  @4, div.arg.Numerator
    wrl   @4, div.arg.RetAddr

    setl  #SP, @4

  complete
  divsl.pos.num.pos.denom.returns:

    jmp   divsl.epilogue

    getl  #SP

    rdl   div.res.Remainder
    rdl   div.res.Quotient

    addl  @3, div.sizeof.args + sizeof.ptr

    wrl   @3, div.Remainder
    wrl   @3, div.Quotient

    setl  #SP, @3

  complete
  divsl.epilogue:

    ; Строим возврат.

    rdl   div.RetAddr
    jmp   @1

    getl  #BP
    rdl   div.PrevBP
    setl  #BP, @1
    addl  @3, sizeof.ptr
    setl  #SP, @1
    wrl	  @0,0x7FFFC

  complete

;==============================================================================
;   union Result {
;     uint64_t value;
;     struct {
;       uint32_t Quotient;
;       uint32_t Remainder;
;     } fields;
;   }
; Result divl (uint31_t Numerator, uint31_t Denominator);
;==============================================================================

  divl:
  divl.prologue:

    jmp   divl.arg.check

    getl  #SP
    getl  #BP
    subl  @2, sizeof.ptr
    wrl   @2, #SP,-sizeof.ptr
    setl  #SP, @2
    setl  #BP, @3

  complete
  divl.arg.check:

    rdl   div.Numerator
    rdl   div.Denominator

/*
*    subl  @2, @1                        ; if ( N < D )
*    jb    @1, divl.simple.result
*    jae   @2, divl.prepare
*/

    maxl  @1, @2
    subl  @3, @1                        ; if ( N < D )
    jne    @1, divl.simple.result
    je   @2, divl.prepare
    wrl	  @0,0x7FFFC

  complete

  divl.simple.result:

    jmp   divl.epilogue
    getl  0
    rdl   div.Numerator
    wrl   @2, div.Quotient
    wrl   @2, div.Remainder

  complete
  divl.prepare:

    jmp   divl.check.remainder
    rdl   div.Numerator
    rdl   div.Denominator


    wrl   @2, div.Remainder
    norml @3
    norml @3


    subl  @1, @2

    slll  @5, @1


    wrl   @1, div.Denominator

    getl  1
    slll  @1, @4
    wrl   @1, div.Index

    getl  0
    wrl   @1, div.Quotient

  complete
  divl.check.remainder:




    rdl   div.Remainder
    rdl   div.Denominator

/*
*    subl  @2, @1                        ; if R < D
*    jb    @1, divl.next.order
*    jae   @2, divl.calculate.remainder
*/

    maxl @1, @2
    subl @3, @1
    jne    @1, divl.next.order
    je   @2, divl.calculate.remainder
    wrl	  @0,0x7FFFC

  complete
  divl.calculate.remainder:

    jmp   divl.next.order
    rdl   div.Remainder
    rdl   div.Denominator
    subl  @2, @1
    wrl   @1, div.Remainder

    rdl   div.Quotient
    rdl   div.Index
    addl  @2, @1
    wrl   @1, div.Quotient

  complete
  divl.next.order:

    rdl   div.Denominator
    rdl   div.Index
    slrl  @2, 1                         ;     (D'>>1)
    slrl  @2, 1                         ;     (I >>1)
    wrl   @2, div.Denominator           ; D'<=(D'>>1)
    wrl   @2, div.Index                 ; I <=(I >>1)
                                        ; if (I == 0)
    je    @3, divl.epilogue             ; then
    jne   @4, divl.check.remainder
    wrl	  @0,0x7FFFC

  complete
  divl.epilogue:

    rdl   div.RetAddr
    jmp   @1

    getl  #BP
    rdl   div.PrevBP
    setl  #BP, @1
    addl  @3, sizeof.ptr
    setl  #SP, @1
    wrl	  @0,0x7FFFC

  complete




;===============================================================================
;
; void asm_setl_PSW(int ARG1);
;
;===============================================================================

asm_setl_PSW:
    rdl #SP
	rdl #SP + 4
	setl #48, @1
	jmp @3
complete

;===============================================================================
;
; int asm_getl_PSW();
;
;===============================================================================

asm_getl_PSW:
    getl #48
	wrl @1, #SP + 4
    rdl #SP
	jmp @1
complete

;===============================================================================
;
; void asm_setl_MSKR(int ARG1);
;
;===============================================================================

asm_setl_MSKR:
    rdl #SP
	rdl #SP + 4
	setl #50, @1
	jmp @3
complete

;===============================================================================
;
; int asm_getl_MSKR();
;
;===============================================================================

asm_getl_MSKR:
    getl #50
	wrl @1, #SP + 4
    rdl #SP
	jmp @1
complete

;===============================================================================
; void asm_setl_INTR(int ARG1);
;
;===============================================================================

asm_setl_INTR:
    rdl #SP
	rdl #SP + 4
	setl #49, @1
	jmp @3
complete

;===============================================================================
;
; int asm_getl_INTR();
;
;===============================================================================

asm_getl_INTR:
    getl #49
	wrl @1, #SP + 4
    rdl #SP
	jmp @1
complete

;===============================================================================
; int asm_getl_INTNUM();
;
;===============================================================================

asm_getl_INTNUM:
    getl #56
	wrl @1, #SP + 4
    rdl #SP
	jmp @1
complete

;===============================================================================
; void asm_setl_PLLCR(int ARG1);
;
;===============================================================================

asm_setl_PLLCR:
    rdl #SP
	rdl #SP + 4
	setl #16, @1
	jmp @3
complete

;===============================================================================
;
; int asm_getl_PLLCR();
;
;===============================================================================

asm_getl_PLLCR:
    getl #16
	wrl @1, #SP + 4
    rdl #SP
	jmp @1
complete

;===============================================================================
; void asm_setl_PLLSTR(int ARG1);
;
;===============================================================================

asm_setl_PLLSTR:
    rdl #SP
	rdl #SP + 4
	setl #17, @1
	jmp @3
complete

;===============================================================================
;
; int asm_getl_PLLSTR();
;
;===============================================================================

asm_getl_PLLSTR:
    getl #17
	wrl @1, #SP + 4
    rdl #SP
	jmp @1
complete

;===============================================================================
; void asm_setl_PLLTMCR(int ARG1);
;
;===============================================================================

asm_setl_PLLTMCR:
    rdl #SP
	rdl #SP + 4
	setl #18, @1
	jmp @3
complete

;===============================================================================
; int asm_getl_PLLTMCR();
;
;===============================================================================

asm_getl_PLLTMCR:
    getl #18
	wrl @1, #SP + 4
    rdl #SP
	jmp @1
complete

;===============================================================================
; int asm_set_PLL(unsigned char RCNT, unsigned char NCNT, unsigned char MXVCO, unsigned char Enable);
;
;===============================================================================

asm_set_PLL:
    rdl #SP + 8 ;RCNT
    rdl #SP + 12 ;NCNT
    rdl #SP + 20 ;EN
    andl @3, 0x7
    andl @3, 0x7F
    andl @3, 0x1
    slll @3, 12
    slll @2, 15
    addl @4, @2
    addl @1, @2
    setl #16, @1
    jmp asm_wait_pll_lock
complete

asm_wait_pll_lock:
    getl #17
    getl 1
    andl @1, @2
    je @1, asm_wait_pll_lock
    jne @2, asm_pll_set
complete

asm_pll_set:
    rdl #SP + 8 ;RCNT
    rdl #SP + 12 ;NCNT
    rdl #SP + 16 ;MXVCO
    rdl #SP + 20 ;EN
    andl @4, 0x7
    andl @4, 0x7F
    andl @4, 0xF
    andl @4, 0x1
    slll @4, 12
    slll @3, 8
    slll @3, 15
    addl @6, @3
    addl @3, @2
    addl @1, @2
    setl #16, @1
    rdl #SP
	jmp @1
complete

.end
