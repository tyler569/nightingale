#include <basic.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t line_count(FILE *f) {
    size_t nlines = 0;
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') nlines++;
    }
    fseek(f, 0, SEEK_SET);
    return nlines - 1; // TODO where is the extra newline coming from!?
}

size_t line_length(FILE *f) {
    char buffer[512] = {0};
    fgets(buffer, 512, f);
    fseek(f, 0, SEEK_SET);
    return strlen(buffer);
}


struct board {
    size_t w, h;
    char *data;
};

enum cell {
    NUL = 0,
    FLOOR = '.',
    SEAT = 'L',
    FSEAT = '#',
};

struct board make_board(FILE *f) {
    size_t fh = line_count(f);
    size_t fw = line_length(f) - 1; // not counting '\n'

    size_t real_h = fh + 2;
    size_t real_w = fw + 2;
    
    char *data = zmalloc(real_h * real_w);

    size_t r = 1;
    size_t c = 1;

    char buffer[512] = {0};
    while (fgets(buffer, 512, f)) {
        size_t index = r * real_w + c;
        memcpy(&data[index], buffer, fw);
        memset(buffer, 0, 512);
        r += 1;

        assert(r < real_h);
    }

    return (struct board) {
        .w = real_w,
        .h = real_h,
        .data = data,
    };
}

void print_board(struct board b) {
    for (size_t r = 0; r < b.h; r++) {
        for (size_t c = 0; c < b.w; c++ /* dangerous */) {
            char h = b.data[r * b.w + c];
            if (h == 0) printf("0 ");
            else printf("%c ", h);
        }
        printf("\n");
    }
}

int can_see(struct board b, size_t r, size_t c, size_t dr, size_t dc) {
    while (true) {
        r += dr;
        c += dc;
        char h = b.data[r * b.w + c];
        if (h == NUL) return 0;
        if (h == SEAT) return 0;
        if (h == FSEAT) return 1;
    }
}

bool iterate(struct board b) {
    bool any_changed = false;
    char *shadow = zmalloc(b.h * b.w);
    for (size_t r = 1; r < b.h - 1; r++) {
        for (size_t c = 1; c < b.w - 1; c++ /* dangerous */) {
            size_t i = r * b.w + c;
            char h = b.data[i];

            int adjacent = 0;
            adjacent += can_see(b, r, c, -1, -1);
            adjacent += can_see(b, r, c, -1,  0);
            adjacent += can_see(b, r, c, -1,  1);
            adjacent += can_see(b, r, c,  0, -1);
            adjacent += can_see(b, r, c,  0,  1);
            adjacent += can_see(b, r, c,  1, -1);
            adjacent += can_see(b, r, c,  1,  0);
            adjacent += can_see(b, r, c,  1,  1);

            if (h == SEAT && adjacent == 0) {
                shadow[i] = FSEAT;
                any_changed = true;
            } else if (h == FSEAT && adjacent >= 5) {
                shadow[i] = SEAT;
                any_changed = true;
            } else {
                shadow[i] = h;
            }
        }
    }
    memcpy(b.data, shadow, b.w * b.h);
    free(shadow);
    return any_changed;
}

long count_seats(struct board b) {
    long npeople = 0;
    for (size_t r = 1; r < b.h - 1; r++) {
        for (size_t c = 1; c < b.w - 1; c++ /* dangerous */) {
            char h = b.data[r * b.w + c];
            npeople += h == FSEAT;
        }
    }
    return npeople;
}


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "argument required");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("fopen");
        return 1;
    }

    struct board b = make_board(file);
    printf("%zu %zu\n", b.w, b.h);
    print_board(b);
    printf("\n");

    while (iterate(b)) {
        print_board(b);
        printf("\n");
    }

    long people = count_seats(b);
    printf("people: %li\n", people);
}
