#include "../nob.h"
#include <stdint.h>

typedef struct {
    int64_t num, den;
} Rat;

// ============== Part 1: XOR Toggle (Optimized) ==============

static inline uint64_t extract_lights(const char *line) {
    const char *start = strchr(line, '[');
    if (!start) return 0;
    start++;
    const char *end = strchr(start, ']');
    if (!end) return 0;
    
    uint64_t target = 0;
    for (size_t i = 0; i < (size_t)(end - start); i++) {
        if (start[i] == '#') target |= (1UL << i);
    }
    return target;
}

static inline int extract_buttons_inline(const char *line, uint64_t *buttons) {
    int button_count = 0;
    const char *cursor = line;
    
    while ((cursor = strchr(cursor, '(')) != NULL && button_count < 64) {
        cursor++;
        const char *end = strchr(cursor, ')');
        if (!end) break;
        
        uint64_t btn = 0;
        int idx = 0;
        while (cursor < end) {
            if (isdigit(*cursor)) {
                idx = idx * 10 + (*cursor++ - '0');
            } else if (*cursor == ',') {
                if (idx < 64) btn |= (1UL << idx);
                idx = 0;
                cursor++;
            } else {
                cursor++;
            }
        }
        if (idx < 64) btn |= (1UL << idx);
        buttons[button_count++] = btn;
        cursor = end + 1;
    }
    return button_count;
}

int solve_part1(const char *line) {
    uint64_t target = extract_lights(line);
    uint64_t buttons[64];
    
    int button_count = extract_buttons_inline(line, buttons);
    if (button_count == 0) return -1;
    
    size_t min_presses = SIZE_MAX;
    int search_limit = button_count > 20 ? (1 << 20) : (1 << button_count);
    
    for (int combo = 0; combo < search_limit; combo++) {
        uint64_t state = 0;
        for (int i = 0; i < button_count; i++) {
            if ((combo >> i) & 1) state ^= buttons[i];
        }
        
        if (state == target) {
            size_t presses = __builtin_popcountll(combo);
            if (presses < min_presses) min_presses = presses;
        }
    }
    
    return (min_presses == SIZE_MAX) ? -1 : (int)min_presses;
}

// ============== Part 2: Rational Arithmetic (Optimized) ==============

static inline int64_t gcd(int64_t a, int64_t b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) {
        int64_t tmp = b;
        b = a % b;
        a = tmp;
    }
    return a;
}

static inline Rat rat_new(int64_t n, int64_t d) {
    if (d == 0) return (Rat){n, 1};
    int64_t g = gcd(n, d);
    if (d < 0) return (Rat){-n/g, -d/g};
    return (Rat){n/g, d/g};
}

static inline int rat_is_zero(Rat r) { return r.num == 0; }

static inline int rat_to_int(Rat r, int64_t *out) {
    if (r.den == 1) {
        *out = r.num;
        return 1;
    }
    if (r.num % r.den == 0) {
        *out = r.num / r.den;
        return 1;
    }
    return 0;
}

static inline Rat rat_add(Rat a, Rat b) {
    return rat_new(a.num * b.den + b.num * a.den, a.den * b.den);
}

static inline Rat rat_sub(Rat a, Rat b) {
    return rat_new(a.num * b.den - b.num * a.den, a.den * b.den);
}

static inline Rat rat_mul(Rat a, Rat b) {
    return rat_new(a.num * b.num, a.den * b.den);
}

static inline Rat rat_div(Rat a, Rat b) {
    return rat_new(a.num * b.den, a.den * b.num);
}

static inline Rat rat_neg(Rat r) { return (Rat){-r.num, r.den}; }

// ============== Part 2: Extract and Solve ==============

static inline int extract_targets_inline(const char *line, int64_t *targets) {
    const char *start = strchr(line, '{');
    if (!start) return 0;
    start++;
    const char *end = strchr(start, '}');
    if (!end) return 0;
    
    int count = 0;
    const char *p = start;
    while (p < end && count < 128) {
        if (isdigit(*p) || (*p == '-' && isdigit(p[1]))) {
            int64_t val = 0;
            int sign = 1;
            if (*p == '-') { sign = -1; p++; }
            while (p < end && isdigit(*p)) {
                val = val * 10 + (*p++ - '0');
            }
            targets[count++] = sign * val;
        } else {
            p++;
        }
    }
    return count;
}

