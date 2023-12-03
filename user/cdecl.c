/*
 * Minor adaptation of the `dcl` program from The C Programming Language
 * (K&R, 2nd edition), 5.12, p122
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAXTOKEN 100

enum { NAME, PARENS, BRACKETS };

void dcl(void);
void dir_dcl(void);
void param_list(void);

int gettoken(void);
int tokentype;
char token[MAXTOKEN];
char name[MAXTOKEN];
char datatype[MAXTOKEN];
char out[1000];

int main()
{
    while (gettoken() != EOF) {
        strcpy(datatype, token);
        out[0] = '\0';
        dcl();
        if (tokentype != '\n')
            printf("syntax error\n");
        else
            printf("%s: %s %s\n", name, out, datatype);
    }
}

void dcl(void)
{
    int ns;

    for (ns = 0; gettoken() == '*';)
        ns++;

    dir_dcl();

    while (ns-- > 0)
        strcat(out, " pointer to");
}

void dir_dcl(void)
{
    int type;

    if (tokentype == '(') {
        dcl();
        if (tokentype != ')')
            printf("error: missing ')'\n");
    } else if (tokentype == NAME) {
        strcpy(name, token);
    } else {
        printf("error: expected name or (dcl)\n");
    }

    while ((type = gettoken()) == PARENS || type == BRACKETS) {
        if (type == PARENS) {
            strcat(out, " function returning");
        } else {
            strcat(out, " array");
            strcat(out, token);
            strcat(out, " of");
        }
    }
}

int gettoken(void)
{
    int c;

    char *p = token;

    while ((c = getc(stdin)) == ' ' || c == '\t') { }

    if (c == '(') {
        if ((c = getc(stdin)) == ')') {
            strcpy(token, "()");
            return tokentype = PARENS;
        } else {
            ungetc(c, stdin);
            return tokentype = '(';
        }
    } else if (c == '[') {
        for (*p++ = c; (*p++ = getc(stdin)) != ']';) { }
        *p = '\0';
        return tokentype = BRACKETS;
    } else if (isalpha(c)) {
        for (*p++ = c; isalnum(c = getc(stdin));) {
            *p++ = c;
        }
        *p = '\0';
        ungetc(c, stdin);
        return tokentype = NAME;
    } else {
        return tokentype = c;
    }
}
