/* File: VM
 * Functionality pertaining to the virtual machine.
 */

tp_vm *_tp_init(void) {
    int i;
    tp_obj sys;
    tp_vm *tp = (tp_vm*)calloc(sizeof(tp_vm),1);
    tp->time_limit = TP_NO_LIMIT;
    tp->clocks = clock();
    tp->time_elapsed = 0.0;
    tp->mem_limit = TP_NO_LIMIT;
    tp->mem_exceeded = 0;
    tp->mem_used = sizeof(tp_vm);
    tp->cur = 0;
    tp->jmp = 0;
    tp->ex = tp_None;
    tp->root = tp_list_nt(tp);
    for (i=0; i<256; i++) { tp->chars[i][0]=i; }
    tp_gc_init(tp);
    tp->_regs = tp_list(tp);
    for (i=0; i<TP_REGS; i++) { tp_set(tp,tp->_regs,tp_None,tp_None); }
    tp->builtins = tp_dict(tp);
    tp->modules = tp_dict(tp);
    tp->_params = tp_list(tp);
    for (i=0; i<TP_FRAMES; i++) { tp_set(tp,tp->_params,tp_None,tp_list(tp)); }
    tp_set(tp,tp->root,tp_None,tp->builtins);
    tp_set(tp,tp->root,tp_None,tp->modules);
    tp_set(tp,tp->root,tp_None,tp->_regs);
    tp_set(tp,tp->root,tp_None,tp->_params);
    tp_set(tp,tp->builtins,tp_string("MODULES"),tp->modules);
    tp_set(tp,tp->modules,tp_string("BUILTINS"),tp->builtins);
    tp_set(tp,tp->builtins,tp_string("BUILTINS"),tp->builtins);
    sys = tp_dict(tp);
    tp_set(tp, sys, tp_string("version"), tp_string("tinypy 1.2+SVN"));
    tp_set(tp,tp->modules, tp_string("sys"), sys);
    tp->regs = tp->_regs.list.val->items;
    tp_full(tp);
    return tp;
}


/* Function: tp_deinit
 * Destroys a VM instance.
 *
 * When you no longer need an instance of tinypy, you can use this to free all
 * memory used by it. Even when you are using only a single tinypy instance, it
 * may be good practice to call this function on shutdown.
 */
void tp_deinit(TP) {
    while (tp->root.list.val->len) {
        _tp_list_pop(tp,tp->root.list.val,0,"tp_deinit");
    }
    tp_full(tp); tp_full(tp);
    tp_delete(tp,tp->root);
    tp_gc_deinit(tp);
    tp->mem_used -= sizeof(tp_vm);
    free(tp);
}

/* tp_frame_*/
void tp_frame(TP,tp_obj globals,tp_obj code,tp_obj *ret_dest) {
    tp_frame_ f;
    f.globals = globals;
    f.code = code;
    f.cur = (tp_code*)f.code.string.val;
    f.jmp = 0;
/*     fprintf(stderr,"tp->cur: %d\n",tp->cur);*/
    f.regs = (tp->cur <= 0?tp->regs:tp->frames[tp->cur].regs+tp->frames[tp->cur].cregs);

    f.regs[0] = f.globals;
    f.regs[1] = f.code;
    f.regs += TP_REGS_EXTRA;

    f.ret_dest = ret_dest;
    f.lineno = 0;
    f.line = tp_string("");
    f.name = tp_string("?");
    f.fname = tp_string("?");
    f.cregs = 0;
/*     return f;*/
    if (f.regs+(256+TP_REGS_EXTRA) >= tp->regs+TP_REGS || tp->cur >= TP_FRAMES-1) {
        tp_raise(,tp_string("(tp_frame) RuntimeError: stack overflow"));
    }
    tp->cur += 1;
    tp->frames[tp->cur] = f;
}

