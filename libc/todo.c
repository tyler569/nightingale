
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

/*

#define UNIMPLEMENTED_FUNCTION(name, return_t) \
        return_t name(...) { \
                printf("unimplemented function " #name "\n"); \
                exit(1); \
        }


UNIMPLEMENTED_FUNCTION(difftime, double);
UNIMPLEMENTED_FUNCTION(flockfile, void);
UNIMPLEMENTED_FUNCTION(fseeko, int);
UNIMPLEMENTED_FUNCTION(ftello, off_t);
UNIMPLEMENTED_FUNCTION(funlockfile, void);
UNIMPLEMENTED_FUNCTION(gmtime_r, struct tm *);
UNIMPLEMENTED_FUNCTION(localtime_r, struct tm *);
UNIMPLEMENTED_FUNCTION(mkstemp, int);
UNIMPLEMENTED_FUNCTION(mktime, time_t);
UNIMPLEMENTED_FUNCTION(pclose, int)
UNIMPLEMENTED_FUNCTION(remove, int)
UNIMPLEMENTED_FUNCTION(rename, int)
UNIMPLEMENTED_FUNCTION(setvbuf, int)
UNIMPLEMENTED_FUNCTION(strcoll, int)
UNIMPLEMENTED_FUNCTION(strspn, size_t)
UNIMPLEMENTED_FUNCTION(system, int)
UNIMPLEMENTED_FUNCTION(tmpfile, FILE *)
UNIMPLEMENTED_FUNCTION(gettimeofday, int)

*/

double difftime(time_t time1, time_t time0) {
        printf("called unimplemented function difftime\n");
        exit(1);
}

void flockfile(FILE *filehandle) {
        printf("called unimplemented function flockfile\n");
        exit(1);
}

void funlockfile(FILE *stream) {
        printf("called unimplemented function funlockfile\n");
        exit(1);
}

struct tm *gmtime_r(const time_t *timep, struct tm *result) {
        printf("called unimplemented function gmtime_r\n");
        exit(1);
}

struct tm *localtime_r(const time_t *timep, struct tm *result) {
        printf("called unimplemented function localtime_r\n");
        exit(1);
}

int mkstemp(char *template) {
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

int strcoll(const char *s1, const char *s2) {
        printf("called unimplemented function strcoll\n");
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

int gettimeofday(struct timeval *time, void *thing) {
        printf("called unimplemented function gettimeofday\n");
        exit(1);
}

int sscanf(const char *s, const char *format, ...) {
        printf("called unimplemented function sscanf\n");
        exit(1);
}

int rand(void) {
        printf("called unimplemented function rand\n");
        exit(1);
}

void srand(unsigned int seed) {
        printf("called unimplemented function srand\n");
        exit(1);
}

char *tmpnam(char *s) {
        printf("called unimplemented function tmpnam\n");
        exit(1);
}

struct tm *localtime(const time_t *timep) {
        printf("called unimplemented function localtime\n");
        exit(1);
}

struct tm *gmtime(const time_t *timep) {
        printf("called unimplemented function gmtime\n");
        exit(1);
}