static inline int extract_buttons_vectors_inline(const char *line, int buttons[64][128], int button_sizes[64]) {
    int button_count = 0;
    const char *cursor = line;
    
    while ((cursor = strchr(cursor, '(')) != NULL && button_count < 64) {
        cursor++;
        const char *end = strchr(cursor, ')');
        if (!end) break;
        
        int idx = 0;
        while (cursor < end && button_sizes[button_count] < 128) {
            if (isdigit(*cursor)) {
                idx = idx * 10 + (*cursor++ - '0');
            } else if (*cursor == ',') {
                buttons[button_count][button_sizes[button_count]++] = idx;
                idx = 0;
                cursor++;
            } else {
                cursor++;
            }
        }
        if (idx > 0) buttons[button_count][button_sizes[button_count]++] = idx;
        button_count++;
        cursor = end + 1;
    }
    return button_count;
}

static void search_recursive(int64_t *fv, int idx, int64_t bound, const Rat *part,
                            const Rat (*null_basis)[128], int null_rows, int n, size_t *best) {
    if (idx == null_rows) {
        int64_t total = 0;
        int valid = 1;
        for (int j = 0; j < n; j++) {
            Rat val = part[j];
            for (int i = 0; i < null_rows; i++) {
                Rat prod = rat_mul(null_basis[i][j], (Rat){fv[i], 1});
                val = rat_add(val, prod);
            }
            int64_t v;
            if (!rat_to_int(val, &v) || v < 0) {
                valid = 0;
                break;
            }
            total += v;
        }
        if (valid && (*best == 0 || (size_t)total < *best)) {
            *best = (size_t)total;
        }
        return;
    }
    
    int64_t search_bound = bound;
    if (null_rows > 3) search_bound = (bound < 100) ? bound : 100;
    
    for (int64_t v = -search_bound; v <= search_bound; v++) {
        fv[idx] = v;
        search_recursive(fv, idx + 1, bound, part, null_basis, null_rows, n, best);
    }
}

