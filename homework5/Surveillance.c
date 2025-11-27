#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define RZ_MAX_LEN 1000

typedef struct {
    int camera;
    int time; // minutes from year start (non-leap)
} Report;

typedef struct RZNode {
    char *rz;
    Report *reports;
    size_t rcnt;
    size_t rcap;
    struct RZNode *next;
} RZNode;

typedef struct {
    int time;
    int camera;
    char *rz; // pointer to RZNode->rz
} GlobalEntry;

static const char *months[] = {"","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
static const int monthDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

// forward declaration of cleanup and global pointers (used by error handler)
void free_all(RZNode *head, GlobalEntry *globals, char *buf);
static RZNode *g_rzhead = NULL;
static GlobalEntry *g_globals = NULL;
static char *g_buf = NULL;

static int g_header_printed = 0;
void error_and_exit(void) {
    // always print header first if not already done
    if (!g_header_printed) {
        printf("Camera reports:\n");
        g_header_printed = 1;
    }
    // attempt to free any allocated memory, then exit
    free_all(g_rzhead, g_globals, g_buf);
    printf("Invalid input.\n");
    exit(0);
}

int month_to_int(const char *m) {
    for (int i = 1; i <= 12; ++i) if (strcmp(months[i], m) == 0) return i;
    return -1;
}

int compute_minutes(int mon, int day, int hour, int min) {
    int days = 0;
    for (int i = 1; i < mon; ++i) days += monthDays[i];
    days += (day - 1);
    return days * 24 * 60 + hour * 60 + min;
}

// dynamic append
void rznode_append(RZNode *node, int camera, int time) {
    if (node->rcap == 0) {
        node->rcap = 4;
        node->reports = (Report*)malloc(node->rcap * sizeof(Report));
        if (!node->reports) { perror("malloc"); exit(1); }
    }
    if (node->rcnt >= node->rcap) {
        node->rcap *= 2;
        node->reports = (Report*)realloc(node->reports, node->rcap * sizeof(Report));
        if (!node->reports) { perror("realloc"); exit(1); }
    }
    node->reports[node->rcnt].camera = camera;
    node->reports[node->rcnt].time = time;
    node->rcnt++;
}

RZNode *rz_list_find(RZNode *head, const char *rz) {
    for (RZNode *p = head; p; p = p->next) if (strcmp(p->rz, rz) == 0) return p;
    return NULL;
}

RZNode *rz_list_add(RZNode **headp, const char *rz) {
    RZNode *node = (RZNode*)malloc(sizeof(RZNode));
    if (!node) { perror("malloc"); exit(1); }
    node->rz = strdup(rz);
    if (!node->rz) { perror("strdup"); exit(1); }
    node->reports = NULL; node->rcnt = 0; node->rcap = 0;
    node->next = *headp; *headp = node;
    // update global head so error handler can free
    g_rzhead = *headp;
    return node;
}

int compare_reports(const void *a, const void *b) {
    const Report *A = (const Report*)a; 
    const Report *B = (const Report*)b;
    if (A->time != B->time) return (A->time < B->time) ? -1 : 1;
    if (A->camera != B->camera) return (A->camera < B->camera) ? -1 : 1;
    return 0;
}

int compare_int(const void *a, const void *b) {
    int A = *(const int*)a;
    int B = *(const int*)b;
    return (A < B) ? -1 : (A > B) ? 1 : 0;
}

int compare_global(const void *a, const void *b) {
    const GlobalEntry *A = (const GlobalEntry*)a; 
    const GlobalEntry *B = (const GlobalEntry*)b;
    if (A->time != B->time) return (A->time < B->time) ? -1 : 1;
    if (A->camera != B->camera) return (A->camera < B->camera) ? -1 : 1;
    int cmp = strcmp(A->rz, B->rz);
    return cmp;
}

// Helper: format time into provided buffers for reuse
void format_time(int minutes, char *out, size_t outsz) {
    int daymins = 24*60;
    int remdays = minutes / daymins;
    int d = remdays + 1;
    int m = 1;
    while (m <= 12) {
        if (d <= monthDays[m]) break;
        d -= monthDays[m];
        m++;
    }
    int hour = (minutes % daymins) / 60;
    int min = minutes % 60;
    snprintf(out, outsz, "%s %d %02d:%02d", months[m], d, hour, min);
}

// free all dynamic memory allocated during program execution
void free_all(RZNode *head, GlobalEntry *globals, char *buf) {
    // free globals array
    if (globals) free(globals);
    // free RZ nodes
    RZNode *n = head;
    while (n) {
        RZNode *nx = n->next;
        if (n->reports) free(n->reports);
        if (n->rz) free(n->rz);
        free(n);
        n = nx;
    }
    if (buf) free(buf);
}

// read entire stdin into dynamically growing buffer
char *read_all_stdin(void) {
    size_t cap = 4096;
    size_t len = 0;
    char *buf = (char*)malloc(cap);
    if (!buf) { perror("malloc"); exit(1); }
    int c;
    while ((c = fgetc(stdin)) != EOF) {
        buf[len++] = (char)c;
        if (len + 1 >= cap) {
            cap *= 2;
            buf = (char*)realloc(buf, cap);
            if (!buf) { perror("realloc"); exit(1); }
        }
    }
    buf[len] = '\0';
    g_buf = buf;
    return buf;
}

// forward declaration for cleanup (defined above)
void free_all(RZNode *head, GlobalEntry *globals, char *buf);

int main(void) {
    g_header_printed = 0;
    char *buf = read_all_stdin();
    char *p = buf;

    // skip spaces
    while (isspace((unsigned char)*p)) p++;
    if (*p != '{') error_and_exit();
    p++;

    RZNode *rzhead = NULL;
    GlobalEntry *globals = NULL; size_t gcap = 0, gcnt = 0;

    int found_any = 0;
    // parse reports until '}'
    for (;;) {
        while (isspace((unsigned char)*p)) p++;
        if (*p == '}') { p++; break; }

        // parse camera id -- must be unsigned integer (no sign)
        if (!isdigit((unsigned char)*p)) error_and_exit();
        long cam = 0;
        int digits = 0;
        while (isdigit((unsigned char)*p)) {
            digits = 1;
            int d = *p - '0';
            if (cam > (LONG_MAX - d) / 10) error_and_exit();
            cam = cam*10 + d;
            p++;
        }
        if (!digits) error_and_exit();
        if (cam > INT_MAX) error_and_exit();
        while (isspace((unsigned char)*p)) p++;
        if (*p != ':') error_and_exit();
        p++; // skip ':'
        while (isspace((unsigned char)*p)) p++;
        // parse RZ token (no spaces)
        if (*p == '\0') error_and_exit();
        char rzbuf[RZ_MAX_LEN+2];
        int rzlen = 0;
        while (*p != '\0' && !isspace((unsigned char)*p)) {
            if (rzlen < RZ_MAX_LEN) rzbuf[rzlen++] = *p;
            p++;
        }
        rzbuf[rzlen] = '\0';
        if (rzlen == 0) error_and_exit();
        if (rzlen > RZ_MAX_LEN) error_and_exit();
        while (isspace((unsigned char)*p)) p++;
        // parse month (3-letter)
        char monbuf[16]; int monlen = 0;
        while (*p && !isspace((unsigned char)*p)) { if (monlen < 15) monbuf[monlen++] = *p; p++; }
        monbuf[monlen] = '\0';
        if (monlen == 0) error_and_exit();
        int mon = month_to_int(monbuf);
        if (mon == -1) error_and_exit();
        while (isspace((unsigned char)*p)) p++;
        // parse day
        if (!isdigit((unsigned char)*p)) error_and_exit();
        int day = 0;
        while (isdigit((unsigned char)*p)) { day = day*10 + (*p - '0'); p++; }
        if (day < 1 || day > 31) error_and_exit();
        // validate day vs month
        if (day > monthDays[mon]) error_and_exit();
        while (isspace((unsigned char)*p)) p++;
        // parse hour
        if (!isdigit((unsigned char)*p)) error_and_exit();
        int hour = 0;
        while (isdigit((unsigned char)*p)) { hour = hour*10 + (*p - '0'); p++; }
        if (hour < 0 || hour > 23) error_and_exit();
        while (isspace((unsigned char)*p)) p++;
        if (*p != ':') error_and_exit();
        p++;
        while (isspace((unsigned char)*p)) p++;
        if (!isdigit((unsigned char)*p)) error_and_exit();
        int minute = 0;
        while (isdigit((unsigned char)*p)) { minute = minute*10 + (*p - '0'); p++; }
        if (minute < 0 || minute > 59) error_and_exit();

        // successfully parsed one report
        found_any = 1;
        int t = compute_minutes(mon, day, hour, minute);
        // add to rz list
        RZNode *node = rz_list_find(rzhead, rzbuf);
        if (!node) node = rz_list_add(&rzhead, rzbuf);
        rznode_append(node, (int)cam, t);
        // add to globals
        if (gcnt >= gcap) { 
            gcap = gcap ? gcap*2 : 8; 
            globals = (GlobalEntry*)realloc(globals, gcap * sizeof(GlobalEntry)); 
            if (!globals) { perror("realloc"); exit(1); } 
            g_globals = globals;
        }
        globals[gcnt].time = t; globals[gcnt].camera = (int)cam; globals[gcnt].rz = node->rz; gcnt++;

        while (isspace((unsigned char)*p)) p++;
        if (*p == ',') { p++; continue; }
        else if (*p == '}') { p++; break; }
        else error_and_exit();
    }

    if (!found_any) error_and_exit();

    // sort reports per rz and globals
    for (RZNode *n = rzhead; n; n = n->next) {
        if (n->rcnt > 1) qsort(n->reports, n->rcnt, sizeof(Report), compare_reports);
    }
    if (gcnt > 1) qsort(globals, gcnt, sizeof(GlobalEntry), compare_global);

    // now parse search commands until EOF
    char outtime[64];
    // print required headers
    if (!g_header_printed) {
        printf("Camera reports:\n");
        g_header_printed = 1;
    }
    // determine if there are search queries following the report block
    char *q = p;
    while (isspace((unsigned char)*q)) q++;
    int has_search = (*q != '\0');
    if (has_search) printf("Search:\n");
    while (*p) {
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0') break;
        // parse RZ
        char rzq[RZ_MAX_LEN+2]; int l = 0;
        while (*p && !isspace((unsigned char)*p)) { if (l < RZ_MAX_LEN) rzq[l++] = *p; p++; }
        rzq[l] = '\0';
        if (l == 0) { error_and_exit(); }
        if (l > RZ_MAX_LEN) error_and_exit();
        while (isspace((unsigned char)*p)) p++;
        char monbuf[16]; int monlen = 0;
        while (*p && !isspace((unsigned char)*p)) { if (monlen < 15) monbuf[monlen++] = *p; p++; }
        monbuf[monlen] = '\0';
        if (monlen == 0) error_and_exit();
        int mon = month_to_int(monbuf);
        if (mon == -1) error_and_exit();
        while (isspace((unsigned char)*p)) p++;
        if (!isdigit((unsigned char)*p)) error_and_exit();
        int day = 0;
        while (isdigit((unsigned char)*p)) { day = day*10 + (*p - '0'); p++; }
        if (day < 1 || day > monthDays[mon]) error_and_exit();
        while (isspace((unsigned char)*p)) p++;
        if (!isdigit((unsigned char)*p)) error_and_exit();
        int hour = 0;
        while (isdigit((unsigned char)*p)) { hour = hour*10 + (*p - '0'); p++; }
        if (hour < 0 || hour > 23) error_and_exit();
        while (isspace((unsigned char)*p)) p++;
    if (*p != ':') error_and_exit();
    p++;
        while (isspace((unsigned char)*p)) p++;
        if (!isdigit((unsigned char)*p)) error_and_exit();
        int minute = 0;
        while (isdigit((unsigned char)*p)) { minute = minute*10 + (*p - '0'); p++; }
        if (minute < 0 || minute > 59) error_and_exit();

        int t = compute_minutes(mon, day, hour, minute);

        // process query
        RZNode *node = rz_list_find(rzhead, rzq);
        if (!node) {
            printf("> Car not found.\n");
        } else {
            int first = -1, last = -1;
            for (size_t i = 0; i < node->rcnt; ++i) {
                if (node->reports[i].time == t) { if (first == -1) first = (int)i; last = (int)i; }
            }
            if (first != -1) {
                int count = last - first + 1;
                int *cams = (int*)malloc(count * sizeof(int));
                if (!cams) { perror("malloc"); exit(1); }
                for (int i = 0; i < count; ++i) cams[i] = node->reports[first + i].camera;
                qsort(cams, count, sizeof(int), compare_int);
                format_time(t, outtime, sizeof(outtime));
                printf("> Exact: %s, %dx [", outtime, count);
                for (int i = 0; i < count; ++i) { if (i) printf(", "); printf("%d", cams[i]); }
                printf("]\n");
                free(cams);
            } else {
                int prevtime = -1, nexttime = -1;
                for (size_t i = 0; i < node->rcnt; ++i) {
                    if (node->reports[i].time < t) prevtime = node->reports[i].time;
                    if (node->reports[i].time > t) { nexttime = node->reports[i].time; break; }
                }
                if (prevtime == -1) printf("> Previous: N/A\n");
                else {
                    int cnt = 0;
                    for (size_t i = 0; i < node->rcnt; ++i) if (node->reports[i].time == prevtime) cnt++;
                    int *cams = (int*)malloc(cnt * sizeof(int)); 
                    if (!cams) { perror("malloc"); exit(1); }
                    int k = 0;
                    for (size_t i = 0; i < node->rcnt; ++i) if (node->reports[i].time == prevtime) cams[k++] = node->reports[i].camera;
                    qsort(cams, cnt, sizeof(int), compare_int);
                    format_time(prevtime, outtime, sizeof(outtime));
                    printf("> Previous: %s, %dx [", outtime, cnt);
                    for (int i = 0; i < cnt; ++i) { if (i) printf(", "); printf("%d", cams[i]); }
                    printf("]\n"); 
                    free(cams);
                }
                if (nexttime == -1) printf("> Next: N/A\n");
                else {
                    int cnt = 0;
                    for (size_t i = 0; i < node->rcnt; ++i) if (node->reports[i].time == nexttime) cnt++;
                    int *cams = (int*)malloc(cnt * sizeof(int));
                    if (!cams) { perror("malloc"); exit(1); }
                    int k = 0;
                    for (size_t i = 0; i < node->rcnt; ++i) if (node->reports[i].time == nexttime) cams[k++] = node->reports[i].camera;
                    qsort(cams, cnt, sizeof(int), compare_int);
                    format_time(nexttime, outtime, sizeof(outtime));
                    printf("> Next: %s, %dx [", outtime, cnt);
                    for (int i = 0; i < cnt; ++i) { if (i) printf(", "); printf("%d", cams[i]); }
                    printf("]\n"); 
                    free(cams);
                }
            }
        }
    }

    return 0;
}