#pragma once
#ifndef NG_SH_PARSE_H
#define NG_SH_PARSE_H

#include "token.h"
#include <list.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

enum node_type {
    NODE_PIPELINE,
    // todo:
    NODE_BINOP,
    NODE_ASSIGN,
    NODE_IF,
    NODE_CASE,
    NODE_WHILE,
    NODE_FOR,
    NODE_FUNCTION,
};

enum node_op {
    NODE_OR,   // ||
    NODE_AND,  // &&
    NODE_THEN, // ;
};

struct pipeline;

struct node {
    enum node_type type;

    struct pipeline *pipeline; // NODE_PIPELINE

    enum node_op op; // NODE_BINOP
    struct node *left;
    struct node *right; // (+ NODE_ASSIGN)

    char *varname; // NODE_ASSIGN
};

enum pipeline_flags {
    PIPELINE_NONE,
    PIPELINE_BACKGROUND = 1,
};

struct pipeline {
    list commands;
    pid_t pgrp;
    enum pipeline_flags flags;
};

struct command {
    char **argv;
    char *args;
    char *stdin_file;
    char *stdout_file;
    char *stderr_file;
    int stdin_fd, stdout_fd, stderr_fd; // used in eval
    list_node node;
};

struct node *parse(list *tokens);
void node_print(struct node *);
void node_fprint(FILE *, struct node *);

#endif // NG_SH_PARSE_H
