#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __MACH__
#define setjmp(e) sigsetjmp(e, 0)
#define longjmp siglongjmp
#endif
#ifdef _WIN32
#include <windows.h>
#define X __declspec(dllexport)
#else
#define X
#endif

#ifndef _WIN32
#include <sys/time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/utsname.h>
#endif

#include <sys/select.h>

typedef int lval;
lval *o2c(lval o) {
	return (lval *) (o - 1);
}
lval c2o(lval * c)
{
	return (lval) c + 1;
}
int cp(lval o)
{
	return (o & 3) == 1;
}
lval *o2a(lval o) {
	return (lval *) (o - 2);
}
lval a2o(lval * a)
{
	return (lval) a + 2;
}
int ap(lval o)
{
	return (o & 3) == 2;
}
lval *o2s(lval o) {
	return (lval *) (o - 3);
}
char *o2z(lval o) {
	return (char *) (o - 3 + 2 * sizeof(lval));
}
lval s2o(lval * s)
{
	return (lval) s + 3;
}
int sp(lval o)
{
	return (o & 3) == 3;
}
typedef lval (*func_t)(void*, ...);

struct symbol_init {
	const char *name;
	func_t fun;
	int argc;
	func_t setfun;
	int setargc;
	lval sym;
};
extern struct symbol_init symi[];
#define TRUE symi[1].sym
#define T g[-2]
#define U g[-3]
#define V g[-4]
#define W g[-5]
#define NF(n) lval *g; g=f+n+3; f[1]=0; g[-1]=(n<<5)|16; *g=*f;
#define E *f
#define NE *g
lval car(lval c)
{
	return (c & 3) == 1 ? o2c(c)[0] : 0;
}
lval cdr(lval c)
{
	return (c & 3) == 1 ? o2c(c)[1] : 0;
}
lval caar(lval c) {
	return car(car(c));
} lval lread(lval *);
lval cdar(lval c) {
	return cdr(car(c));
} lval evca(lval *, lval);
lval cadr(lval c) {
	return car(cdr(c));
} int dbgr(lval *, int, lval, lval *);
lval cddr(lval c) {
	return cdr(cdr(c));
} void print(lval);
lval set_car(lval c, lval val)
{
	return o2c(c)[0] = val;
}
lval set_cdr(lval c, lval val)
{
	return o2c(c)[1] = val;
}
lval *binding(lval * f, lval sym, int type, int *macro)
{
	lval env;
st:
	for (env = E; env; env = cdr(env)) {
		lval e = caar(env);
		if (type || cp(e) ? car(e) == sym && (cdr(e) >> 4) == type : e == sym) {
			if (macro)
				*macro = cp(e) && cdr(e) & 8;
			return o2c(car(env)) + 1;
		}
	}
	if (macro)
		*macro = (o2a(sym)[8] >> type) & 32;
	if (type > 2) {
		dbgr(f, type, sym, &sym);
		goto st;
	}
	return o2a(sym) + 4 + type;
}
lval *memory;
lval *memf;
int memory_size;
lval *stack;
lval xvalues = 8;
lval dyns = 0;
jmp_buf top_jmp;
lval pkg;
lval pkgs;
lval kwp = 0;
void gcm(lval v) {
	lval *t;
	int i;
st:	t = (lval *) (v & ~3);
	if (v & 3 && !(t[0] & 4)) {
		t[0] |= 4;
		switch (v & 3) {
		case 1:
			gcm(t[0] - 4);
			v = t[1];
			goto st;
		case 2:
			gcm(t[1] - 4);
			if (t[0] >> 8) {
				for (i = 1; i < t[0] >> 8; i++)
					gcm(t[i + 1]);
				v = t[i + 1];
				goto st;
			}
		}
	}
}

