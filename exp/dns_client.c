
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#define packed __attribute__((packed))

struct packed dns_header {
    uint16_t ident;

    union {
        struct {
            uint16_t qr : 1;
            uint16_t opcode : 4;
            uint16_t authoritative : 1;
            uint16_t truncated : 1;
            uint16_t recursion_desired : 1;
            uint16_t recursion_available : 1;
            uint16_t _zero : 3;
            uint16_t rcode : 4;
        },
        uint16_t control;
    }

    uint16_t n_answer;
    uint16_t n_authority;
    uint16_t n_question;
};

struct packed dns_question {

};

char *name_to_dns(char *name) {
    /*
     * converts DNS names to the representation used in a DNS packet
     *
     * i.e.:
     * a.b.c => x1 a x1 b x1 cx 0
     * example.com => x7 e x a m p l e x3 c o m x0
     */


}

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) { perror("socket()"; return 1; }

    struct sockaddr me = {0};
    struct sockaddr remote = {0};

    int res = bind(fd);

    // TODO


    return 0;
}

