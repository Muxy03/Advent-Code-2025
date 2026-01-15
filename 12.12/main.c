#include "../nob.h"
#include <stdint.h>

typedef struct {
    int data[3][3];
    int h, w;
} Shape;

typedef struct {
    Shape *items;
    size_t count, capacity;
} Shapes;

typedef struct {
    uint32_t *items;
    size_t count, capacity;
} Data;

typedef struct {
    uint32_t w, h;
    Data needed;
} Region;

typedef struct {
    Region *items;
    size_t count, capacity;
} Regions;

// Get all 8 rotations/flips of a shape
void get_transformations(Shape s, Shape *transforms, int *count) {
    *count = 0;
    Shape seen[8];
    int seen_count = 0;
    
    // Original
    Shape current = s;
    
    for (int flip = 0; flip < 2; flip++) {
        for (int rot = 0; rot < 4; rot++) {
            // Check if we've seen this
            bool found = false;
            for (int i = 0; i < seen_count; i++) {
                bool same = true;
                for (int r = 0; r < 3 && same; r++)
                    for (int c = 0; c < 3 && same; c++)
                        if (seen[i].data[r][c] != current.data[r][c])
                            same = false;
                if (same && seen[i].h == current.h && seen[i].w == current.w) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                transforms[*count] = current;
                seen[seen_count++] = current;
                (*count)++;
            }
            
            // Rotate 90 degrees clockwise
            Shape rotated;
            rotated.h = current.w;
            rotated.w = current.h;
            for (int r = 0; r < 3; r++)
                for (int c = 0; c < 3; c++)
                    rotated.data[r][c] = current.data[2-c][r];
            current = rotated;
        }
        
        // Flip horizontally
        Shape flipped;
        flipped.h = current.h;
        flipped.w = current.w;
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                flipped.data[r][c] = current.data[r][2-c];
        current = flipped;
    }
}

bool try_place(char *grid, int gw, int gh, Shape s, int sx, int sy) {
    for (int r = 0; r < s.h; r++) {
        for (int c = 0; c < s.w; c++) {
            if (s.data[r][c] == '#') {
                int x = sx + c;
                int y = sy + r;
                if (x < 0 || x >= gw || y < 0 || y >= gh)
                    return false;
                if (grid[y * gw + x] != '.')
                    return false;
            }
        }
    }
    return true;
}

void place(char *grid, int gw, int gh, Shape s, int sx, int sy, char ch) {
    for (int r = 0; r < s.h; r++) {
        for (int c = 0; c < s.w; c++) {
            if (s.data[r][c] == '#') {
                int x = sx + c;
                int y = sy + r;
                grid[y * gw + x] = ch;
            }
        }
    }
}

void unplace(char *grid, int gw, int gh, Shape s, int sx, int sy) {
    for (int r = 0; r < s.h; r++) {
        for (int c = 0; c < s.w; c++) {
            if (s.data[r][c] == '#') {
                int x = sx + c;
                int y = sy + r;
                grid[y * gw + x] = '.';
            }
        }
    }
}

bool backtrack(char *grid, int gw, int gh, Shapes *shapes, 
               uint32_t *needed, int shape_idx) {
    // Find first shape that still needs to be placed
    while (shape_idx < shapes->count && needed[shape_idx] == 0)
        shape_idx++;
    
    if (shape_idx >= shapes->count)
        return true; // All shapes placed!
    
    needed[shape_idx]--;
    
    Shape transforms[8];
    int trans_count;
    get_transformations(shapes->items[shape_idx], transforms, &trans_count);
    
    for (int y = 0; y < gh; y++) {
        for (int x = 0; x < gw; x++) {
            for (int t = 0; t < trans_count; t++) {
                if (try_place(grid, gw, gh, transforms[t], x, y)) {
                    place(grid, gw, gh, transforms[t], x, y, '#');
                    
                    if (backtrack(grid, gw, gh, shapes, needed, shape_idx + 1))
                        return true;
                    
                    unplace(grid, gw, gh, transforms[t], x, y);
                }
            }
        }
    }
    
    needed[shape_idx]++;
    return false;
}

bool can_fit_region(Region *r, Shapes *shapes) {
    char *grid = malloc(r->w * r->h);
    memset(grid, '.', r->w * r->h);
    
    uint32_t *needed = malloc(shapes->count * sizeof(uint32_t));
    memcpy(needed, r->needed.items, r->needed.count * sizeof(uint32_t));
    
    bool result = backtrack(grid, r->w, r->h, shapes, needed, 0);
    
    free(grid);
    free(needed);
    return result;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("fopen");
        return 1;
    }
    
    Shapes shapes = {0};
    Regions regions = {0};
    
    char *line = NULL;
    size_t line_cap = 0;
    ssize_t line_len;
    
    Shape current_shape = {0};
    int shape_line = 0;
    
    while ((line_len = getline(&line, &line_cap, input)) != -1) {
        if (line_len > 0 && line[line_len-1] == '\n')
            line[--line_len] = '\0';
        
        if (line_len == 0)
            continue;
        
        // Check if it's a shape header (e.g., "0:")
        if (line_len == 2 && isdigit(line[0]) && line[1] == ':') {
            memset(current_shape.data, '.', sizeof(current_shape.data));
            current_shape.h = 3;
            current_shape.w = 3;
            shape_line = 0;
        }
        // Check if it's a shape line
        else if (line_len == 3 && (line[0] == '#' || line[0] == '.')) {
            for (int i = 0; i < 3; i++)
                current_shape.data[shape_line][i] = line[i];
            shape_line++;
            
            if (shape_line == 3) {
                da_append(&shapes, current_shape);
            }
        }
        // Check if it's a region line (contains 'x')
        else if (strchr(line, 'x')) {
            Region r = {0};
            sscanf(line, "%ux%u", &r.w, &r.h);
            
            char *p = line;
            while (*p && !isdigit(*p)) p++;
            
            while (*p) {
                if (isdigit(*p)) {
                    uint32_t val = strtoul(p, &p, 10);
                    da_append(&r.needed, val);
                } else {
                    p++;
                }
            }
            
            da_append(&regions, r);
        }
    }
    
    fclose(input);
    
    // Solve
    uint32_t part1 = 0;
    
    for (size_t i = 0; i < regions.count; i++) {
        if (can_fit_region(&regions.items[i], &shapes)) {
            part1++;
        }
    }
    
    printf("Part 1: %u\n", part1);
    
    // Cleanup
    for (size_t i = 0; i < regions.count; i++) {
        free(regions.items[i].needed.items);
    }
    free(regions.items);
    free(shapes.items);
    free(line);
    
    return 0;
}
