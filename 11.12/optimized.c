#include "../nob.h"

typedef struct {
    int *items;
    int count;
    int capacity;
} Node;

typedef struct {
    string name;
    Node node;
} Device;

typedef struct {
    Device *items;
    size_t count;
    size_t capacity;
} Devices;

static inline int findDevice(const Devices *g, const char *name) {
    for (size_t i = 0; i < g->count; i++) {
        if (strcmp(g->items[i].name, name) == 0) return i;
    }
    return -1;
}

static inline int getDevice(Devices *g, const char *name) {
    int id = findDevice(g, name);
    if (id != -1) return id;
    
    Device d = {strdup(name), {NULL, 0, 0}};
    da_append(g, d);
    return g->count - 1;
}

static ll dfs(const Devices *g, ll *memo, int u, int target) {
    if (u == target) return 1;
    if (memo[u] != -1) return memo[u];
    
    ll paths = 0;
    const Node *node = &g->items[u].node;
    for (int i = 0; i < node->count; i++) {
        paths += dfs(g, memo, node->items[i], target);
    }
    
    return memo[u] = paths;
}

static ll count_paths_between(const Devices *g, int a, int b) {
    ll *memo = malloc(g->count * sizeof(ll));
    memset(memo, -1, g->count * sizeof(ll));
    ll result = dfs(g, memo, a, b);
    free(memo);
    return result;
}

static ll count_paths_two_required(const Devices *g, int start, int mustA, int mustB, int target) {
    ll p1 = count_paths_between(g, start, mustA)
          * count_paths_between(g, mustA, mustB)
          * count_paths_between(g, mustB, target);
    ll p2 = count_paths_between(g, start, mustB)
          * count_paths_between(g, mustB, mustA)
          * count_paths_between(g, mustA, target);
    return p1 + p2;
}

int main(int argc, char **argv) {
    FILE *input = open_input_or_die(argc, argv);
    
    Devices g = {0};
    string line = NULL;
    size_t size = 0;
    ssize_t read = 0;
    
    while ((read = getline_or_die(&line, &size, input)) != -1) {
        size_t len = strlen(line);
        if (len && line[len - 1] == '\n') line[--len] = '\0';
        if (len == 0) continue;
        
        char *tok = strtok(line, ": ");
        int src = getDevice(&g, tok);
        
        while ((tok = strtok(NULL, " :\t")) != NULL) {
            int dst = getDevice(&g, tok);

            Node *node = &g.items[src].node;
            da_append(node, dst);
        }
    }
    
    int start = findDevice(&g, "you");
    int target = findDevice(&g, "out");
    
    ll part1 = 0;
    if (start != -1 && target != -1) {
        part1 = count_paths_between(&g, start, target);
    }
    
    start = findDevice(&g, "svr");
    int must1 = findDevice(&g, "fft");
    int must2 = findDevice(&g, "dac");
    ll part2 = count_paths_two_required(&g, start, must1, must2, target);
    
    printf("Part1: %lld\n", part1);
    printf("Part2: %lld\n", part2);
    
    for (size_t i = 0; i < g.count; i++) {
        free(g.items[i].name);
        da_free(&g.items[i].node);
    }
    
    free(line);
    da_free(&g);
    fclose(input);
    
    return 0;
}
