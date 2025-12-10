#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Use __int128 for intermediate exact integer arithmetic */
typedef __int128 i128;

static void i128_to_str(i128 v, char *out){
    int neg = 0;
    char buf[80];
    int p = 0;
    int q;
    if(v==0){ out[0]='0'; out[1]='\0'; return; }
    if(v < 0){ neg = 1; v = -v; }
    while(v > 0){
        int digit = (int)(v % 10);
        buf[p++] = '0' + digit;
        v /= 10;
    }
    if(neg) buf[p++] = '-';
    q = 0;
    while(p>0) out[q++] = buf[--p];
    out[q] = '\0';
}

static i128 gcd_i128(i128 a, i128 b){
    if(a<0) a = -a;
    if(b<0) b = -b;
    i128 r;
    while(b!=0){
        r = a % b;
        a = b;
        b = r;
    }
    return a;
}

typedef struct {
    i128 num;
    i128 den;
} Frac;

static Frac frac_norm(Frac f){
    if(f.num==0){
        f.den = 1;
        return f;
    }
    if(f.den < 0){
        f.den = -f.den;
        f.num = -f.num;
    }
    i128 g = gcd_i128(f.num < 0 ? -f.num : f.num, f.den);
    if(g > 1){
        f.num /= g;
        f.den /= g;
    }
    return f;
}

static Frac make_frac_i128(i128 v){
    Frac f; f.num = v; f.den = 1; return f;
}

static Frac frac_add(Frac a, Frac b){
    Frac r;
    r.num = a.num * b.den + b.num * a.den;
    r.den = a.den * b.den;
    return frac_norm(r);
}

static Frac frac_sub(Frac a, Frac b){
    Frac r;
    r.num = a.num * b.den - b.num * a.den;
    r.den = a.den * b.den;
    return frac_norm(r);
}

static Frac frac_mul(Frac a, Frac b){
    Frac r;
    r.num = a.num * b.num;
    r.den = a.den * b.den;
    return frac_norm(r);
}

static int frac_is_zero(Frac a){
    return a.num == 0;
}

static Frac frac_div(Frac a, Frac b){
    Frac r;
    r.num = a.num * b.den;
    r.den = a.den * b.num;
    return frac_norm(r);
}

enum Prec { P_ADD = 1, P_MUL = 2, P_NUM = 3 };

typedef struct Expr {
    Frac val;
    char *s;
    int prec;
} Expr;

typedef struct VecExpr {
    Expr *items;
    int cnt;
    int cap;
} VecExpr;

static void ve_init(VecExpr *v){
    v->items = NULL;
    v->cnt = 0;
    v->cap = 0;
}
static void ve_push(VecExpr *v, Expr e){
    if(v->cnt == v->cap){
        int nc = v->cap ? v->cap*2 : 8;
        Expr *tmp = (Expr*)realloc(v->items, sizeof(Expr)*nc);
        if(!tmp){ fprintf(stderr,"alloc failed\n"); exit(1); }
        v->items = tmp;
        v->cap = nc;
    }
    v->items[v->cnt++] = e;
}
static void ve_free(VecExpr *v){
    int i;
    for(i=0;i<v->cnt;i++) free(v->items[i].s);
    free(v->items);
    v->items = NULL;
    v->cnt = v->cap = 0;
}

static char digits[32];
static int n;

static void invalid_and_exit(){
    printf("Invalid input.\n");
    exit(0);
}

static int need_paren_left(int child_prec, int op_prec){
    if(child_prec == P_NUM) return 0;
    return child_prec < op_prec;
}
static int need_paren_right(int child_prec, int op_prec, char op){
    if(child_prec == P_NUM) return 0;
    if(child_prec < op_prec) return 1;
    if(child_prec == op_prec){
        if(op == '-' || op == '/') return 1;
    }
    return 0;
}

static char *strdup_range(const char *s, int a, int b){ /* [a,b) */
    int len = b - a;
    char *r = malloc(len+1);
    if(!r){ fprintf(stderr,"alloc failed\n"); exit(1); }
    memcpy(r, s+a, len);
    r[len] = '\0';
    return r;
}

/* simple string set to deduplicate strings per substring */
typedef struct StrNode {
    char *s;
    struct StrNode *next;
} StrNode;

typedef struct StrSet {
    StrNode **buckets;
    int size;
} StrSet;

static unsigned djb_hash(const char *str){
    unsigned long h = 5381;
    while(*str) h = ((h << 5) + h) + (unsigned char)(*str++);
    return (unsigned)h;
}

