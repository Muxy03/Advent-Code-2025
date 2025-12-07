#ifndef NOB_H
#define NOB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#ifdef __cplusplus
	#define NOB_DECLTYPE_CAST(T) (decltype(T))
#else
 	#define NOB_DECLTYPE_CAST(T)
#endif  // __cplusplus

// --- Dynamic array reserve ---
#define da_reserve(da, expected_capacity)                                                   \
     do {                                                                                   \
         if ((expected_capacity) > (da)->capacity) {                                        \
             if ((da)->capacity == 0) {                                                     \
                 (da)->capacity = 256;                                                      \
             }                                                                              \
             while ((expected_capacity) > (da)->capacity) {                                 \
                 (da)->capacity *= 2;                                                       \
             }                                                                              \
             (da)->items = NOB_DECLTYPE_CAST((da)->items)                                   \
                 realloc((da)->items, (da)->capacity * sizeof(*(da)->items));               \
             assert((da)->items != NULL && "Out of memory");                                \
         }                                                                                  \
     } while (0)

// --- Dynamic array append ---
#define da_append(da, item)                     \
     do {                                       \
         da_reserve((da), (da)->count + 1);     \
         (da)->items[(da)->count++] = (item);   \
     } while (0)

#define da_free(da)                \
    do {                            \
        free((da)->items);          \
        (da)->items = NULL;         \
        (da)->count = 0;            \
        (da)->capacity = 0;         \
    } while(0)

#define da_free_deep(da)                                \
    do {                                                \
        if ((da)->items) {                              \
            for (size_t i = 0; i < (da)->count; ++i)   \
                free((da)->items[i]);                  \
            free((da)->items);                          \
        }                                               \
        (da)->items = NULL;                             \
        (da)->count = 0;                                \
        (da)->capacity = 0;                             \
    } while(0)

#define da_append_many(da, src, n)                            \
    do {                                                      \
        size_t _n = (n);                                      \
        if (_n > 0) {                                         \
            if ((da)->count + _n > (da)->capacity) {          \
                size_t _newcap = (da)->capacity ? (da)->capacity : 256; \
                while (_newcap < (da)->count + _n) _newcap *= 2;          \
                (da)->items = realloc((da)->items, _newcap);  \
                if (!(da)->items) { perror("realloc"); exit(1); }         \
                (da)->capacity = _newcap;                     \
            }                                                 \
            memcpy((da)->items + (da)->count, (src), _n);     \
            (da)->count += _n;                                \
        }                                                     \
    } while(0)

// --- Types ---
#define ll long long           // typedef long long ll;
#define uch unsigned char      // typedef unsigned char uch;
#define ull unsigned long long // typedef unsigned long long ull;
#define string char*           // typedef char* string;

typedef struct {
    string items;
    size_t count;
    size_t capacity;
} DA;

// --- Functions --
static void die(const char *msg) {
    if (msg && *msg) {
        perror(msg);
    } else {
        fprintf(stderr, "Fatal error\n");
    }
    exit(EXIT_FAILURE);
}

static FILE* open_input_or_die(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Error: missing input file.\nUsage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) die("Error opening input file");

    return f;
}

static ssize_t getline_or_die(char **lineptr, size_t *n, FILE *stream) {
    ssize_t len = getline(lineptr, n, stream);
    if (len == -1) {
        if (feof(stream)) return -1;
        die("getline failed");
    }
    return len;
}

#endif // NOB_H