void _tp_raise(TP,tp_obj e) {
    /*char *x = 0; x[0]=0;*/
    if (!tp || !tp->jmp) {
#ifndef CPYTHON_MOD
        printf("\nException:\n"); tp_echo(tp,e); printf("\n");
        exit(-1);
#else
        tp->ex = e;
        longjmp(tp->nextexpr,1);
#endif
    }
    if (e.type != TP_NONE) { tp->ex = e; }
    tp_grey(tp,e);
    longjmp(tp->buf,1);
}

void tp_print_stack(TP) {
    int i;
    printf("\n");
    for (i=0; i<=tp->cur; i++) {
        if (!tp->frames[i].lineno) { continue; }
        printf("File \""); tp_echo(tp,tp->frames[i].fname); printf("\", ");
        printf("line %d, in ",tp->frames[i].lineno);
        tp_echo(tp,tp->frames[i].name); printf("\n ");
        tp_echo(tp,tp->frames[i].line); printf("\n");
    }
    printf("\nException:\n"); tp_echo(tp,tp->ex); printf("\n");
}

void tp_handle(TP) {
    int i;
    for (i=tp->cur; i>=0; i--) {
        if (tp->frames[i].jmp) { break; }
    }
    if (i >= 0) {
        tp->cur = i;
        tp->frames[i].cur = tp->frames[i].jmp;
        tp->frames[i].jmp = 0;
        return;
    }
#ifndef CPYTHON_MOD
    tp_print_stack(tp);
    exit(-1);
#else
    longjmp(tp->nextexpr,1);
#endif
}

/* Function: tp_call
 * Calls a tinypy function.
 *
 * Use this to call a tinypy function.
 *
 * Parameters:
 * tp - The VM instance.
 * self - The object to call.
 * params - Parameters to pass.
 *
 * Example:
 * > tp_call(tp,
 * >     tp_get(tp, tp->builtins, tp_string("foo")),
 * >     tp_params_v(tp, tp_string("hello")))
 * This will look for a global function named "foo", then call it with a single
 * positional parameter containing the string "hello".
 */
tp_obj tp_call(TP,tp_obj self, tp_obj params) {
    /* I'm not sure we should have to do this, but
    just for giggles we will. */
    tp->params = params;

    if (self.type == TP_DICT) {
        if (self.dict.dtype == 1) {
            tp_obj meta; if (_tp_lookup(tp,self,tp_string("__new__"),&meta)) {
                _tp_list_insert(tp,params.list.val,0,self);
                return tp_call(tp,meta,params);
            }
        } else if (self.dict.dtype == 2) {
            TP_META_BEGIN(self,"__call__");
                return tp_call(tp,meta,params);
            TP_META_END;
        }
    }
    if (self.type == TP_FNC && !(self.fnc.ftype&1)) {
        tp_obj r = _tp_tcall(tp,self);
        tp_grey(tp,r);
        return r;
    }
    if (self.type == TP_FNC) {
        tp_obj dest = tp_None;
        tp_frame(tp,self.fnc.info->globals,self.fnc.info->code,&dest);
        if ((self.fnc.ftype&2)) {
            tp->frames[tp->cur].regs[0] = params;
            _tp_list_insert(tp,params.list.val,0,self.fnc.info->self);
        } else {
            tp->frames[tp->cur].regs[0] = params;
        }
        tp_run(tp,tp->cur);
        return dest;
    }
    tp_params_v(tp,1,self); tp_print(tp);
    tp_raise(tp_None,tp_string("(tp_call) TypeError: object is not callable"));
}


void tp_return(TP, tp_obj v) {
    tp_obj *dest = tp->frames[tp->cur].ret_dest;
    if (dest) { *dest = v; tp_grey(tp,v); }
/*     memset(tp->frames[tp->cur].regs,0,TP_REGS_PER_FRAME*sizeof(tp_obj));
       fprintf(stderr,"regs:%d\n",(tp->frames[tp->cur].cregs+1));*/
    memset(tp->frames[tp->cur].regs-TP_REGS_EXTRA,0,(TP_REGS_EXTRA+tp->frames[tp->cur].cregs)*sizeof(tp_obj));
    tp->cur -= 1;
}

