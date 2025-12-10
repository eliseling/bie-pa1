#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_RESULTS 50

typedef struct {
    double frequency;
    char *sequence;
} DNASample;

typedef struct {
    DNASample **samples;
    size_t count;
} SearchResults;

int cmp_frequency_desc(const void *a, const void *b) {
    DNASample *sa = *(DNASample **)a;
    DNASample *sb = *(DNASample **)b;
    if (sa->frequency < sb->frequency) return 1;
    if (sa->frequency > sb->frequency) return -1;
    return 0;
}

int is_valid_dna(const char *s) {
    size_t len = strlen(s);
    if (len % 3 != 0) return 0;
    for (size_t i = 0; i < len; i++) {
        if (s[i] != 'A' && s[i] != 'T' && s[i] != 'C' && s[i] != 'G') return 0;
    }
    return 1;
}

int is_valid_frequency(const char *s, double *freq) {
    char *endptr;
    *freq = strtod(s, &endptr);
    if (endptr == s || *endptr != '\0') return 0;
    return 1;
}

int main(void) {
    // Print only the first header at the very start
    printf("DNA database:\n");

    DNASample *samples = NULL;
    size_t sample_count = 0;
    size_t sample_capacity = 0;

    char *line = NULL;
    size_t len = 0;

    // --- Read DNA database ---
    while (getline(&line, &len, stdin) != -1) {
        if (line[0] == '\n') break; // empty line ends database

        char *colon = strchr(line, ':');
        if (!colon) {
            printf("Invalid input.\n");
            free(line);
            free(samples);
            return 0;
        }

        *colon = '\0';
        char *freq_str = line;
        char *dna_str = colon + 1;

        // Remove trailing newline
        size_t dna_len = strlen(dna_str);
        if (dna_len > 0 && dna_str[dna_len - 1] == '\n') dna_str[dna_len - 1] = '\0';

        double freq;
        if (!is_valid_frequency(freq_str, &freq)) {
            printf("Invalid input.\n");
            free(line);
            free(samples);
            return 0;
        }

        if (!is_valid_dna(dna_str)) {
            printf("Invalid input.\n");
            free(line);
            free(samples);
            return 0;
        }

        if (sample_count >= sample_capacity) {
            sample_capacity = sample_capacity ? sample_capacity * 2 : 16;
            samples = (DNASample*)realloc(samples, sample_capacity * sizeof(DNASample));
            if (!samples) exit(1);
        }

        samples[sample_count].frequency = freq;
        samples[sample_count].sequence = strdup(dna_str);
        sample_count++;
    }

    if (sample_count == 0) {
        printf("Invalid input.\n");
        free(line);
        free(samples);
        return 0;
    }

    // Only print 'Searches:' if the database is valid
    printf("Searches:\n");

    // --- Process search strings ---
    while (getline(&line, &len, stdin) != -1) {
        // Remove trailing newline
        size_t search_len = strlen(line);
        if (search_len > 0 && line[search_len - 1] == '\n') line[search_len - 1] = '\0';
        search_len = strlen(line);

        if (search_len == 0) continue;
        if (!is_valid_dna(line)) {
            printf("Invalid input.\n");
            break;
        }

        SearchResults results = {(DNASample**)malloc(sample_count * sizeof(DNASample *)), 0};
        if (!results.samples) {
            // Allocation failed, clean up and exit
            for (size_t i = 0; i < sample_count; i++) free(samples[i].sequence);
            free(samples);
            free(line);
            return 1;
        }

        // Search for matches on triplet boundaries
        for (size_t i = 0; i < sample_count; i++) {
            char *seq = samples[i].sequence;
            size_t seq_len = strlen(seq);

            for (size_t pos = 0; pos + search_len <= seq_len; pos += 3) {
                if (strncmp(seq + pos, line, search_len) == 0) {
                    results.samples[results.count++] = &samples[i];
                    break; // match found, go to next sample
                }
            }
        }

        // Sort by frequency descending
        qsort(results.samples, results.count, sizeof(DNASample *), cmp_frequency_desc);

        printf("Found: %zu\n", results.count);
        for (size_t i = 0; i < results.count && i < MAX_RESULTS; i++) {
            printf("> %s\n", results.samples[i]->sequence);
        }

        free(results.samples);
    }

    // --- Cleanup ---
    for (size_t i = 0; i < sample_count; i++) free(samples[i].sequence);
    free(samples);
    free(line);

    return 0;
}