lval gc(lval * f)
{
	int i;
	lval *m;
	int l;
	int u = 0;
	int ml = 0;
	//printf(";garbage collecting...\n");
	while (memf) {
		lval *n = (lval *) memf[0];
		memset(memf, 0, 4 * memf[1]);
		memf = (lval *) n;
	}
	gcm(xvalues);
	gcm(pkgs);
	gcm(dyns);
	for (; f > stack; f--) {
		if ((*f & 3) && (*f < (int)memory || *f > (int)memory + memory_size / 4))
			//printf("%x\n", *f);
		gcm(*f);
	}
	memf = 0;
	m = memory;
	i = 0;
	while (m < memory + memory_size / 4) {
		l = ((m[1] & 4 ? m[0] >> 8 : 0) + 1) & ~1;
		if (m[0] & 4) {
			if (u) {
				m[-ml] = (lval) memf;
				m[1 - ml] = ml;
				memf = m - ml;
				u = 0;
				i += ml;
			}
		} else {
			if (!u)
				ml = 0;
			ml += l + 2;
			u = 1;
		}
		m[0] &= ~4;
		m += l + 2;
	}
	if (u) {
		m[-ml] = (lval) memf;
		m[1 - ml] = ml;
		memf = m - ml;
		i += ml;
	}
	//printf(";done. %d free.\n", i);
	return 0;
}
lval *m0(lval * g, int n)
{
	lval *m = memf;
	lval *p = 0;
	n = (n + 1) & ~1;
	for (; m; m = (lval *) m[0]) {
		if (n <= m[1]) {
			if (m[1] == n)
				if (p)
					p[0] = m[0];
				else
					memf = (lval *) m[0];
			else {
				m[1] -= n;
				m += m[1];
			}
			return m;
		} p = m;
	} return 0;
}
lval *ma0(lval * g, int n) {
	lval *m;
st:	m = m0(g, n + 2);
	if (!m) {
		gc(g);
		goto st;
	} *m = n << 8;
	return m;
}
lval *ms0(lval * g, int n) {
	lval *m;
st:	m = m0(g, (n + 12) / 4);
	if (!m) {
		gc(g);
		goto st;
	} *m = (n + 4) << 6;
	return m;
}
lval *mb0(lval * g, int n) {
	lval *m;
st:	m = m0(g, (n + 95) / 32);
	if (!m) {
		gc(g);
		goto st;
	} *m = (n + 31) << 3;
	return m;
}
X lval ma(lval * g, int n,...) {
	va_list v;
	int i;
	lval *m;
st:	va_start(v, n);
	m = m0(g, n + 2);
	if (!m) {
		for (i = -1; i < n; i++)
			gcm(va_arg(v, lval));
		gc(g);
		goto st;
	}
	*m = n << 8;
	for (i = -1; i < n; i++)
		m[2 + i] = va_arg(v, lval);
	return a2o(m);
}
X lval ms(lval * g, int n,...) {
	va_list v;
	int i;
	lval *m;
st:	va_start(v, n);
	m = m0(g, n + 2);
	if (!m) {
		gc(g);
		goto st;
	} *m = n << 8;
	for (i = -1; i < n; i++)
		m[2 + i] = va_arg(v, lval);
	return s2o(m);
}
double o2d(lval o)
{
	return sp(o) ? *(double *) (o2s(o) + 2) : o >> 5;
}
lval d2o(lval * g, double d)
{
	lval x = (lval) d << 5 | 16;
	lval *a;
	if (o2d(x) == d)
		return x;
	a = ma0(g, 2);
	a[1] = 84;
	*(double *) (a + 2) = d;
	return s2o(a);
}
int o2i(lval o) {
	return (int) o2d(o);
}
unsigned o2u(lval o) {
	return (unsigned) o2d(o);
}
lval cons(lval * g, lval a, lval d) {
	lval *c = m0(g, 2);
	if (!c) {
		gcm(a);
		gcm(d);
		gc(g);
		c = m0(g, 2);
	} c[0] = a;
	c[1] = d;
	return c2o(c);
}
int string_equal_do(lval a, lval b)
{
	int i;
	for (i = 0; i < o2s(a)[0] / 64 - 4; i++)
		if (o2z(a)[i] != o2z(b)[i])
			return 0;
	return 1;
}
int string_equal(lval a, lval b)
{
	return a == b || (sp(a) && sp(b) &&
		o2s(a)[1] == 20 && o2s(b)[1] == 20 && o2s(a)[0] == o2s(b)[0]
			  && string_equal_do(a, b));
}
lval argi(lval a, lval * b) {
	if (cp(a)) {
		*b = cdr(a);
		return car(a);
	}
	*b = 0;
	return a;
}
lval rest(lval * h, lval * g)
{
	lval *f = h - 1;
	lval r = 0;
	for (; f >= g; f--)
		r = cons(h, *f, r);
	return r;
}
lval args(lval *, lval, int);
lval argd(lval * f, lval n, lval a)
{
	if (cp(n)) {
		lval *h = f;
		for (; a; a = cdr(a))
			*++h = car(a);
		++h;
		*++h = *f;
		return args(f, n, h - f - 2);
	}
	return cons(f, cons(f, n, a), *f);
}
lval args(lval * f, lval m, int c)
{
	lval *g = f + 1;
	lval *h = f + c + 2;
	int t;
	lval k, *l;
st:	t = 0;
	while (cp(m)) {
		lval n = car(m);
		m = cdr(m);
		switch (cp(n) ? -1 : o2a(n)[7] >> 3) {
		case 2:
		case 3:
			t = 1;
			continue;
		case 4:
			t = 2;
			continue;
		case 5:
			t = -2;
			continue;
		case 6:
			t = 4;
			continue;
		case 7:
			t = 5;
			continue;
		default:
			switch (t) {
			case 0:
				if (g >= h - 1) {
					dbgr(g, 7, 0, h);
					goto st;
				} *h = argd(h, n, *g);
				break;
			case 1:
				*h = cons(h, cons(h, n, rest(h - 1, g)), *h);
				t = -1;
				continue;
			case 2:
				n = argi(n, &k);
				*h = argd(h, n, g < h - 1 ? *g : evca(h, k));
				break;
			case -2:
				n = argi(n, &k);
				for (l = g; l < h - 1; l += 2)
					if (string_equal(o2a(n)[2], o2a(*l)[2]) && o2a(*l)[9] == kwp) {
						k = l[1];
						break;
					} *h = argd(h, n, l < h - 1 ? k : evca(h, k));
				continue;
			case 4:
				*h = cons(h, cons(h, n, rest(h - 1, f + 1)), *h);
				t = 0;
				continue;
			case 5:
				*h = cons(h, cons(h, n, f[-1]), *h);
				t = 0;
				continue;
			}
		} g++;
	} if (m)
		return cons(h, cons(h, m, rest(h - 1, g)), *h);
	if (g < h - 1 && t >= 0) {
		h[-1] = (c << 5) | 16;
		dbgr(h, 6, 0, h);
		goto st;
	} return *h;
}
lval eval_body(lval * f, lval ex)
{
	NF(1) T = 0;
	for (; ex; ex = cdr(ex))
		T = evca(g, ex);
	return T;
}
int map_eval(lval * f, lval ex) {
	lval *g = f + 3;
	for (; ex; ex = cdr(ex), g++) {
		g[-1] = ((g - f - 3) << 5) | 16;
		*g = *f;
		g[-1] = evca(g, ex);
	} return g - f - 3;
}
lval eval(lval * f, lval expr) {
	NF(1) T = 0;
	T = cons(g, expr, 0);
	return evca(g, T);
}
lval rvalues(lval * g, lval v)
{
	return xvalues == 8 ? cons(g, v, 0) : xvalues;
}
lval mvalues(lval a) {
	xvalues = a;
	return car(a);
}
lval infn(lval * f, lval * h)
{
	jmp_buf jmp;
	lval vs;
	lval *g = h + 1;
	lval fn = *f;
	int d = h - f - 1;
	h[1] = o2a(fn)[3];
	NE = args(f, o2a(fn)[4], d);
	g[-1] = cons(g, dyns, ms(g, 1, 20, &jmp));
	NE = cons(g, cons(g, cons(g, o2a(fn)[6], 64), g[-1]), NE);
	g[-1] = (d << 5) | 16;
	if (!(vs = setjmp(jmp)))
		return eval_body(g, o2a(fn)[5]);
	return mvalues(car(vs));
}
X lval call(lval * f, lval fn, unsigned d)
{
	lval *g = f + d + 3;
	xvalues = 8;
	if (o2a(fn)[1] == 20)
		fn = o2a(fn)[5];
	if (o2a(fn)[0] & 16)
		fn = o2a(fn)[3];
	*++f = fn;
	fn = o2a(fn)[2];
	if (d < (unsigned) o2s(fn)[3])
		dbgr(g, 7, 0, f);
	if (d > (unsigned) o2s(fn)[4])
		dbgr(g, 6, 0, f);
	return ((func_t) o2s(fn)[2]) (f, f + d + 1);
}
lval eval_quote(lval * g, lval ex) {
	return car(ex);
}
int specp(lval * f, lval ex, lval s)
{
	for (; ex; ex = cdr(ex))
		if (ap(caar(ex)) && o2a(caar(ex))[7] == 3 << 3) {
			lval e = cdar(ex);
			for (; e; e = cdr(e))
				if (o2a(caar(e))[7] == 4 << 3) {
					lval sp = cdar(e);
					for (; sp; sp = cdr(sp))
						if (car(sp) == s)
							return 1;
				}
		} else
			break;
	return 0;
}
void unwind(lval * f, lval c)
{
	lval e;
	NF(0) for (; dyns != c; dyns = cdr(dyns))
		if (ap(car(dyns)))
			if (o2a(car(dyns))[1] == 52) {
				NE = o2a(car(dyns))[2];
				eval_body(g, o2a(car(dyns))[3]);
			} else
				for (e = o2a(car(dyns))[2]; e; e = cdr(e))
					o2a(caar(e))[4] = cdar(e);
		else
			o2s(car(dyns))[2] = 0;
}
lval eval_let(lval * f, lval ex) {
	lval r;
	NF(3) T = car(ex);
	U = E;
	V = 0;
	r = ma(g, 1, 84, 0);
	dyns = cons(g, r, dyns);
	for (; T; T = cdr(T)) {
		V = evca(g, cdar(T));
		if (o2a(caar(T))[8] & 128 || specp(g, cdr(ex), caar(T))) {
			o2a(r)[2] = cons(g, cons(g, caar(T), V), o2a(r)[2]);
		} else
			U = cons(g, cons(g, caar(T), V), U);
	} for (r = o2a(r)[2]; r; r = cdr(r)) {
		T = o2a(caar(r))[4];
		o2a(caar(r))[4] = cdar(r);
		set_cdr(car(r), T);
		U = cons(g, cons(g, caar(r), -8), U);
	} NE = U;
	T = eval_body(g, cdr(ex));
	unwind(g, cdr(dyns));
	return T;
}
lval eval_letm(lval * f, lval ex) {
	lval r;
	NF(2) T = U = 0;
	r = ma(g, 1, 84, 0);
	dyns = cons(g, r, dyns);
	for (T = car(ex); T; T = cdr(T)) {
		U = evca(g, cdar(T));
		if (o2a(caar(T))[8] & 128 || specp(g, cdr(ex), caar(T))) {
			o2a(r)[2] = cons(g, cons(g, caar(T), o2a(caar(T))[4]), o2a(r)[2]);
			o2a(caar(T))[4] = U;
			U = -8;
		} U = cons(g, caar(T), U);
		NE = cons(g, U, NE);
	}
	T = eval_body(g, cdr(ex));
	unwind(g, cdr(dyns));
	return T;
}
lval eval_progv(lval * f, lval ex) {
	lval r;
	NF(2) T = U = 0;
	r = ma(g, 1, 84, 0);
	T = evca(g, ex);
	U = evca(g, cdr(ex));
	dyns = cons(g, r, dyns);
	for (; T && U; T = cdr(T), U = cdr(U)) {
		o2a(r)[2] = cons(g, cons(g, car(T), o2a(car(T))[4]), o2a(r)[2]);
		o2a(car(T))[4] = car(U);
	} T = eval_body(g, cddr(ex));
	unwind(f, cdr(dyns));
	return T;
}
lval eval_flet(lval * f, lval ex) {
	NF(4) V = W = 0;
	U = E;
	for (T = car(ex); T; T = cdr(T)) {
		V = ma(g, 5, 212, ms(f, 3, 212, infn, 0, -1), E, cadr(car(T)), cddr(car(T)), caar(T));
		W = cons(g, caar(T), 16);
		V = cons(g, W, V);
		U = cons(g, V, U);
	} NE = U;
	return eval_body(g, cdr(ex));
}
lval eval_labels(lval * f, lval ex) {
	NF(4) V = W = 0;
	U = E;
	for (T = car(ex); T; T = cdr(T))
		U = cons(g, 0, U);
	NE = U;
	for (T = car(ex); T; T = cdr(T), U = cdr(U)) {
		V = ma(g, 5, 212, ms(f, 3, 212, infn, 0, -1), NE, cadr(car(T)), cddr(car(T)), caar(T));
		W = cons(g, caar(T), 16);
		set_car(U, cons(g, W, V));
	} return eval_body(g, cdr(ex));
}
lval eval_macrolet(lval * f, lval ex) {
	NF(4) V = W = 0;
	U = E;
	for (T = car(ex); T; T = cdr(T)) {
		V = ma(g, 5, 212, ms(f, 3, 212, infn, 0, -1), E, cadr(car(T)), cddr(car(T)), caar(T));
		W = cons(g, caar(T), 24);
		V = cons(g, W, V);
		U = cons(g, V, U);
	} NE = U;
	return eval_body(g, cdr(ex));
}
lval eval_symbol_macrolet(lval * f, lval ex)
{
	NF(3) V = 0;
	U = E;
	for (T = car(ex); T; T = cdr(T)) {
		V = cons(g, caar(T), 8);
		V = cons(g, V, cadr(car(T)));
		U = cons(g, V, U);
	} NE = U;
	return eval_body(g, cdr(ex));
}
lval eval_setq(lval * f, lval ex) {
	lval r;
	do {
		r = evca(f, cdr(ex));
		*binding(f, car(ex), 0, 0) = r;
		ex = cddr(ex);
	} while (ex);
	return r;
}
lval eval_function(lval * f, lval ex) {
	lval x;
	ex = car(ex);
	if (cp(ex))
		if (car(ex) == symi[75].sym) {
			lval n = 0;
			x = cddr(ex);
			if (!cdr(x) && caar(x) == symi[23].sym) {
				x = car(x);
				n = cadr(x);
				x = cddr(x);
			}
			return ma(f, 5, 212, ms(f, 3, 212, infn, 0, -1), E, cadr(ex), x, n);
		} else
			x = *binding(f, cadr(ex), 2, 0);
	else
		x = *binding(f, ex, 1, 0);
	if (x != 8)
		return x;
	dbgr(f, 1, ex, &x);
	return x;
}
lval eval_tagbody(lval * f, lval ex)
{
	jmp_buf jmp;
	lval tag;
	lval e;
	NF(2) T = U = 0;
	U = ms(g, 1, 52, &jmp);
	dyns = cons(g, U, dyns);
	for (e = ex; e; e = cdr(e))
		if (ap(car(e))) {
			T = cons(g, dyns, U);
			NE = cons(g, cons(g, cons(g, car(e), 48), T), NE);
		} e = ex;
again:
	if (!(tag = setjmp(jmp))) {
		for (; e; e = cdr(e))
			if (!ap(car(e)))
				evca(g, e);
	} else
		for (e = ex; e; e = cdr(e))
			if (car(e) == tag) {
				e = cdr(e);
				goto again;
			}
	unwind(g, cdr(dyns));
	return 0;
}
lval eval_go(lval * f, lval ex) {
	lval b = *binding(f, car(ex), 3, 0);
	if (o2s(cdr(b))[2]) {
		unwind(f, car(b));
		longjmp(*(jmp_buf *) (o2s(cdr(b))[2]), car(ex));
	} dbgr(f, 9, car(ex), &ex);
	longjmp(top_jmp, 1);
}
lval eval_block(lval * f, lval ex)
{
	jmp_buf jmp;
	lval vs;
	NF(2) T = U = 0;
	T = ms(g, 1, 52, &jmp);
	U = cons(g, dyns, T);
	dyns = cons(g, T, dyns);
	NE = cons(g, cons(g, cons(g, car(ex), 64), U), NE);
	if (!(vs = setjmp(jmp))) {
		T = eval_body(g, cdr(ex));
		unwind(g, cdr(dyns));
		return T;
	}
	return mvalues(car(vs));
}
lval eval_return_from(lval * f, lval ex)
{
	lval b;
	jmp_buf *jmp;
	NF(1) T = 0;
	b = *binding(g, car(ex), 4, 0);
	jmp = (jmp_buf *) o2s(cdr(b))[2];
	if (jmp) {
		unwind(g, car(b));
		T = rvalues(g, evca(g, cdr(ex)));
		longjmp(*jmp, cons(g, T, 0));
	} dbgr(g, 8, car(ex), &T);
	longjmp(top_jmp, 1);
}
lval eval_catch(lval * f, lval ex)
{
	jmp_buf jmp;
	lval vs;
	lval oc = dyns;
	NF(2)
		T = U = 0;
	U = evca(g, ex);
	T = ms(g, 1, 20, &jmp);
	T = cons(g, U, T);
	dyns = cons(g, T, dyns);
	if (!(vs = setjmp(jmp)))
		vs = eval_body(g, cdr(ex));
	else
		vs = mvalues(car(vs));
	dyns = oc;
	return vs;
}
lval eval_throw(lval * f, lval ex) {
	lval c;
	NF(1) T = 0;
	T = evca(g, ex);
st:
	for (c = dyns; c; c = cdr(c))
		if (cp(car(c)) && caar(c) == T) {
			unwind(g, c);
			T = evca(g, cdr(ex));
			T = rvalues(g, T);
			longjmp(*(jmp_buf *) (o2s(cdar(c))[2]), cons(g, T, 0));
		}
	dbgr(g, 5, T, &T);
	goto st;
}
lval eval_unwind_protect(lval * f, lval ex) {
	NF(1) T = 0;
	T = ma(g, 2, 52, E, cdr(ex));
	dyns = cons(g, T, dyns);
	T = evca(g, ex);
	T = rvalues(g, T);
	unwind(g, cdr(dyns));
	return mvalues(T);
}
lval eval_if(lval * f, lval ex) {
	return evca(f, evca(f, ex) ? cdr(ex) : cddr(ex));
}
lval eval_multiple_value_call(lval * f, lval ex)
{
	lval *g = f + 3;
	lval l;
	f[1] = evca(f, ex);
	for (ex = cdr(ex); ex; ex = cdr(ex)) {
		*g = *f;
		g[-1] = ((g - f - 3) << 5) | 16;
		for (l = rvalues(g, evca(g, ex)); l; l = cdr(l)) {
			g[-1] = car(l);
			g++;
		}
	} xvalues = 8;
	return call(f, f[1], g - f - 3);
}
lval eval_multiple_value_prog1(lval * f, lval ex) {
	NF(1) T = 0;
	T = evca(g, ex);
	T = rvalues(g, T);
	eval_body(g, cdr(ex));
	return mvalues(T);
}
lval eval_declare(lval * f, lval ex)
{
	return 0;
}
lval l2(lval * f, lval a, lval b) {
	return cons(f, a, cons(f, b, 0));
}
lval eval_setf(lval * f, lval ex)
{
	lval r;
	int m;
	NF(1) T = 0;
ag:	if (!cp(car(ex))) {
		r = *binding(g, car(ex), 0, &m);
		if (!m)
			return *binding(g, car(ex), 0, 0)
				= evca(g, cdr(ex));
		set_car(ex, r);
		goto ag;
	} r = *binding(g, caar(ex), 2, 0);
	if (r == 8)
		dbgr(g, 1, l2(f, symi[33].sym, caar(ex)), &r);
	T = cons(g, cadr(ex), cdar(ex));
	return call(g, r, map_eval(g, T));
}
lval llist(lval * f, lval * h) {
	return rest(h, f + 1);
}
lval lvalues(lval * f, lval * h) {
	return mvalues(rest(h, f + 1));
}
lval lfuncall(lval * f, lval * h) {
	return call(f, f[1], h - f - 2);
}
lval lapply(lval * f, lval * h) {
	while (h[-1]) {
		h[0] = cdr(h[-1]);
		h[-1] = car(h[-1]);
		h++;
	} return call(f, f[1], h - f - 3);
}
lval leq(lval * f)
{
	return f[1] == f[2] ? TRUE : 0;
}
lval lcons(lval * f) {
	return cons(f, f[1], f[2]);
}
lval lcar(lval * f) {
	return car(f[1]);
}
lval setfcar(lval * f) {
	return set_car(f[2], f[1]);
}
lval lcdr(lval * f) {
	return cdr(f[1]);
}
lval setfcdr(lval * f) {
	return set_cdr(f[2], f[1]);
}
lval lequ(lval * f, lval * h) {
	double s = o2d(f[1]);
	for (f += 2; f < h; f++)
		if (s != o2d(*f))
			return 0;
	return TRUE;
}
lval lless(lval * f, lval * h) {
	double s = o2d(f[1]);
	for (f += 2; f < h; f++)
		if (s < o2d(*f))
			s = o2d(*f);
		else
			return 0;
	return TRUE;
}
lval lplus(lval * f, lval * h)
{
	double s = 0;
	for (f++; f < h; f++)
		s += o2d(*f);
	return d2o(f, s);
}
lval lminus(lval * f, lval * h) {
	double s = o2d(f[1]);
	f += 2;
	if (f < h)
		for (; f < h; f++)
			s -= o2d(*f);
	else
		s = -s;
	return d2o(f, s);
}
lval ltimes(lval * f, lval * h)
{
	double s = 1;
	for (f++; f < h; f++)
		s *= o2d(*f);
	return d2o(f, s);
}
lval ldivi(lval * f, lval * h) {
	double s = o2d(f[1]);
	f += 2;
	if (f < h)
		for (; f < h; f++)
			s /= o2d(*f);
	else
		s = 1 / s;
	return d2o(f, s);
}
lval ldpb(lval * f) {
	int s = o2i(car(f[2]));
	int p = o2i(cdr(f[2]));
	int m = (1 << s) - 1;
	return d2o(f, (o2i(f[1]) & m) << p | (o2i(f[3]) & ~(m << p)));
}
lval lldb(lval * f) {
	int s = o2i(car(f[1]));
	int p = o2i(cdr(f[1]));
	return d2o(f, o2i(f[2]) >> p & ((1 << s) - 1));
}
lval lfloor(lval * f, lval * h) {
	double n = o2d(f[1]);
	double d = h - f > 2 ? o2d(f[2]) : 1;
	double q = floor(n / d);
	return mvalues(l2(f, d2o(f, q), d2o(f, n - q * d)));
}
int gensymc = 0;
lval lgensym(lval * f) {
	lval *r = ms0(f, 4);
	r[1] = 20;
	sprintf((char *) (r + 2),
		"g%3.3d", gensymc++);
	return ma(f, 9, 20, s2o(r), 0, 8, 8, 8, -8, 16, 0, 0);
}
lval lcode_char(lval * f) {
	unsigned int c = o2u(f[1]);
	return c < 256 ? 32 * c + 24 : 0;
}
lval lchar_code(lval * f)
{
	return f[1] & ~8;
}
lval lmakef(lval * f) {
	return d2o(f, f - stack);
}
lval lfref(lval * f)
{
	return stack[o2i(f[1])];
}
lval stringify(lval * f, lval l)
{
	int i;
	lval *r;
	lval t = l;
	*++f = l;
	for (i = 0; t; i++, t = cdr(t));
	r = ms0(f, i);
	r[1] = 20;
	((char *) r)[i + 8] = 0;
	for (i = 8; l; i++, l = cdr(l))
		((char *) r)[i] = car(l) >> 5;
	return s2o(r);
}
lval lstring(lval * f, lval * h) {
	return stringify(f, rest(h, f + 1));
}
lval lival(lval * f) {
	return d2o(f, f[1]);
}
lval lmakei(lval * f, lval * h) {
	int i = 2;
	int l = o2i(f[1]);
	lval *r = ma0(h, l);
	r[1] = f[2] | 4;
	memset(r + 2, 0, 4 * o2i(f[1]));
	for (f += 3; f < h; f++, i++) {
		if (i >= l + 2)
			printf("overinitializing in makei\n");
		r[i] = *f;
	}
	return a2o(r);
}
lval liboundp(lval * f)
{
	return o2a(f[1])[o2u(f[2])] == 8 ? 0 : TRUE;
}
lval limakunbound(lval * f)
{
	o2a(f[1])[o2u(f[2])] = 8;
	return 0;
}
lval liref(lval * f) {
	if (o2u(f[2]) >= o2a(f[1])[0] / 256 + 2)
		write(1, "out of bounds in iref\n", 22);
	return ((lval *) (f[1] & ~3))[o2u(f[2])] & ~4;
}
lval setfiref(lval * f) {
	int i = o2i(f[3]);
	if (i >= o2a(f[2])[0] / 256 + 2)
		printf("out of bounds in setf iref\n");
	return ((lval *) (f[2] & ~3))[i] = i == 1 ? f[1] | 4 : f[1];
}
lval lmakej(lval * f) {
	lval *r = mb0(f, o2i(f[1]));
	r[1] = o2i(f[2]);
	memset(r + 2, 0, (o2i(f[1]) + 7) / 8);
	return s2o(r);
}
lval ljref(lval * f) {
	return d2o(f, o2s(f[1])[o2u(f[2])]);
}
lval setfjref(lval * f) {
	return o2s(f[2])[o2u(f[3])] = o2u(f[1]);
}
#ifdef _WIN32
lval lmake_fs(lval * f) {
	HANDLE fd = CreateFile(o2z(f[1]), f[2] ? GENERIC_WRITE :
			       GENERIC_READ, f[2] ? FILE_SHARE_WRITE : FILE_SHARE_READ, NULL, OPEN_EXISTING,
			       FILE_ATTRIBUTE_NORMAL, NULL);
	return ms(f, 4, 116, 1, fd, f[2], 0);
}
lval lclose_fs(lval * f) {
	CloseHandle(o2s(f[1])[3]);
	return 0;
}
lval llisten_fs(lval * f)
{
	return WaitForSingleObject(o2s(f[1])[3], 0) == WAIT_OBJECT_0 ? TRUE : 0;
}
lval lread_fs(lval * f) {
	int l = o2i(f[3]);
	if (!ReadFile(o2s(f[1])[3],
		      o2z(f[2]) + l, (o2s(f[2])[0] >> 6) - 4 - l, &l, NULL))
		return 0;
	return d2o(f, l);
}
lval lwrite_fs(lval * f) {
	int l = o2i(f[3]);
	if (!WriteFile(o2s(f[1])[3],
		       o2z(f[2]) + l, o2i(f[4]) - l, &l, NULL))
		return 0;
	return d2o(f, l);
}
lval lfinish_fs(lval * f) {
	FlushFileBuffers(o2s(f[1])[3]);
	return 0;
}
lval lfasl(lval * f) {
	HMODULE h;
	FARPROC s;
	h = LoadLibrary(o2z(f[1]));
	s = GetProcAddress(h, "init");
	return s(f);
}
lval luname(lval * f)
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	f[1] = cons(f + 1, strf(f + 1, osvi.szCSDVersion), 0);
	f[1] = cons(f + 1, d2o(f + 1, osvi.dwBuildNumber), f[1]);
	f[1] = cons(f + 1, d2o(f + 1, osvi.dwMinorVersion), f[1]);
	return cons(f + 1, d2o(f, osvi.dwMajorVersion), f[1]);
}
#else
lval lmake_fs(lval * f) {
	int fd = open(o2z(f[1]), f[2] ? O_WRONLY | O_CREAT | O_TRUNC : O_RDONLY, 0600);
	return fd >= 0 ? ms(f, 4, 116, 1, fd, f[2], 0) : d2o(f, errno);
}
lval lclose_fs(lval * f) {
	close(o2s(f[1])[3]);
	return 0;
}
lval llisten_fs(lval * f)
{
	fd_set r;
	struct timeval t;
	t.tv_sec = 0;
	t.tv_usec = 0;
	FD_ZERO(&r);
	FD_SET(o2s(f[1])[3], &r);
	return select(o2s(f[1])[3] + 1, &r, NULL, NULL, &t) ? TRUE : 0;
}
lval lread_fs(lval * f) {
	int l = o2i(f[3]);
	l = read(o2s(f[1])[3], o2z(f[2]) + l,
		 (o2s(f[2])[0] >> 6) - 4 - l);
	return l < 0 ? cons(f, errno, 0) : d2o(f, l);
}
lval lwrite_fs(lval * f) {
	int l = o2i(f[3]);
	l = write(o2s(f[1])[3],
		  o2z(f[2]) + l, o2i(f[4]) - l);
	return l < 0 ? cons(f, errno, 0) : d2o(f, l);
}
lval lfinish_fs(lval * f) {
	fsync(o2s(f[1])[3]);
	return 0;
}
lval lfasl(lval * f) {
#if 0
	void *h;
	lval(*s) ();
	h = dlopen(o2z(f[1]), RTLD_NOW);
	s = dlsym(h, "init");
	return s(f);
#endif
	return 0;
}
lval strf(lval * f, const char *s);
lval luname(lval * f) {
	struct utsname un;
	uname(&un);
	f[1] = cons(f + 1, strf(f + 1, un.machine), 0);
	f[1] = cons(f + 1, strf(f + 1, un.version), f[1]);
	f[1] = cons(f + 1, strf(f + 1, un.release), f[1]);
	return cons(f + 1, strf(f, un.sysname), f[1]);
}
#endif
FILE *ins;
void load(lval * f, char *s) {
	lval r;
	FILE *oldins = ins;
	ins = fopen(s, "r");
	if (ins) {
		do
			r = eval(f, lread(f));
		while (r != 8);
		fclose(ins);
	}
	ins = oldins;
}
lval lload(lval * f) {
	load(f, o2z(f[1]));
	return symi[1].sym;
}
lval lstring_equal(lval * f)
{
	return string_equal(f[1], f[2]) ? TRUE : 0;
}
lval leval(lval * f, lval * h) {
	f[-1] = h - f > 2 ? f[2] : 0;
	return eval(f - 1, f[1]);
}
void psym(lval p, lval n) {
	int i;
	if (!p)
		printf("#:");
	else if (p != pkg) {
		lval m = car(o2a(p)[2]);
		for (i = 0; i < o2s(m)[0] / 64 - 4; i++)
			putchar(o2z(m)[i]);
		putchar(':');
	} for (i = 0; i < o2s(n)[0] / 64 - 4; i++)
		putchar(o2z(n)[i]);
}
void print(lval x)
{
	int i;
	switch (x & 3) {
	case 0:
		if (x)
			if (x & 8)
				if (x >> 5 < 256 && isgraph(x >> 5))
					printf("#\\%c", x >> 5);
				else
					printf("#\\U+%d", x >> 5);
			else
				printf("%d", x >> 5);
		else
			printf("nil");
		break;
	case 1:
		printf("(");
		print(car(x));
		for (x = cdr(x); cp(x); x = cdr(x)) {
			printf(" ");
			print(car(x));
		}
		if (x) {
			printf(" . ");
			print(x);
		} printf(")");
		break;
	case 2:
		switch (o2a(x)[1]) {
		case 212:
			printf("#<function ");
			print(o2a(x)[6]);
			printf(">");
			break;
		case 20:
			psym(o2a(x)[9], o2a(x)[2]);
			break;
		case 116:
			printf("#(");
			for (i = 0; i < o2a(x)[0] >> 8; i++) {
				if (i)
					printf(" ");
				print(o2a(x)[i + 2]);
			} printf(")");
			break;
		case 180:
			printf("#<package ");
			print(car(o2a(x)[2]));
			printf(">");
			break;
		default:
			if (ap(o2a(x)[1])) {
				printf("#<");
				print(o2a(o2a(o2a(x)[1] - 4)[2])[2]);
				printf(">");
			} else {
				printf("#(");
				for (i = 0; i <= o2a(x)[0] >> 8; i++)
					print(o2a(x)[i + 1]);
				printf(")");
			}
		} break;
	case 3:
		switch (o2s(x)[1]) {
		case 20:
			printf("\"");
			for (i = 0; i < o2s(x)[0] / 64 - 4; i++) {
				char c = o2z(x)[i];
				printf((c == '\\' || c == '\"' ? "\\%c" : "%c"), c);
			} printf("\"");
			break;
		case 84:
			printf("%g", o2d(x));
		}
	}
}
lval lprint(lval * f) {
	print(f[1]);
	return f[1];
}
int ep(lval * g, lval expr) {
	int i;
	lval v = rvalues(g, eval(g, expr));
	if (car(v) == 8)
		return 0;
	if (v)
		for (i = 0; v; v = cdr(v)) {
			printf(";%d: ", i++);
			print(car(v));
			printf("\n");
		}
	else
		printf(";no values\n");
	return 1;
}
char *exmsg[] = {"variable unbound", "function unbound",
	"array index out of bounds", "go tag not bound", "block name not bound",
	"catch tag not dynamically bound", "too many arguments", "too few arguments",
"dynamic extent of block exited", "dynamic extent of tagbody exited"};
int dbgr(lval * f, int x, lval val, lval * vp)
{
	lval ex;
	int i;
	lval *h = f;
	int l = 0;
	NF(0) ex = o2a(symi[59].sym)[5];
	if (ex != 8) {
		h++;
		*++h = d2o(f, x);
		*++h = val;
		ex = call(f, ex, h - f - 1);
		longjmp(top_jmp, 1);
	}
	printf(";exception: %s ", exmsg[x]);
	if (val)
		print(val);
	printf("\n;restarts:\n;[t]oplevel\n;[u]se <form> instead\n;[r]eturn <form> from function\n");
	while (1) {
		lval *j;
		printf(";%d> ", l);
		ex = lread(g);
		if (ex == 8)
			longjmp(top_jmp, 1);
		if (sp(ex) && o2s(ex)[1] == 84) {
			for (h = f, l = i = o2i(ex); i; i--) {
				if (!h[2])
					break;
				h = o2a(h[2]);
			}
		} else if (ap(ex) && o2a(ex)[1] == 20) {
			switch (o2z(o2a(ex)[2])[0]) {
			case 'B':
				printf(";backtrace:\n");
				j = f;
				for (i = 0; j; i++) {
					printf(";%d: ", i);
					if (j[0] >> 5 == 4) {
						print(o2a(j[5])[6]);
						printf(" ");
						print(j[4]);
					} printf("\n");
					if (!j[2])
						break;
					j = o2a(j[2]);
				} break;
			case 'R':
				*vp = eval(g, lread(g));
				return 1;
			case 'T':
				longjmp(top_jmp, 1);
			case 'U':
				*vp = eval(g, lread(g));
				return 0;
			}
		} else
			ep(h, ex);
	}
}
lval evca(lval * f, lval co) {
	lval ex = car(co);
	lval x = ex;
	int m;
ag:	xvalues = 8;
	if (cp(ex)) {
		lval fn = 8;
		if (ap(car(ex)) && o2a(car(ex))[1] == 20) {
			int i = o2a(car(ex))[7] >> 3;
			if (i > 11 && i < 34)
				return symi[i].fun(f, cdr(ex));
			fn = *binding(f, car(ex), 1, &m);
			if (m) {
				lval *g = f + 1;
				for (ex = cdr(ex); ex; ex = cdr(ex))
					*++g = car(ex);
				x = ex = call(f, fn, g - f - 1);
				set_car(co, ex);
				goto ag;
			}
} st:		if (fn == 8) {
			if (dbgr(f, 1, car(ex), &fn))
				return fn;
			else
				goto st;
		} ex = cdr(ex);
		ex = call(f, fn, map_eval(f, ex));
	} else if (ap(ex) && o2a(ex)[1] == 20) {
		ex = *binding(f, ex, 0, &m);
		if (m) {
			x = ex;
			set_car(co, ex);
			goto ag;
		}
		if (ex == 8)
			dbgr(f, 0, x, &ex);
	} return ex == -8 ? o2a(x)[4] : ex;
}
int getnws(void) {
	int c;
	do
		c = getc(ins);
	while (isspace(c));
	return c;
}
lval read_list(lval * f) {
	int c;
	NF(1) T = 0;
	c = getnws();
	if (c == ')')
		return 0;
	if (c == '.') {
		lval r = lread(g);
		getnws();
		return r;
	}
	ungetc(c, ins);
	T = lread(g);
	return cons(g, T, read_list(g));
}
lval read_string_list(lval * g)
{
	int c = getc(ins);
	if (c == '\"')
		return 0;
	if (c == '\\')
		c = getc(ins);
	return cons(g, (c << 5) | 24, read_string_list(g));
}
unsigned hash(lval s) {
	unsigned char *z = (unsigned char *) o2z(s);
	unsigned i = 0, h = 0, g;
	while (i < o2s(s)[0] / 64 - 4) {
		h = (h << 4) + z[i++];
		g = h & 0xf0000000;
		if (g)
			h = h ^ (g >> 24) ^ g;
	}
	return h;
} lval lhash(lval * f) {
	return d2o(f, hash(f[1]));
}
lval is(lval * g, lval p, lval s)
{
	int h = hash(s) % 1021;
	int i = 3;
	lval m;
	for (; i < 5; i++) {
		m = o2a(o2a(p)[i])[2 + h];
		for (; m; m = cdr(m)) {
			lval y = car(m);
			if (string_equal(o2a(y)[2], s))
				return o2a(y)[7] ? y : 0;
		}
	}
	m = ma(g, 9, 20, s, 0, 8, 8, 8, -8, 16, p, 0);
	if (p == kwp)
		o2a(m)[4] = m;
	o2a(o2a(p)[3])[2 + h] = cons(g, m, o2a(o2a(p)[3])[2 + h]);
	return m;
}
lval read_symbol(lval * g) {
	int c = getc(ins);
	if (isspace(c) || c == ')' || c == EOF) {
		if (c != EOF)
			ungetc(c, ins);
		return 0;
	} if (c > 96 && c < 123)
		c -= 32;
	return cons(g, (c << 5) | 24, read_symbol(g));
}
lval list2(lval * g, int a) {
	return l2(g, symi[a].sym, lread(g));
}
lval lread(lval * g) {
	int c = getnws();
	if (c == EOF)
		return 8;
	if (c == '(')
		return read_list(g);
	if (c == '\"')
		return stringify(g, read_string_list(g));
	if (c == '\'')
		return list2(g, 12);
	if (c == '#') {
		c = getnws();
		if (c == '\'')
			return list2(g, 20);
		return 0;
	} if (c == '`')
		return list2(g, 38);
	if (c == ',') {
		c = getnws();
		if (c == '@')
			return list2(g, 40);
		ungetc(c, ins);
		return list2(g, 39);
	} ungetc(c, ins);
	if (isdigit(c)) {
		int i;
		double d;
		fscanf(ins, "%d", &i);
		d = i;
		return d2o(g, d);
	} if (c == ':')
		getnws();
	return is(g, c == ':' ? kwp : pkg, stringify(g, read_symbol(g)));
}
lval strf(lval * f, const char *s) {
	int j = strlen(s);
	lval *str = ms0(f, j);
	str[1] = 20;
	for (j++; j; j--)
		((char *) str)[7 + j] = s[j - 1];
	return s2o(str);
}
lval mkv(lval * f) {
	int i = 2;
	lval *r = ma0(f, 1021);
	r[1] = 116;
	while (i < 1023)
		r[i++] = 0;
	return a2o(r);
}
lval mkp(lval * f, const char *s0, const char *s1)
{
	return ma(f, 6, 180,
		  l2(f, strf(f, s0), strf(f, s1)), mkv(f), mkv(f), 0, 0, 0);
}
#if 0
lval fr(lval * o, lval * p, lval * s, lval * c, lval * b, lval x)
{
	int t;
	if (!(x & 3))
		return x;
	t = (x >> 30) & 3;
	x &= 0x3fffffff;
	switch (t) {
	case 0:
		return sp(x) ? (lval) o + x : (lval) b + x;
	case 1:
		return c[x / 4];
	case 2:
		return s[x / 4];
	default:
		return p[x / 4];
	}
}
X lval fasr(lval * f, lval * p, int pz, lval * s, lval * sp, int sz, lval * c,
         int cz, lval * v, int vz, lval * o, int oz, lval ** rv, lval ** ro)
{
	lval *x, *y;
	int i, l, j;
	lval pc, nc;
	y = ma0(f, oz - 2);
	memcpy(y, o, 4 * oz);
	for (i = 0; i < pz; i++)
		for (pc = o2a(symi[81].sym)[4]; pc; pc = cdr(pc))
			for (nc = o2a(car(pc))[2]; nc; nc = cdr(nc))
				if (string_equal(car(nc), s2o(y + p[i]))) {
					p[i] = car(pc);
					break;
				}
	for (i = 0; i < sz; i++)
		s[i] = is(f, p[sp[i]], s2o(y + s[i]));
	for (i = 0; i < cz; i++)
		c[i] = o2a(s[c[i]])[3];
	x = ma0(f, vz - 2);
	memcpy(x, v, 4 * vz);
	for (i = 0; i < vz; i += ((l + 3) & ~1))
		if (x[i + 1] & 4) {
			l = x[i] >> 8;
			x[i + 1] = fr(y, p, s, c, x, x[i + 1] - 4) + 4;
			for (j = 0; j < l; j++)
				x[i + j + 2] = fr(y, p, s, c, x, x[i + j + 2]);
		} else {
			l = 0;
			x[i] = fr(y, p, s, c, x, x[i]);
			x[i + 1] = fr(y, p, s, c, x, x[i + 1]);
		} *rv = x;
	*ro = y;
}
#endif
#ifdef _WIN32
lval lrp(lval * f, lval * h)
{
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	si.cb = sizeof(si);
	if (CreateProcess(o2z(f[1]), o2z(f[2]), NULL, NULL, FALSE,
			  0, NULL, NULL, &si, &pi))
		return TRUE;
	return 0;
}
#else
lval lrp(lval * f, lval * h) {
	pid_t p = 0;
	int r;
	//p = fork();
	if (p)
		waitpid(p, &r, 0);
	else {
		int i = 0;
		char **v = malloc((h - f - 1) * sizeof(char *));
		for (; i < h - f - 2; i++)
			v[i] = o2z(f[i + 2]);
		v[i] = 0;
		execv(o2z(f[1]), v);
	} return d2o(f, r);
}
#endif
int lisp5000_main(int argc, char *argv[])
{
	lval *g;
	int i;
	lval sym;
	memory_size = 1 * 1024 * 1024;
	//memory = malloc(memory_size);
	memory = memalign(8, memory_size);
	memf = memory;
	memset(memory, 0, memory_size);
	memf[0] = 0;
	memf[1] = memory_size / 4;
	//stack = malloc(256 * 1024);
	stack = memalign(8, 256 * 1024);
	memset(stack, 0, 256 * 1024);
	g = stack + 5;
	pkg = mkp(g, "CL", "COMMON-LISP");
	for (i = 0; i < 88; i++) {
		sym = is(g, pkg, strf(g, symi[i].name));
		if (i < 10)
			o2a(sym)[4] = sym;
		ins = stdin;
		symi[i].sym = sym;
		if (symi[i].fun)
			o2a(sym)[5] = ma(g, 5, 212, ms(g, 3, 212, symi[i].fun, 0, -1), 0, 0, 0, sym);
		if (symi[i].setfun)
			o2a(sym)[6] = ma(g, 5, 212, ms(g, 3, 212, symi[i].setfun, 0, -1), 8, 0, 0, sym);
		o2a(sym)[7] = i << 3;
	}
	kwp = mkp(g, "", "KEYWORD");
	o2a(symi[81].sym)[4] = pkgs = l2(g, kwp, pkg);
#ifdef _WIN32
	o2a(symi[78].sym)[4] = ms(g, 3, 116, 1, GetStdHandle(STD_INPUT_HANDLE), 0, 0);
	o2a(symi[79].sym)[4] = ms(g, 3, 116, 1, GetStdHandle(STD_OUTPUT_HANDLE), TRUE, 0);
	o2a(symi[80].sym)[4] = ms(g, 3, 116, 1, GetStdHandle(STD_ERROR_HANDLE), TRUE, 0);
#else
	o2a(symi[78].sym)[4] = ms(g, 3, 116, 1, 0, 0, 0);
	o2a(symi[79].sym)[4] = ms(g, 3, 116, 1, 1, TRUE, 0);
	o2a(symi[80].sym)[4] = ms(g, 3, 116, 1, 2, TRUE, 0);
#endif
	for (i = 1; i < argc; i++)
		load(g, argv[i]);
	setjmp(top_jmp);
	do
		printf("? ");
	while (ep(g, lread(g)));
	free(memory);
	free(stack);
	return 0;
}
struct symbol_init symi[] = {{"NIL"}, {"T"}, {"&REST"}, {"&BODY"},
{"&OPTIONAL"}, {"&KEY"}, {"&WHOLE"}, {"&ENVIRONMENT"}, {"&AUX"},
{"&ALLOW-OTHER-KEYS"}, {"DECLARE", (func_t)eval_declare, -1}, {"SPECIAL"},
{"QUOTE", (func_t)eval_quote, 1}, {"LET", (func_t)eval_let, -2}, {"LET*", (func_t)eval_letm, -2},
{"FLET", (func_t)eval_flet, -2}, {"LABELS", (func_t)eval_labels, -2}, {"MACROLET", (func_t)eval_macrolet, -2},
{"SYMBOL-MACROLET", (func_t)eval_symbol_macrolet, -2}, {"SETQ", (func_t)eval_setq, 2},
{"FUNCTION", (func_t)eval_function, 1}, {"TAGBODY", (func_t)eval_tagbody, -1}, {"GO", (func_t)eval_go, 1},
{"BLOCK", (func_t)eval_block, -2}, {"RETURN-FROM", (func_t)eval_return_from, 2},
{"CATCH", (func_t)eval_catch, -2}, {"THROW", (func_t)eval_throw, -2},
{"UNWIND-PROTECT", (func_t)eval_unwind_protect, -2}, {"IF", (func_t)eval_if, -3},
{"MULTIPLE-VALUE-CALL", (func_t)eval_multiple_value_call, -2},
{"MULTIPLE-VALUE-PROG1", (func_t)eval_multiple_value_prog1, -2}, {"PROGN", (func_t)eval_body, -1},
{"PROGV", (func_t)eval_progv, -3}, {"_SETF", (func_t)eval_setf, 2},
{"FINISH-FILE-STREAM", (func_t)lfinish_fs, 1}, {"MAKEI", (func_t)lmakei, -3}, {"DPB", (func_t)ldpb, 3},
{"LDB", (func_t)lldb, 2}, {"BACKQUOTE"}, {"UNQUOTE"}, {"UNQUOTE-SPLICING"},
{"IBOUNDP", (func_t)liboundp, 2}, {"LISTEN-FILE-STREAM", (func_t)llisten_fs, 1}, {"LIST", (func_t)llist, -1},
{"VALUES", (func_t)lvalues, -1},
{"FUNCALL", (func_t)lfuncall, -2}, {"APPLY", (func_t)lapply, -2}, {"EQ", (func_t)leq, 2}, {"CONS", (func_t)lcons, 2},
{"CAR", (func_t)lcar, 1, (func_t)setfcar, 2}, {"CDR", (func_t)lcdr, 1, (func_t)setfcdr, 2}, {"=", (func_t)lequ, -2},
{"<", (func_t)lless, -2}, {"+", (func_t)lplus, -1}, {"-", (func_t)lminus, -2}, {"*", (func_t)ltimes, -1},
{"/", (func_t)ldivi, -2}, {"MAKE-FILE-STREAM", (func_t)lmake_fs, 2}, {"HASH", (func_t)lhash, 1},
{"IERROR"}, {"GENSYM", (func_t)lgensym, 0}, {"STRING", (func_t)lstring, -1}, {"FASL", (func_t)lfasl, 1},
{"MAKEJ", (func_t)lmakej, 2}, {"MAKEF", (func_t)lmakef, 0}, {"FREF", (func_t)lfref, 1},
{"PRINT", (func_t)lprint, 1}, {"GC", (func_t)gc, 0}, {"CLOSE-FILE-STREAM", (func_t)lclose_fs, 1},
{"IVAL", (func_t)lival, 1}, {"FLOOR", (func_t)lfloor, -2}, {"READ-FILE-STREAM", (func_t)lread_fs, 3},
{"WRITE-FILE-STREAM", (func_t)lwrite_fs, 4}, {"LOAD", (func_t)lload, 1},
{"IREF", (func_t)liref, 2, (func_t)setfiref, 3}, {"LAMBDA"}, {"CODE-CHAR", (func_t)lcode_char, 1},
{"CHAR-CODE", (func_t)lchar_code, 1}, {"*STANDARD-INPUT*"}, {"*STANDARD-OUTPUT*"},
{"*ERROR-OUTPUT*"}, {"*PACKAGES*"}, {"STRING=", (func_t)lstring_equal, 2},
{"IMAKUNBOUND", (func_t)limakunbound, 2}, {"EVAL", (func_t)leval, -2}, {"JREF", (func_t)ljref, 2, (func_t)setfjref, 3},
{"RUN-PROGRAM", (func_t)lrp, -2}, {"UNAME", (func_t)luname, 0}};
