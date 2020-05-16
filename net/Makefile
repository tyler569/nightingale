
.PHONY: all

SRC= net.c socket.c i_udp_echo.c i_tcp_out.c i_tcp_echo.c

TARGET= net

CFLAGS= -Wall -Wno-address-of-packed-member -g

$(TARGET): $(SRC) $(shell find . -name '*.h')
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(SRC) -lpthread

clean:
	rm net