static void strset_init(StrSet *ss, int sz){
    ss->size = sz;
    ss->buckets = (StrNode**)calloc(sz, sizeof(StrNode*));
}

static int strset_add(StrSet *ss, const char *s){
    unsigned h = djb_hash(s) % ss->size;
    StrNode *p = ss->buckets[h];
    StrNode *nnode;
    while(p){
        if(strcmp(p->s, s)==0) return 0;
        p = p->next;
    }
    nnode = malloc(sizeof(StrNode));
    if(!nnode){ fprintf(stderr,"alloc failed\n"); exit(1); }
    nnode->s = strdup(s);
    nnode->next = ss->buckets[h];
    ss->buckets[h] = nnode;
    return 1;
}

static void strset_free(StrSet *ss){
    int i;
    StrNode *p;
    for(i=0;i<ss->size;i++){
        p = ss->buckets[i];
        while(p){
            StrNode *nx = p->next;
            free(p->s);
            free(p);
            p = nx;
        }
    }
    free(ss->buckets);
    ss->buckets = NULL;
    ss->size = 0;
}

/* Map from fraction key string to linked list of strings */
typedef struct ListNode {
    char *s;
    struct ListNode *next;
} ListNode;

typedef struct MapEntry {
    char *key;
    ListNode *list;
    struct MapEntry *next;
} MapEntry;

typedef struct Map {
    MapEntry **buckets;
    int size;
} Map;

static void map_init(Map *m, int sz){
    m->size = sz;
    m->buckets = (MapEntry**)calloc(sz, sizeof(MapEntry*));
}

static void map_add(Map *m, const char *key, const char *s){
    unsigned h = djb_hash(key) % m->size;
    MapEntry *p = m->buckets[h];
    while(p){
        if(strcmp(p->key, key)==0) break;
        p = p->next;
    }
    if(!p){
        p = malloc(sizeof(MapEntry));
        p->key = strdup(key);
        p->list = NULL;
        p->next = m->buckets[h];
        m->buckets[h] = p;
    }
    ListNode *ln = malloc(sizeof(ListNode));
    ln->s = strdup(s);
    ln->next = p->list;
    p->list = ln;
}

static ListNode *map_get(Map *m, const char *key){
    unsigned h = djb_hash(key) % m->size;
    MapEntry *p = m->buckets[h];
    while(p){
        if(strcmp(p->key, key)==0) return p->list;
        p = p->next;
    }
    return NULL;
}

static void map_free(Map *m){
    for(int i=0;i<m->size;i++){
        MapEntry *p = m->buckets[i];
        while(p){
            MapEntry *nx = p->next;
            free(p->key);
            ListNode *ln = p->list;
            while(ln){
                ListNode *lnx = ln->next;
                free(ln->s);
                free(ln);
                ln = lnx;
            }
            free(p);
            p = nx;
        }
    }
    free(m->buckets);
    m->buckets = NULL;
    m->size = 0;
}

static char *frac_key(Frac f){
    Frac g = frac_norm(f);
    char *buf = malloc(200);
    char a[100], b[100];
    i128_to_str(g.num, a);
    i128_to_str(g.den, b);
    snprintf(buf, 200, "%s/%s", a, b);
    return buf;
}

/* DP table: dp[i*(n+1)+j] holds all expressions for substring [i,j) */
static VecExpr **dp;

static int precedence_of_op(char op){
    if(op=='+' || op=='-') return P_ADD;
    return P_MUL;
}

