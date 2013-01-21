void _tp_list_realloc(TP, _tp_list *self,int len) {
    if (!len) { len=1; }
    self->items = (tp_obj*)tp_realloc(tp, self->items,len*sizeof(tp_obj));
    self->alloc = len;
}

void _tp_list_set(TP,_tp_list *self,int k, tp_obj v, const char *error) {
    if (k >= self->len) {
        tp_raise(,tp_string("(_tp_list_set) KeyError"));
    }
    self->items[k] = v;
    tp_grey(tp,v);
}
void _tp_list_free(TP, _tp_list *self) {
    tp_free(tp, self->items);
    tp_free(tp, self);
}

tp_obj _tp_list_get(TP,_tp_list *self,int k,const char *error) {
    if (k >= self->len) {
        tp_raise(tp_None,tp_string("(_tp_list_set) KeyError"));
    }
    return self->items[k];
}
void _tp_list_insertx(TP,_tp_list *self, int n, tp_obj v) {
    if (self->len >= self->alloc) {
        _tp_list_realloc(tp, self,self->alloc*2);
    }
    if (n < self->len) { memmove(&self->items[n+1],&self->items[n],sizeof(tp_obj)*(self->len-n)); }
    self->items[n] = v;
    self->len += 1;
}
void _tp_list_appendx(TP,_tp_list *self, tp_obj v) {
    _tp_list_insertx(tp,self,self->len,v);
}
void _tp_list_insert(TP,_tp_list *self, int n, tp_obj v) {
    _tp_list_insertx(tp,self,n,v);
    tp_grey(tp,v);
}
void _tp_list_append(TP,_tp_list *self, tp_obj v) {
    _tp_list_insert(tp,self,self->len,v);
}
tp_obj _tp_list_pop(TP,_tp_list *self, int n, const char *error) {
    tp_obj r = _tp_list_get(tp,self,n,error);
    if (n != self->len-1) { memmove(&self->items[n],&self->items[n+1],sizeof(tp_obj)*(self->len-(n+1))); }
    self->len -= 1;
    return r;
}

int _tp_list_find(TP,_tp_list *self, tp_obj v) {
    int n;
    for (n=0; n<self->len; n++) {
        if (tp_cmp(tp,v,self->items[n]) == 0) {
            return n;
        }
    }
    return -1;
}

tp_obj tp_index(TP) {
    tp_obj self = TP_OBJ();
    tp_obj v = TP_OBJ();
    int i = _tp_list_find(tp,self.list.val,v);
    if (i < 0) {
        tp_raise(tp_None,tp_string("(tp_index) ValueError: list.index(x): x not in list"));
    }
    return tp_number(i);
}

_tp_list *_tp_list_new(TP) {
    return (_tp_list*)tp_malloc(tp, sizeof(_tp_list));
}

tp_obj _tp_list_copy(TP, tp_obj rr) {
    tp_obj val = {TP_LIST};
    _tp_list *o = rr.list.val;
    _tp_list *r = _tp_list_new(tp);
    *r = *o; r->gci = 0;
    r->items = (tp_obj*)tp_malloc(tp, sizeof(tp_obj)*o->len);
    memcpy(r->items,o->items,sizeof(tp_obj)*o->len);
    val.list.val = r;
    return tp_track(tp,val);
}

tp_obj tp_append(TP) {
    tp_obj self = TP_OBJ();
    tp_obj v = TP_OBJ();
    _tp_list_append(tp,self.list.val,v);
    return tp_None;
}

tp_obj tp_pop(TP) {
    tp_obj self = TP_OBJ();
    return _tp_list_pop(tp,self.list.val,self.list.val->len-1,"pop");
}

tp_obj tp_insert(TP) {
    tp_obj self = TP_OBJ();
    int n = TP_NUM();
    tp_obj v = TP_OBJ();
    _tp_list_insert(tp,self.list.val,n,v);
    return tp_None;
}

tp_obj tp_extend(TP) {
    tp_obj self = TP_OBJ();
    tp_obj v = TP_OBJ();
    int i;
    for (i=0; i<v.list.val->len; i++) {
        _tp_list_append(tp,self.list.val,v.list.val->items[i]);
    }
    return tp_None;
}

tp_obj tp_list_nt(TP) {
    tp_obj r = {TP_LIST};
    r.list.val = _tp_list_new(tp);
    return r;
}

tp_obj tp_list(TP) {
    tp_obj r = {TP_LIST};
    r.list.val = _tp_list_new(tp);
    return tp_track(tp,r);
}

tp_obj tp_list_n(TP,int n,tp_obj *argv) {
    int i;
    tp_obj r = tp_list(tp); _tp_list_realloc(tp, r.list.val,n);
    for (i=0; i<n; i++) {
        _tp_list_append(tp,r.list.val,argv[i]);
    }
    return r;
}

int _tp_sort_cmp(tp_obj *a,tp_obj *b) {
    return tp_cmp(0,*a,*b);
}

tp_obj tp_sort(TP) {
    tp_obj self = TP_OBJ();
    qsort(self.list.val->items, self.list.val->len, sizeof(tp_obj), (int(*)(const void*,const void*))_tp_sort_cmp);
    return tp_None;
}

