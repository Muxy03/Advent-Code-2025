#include "../nob.h"

typedef struct node {
    int id;
    struct node *next;
} Node;

typedef struct {
    string name;
    Node *next;
} Device;

typedef struct {
    Device *items;
    size_t count;
    size_t capacity;
} Devices;


int findDevice(Devices *g, const char *name) {
    for(size_t i = 0; i < g->count; i++)
        if(strcmp(g->items[i].name, name) == 0)
            return i;
    return -1;
}

int getDevice(Devices *g, const char *name) {
    int id = findDevice(g, name);
    if(id != -1) return id;

    Device d = { strdup(name), NULL };
    da_append(g, d);

    return g->count - 1;
}

void addEdge(Devices *g, int src, int dst) {
    Node *n = malloc(sizeof(Node));
    n->id = dst;
    n->next = g->items[src].next;
    g->items[src].next = n;
}

ll dfs(Devices *g, ll *memo, bool *memoUsed, int u, int target) {
    if(u == target) return 1;

    if(memoUsed[u]) return memo[u];
    memoUsed[u] = true;

   ll paths = 0;
    
    for(Node *n = g->items[u].next; n; n = n->next) {
		paths += dfs(g, memo, memoUsed, n->id, target);
    }

    memo[u] = paths;
    return paths;
}

ll count_paths_between(Devices *g, int a, int b) {
    ll *memo   = calloc(g->count, sizeof(ll));
    bool *used = calloc(g->count, sizeof(bool));

    ll result = dfs(g, memo, used, a, b);

    free(memo);
    free(used);
    return result;
}

ll count_paths_two_required(Devices *g,
                            int start, int mustA, int mustB, int target)
{
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

    string line = NULL;
    size_t size = 0;
    ssize_t read = 0;

    Devices g = {0};

	while ((read = getline_or_die(&line, &size, input)) != -1) {
	    size_t len = strlen(line);
	    if (len && line[len-1] == '\n') line[--len] = '\0';
	    if (len == 0) continue;

	    char *tok = strtok(line, ": ");
	    int src = getDevice(&g, tok);

	    while ((tok = strtok(NULL, " :\t")) != NULL) {
	        int dst = getDevice(&g, tok);
	        addEdge(&g, src, dst);
	    }
	}


    int start = findDevice(&g, "you");
    int target = findDevice(&g, "out");

    ll *memo = calloc(g.count, sizeof(*memo));
    bool *memoUsed = calloc(g.count, sizeof(*memoUsed));

	ll part1 = (start == -1 || target == -1) ?
                0 : dfs(&g, memo, memoUsed, start, target);

    start = findDevice(&g, "svr");
    int must1 = findDevice(&g, "fft");
    int must2 = findDevice(&g, "dac");

	ll part2 = count_paths_two_required(&g, start, must1, must2, target);

    printf("Part1: %lld\n", part1);
    printf("Part2: %lld\n", part2);

    for(size_t i = 0; i < g.count; ++i) {
        free(g.items[i].name);

        Node *n = g.items[i].next;
        while(n) {
            Node *tmp = n->next;
            free(n);
            n = tmp;
        }
    }

    free(line);
    da_free(&g);
    free(memo);
    free(memoUsed);
    fclose(input);
    return 0;
}