enum {
    TP_IEOF,TP_IADD,TP_ISUB,TP_IMUL,TP_IDIV,TP_IPOW,TP_IBITAND,TP_IBITOR,TP_ICMP,TP_IGET,TP_ISET,
    TP_INUMBER,TP_ISTRING,TP_IGGET,TP_IGSET,TP_IMOVE,TP_IDEF,TP_IPASS,TP_IJUMP,TP_ICALL,
    TP_IRETURN,TP_IIF,TP_IDEBUG,TP_IEQ,TP_ILE,TP_ILT,TP_IDICT,TP_ILIST,TP_INONE,TP_ILEN,
    TP_ILINE,TP_IPARAMS,TP_IIGET,TP_IFILE,TP_INAME,TP_INE,TP_IHAS,TP_IRAISE,TP_ISETJMP,
    TP_IMOD,TP_ILSH,TP_IRSH,TP_IITER,TP_IDEL,TP_IREGS,TP_IBITXOR, TP_IIFN,
    TP_INOT, TP_IBITNOT,
    TP_ITOTAL
};

/* char *tp_strings[TP_ITOTAL] = {
       "EOF","ADD","SUB","MUL","DIV","POW","BITAND","BITOR","CMP","GET","SET","NUM",
       "STR","GGET","GSET","MOVE","DEF","PASS","JUMP","CALL","RETURN","IF","DEBUG",
       "EQ","LE","LT","DICT","LIST","NONE","LEN","LINE","PARAMS","IGET","FILE",
       "NAME","NE","HAS","RAISE","SETJMP","MOD","LSH","RSH","ITER","DEL","REGS",
       "BITXOR", "IFN", "NOT", "BITNOT",
   };*/

#define VA ((int)e.regs.a)
#define VB ((int)e.regs.b)
#define VC ((int)e.regs.c)
#define RA regs[e.regs.a]
#define RB regs[e.regs.b]
#define RC regs[e.regs.c]
#define UVBC (unsigned short)(((VB<<8)+VC))
#define SVBC (short)(((VB<<8)+VC))
#define GA tp_grey(tp,RA)
#define SR(v) f->cur = cur; return(v);


