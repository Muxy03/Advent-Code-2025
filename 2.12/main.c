#include "../nob.h"

static bool da_is_double_repeat(const DA *da) {
    if (da->count % 2 != 0) return false;
    size_t half = da->count / 2;
    for (size_t i = 0; i < half; i++)
        if (da->items[i] != da->items[i + half]) return false;
    return true;
}

static bool da_is_multi_repeat(const DA *da) {
    size_t n = da->count;
    if (n <= 1) return false;
    for (size_t len = 1; len <= n / 2; len++) {
        if (n % len != 0) continue;
        bool ok = true;
        for (size_t i = 0; i < n; i++)
            if (da->items[i] != da->items[i % len]) { ok = false; break; }
        if (ok) return true;
    }
    return false;
}

int main(int argc, char **argv) {
    
    FILE *input = open_input_or_die(argc, argv);
    
    char *line = NULL;
    size_t size = 0;
    ssize_t len = getline_or_die(&line, &size, input);

    fclose(input);
    
    if (len <= 0) {
        fprintf(stderr, "Error: could not read line from file.\n");
        free(line);
        return 1;
    }
    
    if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
    
    ll part1 = 0, part2 = 0;

    DA Line = {0};
	da_append_many(&Line, line, len);
    
    string p = Line.items;

    while (*p) {
        ll a = strtoll(p, &p, 10);
        if (*p != '-') break;
        p++;
        ll b = strtoll(p, &p, 10);
        if (*p == ',') p++;

        DA buf = {0};
        char tmpbuf[32];
        for (ll x = a; x <= b; x++) {
            int n = snprintf(tmpbuf, sizeof(tmpbuf), "%lld", x);
            da_append_many(&buf, tmpbuf, n);
            if (da_is_double_repeat(&buf)) part1 += x;
            if (da_is_multi_repeat(&buf)) part2 += x;
            buf.count = 0;
        }
        da_free(&buf);
    }

    printf("Part1: %lld\n", part1);
    printf("Part2: %lld\n", part2);

	free(line);
    da_free(&Line);

    return 0;
}