int main(void){
    char line[256];
    if(!fgets(line, sizeof(line), stdin)) return 0;
    printf("Digits:\n");
    char tok[64];
    if(sscanf(line," %63s", tok) != 1) invalid_and_exit();
    int len = strlen(tok);
    if(len == 0 || len > 10) invalid_and_exit();
    for(int i=0;i<len;i++) if(!isdigit((unsigned char)tok[i])) invalid_and_exit();
    strcpy(digits, tok);
    n = len;

    int total_cells = (n+1)*(n+1);
    dp = malloc(sizeof(VecExpr*) * total_cells);
    if(!dp){ fprintf(stderr,"alloc failed\n"); exit(1); }
    for(int i=0;i<total_cells;i++) dp[i] = NULL;

    /* initialize base substrings as numbers */
    for(int i=0;i<n;i++){
        for(int j=i+1;j<=n;j++){
            int idx = i*(n+1) + j;
            dp[idx] = malloc(sizeof(VecExpr));
            ve_init(dp[idx]);
            char *numstr = strdup_range(digits, i, j);
            i128 v = 0;
            for(int k=i;k<j;k++) v = v*10 + (digits[k]-'0');
            Expr e;
            e.val = frac_norm(make_frac_i128(v));
            e.s = numstr;
            e.prec = P_NUM;
            ve_push(dp[idx], e);
        }
    }

    /* DP combine substrings by length, deduplicating by textual expression */
    for(int L=2; L<=n; L++){
        for(int i=0;i+L<=n;i++){
            int j = i + L;
            int idx = i*(n+1) + j;
            if(!dp[idx]){ dp[idx] = malloc(sizeof(VecExpr)); ve_init(dp[idx]); }
            StrSet ss;
            strset_init(&ss, 409);
            for(int k=i+1;k<j;k++){
                VecExpr *Lvec = dp[i*(n+1) + k];
                VecExpr *Rvec = dp[k*(n+1) + j];
                if(!Lvec || !Rvec) continue;
                for(int a=0;a<Lvec->cnt;a++){
                    for(int b=0;b<Rvec->cnt;b++){
                        Expr *le = &Lvec->items[a];
                        Expr *re = &Rvec->items[b];
                        Frac results[4];
                        int ok[4] = {1,1,1,1};
                        results[0] = frac_add(le->val, re->val);   /* + */
                        results[1] = frac_sub(le->val, re->val);   /* - */
                        results[2] = frac_mul(le->val, re->val);   /* * */
                        if(frac_is_zero(re->val)) ok[3] = 0;
                        else results[3] = frac_div(le->val, re->val); /* / */
                        char ops[4] = {'+','-','*','/'};
                        for(int opi=0; opi<4; opi++){
                            if(!ok[opi]) continue;
                            char op = ops[opi];
                            int op_prec = precedence_of_op(op);
                            int needL = need_paren_left(le->prec, op_prec);
                            int needR = need_paren_right(re->prec, op_prec, op);
                            int lenL = strlen(le->s);
                            int lenR = strlen(re->s);
                            int extra = (needL?2:0) + (needR?2:0) + 1;
                            char *ns = malloc(lenL + lenR + extra + 1);
                            char *p = ns;
                            if(needL){ *p++='('; memcpy(p, le->s, lenL); p+=lenL; *p++=')'; }
                            else { memcpy(p, le->s, lenL); p+=lenL; }
                            *p++ = op;
                            if(needR){ *p++='('; memcpy(p, re->s, lenR); p+=lenR; *p++=')'; }
                            else { memcpy(p, re->s, lenR); p+=lenR; }
                            *p = '\0';
                            if(strset_add(&ss, ns)){
                                Expr ne;
                                ne.val = frac_norm(results[opi]);
                                ne.s = ns;
                                ne.prec = op_prec;
                                ve_push(dp[idx], ne);
                            } else {
                                free(ns);
                            }
                        }
                    }
                }
            }
            strset_free(&ss);
        }
    }

    /* Build map for full substring [0,n) */
    Map map;
    map_init(&map, 4096);
    VecExpr *full = dp[0*(n+1) + n];
    if(full){
        for(int i=0;i<full->cnt;i++){
            char *k = frac_key(full->items[i].val);
            map_add(&map, k, full->items[i].s);
            free(k);
        }
    }

    /* Read problems */
    printf("Problems:\n");
    while(fgets(line, sizeof(line), stdin)){
        if(line[0]=='\n' || line[0]=='\r') continue;
        char typ; char numtok[128];
        if(sscanf(line, " %c %127s", &typ, numtok) != 2) invalid_and_exit();
        if(typ!='?' && typ!='#') invalid_and_exit();
        /* validate integer */
        int pos = 0;
        if(numtok[0]=='-'){ pos = 1; if(numtok[1]=='\0') invalid_and_exit(); }
        for(int i=pos; numtok[i]; i++) if(!isdigit((unsigned char)numtok[i])) invalid_and_exit();
        long long target = strtoll(numtok, NULL, 10);
        Frac want = make_frac_i128((i128)target);
        char *key = frac_key(want);
        ListNode *ln = map_get(&map, key);
        long long cnt = 0;
        if(ln){
            ListNode *p = ln;
            while(p){ cnt++; p = p->next; }
            if(typ=='?'){
                p = ln;
                while(p){ printf("= %s\n", p->s); p = p->next; }
            }
        }
        printf("Total: %lld\n", cnt);
        free(key);
    }

    /* cleanup */
    map_free(&map);
    for(int i=0;i<total_cells;i++){
        if(dp[i]){
            ve_free(dp[i]);
            free(dp[i]);
            dp[i] = NULL;
        }
    }
    free(dp);
    return 0;
}