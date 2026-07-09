int count_from_zero = 0;
int count_from_one = 1;

const char *static_string = "this is a compiled-in string";
char values[32] = { 1 };

int get_number();

int foo(int a, int b) {
    if (a + b > 0 && a + b < 32) {
        values[a + b] += 1;
    }

    return a + b + count_from_zero++ + count_from_one++ + values[1] + get_number();
}
