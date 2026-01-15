#include "../nob.h"
#include <stdint.h>

typedef struct {
    int index;
    char data[3][3];
    uint w,h;   // width & height of bounding box
    uint ox,oy; // top-left # offset
} Shape;

typedef struct {
    uint w,h;
    struct { uint *items; size_t count,capacity; } Data;
} Region;

typedef struct { Shape **items; size_t count, capacity; } Shapes;
typedef struct { Region *items; size_t count, capacity; } Regions;

// ---------------- compute bounding box & offsets ----------------
static void compute_shape_size(Shape *s) {
    uint minx=3,maxx=0,miny=3,maxy=0;
    int has_hash=0;
    for(uint y=0;y<3;y++)
        for(uint x=0;x<3;x++)
            if(s->data[y][x]=='#'){
                if(x<minx) minx=x; 
                if(x>maxx) maxx=x;
                if(y<miny) miny=y; 
                if(y>maxy) maxy=y;
                has_hash=1;
            }
    if(!has_hash){ s->w=s->h=s->ox=s->oy=0; return; }
    s->w = maxx-minx+1;
    s->h = maxy-miny+1;
    s->ox = minx;
    s->oy = miny;
}

// ---------------- placement helpers ----------------
static int can_place(uint8_t *grid,uint W,uint H,Shape *s,uint x,uint y){
    if(x+s->w>W || y+s->h>H) return 0;
    for(uint sy=0; sy<s->h; sy++)
        for(uint sx=0; sx<s->w; sx++)
            if(s->data[sy+s->oy][sx+s->ox]=='#' && grid[(y+sy)*W+(x+sx)]) return 0;
    return 1;
}

static void do_place(uint8_t *grid,uint W,Shape *s,uint x,uint y,int val){
    for(uint sy=0; sy<s->h; sy++)
        for(uint sx=0; sx<s->w; sx++)
            if(s->data[sy+s->oy][sx+s->ox]=='#')
                grid[(y+sy)*W+(x+sx)] = val;
}

// ---------------- recursive backtracking ----------------
static int solve(uint8_t *grid,uint W,uint H,Shape **shapes,uint *counts,uint n){
    // all shapes placed?
    int done=1; for(uint i=0;i<n;i++) if(counts[i]>0){done=0; break;}
    if(done) return 1;

    // pick first remaining shape
    uint idx;
    for(idx=0; idx<n; idx++) if(counts[idx]>0) break;
    counts[idx]--;

    for(uint y=0;y<H;y++)
        for(uint x=0;x<W;x++)
            if(can_place(grid,W,H,shapes[idx],x,y)){
                do_place(grid,W,shapes[idx],x,y,1);
                if(solve(grid,W,H,shapes,counts,n)) return 1;
                do_place(grid,W,shapes[idx],x,y,0); // backtrack
            }

    counts[idx]++;
    return 0;
}

// ---------------- main ----------------
int main(int argc,char **argv){
    FILE *input = open_input_or_die(argc,argv);
    char *line=NULL; size_t size=0; ssize_t read;
    Shapes shapes={0}; Regions regions={0}; uint offset=0;

    while((read = getline_or_die(&line,&size,input))!=-1){
        size_t len=strlen(line); if(len && line[len-1]=='\n') line[--len]='\0';
        if(len==0) continue;

        // ---------- parse shapes ----------
        if(len==2 && isdigit(line[0])){
            Shape *s=malloc(sizeof(*s)); memset(s->data,'.',sizeof(s->data));
            s->index=line[0]-'0'; offset=0;
            da_append(&shapes,s);
            continue;
        }
        if(len==3 && shapes.count>0){
            Shape *last=da_last(&shapes);
            for(uint j=0;j<3;j++) last->data[offset][j]=line[j];
            offset++; continue;
        }

        // ---------- parse regions ----------
        if(strchr(line,'x')){
            Region r={0}; sscanf(line,"%ux%u",&r.w,&r.h);
            r.Data.items=NULL; r.Data.count=r.Data.capacity=0;

            char *colon=strchr(line,':');
            if(colon){ colon++;
                while(*colon){
                    if(isdigit(*colon)){
                        char *endptr;
                        uint v=strtoul(colon,&endptr,10);
                        da_append(&r.Data,v); colon=endptr;
                    } else colon++;
                }
            }
            da_append(&regions,r);
        }
    }
    free(line); fclose(input);

    // compute bounding boxes for shapes
    for(uint i=0;i<shapes.count;i++) compute_shape_size(shapes.items[i]);

    // ---------------- evaluate regions ----------------
    uint part1=0;
    for(uint i=0;i<regions.count;i++){
        Region *R=&regions.items[i];
        uint8_t *grid=calloc(R->w*R->h,1);

        uint *counts=malloc(shapes.count*sizeof(uint));
        for(uint j=0;j<shapes.count;j++)
            counts[j]=(j<R->Data.count)? R->Data.items[j]:0;

        if(solve(grid,R->w,R->h,shapes.items,counts,shapes.count)) part1++;

        free(grid); free(counts);
    }

    printf("Part1: %u\n",part1);

    // ---------------- cleanup ----------------
    for(uint i=0;i<shapes.count;i++) free(shapes.items[i]);
    free(shapes.items);
    for(uint i=0;i<regions.count;i++) free(regions.items[i].Data.items);
    free(regions.items);

    return 0;
}