int tp_step(TP) {
    int a;
    tp_frame_ *f = &tp->frames[tp->cur];
    tp_obj *regs = f->regs;
    tp_code *cur = f->cur;
    while(1) {
    #ifdef TP_SANDBOX
    tp_bounds(tp,cur,1);
    #endif
    tp_code e = *cur;
    /*
     fprintf(stderr,"%2d.%4d: %-6s %3d %3d %3d\n",tp->cur,cur - (tp_code*)f->code.string.val,tp_strings[e.i],VA,VB,VC);
       int i; for(i=0;i<16;i++) { fprintf(stderr,"%d: %s\n",i,TP_xSTR(regs[i])); }
    */
    switch (e.i) {
        case TP_IEOF: tp_return(tp,tp_None); SR(0); break;
        case TP_IADD: RA = tp_add(tp,RB,RC); break;
        case TP_ISUB: RA = tp_sub(tp,RB,RC); break;
        case TP_IMUL: RA = tp_mul(tp,RB,RC); break;
        case TP_IDIV: RA = tp_div(tp,RB,RC); break;
        case TP_IPOW: RA = tp_pow(tp,RB,RC); break;
        case TP_IBITAND: RA = tp_bitwise_and(tp,RB,RC); break;
        case TP_IBITOR:  RA = tp_bitwise_or(tp,RB,RC); break;
        case TP_IBITXOR:  RA = tp_bitwise_xor(tp,RB,RC); break;
        case TP_IMOD:  RA = tp_mod(tp,RB,RC); break;
        case TP_ILSH:  RA = tp_lsh(tp,RB,RC); break;
        case TP_IRSH:  RA = tp_rsh(tp,RB,RC); break;
        case TP_ICMP: RA = tp_number(tp_cmp(tp,RB,RC)); break;
        case TP_INE: RA = tp_number(tp_cmp(tp,RB,RC)!=0); break;
        case TP_IEQ: RA = tp_number(tp_cmp(tp,RB,RC)==0); break;
        case TP_ILE: RA = tp_number(tp_cmp(tp,RB,RC)<=0); break;
        case TP_ILT: RA = tp_number(tp_cmp(tp,RB,RC)<0); break;
        case TP_IBITNOT:  RA = tp_bitwise_not(tp,RB); break;
        case TP_INOT: RA = tp_number(!tp_bool(tp,RB)); break;
        case TP_IPASS: break;
        case TP_IIF: if (tp_bool(tp,RA)) { cur += 1; } break;
        case TP_IIFN: if (!tp_bool(tp,RA)) { cur += 1; } break;
        case TP_IGET: RA = tp_get(tp,RB,RC); GA; break;
        case TP_IITER:
            if (RC.number.val < tp_len(tp,RB).number.val) {
                RA = tp_iter(tp,RB,RC); GA;
                RC.number.val += 1;
                #ifdef TP_SANDBOX
                tp_bounds(tp,cur,1);
                #endif
                cur += 1;
            }
            break;
        case TP_IHAS: RA = tp_has(tp,RB,RC); break;
        case TP_IIGET: tp_iget(tp,&RA,RB,RC); break;
        case TP_ISET: tp_set(tp,RA,RB,RC); break;
        case TP_IDEL: tp_del(tp,RA,RB); break;
        case TP_IMOVE: RA = RB; break;
        case TP_INUMBER:
            #ifdef TP_SANDBOX
            tp_bounds(tp,cur,sizeof(tp_num)/4);
            #endif
            RA = tp_number(*(tp_num*)(*++cur).string.val);
            cur += sizeof(tp_num)/4;
            continue;
        case TP_ISTRING: {
            #ifdef TP_SANDBOX
            tp_bounds(tp,cur,(UVBC/4)+1);
            #endif
            /* RA = tp_string_n((*(cur+1)).string.val,UVBC); */
            a = (*(cur+1)).string.val-f->code.string.val;
            RA = tp_string_sub(tp,f->code,a,a+UVBC),
            cur += (UVBC/4)+1;
            }
            break;
        case TP_IDICT: RA = tp_dict_n(tp,VC/2,&RB); break;
        case TP_ILIST: RA = tp_list_n(tp,VC,&RB); break;
        case TP_IPARAMS: RA = tp_params_n(tp,VC,&RB); break;
        case TP_ILEN: RA = tp_len(tp,RB); break;
        case TP_IJUMP: cur += SVBC; continue; break;
        case TP_ISETJMP: f->jmp = SVBC?cur+SVBC:0; break;
        case TP_ICALL:
            #ifdef TP_SANDBOX
            tp_bounds(tp,cur,1);
            #endif
            f->cur = cur + 1;  RA = tp_call(tp,RB,RC); GA;
            return 0; break;
        case TP_IGGET:
            if (!tp_iget(tp,&RA,f->globals,RB)) {
                RA = tp_get(tp,tp->builtins,RB); GA;
            }
            break;
        case TP_IGSET: tp_set(tp,f->globals,RA,RB); break;
        case TP_IDEF: {
/*            RA = tp_def(tp,(*(cur+1)).string.val,f->globals);*/
            #ifdef TP_SANDBOX
            tp_bounds(tp,cur,SVBC);
            #endif
            a = (*(cur+1)).string.val-f->code.string.val;
            RA = tp_def(tp,
                /*tp_string_n((*(cur+1)).string.val,(SVBC-1)*4),*/
                tp_string_sub(tp,f->code,a,a+(SVBC-1)*4),
                f->globals);
            cur += SVBC; continue;
            }
            break;

        case TP_IRETURN: tp_return(tp,RA); SR(0); break;
        case TP_IRAISE: _tp_raise(tp,RA); SR(0); break;
        case TP_IDEBUG:
            tp_params_v(tp,3,tp_string("DEBUG:"),tp_number(VA),RA); tp_print(tp);
            break;
        case TP_INONE: RA = tp_None; break;
        case TP_ILINE:
            #ifdef TP_SANDBOX
            tp_bounds(tp,cur,VA);
            #endif
            ;
            a = (*(cur+1)).string.val-f->code.string.val;
/*            f->line = tp_string_n((*(cur+1)).string.val,VA*4-1);*/
            f->line = tp_string_sub(tp,f->code,a,a+VA*4-1);
/*             fprintf(stderr,"%7d: %s\n",UVBC,f->line.string.val);*/
            cur += VA; f->lineno = UVBC;
            break;
        case TP_IFILE: f->fname = RA; break;
        case TP_INAME: f->name = RA; break;
        case TP_IREGS: f->cregs = VA; break;
        default:
            tp_raise(0,tp_string("(tp_step) RuntimeError: invalid instruction"));
            break;
    }
    #ifdef TP_SANDBOX
    tp_time_update(tp);
    tp_mem_update(tp);
    tp_bounds(tp,cur,1);
    #endif
    cur += 1;
    }
    SR(0);
}

