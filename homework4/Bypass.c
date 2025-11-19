#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

typedef long long ll;

#define MIN_YEAR 1900

/* ---- DATE UTILITIES ---- */
int is_leap(int y) {
    if (y % 400 == 0) return 1;
    if (y % 100 == 0) return 0;
    if (y % 4 == 0) return 1;
    return 0;
}

int mdays(int y, int m) {
    static const int md[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (m == 2) return md[2] + is_leap(y);
    return md[m];
}

long long date_to_days(const char *s) {
    int y = 0, m = 0, d = 0;
    const char *p = s;
    const char *q = p;
    while (*q && isdigit((unsigned char)*q)) q++;
    if (*q != '-' || q == p) return -1;
    {
        const char *t;
        for (t = p; t < q; t++) y = y * 10 + (*t - '0');
    }
    p = q + 1;
    if (!isdigit((unsigned char)p[0]) || !isdigit((unsigned char)p[1]) || p[2] != '-') return -1;
    m = (p[0] - '0') * 10 + (p[1] - '0'); p += 3;
    if (!isdigit((unsigned char)p[0]) || !isdigit((unsigned char)p[1])) return -1;
    d = (p[0] - '0') * 10 + (p[1] - '0');
    if (y < MIN_YEAR || m < 1 || m > 12 || d < 1 || d > mdays(y, m)) return -1;
    long long yy = y;
    int doy = d;
    {
        int ii;
        for (ii = 1; ii < m; ii++) doy += mdays(y, ii);
    }
    return 365 * (yy - 1) + (yy - 1) / 4 - (yy - 1) / 100 + (yy - 1) / 400 + doy;
}

/* ---- STRUCTURES ---- */
typedef struct { ll day; int cost; } upd_t;
typedef struct { upd_t *a; int n, cap; } UpdArr;

void ua_init(UpdArr *u) { u->a = NULL; u->n = u->cap = 0; }
void ua_free(UpdArr *u) { free(u->a); u->a = NULL; u->n = u->cap = 0; }
int ua_push(UpdArr *u, ll d, int c) {
    if (u->n == u->cap) {
        int nc = u->cap ? u->cap * 2 : 4;
        upd_t *p = (upd_t*)realloc(u->a, (size_t)nc * sizeof(upd_t));
        if (!p) return 0;
        u->a = p; u->cap = nc;
    }
    u->a[u->n].day = d; u->a[u->n].cost = c; u->n++; return 1;
}

int ua_find(UpdArr *u, ll day) {
    int lo = 0, hi = u->n - 1, ans = -1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (u->a[mid].day <= day) { ans = mid; lo = mid + 1; }
        else hi = mid - 1;
    }
    return ans;
}

/* ---- HELPERS ---- */
void skipws(const char **p) { while (**p && isspace((unsigned char)**p)) (*p)++; }
int parse_int(const char **p, long long *out) {
    skipws(p);
    int sign = 1; 
    if (**p == '-') { sign = -1; (*p)++; } 
    else if (**p == '+') { (*p)++; }
    if (!isdigit((unsigned char)**p)) return 0;
    long long v = 0;
    while (isdigit((unsigned char)**p)) { v = v * 10 + (**p - '0'); (*p)++; }
    *out = v * sign; return 1;
}

/* ---- INITIAL COST LIST ---- */
int read_initial_costs(int **out, int *n) {
    int c; while ((c = getchar()) != EOF && isspace(c));
    if (c != '{') return -1;
    int *a = (int*)malloc(32 * sizeof(int));
    if (!a) return -1;
    int cap = 32, len = 0;
    while (1) {
        while ((c = getchar()) != EOF && isspace(c));
        if (c == '}') break;
        if (c == EOF) { free(a); return -1; }
        ungetc(c, stdin);
        long long val; 
        if (scanf("%lld", &val) != 1 || val <= 0 || val > INT_MAX) { free(a); return -1; }
        if (len == cap) {
            cap *= 2;
            int *tmp = (int*)realloc(a, cap * sizeof(int));
            if (!tmp) { free(a); return -1; }
            a = tmp;
        }
        a[len++] = (int)val;
        while ((c = getchar()) != EOF && isspace(c));
        if (c == '}') break;
        if (c != ',') { free(a); return -1; }
    }
    if (len < 2 || len > 10000) { free(a); return -1; }
    *out = a; *n = len; return 0;
}

