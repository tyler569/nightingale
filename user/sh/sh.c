#include "parse.h"
#include "readline.h"
#include "token.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <list.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ttyctl.h>
#include <unistd.h>

bool do_buffer = true;
bool token_debug = false;
bool ast_debug = false;
bool interactive = true;
FILE *input_file;

int eval(struct node *);

int handle_one_line() {
	char buffer[1024] = { 0 };
	int ret_val = 127;
	list tokens;
	list_init(&tokens);

	if (do_buffer && interactive) {
		ttyctl(STDIN_FILENO, TTY_SETBUFFER, 0);
		ttyctl(STDIN_FILENO, TTY_SETECHO, 0);
		ttyctl(STDIN_FILENO, TTY_SETPGRP, getpid());
		setvbuf(stdout, NULL, _IONBF, 0);
		if (read_line_interactive(buffer, 256) == -1)
			return 2;
	} else {
		if (input_file == stdin) {
			ttyctl(STDIN_FILENO, TTY_SETBUFFER, 1);
			ttyctl(STDIN_FILENO, TTY_SETECHO, 1);
			ttyctl(STDIN_FILENO, TTY_SETPGRP, getpid());
			setvbuf(stdout, NULL, _IOLBF, 0);
		}
		if (read_line_simple(input_file, buffer, 256) == -1)
			return 2;
	}

	if (!buffer[0])
		return 0;

	if (tokenize(buffer, &tokens)) {
		if (token_debug) {
			list_for_each_safe (&tokens) {
				struct token *t = container_of(struct token, node, it);
				token_fprint(stderr, t);
				printf("\n");
			}
		}
		struct node *node = parse(&tokens);
		if (node) {
			if (ast_debug)
				node_fprint(stderr, node);
			ret_val = eval(node);
		}
	}

	if (ret_val >= 128 && ret_val < 128 + 32) {
		// TODO: signal names
		fprintf(stderr, "terminated by signal %i\n", ret_val - 128);
	} else if (ret_val != 0) {
		fprintf(stderr, "-> %i\n", ret_val);
	}

	return 0;
}

void signal_handler(int signal) {
	// TODO bail out of readline, clear buffer, print new prompt
	return;
}

void help(const char *progname) {
	fprintf(stderr,
		"usage: %s [-nd]\n"
		"  -n     disable tty buffering\n"
		"  -d     token debug mode\n"
		"  -a     ast debug mode\n",
		progname);
}

int main(int argc, char **argv) {
	int pid = getpid();
	setpgid(pid, pid);
	input_file = stdin;

	signal(SIGINT, signal_handler);

	int opt;
	while ((opt = getopt(argc, argv, "ndah")) != -1) {
		switch (opt) {
		case 'n':
			do_buffer = false;
			break;
		case 'd':
			token_debug = true;
			break;
		case 'a':
			ast_debug = true;
			break;
		case '?': // FALLTHROUGH
		case 'h':
			help(argv[0]);
			return 0;
		}
	}

	if (argv[optind]) {
		input_file = fopen(argv[optind], "r");
		if (!input_file) {
			perror("fopen");
			return EXIT_FAILURE;
		}
		do_buffer = false;
		interactive = false;
	}
	if (!isatty(fileno(stdin)))
		interactive = false;

	while (handle_one_line() == 0) { }

	return EXIT_SUCCESS;
}
