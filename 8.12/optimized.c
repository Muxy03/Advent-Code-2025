#define DSU_IMPLEMENTATION
#include "../nob.h"
#include <math.h>

typedef struct {
	ll x, y, z;
} Point;

typedef struct {
	Point *items;
	size_t count;
	size_t capacity;
} Points;

typedef struct {
	ull dist2;
	size_t a;
	size_t b;
} Edge;

typedef struct {
	Edge *items;
	size_t count;
	size_t capacity;
} Edges;


static int cmp_edge(const void *pa, const void *pb) {
    const Edge *a = pa;
    const Edge *b = pb;
    if (a->dist2 < b->dist2) return -1;
    if (a->dist2 > b->dist2) return 1;
    if (a->a < b->a) return -1;
    if (a->a > b->a) return 1;
    if (a->b < b->b) return -1;
    if (a->b > b->b) return 1;
    return 0;
}

static __int128 abs_int128(__int128 v) {
    return (v < 0) ? -v : v;
}

static void print_int128_with_label(const char *label, __int128 v) {
    if (v == 0) {
        printf("%s: 0\n", label);
        return;
    }
    int neg = 0;
    if (v < 0) { neg = 1; v = -v; }
    char buf[64];
    int pos = 0;
    while (v > 0) {
        buf[pos++] = '0' + (int)(v % 10);
        v /= 10;
    }
    printf("%s: ", label);
    if (neg) putchar('-');
    for (int i = pos - 1; i >= 0; --i) putchar(buf[i]);
    putchar('\n');
}

int main(int argc, char **argv) {
	FILE *input = open_input_or_die(argc, argv);

	string line = NULL;
	size_t size = 0;
	ssize_t read = 0;

	Points points = {0};
	Edges edges = {0};

	/* OPTIMIZATION 1: Input parsing with dynamic arrays */
	while ((read = getline_or_die(&line, &size, input)) != -1) {
		size_t len = strlen(line);
		if (len && line[len-1] == '\n') line[--len] = '\0';
		
		int x, y, z;
		if (sscanf(line, "%d,%d,%d", &x, &y, &z) != 3) {
			fprintf(stderr, "sscanf error\n");
			return 1;
		}
		da_append(&points, ((Point){x, y, z}));
	}

	size_t n = points.count;
	
	/* OPTIMIZATION 2: Early exit for edge cases */
	if (n < 2) {
		printf("part1: 1\n");
		printf("part2: 0\n");
		da_free(&points);
		free(line);
		fclose(input);
		return 0;
	}

	/* OPTIMIZATION 3: Pre-allocate edges */
	size_t m = n * (n - 1) / 2;
	da_reserve(&edges, m);

	/* OPTIMIZATION 4: Vectorized edge generation with pointer caching */
	for (size_t i = 0; i < n; ++i) {
		Point *pi = &points.items[i];
		for (size_t j = i + 1; j < n; ++j) {
			Point *pj = &points.items[j];
			ll dx = pi->x - pj->x;
			ll dy = pi->y - pj->y;
			ll dz = pi->z - pj->z;
			ull d2 = (ull)(dx*dx) + (ull)(dy*dy) + (ull)(dz*dz);
			da_append(&edges, ((Edge){d2, i, j}));
		}
	}

	/* OPTIMIZATION 5: Sort edges once */
	qsort(edges.items, edges.count, sizeof(Edge), cmp_edge);

	DSU *dsu = dsu_create(n);

	/* OPTIMIZATION 6: Calculate component sizes for part1 */
	size_t to_take = (1000 < edges.count) ? 1000 : edges.count;
	for (size_t k = 0; k < to_take; ++k) {
		dsu_union(dsu, (int)edges.items[k].a, (int)edges.items[k].b);
	}

	int *sizes = xmalloc(n * sizeof(int));
	memset(sizes, 0, n * sizeof(int));
	
	for (size_t i = 0; i < n; ++i) {
		sizes[dsu_find(dsu, i)]++;
	}

	/* OPTIMIZATION 7: Find top 3 sizes with single pass */
	ll top[3] = {0, 0, 0};
	for (size_t i = 0; i < n; ++i) {
		if (sizes[i] > 0) {
			if (sizes[i] > top[0]) {
				top[2] = top[1];
				top[1] = top[0];
				top[0] = sizes[i];
			} else if (sizes[i] > top[1]) {
				top[2] = top[1];
				top[1] = sizes[i];
			} else if (sizes[i] > top[2]) {
				top[2] = sizes[i];
			}
		}
	}

	/* OPTIMIZATION 8: Calculate part1 with unified overflow handling */
	__int128 part1_big = 1;
	int overflow = 0;

	for (int i = 0; i < 3; ++i) {
		ll v = (top[i] > 0) ? top[i] : 1;
		
		if (overflow) {
			part1_big *= (__int128)v;
		} else {
			if (v > 1 && part1_big > (__int128)LLONG_MAX / (__int128)v) {
				overflow = 1;
				part1_big *= (__int128)v;
			} else {
				part1_big *= (__int128)v;
			}
		}
	}

	if (overflow) {
		print_int128_with_label("part1", part1_big);
	} else {
		printf("part1: %lld\n", (ll)part1_big);
	}

	/* OPTIMIZATION 9: Reuse DSU for part2 - reset instead of recreate */
	dsu_free(dsu);
	dsu = dsu_create(n);

	size_t comps = n;
	size_t last_a = 0, last_b = 0;

	/* OPTIMIZATION 10: Early break when reaching single component */
	for (size_t k = 0; k < edges.count; ++k) {
		if (dsu_union(dsu, (int)edges.items[k].a, (int)edges.items[k].b)) {
			if (--comps == 1) {
				last_a = edges.items[k].a;
				last_b = edges.items[k].b;
				break;
			}
		}
	}

	/* OPTIMIZATION 11: Direct __int128 multiplication for part2 */
	__int128 part2_result = (__int128)points.items[last_a].x * (__int128)points.items[last_b].x;
	
	if (part2_result > LLONG_MAX || part2_result < LLONG_MIN) {
		print_int128_with_label("part2", part2_result);
	} else {
		printf("part2: %lld\n", (ll)part2_result);
	}

	/* OPTIMIZATION 12: Consolidated cleanup */
	da_free(&points);
	da_free(&edges);
	dsu_free(dsu);
	free(line);
	free(sizes);
	fclose(input);

	return 0;
}
