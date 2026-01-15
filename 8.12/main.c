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

	ll part1 = 0;
	ll part2 = 0;
	__int128 part2_big = 0;
	

	string line  = NULL;
	size_t size  = 0;
	ssize_t read = 0;

	Points points = {0};
	Edges edges = {0};

	while((read = getline_or_die(&line, &size, input))!= -1) {
		size_t len = strlen(line);
	       if(len && line[len-1]=='\n') line[--len]='\0';
	       int x = 0;
	       int y = 0;
	       int z = 0;
	       if(sscanf(line ,"%d,%d,%d", &x, &y, &z) != 3) {
			fprintf(stderr, "sscanf error\n");
	       	return 1;
	       }
	       Point p = {x, y, z};
	       da_append(&points, p);
    }

	size_t n = points.count;
    size_t m = n * (n - 1) / 2;

    if(m == 0) {
    	part1  = 1;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i+1; j < points.count; ++j) {
            ll dx = points.items[i].x - points.items[j].x;
            ll dy = points.items[i].y - points.items[j].y;
            ll dz = points.items[i].z - points.items[j].z;
            ull d2 = (ull)(dx*dx) + (ull)(dy*dy) + (ull)(dz*dz);

			Edge e = {d2, i, j};
            da_append(&edges, e);
        }
    }

    qsort(edges.items, m, sizeof(Edge), cmp_edge);

	DSU *dsu = dsu_create(n);
	
    size_t to_take = 1000;

    if (to_take > m) to_take = m;

    for (size_t k = 0; k < to_take; ++k) {
    	size_t a = edges.items[k].a;
    	size_t b = edges.items[k].b;
        dsu_union(dsu, a, b);
    }

    int *sizes = xmalloc(n * sizeof(int));
	memset(sizes, 0, n * sizeof(int));

    for (size_t i = 0; i < n; ++i) {
         int r = dsu_find(dsu, i);
         sizes[r]++;
     }

     int *nonzero = xmalloc(n * sizeof(int));
     memcpy(nonzero, sizes, n*sizeof(int));
     int cnt = 	n;
     // for (size_t i = 0; i < n; ++i) if (sizes[i] > 0) nonzero[cnt++] = sizes[i];

	for (int i = 0; i < cnt; ++i) {
    	int best = i;
        for (int j = i+1; j < cnt; ++j) if (nonzero[j] > nonzero[best]) best = j;
        if (best != i) { int t = nonzero[i]; nonzero[i] = nonzero[best]; nonzero[best] = t; }
    }

  long long p = 1;
   int overflow = 0;

   for (int i = 0; i < 3; i++) {
       long long v = (i < cnt) ? nonzero[i] : 1;

       if (!overflow) {
           if (v != 0 && llabs(p) > LLONG_MAX / llabs(v)) {
               overflow = 1;
           } else {
               p *= v;
           }
       }
   }

   if (!overflow) {
       part1 = p;
       goto part2;
   } else {
       __int128 big = 1;
       for (int i = 0; i < 3; i++) {
           long long v = (i < cnt) ? nonzero[i] : 1;
           big *= (__int128)v;
       }
       print_int128_with_label("part1", big);
   }

part2:
	
   	dsu_free(dsu);
   	dsu = dsu_create(n);

   	size_t comps2 = n;
   	size_t last_a = 0, last_b = 0;

   	for (size_t k = 0; k < edges.count; ++k) {
       	if (dsu_union(dsu, (int)edges.items[k].a, (int)edges.items[k].b)) {
           	comps2--;
           	if (comps2 == 1) { last_a = edges.items[k].a; last_b = edges.items[k].b; break; }
       	}
   	}

	part2 = points.items[last_a].x * points.items[last_b].x;

	if (llabs(points.items[last_a].x) != 0 && llabs(points.items[last_b].x) != 0 &&  llabs(points.items[last_a].x) > LLONG_MAX / llabs(points.items[last_b].x)) {
		part2_big = (__int128)points.items[last_a].x * (__int128)points.items[last_b].x;
	    print_int128_with_label("part2", part2_big);
	} else {
		goto end;
	}

end:
	printf("part1: %lld\n", part1);
	printf("part2: %lld\n", part2);
	
	da_free(&points);
	da_free(&edges);
	dsu_free(dsu);
	free(line);
	free(sizes); 
	free(nonzero);
	fclose(input);
				
	return 0;
}