int solve_part2(const char *line) {
    int64_t targets[128];
    int target_count = extract_targets_inline(line, targets);
    if (target_count == 0) return -1;
    
    int buttons[64][128];
    int button_sizes[64] = {0};
    int button_count = extract_buttons_vectors_inline(line, buttons, button_sizes);
    
    if (button_count == 0) {
        for (int i = 0; i < target_count; i++) {
            if (targets[i] != 0) return -1;
        }
        return 0;
    }
    
    int m = target_count, n = button_count;
    
    // Allocate matrix on heap
    Rat *mat = calloc(m * (n + 1), sizeof(Rat));
    int *pivot_col = malloc(m * sizeof(int));
    
    for (int i = 0; i < m; i++) pivot_col[i] = -1;
    
    // Initialize matrix
    for (int i = 0; i < m; i++) {
        mat[i * (n + 1) + n] = rat_new(targets[i], 1);
    }
    for (int j = 0; j < n; j++) {
        for (int k = 0; k < button_sizes[j]; k++) {
            int ci = buttons[j][k];
            if (ci < m) mat[ci * (n + 1) + j] = rat_new(1, 1);
        }
    }
    
    // Gaussian elimination with inline access
    #define MAT(r, c) mat[(r) * (n + 1) + (c)]
    
    int row = 0;
    for (int col = 0; col < n && row < m; col++) {
        // Find pivot - early exit optimization
        int pr = -1;
        for (int r = row; r < m; r++) {
            if (!rat_is_zero(MAT(r, col))) {
                pr = r;
                break;
            }
        }
        if (pr == -1) continue;
        
        // Swap rows only if needed
        if (pr != row) {
            Rat *row_ptr = &mat[row * (n + 1)];
            Rat *pr_ptr = &mat[pr * (n + 1)];
            for (int c = 0; c <= n; c++) {
                Rat tmp = row_ptr[c];
                row_ptr[c] = pr_ptr[c];
                pr_ptr[c] = tmp;
            }
        }
        
        pivot_col[row] = col;
        Rat pivot = MAT(row, col);
        Rat *row_data = &mat[row * (n + 1)];
        
        // Normalize row - cache the pointer
        for (int c = 0; c <= n; c++) {
            row_data[c] = rat_div(row_data[c], pivot);
        }
        
        // Eliminate column - optimized loop
        for (int r = 0; r < m; r++) {
            if (r == row) continue;
            Rat f = MAT(r, col);
            if (rat_is_zero(f)) continue;
            
            Rat *r_data = &mat[r * (n + 1)];
            for (int c = 0; c <= n; c++) {
                r_data[c] = rat_sub(r_data[c], rat_mul(f, row_data[c]));
            }
        }
        row++;
    }
    
    // Check consistency
    for (int r = row; r < m; r++) {
        if (!rat_is_zero(MAT(r, n))) {
            free(mat);
            free(pivot_col);
            return -1;
        }
    }
    
    // Extract particular solution
    Rat particular[128] = {{0, 1}};
    for (int r = 0; r < row; r++) {
        if (pivot_col[r] != -1) {
            particular[pivot_col[r]] = MAT(r, n);
        }
    }
    
    // Find free variables
    int free_vars[128];
    int free_count = 0;
    for (int c = 0; c < n; c++) {
        int is_pivot = 0;
        for (int r = 0; r < row; r++) {
            if (pivot_col[r] == c) { is_pivot = 1; break; }
        }
        if (!is_pivot) free_vars[free_count++] = c;
    }
    
    // No free variables case
    if (free_count == 0) {
        int64_t total = 0;
        int valid = 1;
        for (int j = 0; j < n; j++) {
            int64_t v;
            if (!rat_to_int(particular[j], &v) || v < 0) { valid = 0; break; }
            total += v;
        }
        free(mat);
        free(pivot_col);
        return valid ? (int)total : -1;
    }
    
    // Build null basis on stack with optimized loop
    Rat null_basis[16][128];
    memset(null_basis, 0, sizeof(null_basis));
    
    for (int i = 0; i < free_count && i < 16; i++) {
        int fv_idx = free_vars[i];
        null_basis[i][fv_idx] = rat_new(1, 1);
        
        for (int r = 0; r < row; r++) {
            int pc = pivot_col[r];
            if (pc != -1) {
                null_basis[i][pc] = rat_neg(mat[r * (n + 1) + fv_idx]);
            }
        }
    }
    
    free(mat);
    free(pivot_col);
    
    // Determine search bound
    int64_t max_target = 0;
    for (int i = 0; i < target_count; i++) {
        if (targets[i] > max_target) max_target = targets[i];
    }
    
    int64_t bound = max_target > 1000 ? 1000 : max_target;
    if (bound < 1) bound = 100;
    
    int64_t fv[16] = {0};
    size_t best = 0;
    search_recursive(fv, 0, bound, particular, (const Rat (*)[128])null_basis, free_count, n, &best);
    
    return (best == 0) ? -1 : (int)best;
}

// ============== Main ==============

int main(int argc, char **argv) {
    FILE *input = open_input_or_die(argc, argv);
    
    ll part1 = 0, part2 = 0;
    string line = NULL;
    size_t size = 0;
    ssize_t read = 0;
    
    while ((read = getline_or_die(&line, &size, input)) != -1) {
        size_t len = strlen(line);
        while (len > 0 && isspace(line[len - 1])) line[--len] = '\0';
        
        int p = solve_part1(line);
        if (p >= 0) part1 += p;
        
        p = solve_part2(line);
        if (p >= 0) part2 += p;
    }
    
    free(line);
    fclose(input);
    printf("Part 1: %lld\n", part1);
    printf("Part 2: %lld\n", part2);
    
    return 0;
}