void _tp_run(TP,int cur) {
    tp->jmp += 1; if (setjmp(tp->buf)) { tp_handle(tp); }
    while (tp->cur >= cur && tp_step(tp) != -1);
    tp->jmp -= 1;
}

void tp_run(TP,int cur) {
    jmp_buf tmp;
    memcpy(tmp,tp->buf,sizeof(jmp_buf));
    _tp_run(tp,cur);
    memcpy(tp->buf,tmp,sizeof(jmp_buf));
}


tp_obj tp_ez_call(TP, const char *mod, const char *fnc, tp_obj params) {
    tp_obj tmp;
    tmp = tp_get(tp,tp->modules,tp_string(mod));
    tmp = tp_get(tp,tmp,tp_string(fnc));
    return tp_call(tp,tmp,params);
}

tp_obj _tp_import(TP, tp_obj fname, tp_obj name, tp_obj code) {
    tp_obj g;

    if (!((fname.type != TP_NONE && _tp_str_index(fname,tp_string(".tpc"))!=-1) || code.type != TP_NONE)) {
        return tp_ez_call(tp,"py2bc","import_fname",tp_params_v(tp,2,fname,name));
    }

    if (code.type == TP_NONE) {
        tp_params_v(tp,1,fname);
        code = tp_load(tp);
    }

    g = tp_dict(tp);
    tp_set(tp,g,tp_string("__name__"),name);
    tp_set(tp,g,tp_string("__code__"),code);
    tp_set(tp,g,tp_string("__dict__"),g);
    tp_frame(tp,g,code,0);
    tp_set(tp,tp->modules,name,g);

    if (!tp->jmp) { tp_run(tp,tp->cur); }

    return g;
}


/* Function: tp_import
 * Imports a module.
 *
 * Parameters:
 * fname - The filename of a file containing the module's code.
 * name - The name of the module.
 * codes - The module's code.  If this is given, fname is ignored.
 * len - The length of the bytecode.
 *
 * Returns:
 * The module object.
 */
tp_obj tp_import(TP, const char * fname, const char * name, void *codes, int len) {
    tp_obj f = fname?tp_string(fname):tp_None;
    tp_obj bc = codes?tp_string_n((const char*)codes,len):tp_None;
    return _tp_import(tp,f,tp_string(name),bc);
}



