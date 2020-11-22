int ngk_1();

int ngk_2();

int ngk_3();

extern const char *ngk_data[];

const char *mod_data[] = {
    "Hello",
    "World",
};

int mod_1(int a) {
    int x = ngk_1(10);
    return x + a;
}

int mod_2(int a) {
    return ngk_2(a);
}

int mod_3(int a) {
    const char *x = "foobar";
    return ngk_3(x, ngk_1() ? "a string" : "not a string", ngk_data[1],
                 mod_data[1]);
}

int init_module(int x) {
    return ngk_1(x + 1);
}
