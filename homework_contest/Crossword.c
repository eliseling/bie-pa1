/* Crossword solver for Progtest-style input
 * Reads a bordered crossword and a list of words from stdin,
 * validates input and either prints the unique solution,
 * reports no solution, invalid input, or prints number of
 * unique solutions when more than one exists.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_IN 18
#define MAX_LINES 1024
#define MAX_INSIDE 16
#define MAX_WORDS 60
#define MAX_WORD_LEN 16

typedef struct {
    int r, c; /* starting position (0-based inside grid) */
    int len;
    int dir; /* 0 horizontal, 1 vertical */
    int used;
} Run;

static char grid[MAX_INSIDE][MAX_INSIDE]; /* working filled letters or ' ' */
static char original[MAX_INSIDE][MAX_INSIDE]; /* stores '*' or ' ' from input */
static int H, W; /* inside height and width */

static Run runs[256];
static int runs_n = 0;

/* unique words storage */
static char uniq[MAX_WORDS][MAX_WORD_LEN+1];
static int uniq_count = 0;
static int uniq_cnt[MAX_WORDS];

static int total_runs = 0;
static long long solution_count = 0;
static char solution_board[MAX_INSIDE][MAX_INSIDE];

/* Utility: trim newline and carriage return */
static void chomp(char *s) {
    int n = strlen(s);
    while (n>0 && (s[n-1]=='\n' || s[n-1]=='\r')) { s[--n] = '\0'; }
}

/* Check top/bottom border validity */
static int is_border(const char *s) {
    int n = strlen(s);
    if (n < 2) return 0;
    if (s[0] != '+' || s[n-1] != '+') return 0;
    for (int i = 1; i < n-1; ++i) if (s[i] != '-') return 0;
    return 1;
}

/* Count runs in the original grid and store run descriptors */
static void extract_runs(void) {
    runs_n = 0;
    /* horizontal */
    for (int r = 0; r < H; ++r) {
        int c = 0;
        while (c < W) {
            while (c < W && original[r][c] == '*') ++c;
            if (c >= W) break;
            int start = c;
            while (c < W && original[r][c] == ' ') ++c;
            int len = c - start;
            if (len > 0) {
                runs[runs_n].r = r; runs[runs_n].c = start;
                runs[runs_n].len = len; runs[runs_n].dir = 0; runs[runs_n].used = 0;
                runs_n++;
            }
        }
    }
    /* vertical */
    for (int c = 0; c < W; ++c) {
        int r = 0;
        while (r < H) {
            while (r < H && original[r][c] == '*') ++r;
            if (r >= H) break;
            int start = r;
            while (r < H && original[r][c] == ' ') ++r;
            int len = r - start;
            if (len > 0) {
                runs[runs_n].r = start; runs[runs_n].c = c;
                runs[runs_n].len = len; runs[runs_n].dir = 1; runs[runs_n].used = 0;
                runs_n++;
            }
        }
    }
}

/* check if a unique word fits run given current grid letters */
static int fits_run(int run_idx, const char *w) {
    Run *rn = &runs[run_idx];
    int r = rn->r, c = rn->c;
    for (int i = 0; i < rn->len; ++i) {
        char g = grid[r][c];
        if (g != ' ' && g != w[i]) return 0;
        if (rn->dir == 0) ++c; else ++r;
    }
    return 1;
}

/* Place a word into run; record which positions were written (prev ' ') */
static void place_word(int run_idx, const char *w, int written_pos[]) {
    Run *rn = &runs[run_idx];
    int r = rn->r, c = rn->c;
    for (int i = 0; i < rn->len; ++i) {
        if (grid[r][c] == ' ') { written_pos[i] = 1; grid[r][c] = w[i]; }
        else written_pos[i] = 0;
        if (rn->dir == 0) ++c; else ++r;
    }
}

static void remove_word(int run_idx, int written_pos[]) {
    Run *rn = &runs[run_idx];
    int r = rn->r, c = rn->c;
    for (int i = 0; i < rn->len; ++i) {
        if (written_pos[i]) grid[r][c] = ' ';
        if (rn->dir == 0) ++c; else ++r;
    }
}

/* choose next run (MRV): unfilled run with minimal number of candidate words */
static int choose_run(void) {
    int best = -1; int best_count = 1000000;
    for (int i = 0; i < runs_n; ++i) {
        if (runs[i].used) continue;
        int cnt = 0;
        for (int u = 0; u < uniq_count; ++u) {
            if (uniq_cnt[u] <= 0) continue;
            if ((int)strlen(uniq[u]) != runs[i].len) continue;
            if (fits_run(i, uniq[u])) ++cnt;
        }
        if (cnt == 0) return i; /* immediate dead end */
        if (cnt < best_count) { best_count = cnt; best = i; }
    }
    return best;
}

/* store current grid as solution_board */
static void store_solution(void) {
    for (int r = 0; r < H; ++r) for (int c = 0; c < W; ++c) solution_board[r][c] = grid[r][c];
}

/* recursive backtracking */
static void solve_rec(int depth) {
    if (depth == runs_n) {
        solution_count++;
        if (solution_count == 1) store_solution();
        return;
    }
    int ri = choose_run();
    if (ri == -1) return; /* no available run (shouldn't happen) */
    /* iterate unique words of matching length */
    int len = runs[ri].len;
    for (int u = 0; u < uniq_count; ++u) {
        if (uniq_cnt[u] <= 0) continue;
        if ((int)strlen(uniq[u]) != len) continue;
        if (!fits_run(ri, uniq[u])) continue;
        /* place */
        int written[MAX_WORD_LEN] = {0};
        place_word(ri, uniq[u], written);
        uniq_cnt[u]--;
        runs[ri].used = 1;
        solve_rec(depth + 1);
        runs[ri].used = 0;
        uniq_cnt[u]++;
        remove_word(ri, written);
    }
}