tp_obj tp_exec_(TP) {
    tp_obj code = TP_OBJ();
    tp_obj globals = TP_OBJ();
    tp_obj r = tp_None;
    tp_frame(tp,globals,code,&r);
    tp_run(tp,tp->cur);
    return r;
}


tp_obj tp_import_(TP) {
    tp_obj mod = TP_OBJ();
    tp_obj r;

    if (tp_has(tp,tp->modules,mod).number.val) {
        return tp_get(tp,tp->modules,mod);
    }

    r = _tp_import(tp,tp_add(tp,mod,tp_string(".tpc")),mod,tp_None);
    return r;
}

void tp_builtins(TP) {
    tp_obj o;
    struct {const char *s;void *f;} b[] = {
    {"print",tp_print}, {"range",tp_range}, {"min",tp_min},
    {"max",tp_max}, {"bind",tp_bind}, {"copy",tp_copy},
    {"import",tp_import_}, {"len",tp_len_}, {"assert",tp_assert},
    {"str",tp_str2}, {"float",tp_float}, {"system",tp_system},
    {"istype",tp_istype}, {"chr",tp_chr}, {"save",tp_save},
    {"load",tp_load}, {"fpack",tp_fpack}, {"abs",tp_abs},
    {"int",tp_int}, {"exec",tp_exec_}, {"exists",tp_exists},
    {"mtime",tp_mtime}, {"number",tp_float}, {"round",tp_round},
    {"ord",tp_ord}, {"merge",tp_merge}, {"getraw",tp_getraw},
    {"setmeta",tp_setmeta}, {"getmeta",tp_getmeta},
    {"bool", tp_builtins_bool},
    #ifdef TP_SANDBOX
    {"sandbox",tp_sandbox_},
    #endif
    {0,0},
    };
    int i; for(i=0; b[i].s; i++) {
        tp_set(tp,tp->builtins,tp_string(b[i].s),tp_fnc(tp,(tp_obj (*)(tp_vm *))b[i].f));
    }

    o = tp_object(tp);
    tp_set(tp,o,tp_string("__call__"),tp_fnc(tp,tp_object_call));
    tp_set(tp,o,tp_string("__new__"),tp_fnc(tp,tp_object_new));
    tp_set(tp,tp->builtins,tp_string("object"),o);
}


void tp_args(TP,int argc, char *argv[]) {
    tp_obj self = tp_list(tp);
    int i;
    for (i=1; i<argc; i++) { _tp_list_append(tp,self.list.val,tp_string(argv[i])); }
    tp_set(tp,tp->builtins,tp_string("ARGV"),self);
}

tp_obj tp_main(TP,char *fname, void *code, int len) {
    return tp_import(tp,fname,"__main__",code, len);
}

/* Function: tp_compile
 * Compile some tinypy code.
 *
 */
tp_obj tp_compile(TP, tp_obj text, tp_obj fname) {
    return tp_ez_call(tp,"BUILTINS","compile",tp_params_v(tp,2,text,fname));
}

/* Function: tp_exec
 * Execute VM code.
 */
tp_obj tp_exec(TP, tp_obj code, tp_obj globals) {
    tp_obj r=tp_None;
    tp_frame(tp,globals,code,&r);
    tp_run(tp,tp->cur);
    return r;
}

tp_obj tp_eval(TP, const char *text, tp_obj globals) {
    tp_obj code = tp_compile(tp,tp_string(text),tp_string("<eval>"));
    return tp_exec(tp,code,globals);
}

/* Function: tp_init
 * Initializes a new virtual machine.
 *
 * The given parameters have the same format as the parameters to main, and
 * allow passing arguments to your tinypy scripts.
 *
 * Returns:
 * The newly created tinypy instance.
 */
tp_vm *tp_init(int argc, char *argv[]) {
    tp_vm *tp = _tp_init();
    tp_builtins(tp);
    tp_args(tp,argc,argv);
    tp_compiler(tp);
    return tp;
}

/**/
