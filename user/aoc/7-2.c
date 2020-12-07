#include <basic.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strcpyto(char *dest, const char *source, char delim) {
    while (*source && *source != delim) { *dest++ = *source++; }
    *dest = 0;
    return (char *)source;
}

static inline uint64_t ror(uint64_t i, int n) {
    asm volatile ("ror %%cl, %0 \n\t"
        : "+r"(i)
        : "c"(n));
    return i;
}

uint64_t hash(char *string) {
    uint64_t hash_value = 4537;
    while (*string) {
        hash_value *= *string * 1003;
        hash_value <<= (*string % 3);
        hash_value = ror(hash_value, *string % 32);
        string++;
    }
    return hash_value;
}

bool eat_char(char **string, char c) {
    if (**string == c) {
        *string = *string + 1; return true;
    }
    return false;
}

bool eat_string(char **string, char *word) {
    bool success;
    while (*word) {
        success = eat_char(string, *word);
        word++;
        if (!success) return false;
    }
    return true;
}

uint64_t hash_word(char **string) {
    char buffer[128] = {0};
    char *end = strcpyto(buffer, *string, ' ');
    *string = end;
    return hash(buffer);
}

uint64_t hash_color(char **string) {
    uint64_t hash = hash_word(string);
    assert(eat_char(string, ' '));
    hash ^= hash_word(string);
    assert(eat_string(string, " bag"));
    eat_char(string, 's');
    return hash;
}

void hash_color_tests() {
    char *v = "whole grain bags";
    uint64_t whole_grain = hash_color(&v);
    assert(*v == 0);

    v = "whole grain bags";
    assert(hash_color(&v) == whole_grain);
    assert(*v == 0);

    v = "whole orange bags";
    uint64_t whole_orange = hash_color(&v);
    assert(*v == 0);

    v = "whole orange bags";
    assert(hash_color(&v) == whole_orange);
    assert(*v == 0);

    assert(whole_orange != whole_grain);
}

#define HTBL_SIZE 100000


struct list htbl[HTBL_SIZE];

struct bag_entry {
    list_node node;
    long count;
    struct list *typ;
};

#define ix(hash) (hash % HTBL_SIZE)

struct list *bag_list(uint64_t color) {
    struct list *bag_list = &htbl[ix(color)];
    if (bag_list->next == 0) list_init(bag_list);
    return bag_list;
}

void add_bag_entry(uint64_t bag, uint64_t color, long n) {
    struct list *bag_l = bag_list(bag);

    struct bag_entry *entry = malloc(sizeof(struct bag_entry));
    entry->count = n;
    entry->typ = bag_list(color);

    list_append(bag_l, &entry->node);
}


void parse_rule(char *string) {
    char **s = &string;
    char *after;

    uint64_t bag = hash_color(s);
    assert(eat_string(s, " contain"));

    printf("%zx: ", bag);
    assert(list_empty(bag_list(bag)));

    do {
        eat_char(s, ','); // optional, on repeats -- at start for '.' checking
        assert(eat_char(s, ' '));
        if (strncmp(*s, "no", 2) == 0) {
            break;
        }
        long n = strtol(*s, &after, 10);
        *s = after;
        assert(eat_char(s, ' '));
        uint64_t c = hash_color(s);

        printf("%li x %zx  ", n, c);

        add_bag_entry(bag, c, n);
    } while (**s != '.');
    printf("\n");
}

long parse_count = 0;

void parse_rules(FILE *stream) {
    char buffer[256];
    while (fgets(buffer, 256, stream)) {
        parse_count += 1;
        parse_rule(buffer);
    }
}

void print_indent(int indent) {
    for (int i=0; i<indent; i++) printf(" ");
}

struct list *shiny_gold;

long recurse_rules(struct list *rules, int indent) {
    long total_bags = 0;
    list_for_each(struct bag_entry, b, rules, node) {
        total_bags += b->count * (recurse_rules(b->typ, indent + 1) + 1);
    }
    return total_bags;
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

    char *shiny_gold_string = "shiny gold bags";
    uint64_t shiny_gold_hash = hash_color(&shiny_gold_string);
    printf("shiny_gold_hash: %zx\n", shiny_gold_hash);
    shiny_gold = bag_list(shiny_gold_hash);

    hash_color_tests();
    parse_rules(file);

    long total_bags = recurse_rules(shiny_gold, 0);
    printf("total: %li\n", total_bags);
}
