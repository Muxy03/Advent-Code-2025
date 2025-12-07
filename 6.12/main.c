#include "../nob.h"

typedef struct {
	string *items;
	size_t count;
	size_t capacity;
} LINES;

static string trim_copy(const string s) {
    const string start = s;
    const string end = s + strlen(s);

    const char *b = start;
    while (*b && isspace((uch)*b)) b++;

    const char *e = end;
    while (e > b && isspace((uch)*(e - 1))) e--;

    size_t len = (size_t)(e - b);
    string out = (string)malloc(len + 1);
    memcpy(out, b, len);
    out[len] = '\0';
    return out;
}

static ll parse_ll(const string s) {
    ll v = 0;
     const char *p = s;
    while (*p) {
        if (isdigit((uch)*p)) v = v * 10 + (*p - '0');
        p++;
    }
    return v;
}

int main(int argc, char **argv) {

	FILE *input = open_input_or_die(argc, argv);

    size_t rows = 0, cols = 0;
    
	LINES lines = {0};

	ll part1 = 0, part2 = 0;

	string line = NULL;
	size_t size = 0;
	ssize_t read = 0;

	while((read = getline_or_die(&line, &size, input)) != -1) {
		size_t len = strlen(line);

		if(len && line[len-1] == '\n') line[--len] = '\0';

		string copy = (string)malloc(len + 1);
		strcpy(copy, line);

	    if (len > cols) cols = len;

		da_append(&lines, copy);
				
		rows++;	
	}

    for (size_t i = 0; i < rows; i++) {
        size_t cur = strlen(lines.items[i]);
        if (cur < cols) {
            lines.items[i] = (string)realloc(lines.items[i], cols + 1);
            memset(lines.items[i] + cur, ' ', cols - cur);
            lines.items[i][cols] = '\0';
        }
    }

	size_t op_row = rows - 1;
	int *col_has = (int *)calloc(cols, sizeof(int));

	for (size_t c = 0; c < cols; c++){
	    for (size_t r = 0; r < rows; r++){
	        if (lines.items[r][c] != ' ') col_has[c] = 1;
        }
    }

	for (size_t c = 0; c < cols;) {
	    while (c < cols && !col_has[c]) c++;
	    if (c >= cols) break;

	    size_t l = c;
	    while (c < cols && col_has[c]) c++;
	    size_t r = c - 1;

	    char op = '+';
	    for (size_t cc = l; cc <= r; cc++) {
	        if (lines.items[op_row][cc] != ' ') op = lines.items[op_row][cc];
        }

	    // PART 1 (TOP → BOTTOM, ROW BASED)
	    ll acc1 = (op == '*') ? 1 : 0;
	    int used1 = 0;

	    for (size_t row = 0; row < op_row; row++) {
	        size_t len = r - l + 1;
	        string sub = (string)malloc(len + 1);
	        memcpy(sub, lines.items[row] + l, len);
	        sub[len] = '\0';

	        string trim = trim_copy(sub);
	        free(sub);

	        if (*trim) {
	            ll v = parse_ll(trim);
	            used1 = 1;
	            if (op == '+') acc1 += v;
	            else acc1 *= v;
	        }

	        free(trim);
	    }

	    if (used1) part1 += acc1;

	    // PART 2 (RIGHT → LEFT, COLUMN BASED)
	    ll acc2 = (op == '*') ? 1 : 0;
	    int used2 = 0;
    	
	    for (size_t cc = r + 1; cc-- > l;) {
			DA buf = {0};

	        for (size_t row = 0; row < op_row; row++) {
	            if (lines.items[row][cc] != ' ') da_append(&buf,lines.items[row][cc]);
            }

			da_append(&buf, '\0');

			if (buf.count) {
	        	ll v = parse_ll(buf.items);
	            used2 = 1;
	            if (op == '+') acc2 += v;
	            else acc2 *= v;
	        }

	        da_free(&buf);
	    }

	    if (used2) part2 += acc2;
	}

    printf("Part 1: %llu\n", (ull) part1);
    printf("Part 2: %llu\n", (ull) part2);

	da_free_deep(&lines);
    free(line);
    free(col_has);
    fclose(input);

    return 0;
}