/* ---- COMMAND PARSING ---- */
int parse_update(const char *l, ll *day, int *sec, int *cost) {
    const char *p = l; skipws(&p); if (*p != '=') return -1; p++;
    skipws(&p);
    char ds[32]; int i = 0;
    while (*p && !isspace((unsigned char)*p) && *p != ':') { if (i < 31) ds[i++] = *p; p++; }
    ds[i] = 0;
    *day = date_to_days(ds);
    if (*day < 0) return -1;
    skipws(&p);
    long long s; if (!parse_int(&p, &s)) return -1;
    skipws(&p);
    if (*p != ':') return -1;
    p++;
    long long c; if (!parse_int(&p, &c) || c <= 0 || c > INT_MAX) return -1;
    *sec = (int)s; *cost = (int)c;
    return 0;
}

int parse_query(const char *l, ll *f, ll *t) {
    const char *p = l; skipws(&p); if (*p != '?') return -1; p++;
    skipws(&p);
    char d1[32], d2[32]; int i = 0;
    while (*p && !isspace((unsigned char)*p)) { if (i < 31) d1[i++] = *p; p++; } d1[i] = 0;
    skipws(&p); i = 0;
    while (*p && !isspace((unsigned char)*p) && *p != '\n') { if (i < 31) d2[i++] = *p; p++; } d2[i] = 0;
    *f = date_to_days(d1); *t = date_to_days(d2);
    if (*f < 0 || *t < 0 || *t < *f) return -1;
    return 0;
}

/* ---- COST AGGREGATION ---- */
void compute_totals(UpdArr *u, int n, ll from, ll to, ll *res) {
    {
        int ii;
        for (ii = 0; ii < n; ii++) {
            int idx = ua_find(&u[ii], from);
            if (idx < 0) { res[ii] = 0; continue; }
            ll sum = 0, curday = from; int c = u[ii].a[idx].cost;
            int nxt = idx + 1;
            while (curday <= to) {
                ll nextd = (nxt < u[ii].n) ? u[ii].a[nxt].day : (to + 1);
                ll end = (nextd - 1 <= to) ? nextd - 1 : to;
                if (end >= curday) sum += (end - curday + 1) * c;
                if (nextd > to) break;
                curday = u[ii].a[nxt].day; c = u[ii].a[nxt].cost; nxt++;
            }
            res[ii] = sum;
        }
    }

}

/* ---- PARTITION SEARCH ---- */
typedef struct { int s, e; } Pair;
typedef struct { Pair *a; int n, cap; } Pairs;

void pa_init(Pairs *p) { p->a = NULL; p->n = p->cap = 0; }
void pa_free(Pairs *p) { free(p->a); p->a = NULL; p->n = p->cap = 0; }
void pa_push(Pairs *p, int s, int e) {
    if (p->n == p->cap) {
        int newcap = p->cap ? 2 * p->cap : 8;
        Pair *tmp = (Pair*)realloc(p->a, newcap * sizeof(Pair));
        if (!tmp) return;
        p->a = tmp; p->cap = newcap;
    }
    p->a[p->n].s = s; p->a[p->n].e = e; p->n++;
}

int is_canon(int s, int len, int n) {
    int s2 = (s + len) % n;
    int l2 = n - len;
    if (s < s2) return 1;
    if (s > s2) return 0;
    return len <= l2;
}