int main(void) {
    char line[256];
    char *top = NULL;
    char *bot = NULL;
    char *lines[MAX_LINES];
    int lines_n = 0;

    /* Read all input lines */
    while (fgets(line, sizeof(line), stdin)) {
        chomp(line);
        if (lines_n >= MAX_LINES) break;
        lines[lines_n] = strdup(line);
        lines_n++;
    }
    if (lines_n == 0) {
        printf("Enter the crossword:\nInvalid input.\n");
        return 0;
    }

    int idx = 0;
    /* find first non-empty line (support some stray blank lines) */
    while (idx < lines_n && strlen(lines[idx]) == 0) idx++;
    if (idx >= lines_n) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
    top = lines[idx++];
    if (!is_border(top)) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
    int toplen = strlen(top);
    if (toplen - 2 > MAX_INSIDE) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }

    /* read interior lines until bottom border found */
    int interior_start = idx;
    int interior_count = 0;
    while (idx < lines_n) {
        char *s = lines[idx++];
        if (is_border(s)) { bot = s; break; }
        /* interior line validation */
        if ((int)strlen(s) != toplen) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
        if (s[0] != '|' || s[toplen-1] != '|') { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
        for (int i = 1; i < toplen-1; ++i) if (s[i] != ' ' && s[i] != '*') { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
        interior_count++;
        if (interior_count > MAX_INSIDE) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
    }
    if (!bot) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
    /* bottom border must be same length and valid already checked */
    /* compute W and H inside */
    W = toplen - 2; H = interior_count;
    if (W <= 0 || H <= 0 || W > MAX_INSIDE || H > MAX_INSIDE) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }

    /* fill original grid */
    for (int r = 0; r < H; ++r) {
        char *s = lines[interior_start + r];
        for (int c = 0; c < W; ++c) original[r][c] = s[c+1];
    }

    /* initialize working grid with spaces for letters, stars preserved */
    for (int r = 0; r < H; ++r) for (int c = 0; c < W; ++c) {
        if (original[r][c] == '*') grid[r][c] = '*'; else grid[r][c] = ' ';
    }

    /* read words from remaining lines (skip blank lines) */
    char words_all[MAX_WORDS*2][MAX_WORD_LEN+1];
    int words_n = 0;
    while (idx < lines_n) {
        char *s = lines[idx++];
        if (strlen(s) == 0) continue;
        if ((int)strlen(s) > MAX_WORD_LEN) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
        for (int i = 0; s[i]; ++i) if (s[i] < 'a' || s[i] > 'z') { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
        if (words_n >= MAX_WORDS) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
        strcpy(words_all[words_n++], s);
    }

    /* Build unique words with counts */
    uniq_count = 0;
    for (int i = 0; i < words_n; ++i) {
        int found = -1;
        for (int j = 0; j < uniq_count; ++j) if (strcmp(uniq[j], words_all[i]) == 0) { found = j; break; }
        if (found >= 0) uniq_cnt[found]++; else { strcpy(uniq[uniq_count], words_all[i]); uniq_cnt[uniq_count] = 1; uniq_count++; }
    }

    /* extract runs */
    extract_runs();
    total_runs = runs_n;

    /* count runs by length and words by length */
    int run_by_len[MAX_WORD_LEN+1] = {0};
    int word_by_len[MAX_WORD_LEN+1] = {0};
    for (int i = 0; i < runs_n; ++i) {
        int L = runs[i].len;
        if (L < 1 || L > MAX_WORD_LEN) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
        run_by_len[L]++;
    }
    for (int i = 0; i < uniq_count; ++i) {
        int L = strlen(uniq[i]);
        if (L < 1 || L > MAX_WORD_LEN) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }
        word_by_len[L] += uniq_cnt[i];
    }
    for (int L = 1; L <= MAX_WORD_LEN; ++L) if (run_by_len[L] != word_by_len[L]) { printf("Enter the crossword:\nInvalid input.\n"); return 0; }

    /* replace original '*'s with '*' in grid and spaces with ' ' already set */
    for (int r = 0; r < H; ++r) for (int c = 0; c < W; ++c) {
        if (original[r][c] == '*') grid[r][c] = '*'; else grid[r][c] = ' ';
    }

    printf("Enter the crossword:\n");

    /* prepare runs used flags */
    for (int i = 0; i < runs_n; ++i) runs[i].used = 0;

    /* run solver */
    solution_count = 0;
    solve_rec(0);

    if (solution_count == 0) {
        printf("No solution.\n");
    } else if (solution_count == 1) {
        printf("There is one solution of the crossword:\n");
        /* print bordered solution */
        /* top border */
        putchar('+'); for (int i = 0; i < W; ++i) putchar('-'); putchar('+'); putchar('\n');
        for (int r = 0; r < H; ++r) {
            putchar('|');
            for (int c = 0; c < W; ++c) putchar(solution_board[r][c]);
            putchar('|'); putchar('\n');
        }
        putchar('+'); for (int i = 0; i < W; ++i) putchar('-'); putchar('+'); putchar('\n');
    } else {
        printf("Unique solutions: %lld\n", solution_count);
    }

    return 0;
}
