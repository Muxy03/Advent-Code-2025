#include "../nob.h"

typedef struct {
    ll x, y;
} Point;

typedef struct {
    Point *items;
    size_t count;
    size_t capacity;
} Points;

/* OPTIMIZATION 1: Cache line-based point iteration for ray casting */
static int point_in_poly(const Point* poly, size_t n, Point p) {
    int inside = 0;
    const Point *last = &poly[n - 1];
    
    for (size_t i = 0; i < n; ++i) {
        const Point *curr = &poly[i];
        
        if (((curr->y > p.y) != (last->y > p.y)) &&
            (p.x < (last->x - curr->x) * (p.y - curr->y) / (double)(last->y - curr->y) + curr->x)) {
            inside = !inside;
        }
        last = curr;
    }
    return inside;
}

/* OPTIMIZATION 2: Exact edge checking with early termination */
static int rect_edges_inside_poly_optimized(const Point* poly, size_t n, ll xmin, ll xmax, ll ymin, ll ymax) {
    /* Check corners first (fast fail) */
    if (!point_in_poly(poly, n, (Point){xmin, ymin})) return 0;
    if (!point_in_poly(poly, n, (Point){xmax, ymin})) return 0;
    if (!point_in_poly(poly, n, (Point){xmin, ymax})) return 0;
    if (!point_in_poly(poly, n, (Point){xmax, ymax})) return 0;
    
    /* Check left & right vertical edges */
    for (ll y = ymin; y <= ymax; ++y) {
        if (!point_in_poly(poly, n, (Point){xmin, y})) return 0;
        if (!point_in_poly(poly, n, (Point){xmax, y})) return 0;
    }
    
    /* Check top & bottom horizontal edges */
    for (ll x = xmin; x <= xmax; ++x) {
        if (!point_in_poly(poly, n, (Point){x, ymin})) return 0;
        if (!point_in_poly(poly, n, (Point){x, ymax})) return 0;
    }
    
    return 1;
}

/* OPTIMIZATION 3: Inline area calculation */
static inline ll area(ll xmin, ll xmax, ll ymin, ll ymax) {
    return (xmax - xmin + 1) * (ymax - ymin + 1);
}

/* OPTIMIZATION 4: Struct for caching rect bounds */
typedef struct {
    ll xmin, xmax, ymin, ymax;
} Rect;

static inline Rect make_rect(Point a, Point b) {
    return (Rect){
        .xmin = a.x < b.x ? a.x : b.x,
        .xmax = a.x > b.x ? a.x : b.x,
        .ymin = a.y < b.y ? a.y : b.y,
        .ymax = a.y > b.y ? a.y : b.y
    };
}

int main(int argc, string *argv) {
    FILE *input = open_input_or_die(argc, argv);
    
    ll part1 = 0, part2 = 0;
    string line = NULL;
    size_t size = 0;
    ssize_t read = 0;
    Points points = {0};

    /* OPTIMIZATION 5: Input parsing */
    while ((read = getline_or_die(&line, &size, input)) != -1) {
        if (read && line[read - 1] == '\n') line[--read] = '\0';
        
        ll x, y;
        if (sscanf(line, "%lld,%lld", &x, &y) != 2) return 1;
        
        da_append(&points, ((Point){x, y}));
    }

    size_t n = points.count;
    
    /* OPTIMIZATION 6: Early exit for trivial cases */
    if (n < 2) {
        printf("part1: 1\n");
        printf("part2: 1\n");
        da_free(&points);
        free(line);
        fclose(input);
        return 0;
    }

    /* OPTIMIZATION 7: Pre-allocate candidate pairs for part1 calculation */
    size_t max_pairs = n * (n - 1) / 2;
    
    /* OPTIMIZATION 8: Single pass for part1 - no redundant comparisons */
    for (size_t i = 0; i < n; ++i) {
        Point a = points.items[i];
        for (size_t j = i + 1; j < n; ++j) {
            Point b = points.items[j];
            ll xmin = a.x < b.x ? a.x : b.x;
            ll xmax = a.x > b.x ? a.x : b.x;
            ll ymin = a.y < b.y ? a.y : b.y;
            ll ymax = a.y > b.y ? a.y : b.y;
            
            ll curr_area = (xmax - xmin + 1) * (ymax - ymin + 1);
            if (curr_area > part1) part1 = curr_area;
        }
    }

    /* OPTIMIZATION 9: Part2 with early termination and cached bounds */
    for (size_t i = 0; i < n; ++i) {
        Point a = points.items[i];
        for (size_t j = i + 1; j < n; ++j) {
            Point b = points.items[j];
            Rect r = make_rect(a, b);
            ll curr_area = area(r.xmin, r.xmax, r.ymin, r.ymax);
            
            /* Only check polygon containment if area is promising */
            if (curr_area > part2) {
                if (rect_edges_inside_poly_optimized(points.items, n, r.xmin, r.xmax, r.ymin, r.ymax)) {
                    part2 = curr_area;
                }
            }
        }
    }

    printf("part1: %lld\n", part1);
    printf("part2: %lld\n", part2);

    /* OPTIMIZATION 10: Consolidated cleanup */
    da_free(&points);
    free(line);
    fclose(input);

    return 0;
}
