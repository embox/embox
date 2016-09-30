/*
 *
*/

.ifndef .LALIASES.INC
.equiv .LALIASES.INC, 1

/*
 *
*/

.ifdef .LMCp0411100101
 //
 .alias SP		39
 .alias BP		38
 .alias SI		37
 .alias DI		36
 .alias CX		35
 .alias sizeof.DM	0x20000
.else
 .ifdef .LMCp042R100102
  //
  .alias SP		15
  .alias BP		14
  .alias SI		13
  .alias DI		12
  .alias CX		11
  .alias sizeof.DM	0x7FFF4
 .else
  .err "Unsupported architecture"
  .end
 .endif
.endif

/*
 *
*/

.alias sizeof.byte	1
.alias sizeof.short	2
.alias sizeof.long	4
.alias sizeof.quad	8
.alias sizeof.float	sizeof.long
.alias sizeof.double	sizeof.quad
.alias sizeof.ptr	sizeof.long

.endif
