#include <assert.h>
#include <string.h>

const char *optarg;
int optind = 1, opterr = 1, optopt;
static char *nextchar;
static const char *progname;

static void report_arg_error(char flag) {
    if (opterr)
        fprintf(stderr, "%s: option requires an argument -- '%c'\n", progname,
                flag);
}

static void report_flag_error(char flag) {
    if (opterr) fprintf(stderr, "%s: invalid option -- '%c'\n", progname, flag);
}

static void consume_argument(char *const argv[], const char *flag) {
    if (flag[1]) {
        // option is rest of this argument
        optind += 1;
        nextchar = NULL; // don't keep scanning this argv, move to next
        optarg = &flag[1];
        return;
    }

    // option is next argument
    const char *arg = argv[optind + 1];
    if (!arg) {
        report_arg_error(*flag); // no argument provided
        optind += 1;             // to stop us running off the end of ARGV
    } else {
        optind += 2;
    }

    optarg = arg;
}

enum option_type {
    NONE,
    SIMPLE,
    ARGUMENT,
};

static enum option_type opttype(char c, const char *optstring) {
    const char *in = strchr(optstring, c);
    if (!in) { return NONE; }
    if (in[1] == ':') { return ARGUMENT; }
    return SIMPLE;
}

static int option(char *const argv[], const char *flag, const char *optstring) {
    enum option_type t = opttype(*flag, optstring);
    if (t == NONE) {
        report_flag_error(*flag); // invalid flag
        optopt = *flag;
        return '?';
    }
    if (t == ARGUMENT) { consume_argument(argv, flag); }
    if (nextchar) nextchar++;
    return *flag;
}

static const char *next_flag_character(int argc, char *const argv[]) {
    if (nextchar && *nextchar) { return nextchar; }
    if (nextchar) {
        // We were scanning a flag argument, but we hit the end. Move along.
        optind += 1;
        nextchar = NULL;
    }
    if (optind > argc || argv[optind] == 0) {
        // We're at the end of the tracks
        return NULL;
    }
    if (argv[optind][0] != '-') {
        // This argiment is not an option, so we're done parsing
        return NULL;
    }
    if (argv[optind][1] && argv[optind][1] != '-') {
        // This is an argument string, -[^-].*
        nextchar = &argv[optind][1];
        return nextchar;
    }
    // This should only be hit by '-' and '--.*', let's check that assumption
    assert(strcmp(argv[optind], "-") == 0 ||
           strncmp(argv[optind], "--", 2) == 0);
    return NULL;
}

int getopt(int argc, char *const argv[], const char *optstring) {
    progname = argv[0];
    const char *c = next_flag_character(argc, argv);
    if (!c) return -1;
    return option(argv, c, optstring);
}
