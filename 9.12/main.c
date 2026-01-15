#include "../nob.h"

typedef struct {
    ll x, y;
} Point;

typedef struct {
    Point *items;
    size_t count;
    size_t capacity;
} Points;

typedef struct {
    ll xmin, xmax, ymin, ymax;
} Rect;

typedef struct {
    Point *items;
    size_t count;
    ll min_x, max_x, min_y, max_y;
} PolyData;

static PolyData poly_preprocess(Point *poly, size_t n) {
    PolyData pd = {.items = poly, .count = n};
    pd.min_x = pd.max_x = poly[0].x;
    pd.min_y = pd.max_y = poly[0].y;
    
    for (size_t i = 1; i < n; ++i) {
        if (poly[i].x < pd.min_x) pd.min_x = poly[i].x;
        if (poly[i].x > pd.max_x) pd.max_x = poly[i].x;
        if (poly[i].y < pd.min_y) pd.min_y = poly[i].y;
        if (poly[i].y > pd.max_y) pd.max_y = poly[i].y;
    }
    return pd;
}

static inline int point_in_poly_fast(const PolyData *pd, Point p) {
    if (p.x < pd->min_x || p.x > pd->max_x || p.y < pd->min_y || p.y > pd->max_y) {
        return 0;
    }
    
    int inside = 0;
    const Point *poly = pd->items;
    size_t n = pd->count;
    
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        ll xi = poly[i].x, yi = poly[i].y;
        ll xj = poly[j].x, yj = poly[j].y;
        
        if (((yi > p.y) != (yj > p.y)) &&
            (p.x < (xj - xi) * (p.y - yi) / (double)(yj - yi) + xi)) {
            inside = !inside;
        }
    }
    return inside;
}

static int rect_edges_inside_poly_fast(const PolyData *pd, Rect r) {
    if (r.xmax < pd->min_x || r.xmin > pd->max_x ||
        r.ymax < pd->min_y || r.ymin > pd->max_y) {
        return 0;
    }
    
    if (!point_in_poly_fast(pd, (Point){r.xmin, r.ymin})) return 0;
    if (!point_in_poly_fast(pd, (Point){r.xmax, r.ymin})) return 0;
    if (!point_in_poly_fast(pd, (Point){r.xmin, r.ymax})) return 0;
    if (!point_in_poly_fast(pd, (Point){r.xmax, r.ymax})) return 0;
    
    for (ll y = r.ymin; y <= r.ymax; ++y) {
        if (!point_in_poly_fast(pd, (Point){r.xmin, y})) return 0;
        if (!point_in_poly_fast(pd, (Point){r.xmax, y})) return 0;
    }
    
    for (ll x = r.xmin; x <= r.xmax; ++x) {
        if (!point_in_poly_fast(pd, (Point){x, r.ymin})) return 0;
        if (!point_in_poly_fast(pd, (Point){x, r.ymax})) return 0;
    }
    
    return 1;
}

static inline ll rect_area(Rect r) {
    return (r.xmax - r.xmin + 1) * (r.ymax - r.ymin + 1);
}

static inline Rect points_to_rect(Point a, Point b) {
    return (Rect){
        .xmin = a.x < b.x ? a.x : b.x,
        .xmax = a.x > b.x ? a.x : b.x,
        .ymin = a.y < b.y ? a.y : b.y,
        .ymax = a.y > b.y ? a.y : b.y
    };
}

static int cmp_area_desc(const void *a, const void *b) {
    Point *pa = (Point *)a;
    Point *pb = (Point *)b;
    ll area_a = llabs(pa->x) + llabs(pa->y);
    ll area_b = llabs(pb->x) + llabs(pb->y);
    return (area_a < area_b) - (area_a > area_b);
}

int main(int argc, string *argv) {
    FILE *input = open_input_or_die(argc, argv);
    
    ll part1 = 0, part2 = 0;
    string line = NULL;
    size_t size = 0;
    ssize_t read = 0;
    Points points = {0};

    while ((read = getline_or_die(&line, &size, input)) != -1) {
        if (read && line[read - 1] == '\n') line[--read] = '\0';
        
        ll x, y;
        if (sscanf(line, "%lld,%lld", &x, &y) != 2) {
            fprintf(stderr, "Parse error\n");
            return 1;
        }
        
        da_append(&points, ((Point){x, y}));
    }

    size_t n = points.count;
    
    if (n < 2) {
    	part1 = 1;
    	goto end;
    }

    for (size_t i = 0; i < n; ++i) {
        Point a = points.items[i];
        for (size_t j = i + 1; j < n; ++j) {
            Point b = points.items[j];
            ll curr_area = (llabs(a.x - b.x) + 1) * (llabs(a.y - b.y) + 1);
            if (curr_area > part1) part1 = curr_area;
        }
    }

    PolyData poly_data = poly_preprocess(points.items, n);

    for (size_t i = 0; i < n; ++i) {
        Point a = points.items[i];
        for (size_t j = i + 1; j < n; ++j) {
            Point b = points.items[j];
            Rect r = points_to_rect(a, b);
            ll curr_area = rect_area(r);
            
            if (curr_area > part2 && rect_edges_inside_poly_fast(&poly_data, r)) {
                part2 = curr_area;
            }
        }
    }

end:
    printf("part1: %lld\n", part1);
    printf("part2: %lld\n", part2);

    da_free(&points);
    free(line);
    fclose(input);

    return 0;
}
