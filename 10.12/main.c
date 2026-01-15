#include "../nob.h"
#include <stdint.h>

typedef struct {
    uint64_t *items;
    size_t count;
    size_t capacity;
} U64Vec;

typedef struct {
    int64_t *items;
    size_t count;
    size_t capacity;
} I64Vec;

typedef struct {
    size_t *items;
    size_t count;
    size_t capacity;
} SizeVec;

typedef struct {
    SizeVec *items;
    size_t count;
    size_t capacity;
} SizeVecVec;

typedef struct {
    int64_t num, den;
} Rat;

typedef struct {
    Rat *data;
    size_t rows, cols;
} RatMatrix;

// ============== Part 1: XOR Toggle ==============

uint64_t extract_lights(const char *line) {
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

U64Vec extract_buttons(const char *line) {
    U64Vec buttons = {0};
    const char *cursor = line;
    
    while ((cursor = strchr(cursor, '(')) != NULL) {
        cursor++;
        const char *end = strchr(cursor, ')');
        if (!end) break;
        
        uint64_t btn = 0;
        char *nums = malloc((size_t)(end - cursor) + 1);
        strncpy(nums, cursor, (size_t)(end - cursor));
        nums[end - cursor] = '\0';
        
        char *saveptr = NULL;
        for (char *tok = strtok_r(nums, ",", &saveptr); tok; tok = strtok_r(NULL, ",", &saveptr)) {
            char *trim = tok;
            while (isspace(*trim)) trim++;
            int idx = atoi(trim);
            btn |= (1UL << idx);
        }
        free(nums);
        
        da_append(&buttons, btn);
        cursor = end + 1;
    }
    return buttons;
}

size_t popcount(uint64_t x) {
    return __builtin_popcountll(x);
}

int solve_part1(const char *line) {
    uint64_t target = extract_lights(line);
    U64Vec buttons = extract_buttons(line);
    
    if (buttons.count == 0) {
        da_free(&buttons);
        return -1;
    }
    
    size_t min_presses = SIZE_MAX;
    for (uint64_t combo = 0; combo < (1UL << buttons.count); combo++) {
        uint64_t state = 0;
        for (size_t i = 0; i < buttons.count; i++) {
            if ((combo >> i) & 1) state ^= buttons.items[i];
        }
        
        if (state == target) {
            size_t presses = popcount(combo);
            if (presses < min_presses) min_presses = presses;
        }
    }
    
    da_free(&buttons);
    return (min_presses == SIZE_MAX) ? -1 : (int)min_presses;
}

// ============== Part 2: Additive Counters ==============

int64_t gcd(int64_t a, int64_t b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

Rat rat_new(int64_t n, int64_t d) {
    if (d == 0) return (Rat){n, 1};
    int64_t g = gcd(labs(n), labs(d));
    if (d < 0) return (Rat){-n / g, -d / g};
    return (Rat){n / g, d / g};
}

Rat rat_zero(void) { return (Rat){0, 1}; }
Rat rat_int(int64_t v) { return (Rat){v, 1}; }
int rat_is_zero(Rat r) { return r.num == 0; }

int rat_to_int(Rat r, int64_t *out) {
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

Rat rat_add(Rat a, Rat b) {
    return rat_new(a.num * b.den + b.num * a.den, a.den * b.den);
}

Rat rat_sub(Rat a, Rat b) {
    return rat_new(a.num * b.den - b.num * a.den, a.den * b.den);
}

Rat rat_mul(Rat a, Rat b) {
    return rat_new(a.num * b.num, a.den * b.den);
}

Rat rat_div(Rat a, Rat b) {
    return rat_new(a.num * b.den, a.den * b.num);
}

Rat rat_neg(Rat r) { return (Rat){-r.num, r.den}; }

I64Vec extract_targets(const char *line) {
    I64Vec targets = {0};
    const char *start = strchr(line, '{');
    if (!start) return targets;
    start++;
    const char *end = strchr(start, '}');
    if (!end) return targets;
    
    char *nums = malloc((size_t)(end - start) + 1);
    strncpy(nums, start, (size_t)(end - start));
    nums[end - start] = '\0';
    
    char *saveptr = NULL;
    for (char *tok = strtok_r(nums, ",", &saveptr); tok; tok = strtok_r(NULL, ",", &saveptr)) {
        char *trim = tok;
        while (isspace(*trim)) trim++;
        da_append(&targets, atoll(trim));
    }
    free(nums);
    
    return targets;
}

SizeVecVec extract_button_vectors(const char *line) {
    SizeVecVec buttons = {0};
    const char *cursor = line;
    
    while ((cursor = strchr(cursor, '(')) != NULL) {
        cursor++;
        const char *end = strchr(cursor, ')');
        if (!end) break;
        
        SizeVec btn = {0};
        char *nums = malloc((size_t)(end - cursor) + 1);
        strncpy(nums, cursor, (size_t)(end - cursor));
        nums[end - cursor] = '\0';
        
        char *saveptr = NULL;
        for (char *tok = strtok_r(nums, ",", &saveptr); tok; tok = strtok_r(NULL, ",", &saveptr)) {
            char *trim = tok;
            while (isspace(*trim)) trim++;
            da_append(&btn, (size_t)atoll(trim));
        }
        free(nums);
        
        da_append(&buttons, btn);
        cursor = end + 1;
    }
    return buttons;
}


RatMatrix mat_new(size_t rows, size_t cols) {
    Rat *data = malloc(rows * cols * sizeof(Rat));
    for (size_t i = 0; i < rows * cols; i++) data[i] = rat_zero();
    return (RatMatrix){data, rows, cols};
}

Rat mat_get(RatMatrix m, size_t r, size_t c) { return m.data[r * m.cols + c]; }
void mat_set(RatMatrix m, size_t r, size_t c, Rat val) { m.data[r * m.cols + c] = val; }
void mat_free(RatMatrix m) { free(m.data); }

void search(int64_t *fv, size_t idx, int64_t bound, const Rat *part, const Rat **null_basis,
            size_t null_rows, size_t n, size_t *best) {
    if (idx == null_rows) {
        int64_t total = 0;
        int valid = 1;
        for (size_t j = 0; j < n; j++) {
            Rat val = part[j];
            for (size_t i = 0; i < null_rows; i++) {
                val = rat_add(val, rat_mul(null_basis[i][j], rat_int(fv[i])));
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
        search(fv, idx + 1, bound, part, null_basis, null_rows, n, best);
    }
}

int solve_part2(const char *line) {
    I64Vec targets = extract_targets(line);
    SizeVecVec buttons = extract_button_vectors(line);
    
    size_t m = targets.count, n = buttons.count;
    if (n == 0) {
        int result = 1;
        for (size_t i = 0; i < m; i++) {
            if (targets.items[i] != 0) { result = 0; break; }
        }
        da_free(&targets);
        da_free(&buttons);
        return result ? 0 : -1;
    }
    
    RatMatrix mat = mat_new(m, n + 1);
    for (size_t i = 0; i < m; i++) mat_set(mat, i, n, rat_int(targets.items[i]));
    for (size_t j = 0; j < n; j++) {
    	size_t limit = buttons.items[j].count;
        for (size_t k = 0; k < limit; k++) {
            size_t ci = buttons.items[j].items[k];
            if (ci < m) mat_set(mat, ci, j, rat_int(1));
        }
    }
    
    int *pivot_col = malloc(m * sizeof(int));
    for (size_t i = 0; i < m; i++) pivot_col[i] = -1;
    
    size_t row = 0;
    for (size_t col = 0; col < n && row < m; col++) {
        int pr = -1;
        for (size_t r = row; r < m; r++) {
            if (!rat_is_zero(mat_get(mat, r, col))) { pr = r; break; }
        }
        if (pr == -1) continue;
        
        if (pr != (int)row) {
            for (size_t c = 0; c <= n; c++) {
                Rat tmp = mat_get(mat, row, c);
                mat_set(mat, row, c, mat_get(mat, pr, c));
                mat_set(mat, pr, c, tmp);
            }
        }
        
        pivot_col[row] = col;
        Rat pivot = mat_get(mat, row, col);
        for (size_t c = 0; c <= n; c++) {
            mat_set(mat, row, c, rat_div(mat_get(mat, row, c), pivot));
        }
        
        for (size_t r = 0; r < m; r++) {
            if (r != row && !rat_is_zero(mat_get(mat, r, col))) {
                Rat f = mat_get(mat, r, col);
                for (size_t c = 0; c <= n; c++) {
                    mat_set(mat, r, c, rat_sub(mat_get(mat, r, c), rat_mul(f, mat_get(mat, row, c))));
                }
            }
        }
        row++;
    }
    
    for (size_t r = row; r < m; r++) {
        if (!rat_is_zero(mat_get(mat, r, n))) {
            free(pivot_col);
            mat_free(mat);
            da_free(&targets);
            da_free(&buttons);
            return -1;
        }
    }
    
    Rat *particular = malloc(n * sizeof(Rat));
    for (size_t i = 0; i < n; i++) particular[i] = rat_zero();
    for (size_t r = 0; r < row; r++) {
        if (pivot_col[r] != -1) {
            particular[pivot_col[r]] = mat_get(mat, r, n);
        }
    }
    
    SizeVec free_vars = {0};
    for (size_t c = 0; c < n; c++) {
        int is_pivot = 0;
        for (size_t r = 0; r < row; r++) {
            if (pivot_col[r] == (int)c) { is_pivot = 1; break; }
        }
        if (!is_pivot) da_append(&free_vars, c);
    }
    
    if (free_vars.count == 0) {
        int64_t total = 0;
        int valid = 1;
        for (size_t j = 0; j < n; j++) {
            int64_t v;
            if (!rat_to_int(particular[j], &v) || v < 0) { valid = 0; break; }
            total += v;
        }
        free(particular);
        da_free(&free_vars);
        free(pivot_col);
        mat_free(mat);
        da_free(&targets);
        da_free(&buttons);
        return valid ? (int)total : -1;
    }
    
    Rat **null_basis = malloc(free_vars.count * sizeof(Rat*));
    for (size_t i = 0; i < free_vars.count; i++) {
        null_basis[i] = malloc(n * sizeof(Rat));
        for (size_t j = 0; j < n; j++) null_basis[i][j] = rat_zero();
        null_basis[i][free_vars.items[i]] = rat_int(1);
        for (size_t r = 0; r < row; r++) {
            if (pivot_col[r] != -1) {
                null_basis[i][pivot_col[r]] = rat_neg(mat_get(mat, r, free_vars.items[i]));
            }
        }
    }
    
    int64_t bound = 0;
    for (size_t i = 0; i < m; i++) {
        if (targets.items[i] > bound) bound = targets.items[i];
    }
    
    if (free_vars.count > 2) {
        int64_t max_bound = 1000;
        if (bound > max_bound) bound = max_bound;
    }
    
    int64_t *fv = malloc(free_vars.count * sizeof(int64_t));
    size_t best = 0;
    search(fv, 0, bound, particular, (const Rat**)null_basis, free_vars.count, n, &best);
    
    free(fv);
    for (size_t i = 0; i < free_vars.count; i++) free(null_basis[i]);
    free(null_basis);
    free(particular);
    da_free(&free_vars);
    free(pivot_col);
    mat_free(mat);
    da_free(&targets);
    da_free(&buttons);
    
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
    
    fclose(input);
    
    printf("Part 1: %lld\n", part1);
    printf("Part 2: %lld\n", part2);
    
    return 0;
}