void find_best(ll *v, int n, ll tot, ll *best, Pairs *sol) {
    ll *d = (ll*)malloc(sizeof(ll) * 2 * n);
    if (!d) { *best = 0; pa_init(sol); return; }
    {
        int ii;
        for (ii = 0; ii < 2 * n; ii++) d[ii] = v[ii % n];
    }
    ll target = tot / 2;
    ll cur = 0;
    int end = 0;
    *best = LLONG_MAX; pa_init(sol);
    {
        int st;
        for (st = 0; st < n; st++) {
            if (end < st) { end = st; cur = 0; }
            while (end < st + n && cur + d[end] <= target) { cur += d[end++]; }
            {
                int L = end - st;
                if (L >= 1 && L <= n - 1) {
                    ll diff = llabs(tot - 2 * cur);
                    if (diff < *best) { *best = diff; sol->n = 0; }
                    if (diff == *best && is_canon(st, L, n)) { int e = (st + L - 1) % n; pa_push(sol, st, e); }
                }
            }
            if (end < st + n) {
                ll diff = llabs(tot - 2 * (cur + d[end]));
                {
                    int L2 = end - st + 1;
                    if (L2 >= 1 && L2 <= n - 1) {
                        if (diff < *best) { *best = diff; sol->n = 0; }
                        if (diff == *best && is_canon(st, L2, n)) { int e = (st + L2 - 1) % n; pa_push(sol, st, e); }
                    }
                }
            }
            if (end > st) cur -= d[st];
        }
    }
    free(d);
}

/* ---- MAIN ---- */
int main(void) {
    int *init = NULL, n = 0;
    /* print header first so even initial-input errors match expected output */
    printf("Daily cost:\n");
    if (read_initial_costs(&init, &n)) { printf("Invalid input.\n"); return 0; }

    UpdArr *U = (UpdArr*)malloc((size_t)n * sizeof(UpdArr));
    if (!U) { printf("Invalid input.\n"); free(init); return 0; }
    {
        int i, j;
        for (i = 0; i < n; i++) {
            ua_init(&U[i]);
            char tmp[16];
            sprintf(tmp, "%04d-01-01", MIN_YEAR);
            ll d = date_to_days(tmp);
            if (!ua_push(&U[i], d, init[i])) {
                printf("Invalid input.\n");
                for (j = 0; j < i; j++) ua_free(&U[j]);
                free(U); free(init); return 0;
            }
        }
    }
    free(init);

    ll last = -1;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        const char *p = line; skipws(&p);
        if (*p == 0 || *p == '\n') continue;
        if (*p == '=') {
            ll day; int s, c;
            if (parse_update(line, &day, &s, &c) || s < 0 || s >= n || day <= last) {
                printf("Invalid input.\n");
                {
                    int ii;
                    for (ii = 0; ii < n; ii++) ua_free(&U[ii]);
                }
                free(U); return 0;
            }
            if (!ua_push(&U[s], day, c)) {
                printf("Invalid input.\n");
                {
                    int ii;
                    for (ii = 0; ii < n; ii++) ua_free(&U[ii]);
                }
                free(U); return 0;
            }
            last = day;
        } else if (*p == '?') {
            ll f, t;
            if (parse_query(line, &f, &t)) {
                printf("Invalid input.\n");
                {
                    int ii;
                    for (ii = 0; ii < n; ii++) ua_free(&U[ii]);
                }
                free(U); return 0;
            }
            ll *vals = (ll*)malloc(sizeof(ll) * (size_t)n);
            if (!vals) {
                printf("Invalid input.\n");
                {
                    int ii;
                    for (ii = 0; ii < n; ii++) ua_free(&U[ii]);
                }
                free(U); return 0;
            }
            compute_totals(U, n, f, t, vals);
            {
                int ii;
                ll tot = 0;
                for (ii = 0; ii < n; ii++) tot += vals[ii];
                ll best;
                Pairs sol;
                find_best(vals, n, tot, &best, &sol);
                printf("Difference: %lld, options: %d\n", best, sol.n);
                for (ii = 0; ii < sol.n; ii++) {
                    int s1 = sol.a[ii].s, e1 = sol.a[ii].e;
                    int s2 = (e1 + 1) % n, e2 = (s1 - 1 + n) % n;
                    printf("* %d - %d, %d - %d\n", s1, e1, s2, e2);
                }
                pa_free(&sol);
            }
            free(vals);
        } else {
            printf("Invalid input.\n");
            {
                int ii;
                for (ii = 0; ii < n; ii++) ua_free(&U[ii]);
            }
            free(U); return 0;
        }
    }
    /* free allocated update arrays */
    {
        int ii;
        for (ii = 0; ii < n; ii++) ua_free(&U[ii]);
    }
    free(U);
}