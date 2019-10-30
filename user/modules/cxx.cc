
extern "C" {
#include <ng/basic.h>
#include <ng/mod.h>
#include <ng/print.h>
}

struct some_struct {
        int data;

        some_struct(int data) : data(data) {
                printf("calling %s\n", __PRETTY_FUNCTION__);
        }
        ~some_struct() {
                printf("calling %s\n", __PRETTY_FUNCTION__);
        }
};

int init_mod(int argument) {
        printf("Hello World from this c++ kernel module!\n");

        some_struct s{10};
        printf("the some_struct data is %i\n", s.data);
        return 0;
}

