/*
 * ttyio_termcap.h
 *
 *  Created on: 02.08.2011
 *      Author: Gerald Hoch
 */

#ifndef TERMCAP_H_
#define TERMCAP_H_

static const char __attribute__ ((section (".rodata"))) termcap[][700]=
{
	{"ex|embox|embox console:\
	:am:xn:bs:\
	:co#80:li#25:\
	:is=\\E[0m:\
	:cd=\\E[0J:cl=\\E[H\\E[0J:\
	:so=\\E[7m:se=\\E[0m:\
	:us=\\E[4m:ue=\\E[0m:\
	:mb=\\E[5m:md=\\E[1m:\
	:mr=\\E[7m:me=\\E[0m:\
	:sr=\\EM:\
	:cm=\\E[%i%d;%dH:\
	:ho=\\E[H:\
	:al=\\E[L:AL=\\E[%dL:\
	:ce=\\E[K:\
	:DC=\\E[%dP:dc=\\E[P:\
	:DL=\\E[%dM:dl=\\E[M:\
	:DO=\\E[%dB:do=\\E[B:\
	:IC=\\E[%d@:ic=\\E[@:\
	:it#8:\
	:le=^H:LE=\\E[%dD:nd=\\E[C:\
	:ri=\\E[C:RI=\\E[%dC:\
	:up=\\E[A:UP=\\E[%dA:\
	:ku=\\E[A:kd=\\E[B:\
	:kl=\\E[D:kr=\\E[C:\
	:kh=\\E[H:\
	:k0=\\E[Y:l0=End:\
	:k1=\\E[V:l1=PgUp:\
	:k2=\\E[U:l2=PgDn:\
	:k3=\\E[T:l3=Num +:\
	:k4=\\E[S:l4=Num -:\
	:k5=\\E[G:l5=Num 5:"},
	{"du|dialup|Dialup line:\
	:bs:co#80:li#24:"},
	{"db|dumb|Really dumb terminal:\
		:bs:co#80:li#24:"},
	{"lp|lp|Line Printer:\
		:co#80:li#66:"},
	{"li|ansi|Ansi standard crt:\
		:am:bs:cd=\\E[J:ce=\\E[K:cl=\\E[2J\\E[H:cm=\\E[%i%d;%dH:co#80:\
		:dn=\\E[B:me=\\E[0m:mb=\\E[5m:mr=\\E[7m:md=\\E[1m:ho=\\E[H:li#24:\
		:nd=\\E[C:ms:pt:so=\\E[7m:se=\\E[0m:us=\\E[4m:ue=\\E[0m:up=\\E[A:\
		:kb=^h:ku=\\E[A:kd=\\E[B:kl=\\E[D:kr=\\E[C:\\"},
	{"d0|vt100|vt100-am|vt100am|dec-vt100|dec vt100:\
		:do=^J:co#80:li#24:cl=\\E[;H\\E[2J:sf=\\ED:\
		:le=^H:bs:am:cm=\\E[%i%d;%dH:nd=\\E[C:up=\\E[A:\
		:ce=\\E[K:cd=\\E[J:so=\\E[7m:se=\\E[m:us=\\E[4m:ue=\\E[m:\
		:md=\\E[1m:mr=\\E[7m:mb=\\E[5m:me=\\E[m:is=\\E[1;24r\\E[24;1H:\
		:rs=\\E>\\E[?3l\\E[?4l\\E[?5l\\E[?7h\\E[?8h:ks=\\E[?1h\\E=:ke=\\E[?1l\\E>:\
		:ku=\\EOA:kd=\\EOB:kr=\\EOC:kl=\\EOD:kb=^H:\
		:ho=\\E[H:k1=\\EOP:k2=\\EOQ:k3=\\EOR:k4=\\EOS:pt:sr=\\EM:vt#3:xn:\
		:sc=\\E7:rc=\\E8:cs=\\E[%i%d;%dr:"}
};



#endif /* TERMCAP_H_ */
