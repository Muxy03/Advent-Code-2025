#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ll long long

#ifdef __cplusplus
#define NOB_DECLTYPE_CAST(T) (decltype(T))
#else
#define NOB_DECLTYPE_CAST(T)
#endif // __cplusplus

#define da_reserve(da, expected_capacity)                                                  \
    do {                                                                                   \
        if ((expected_capacity) > (da)->capacity) {                                        \
            if ((da)->capacity == 0) {                                                     \
                (da)->capacity = 256;                                                      \
            }                                                                              \
            while ((expected_capacity) > (da)->capacity) {                                 \
                (da)->capacity *= 2;                                                       \
            }                                                                              \
            (da)->items = NOB_DECLTYPE_CAST((da)->items)                                   \
                realloc((da)->items, (da)->capacity * sizeof(*(da)->items));               \
            assert((da)->items != NULL && "Out of memory");                                \
        }                                                                                  \
    } while (0)

#define da_append(da, item)                    \
    do {                                       \
        da_reserve((da), (da)->count + 1);     \
        (da)->items[(da)->count++] = (item);   \
    } while (0)

#define da_free(da) free((da).items)

typedef struct {
	ll left;
	ll right;
} Range;

typedef struct {
    Range *items;
    size_t count;
    size_t capacity;
} Ranges;

typedef struct {
    ll *items;
    size_t count;
    size_t capacity;
} DA;

int compare(const void *a, const void *b)
{
    long long x = *(const long long *)a;
    long long y = *(const long long *)b;

    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}

int compare_ranges(const void *a, const void *b) {
    const Range *r1 = a;
    const Range *r2 = b;
    if (r1->left < r2->left) return -1;
    if (r1->left > r2->left) return 1;
    return 0;
}

size_t is_fresh(Range *arr, size_t len, ll id) {
	ssize_t l = 0, r = len-1;

	while(l <= r) {
		size_t mid = l + (r-l) / 2;
		if(arr[mid].left <= id && id <= arr[mid].right) {
			return 1;
		} else if(id < arr[mid].left) {
			r = mid - 1;
		} else {
			l = mid + 1;	
		}
	}

	return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening file");
        return 1;
    }

	char *line = NULL;
  	size_t len = 0;
  	size_t change = 0; // 0 ranges | 1 elements 

	Ranges ranges = {0};
	DA elements   = {0};
  	
   	while (getline(&line, &len, input) != -1) {
   		if(strlen(line) == 1){
   			 change = 1;
   			 continue;
   		}
   		
 		line[strcspn(line, "\r\n")] = '\0';

 		if(change == 0){
 			Range r = {0};
 			ll left = 0, right = 0;

			if (sscanf(line, "%lld-%lld", &left, &right) != 2) {
	            fprintf(stderr, "Warning: malformed range '%s'.\n", line);
	            exit(1);
	        }

	        r.left = left;
	        r.right = right;
 			
 			da_append(&ranges, r);
 		}else {
 			da_append(&elements, atoll(line));
 		}
 		 
   	}

    fclose(input);

    size_t part1 = 0;
    size_t part2 = 0;

    qsort(elements.items, elements.count, sizeof(ll), compare);
	qsort(ranges.items, ranges.count, sizeof(Range), compare_ranges);

	int idx = 0;
	Ranges merged = {0};

	da_append(&merged, ranges.items[0]);

	for(size_t i = 1; i < ranges.count; ++i) {
		Range range = ranges.items[i];
		if(range.left <= merged.items[idx].right + 1) {
			if(range.right > merged.items[idx].right){
				merged.items[idx].right = range.right;
			}
		} else {
			idx++;
			da_append(&merged, range);
		}
	}
	
	for(size_t i = 0; i < elements.count; ++i) {
		part1 += is_fresh(merged.items, merged.count, elements.items[i]);		
	}

	for(size_t i = 0; i < merged.count; ++i) {
			part2 += (merged.items[i].right - merged.items[i].left) + 1;		
	}

	printf("part1: %zu\n", part1);
	printf("part2: %zu\n", part2);
    

    free(line);
    da_free(ranges);
    da_free(elements);
    da_free(merged);

    return 0;
}
