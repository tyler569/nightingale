
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int close(int fd) {
        printf("called unimplemented function close\n");
        exit(1);
}

time_t difftime(time_t time1, time_t time0) {
        printf("called unimplemented function difftime\n");
        exit(1);
}

void flockfile(FILE *filehandle) {
        printf("called unimplemented function flockfile\n");
        exit(1);
}

FILE *freopen(const char *filename, const char *mode, FILE *stream) {
        printf("called unimplemented function freopen\n");
        exit(1);
}

int fseeko(FILE *stream, off_t offset, int whence) {
        printf("called unimplemented function fseeko\n");
        exit(1);
}

off_t ftello(FILE *stream) {
        printf("called unimplemented function ftello\n");
        exit(1);
}

void funlockfile(FILE *stream) {
        printf("called unimplemented function funlockfile\n");
        exit(1);
}

int getc_unlocked(FILE *stream) {
        printf("called unimplemented function getc_unlocked\n");
        exit(1);
}

struct tm *gmtime_r(const time_t *timep, struct tm *result) {
        printf("called unimplemented function gmtime_r\n");
        exit(1);
}

int isgraph(int c) {
        printf("called unimplemented function isgraph\n");
        exit(1);
}

struct tm *localtime_r(const time_t *timep, struct tm *result) {
        printf("called unimplemented function localtime_r\n");
        exit(1);
}

int mkstemp() {
        printf("called unimplemented function mkstemp\n");
        exit(1);
}

time_t mktime(struct tm *tm) {
        printf("called unimplemented function mktime\n");
        exit(1);
}

int pclose(FILE *stream) {
        printf("called unimplemented function pclose\n");
        exit(1);
}

FILE *popen(const char *command, const char *type) {
        printf("called unimplemented function popen\n");
        exit(1);
}

int remove(const char *pathname) {
        printf("called unimplemented function remove\n");
        exit(1);
}

int rename(const char *oldpath, const char *newpath) {
        printf("called unimplemented function rename\n");
        exit(1);
}

int setvbuf(FILE *stream, char *buf) {
        printf("called unimplemented function setvbuf\n");
        exit(1);
}

int strcoll(const char *s1, const char *s2) {
        printf("called unimplemented function strcoll\n");
        exit(1);
}

int strpbrk(const char *s, const char *accept) {
        printf("called unimplemented function strpbrk\n");
        exit(1);
}

size_t strspn(const char *s, const char *accept) {
        printf("called unimplemented function strspn\n");
        exit(1);
}

double strtod(const char *nptr, char **endptr) {
        printf("called unimplemented function strtod\n");
        exit(1);
}

int system(const char *command) {
        printf("called unimplemented function system\n");
        exit(1);
}

FILE *tmpfile(void) {
        printf("called unimplemented function tmpfile\n");
        exit(1);
}

int ungetc(int c, FILE *stream) {
        printf("called unimplemented function ungetc\n");
        exit(1);
}

