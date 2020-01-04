#include <cstring>
#include <cstdio>

struct Reader {
    virtual int read(char *, int) = 0;
};

struct Writer {
    virtual int write(const char *, int) = 0;
};

struct ReadWriter : Reader, Writer {
};

struct PrintfWriter : Writer {
    int write(const char *buf, int len) override {
        printf("%s", buf);
        return len;
    }
};

struct Rot13 : Writer {
    Writer& inner;

    Rot13(Writer& inner) : inner(inner) {}

    static char rot13(char c) {
        if (c >= 'a' && c <= 'm' || c >= 'A' && c <= 'M') {
            return c + 13;
        } else if (c >= 'n' && c <= 'z' || c >= 'N' && c <= 'Z') {
            return c - 13;
        } else {
            return c;
        }
    }

    int write(const char *buf, int len) override {
        char my_buf[len];
        memcpy(my_buf, buf, len);
        for (int i=0; i<len; i++) {
            my_buf[i] = rot13(my_buf[i]);
        }
        return inner.write(my_buf, len);
    }
};

int main() {
    PrintfWriter p;
    Rot13 r(p);

    const char *message = "Hello World\n";
    const int len = strlen(message) + 1;

    r.write(message, len);
}

