CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -D_DEFAULT_SOURCE -Iinclude
PCAP_LIBS = -lpcap

all: sniff_http send_http

sniff_http: src/sniff_http.c include/myheader.h
	$(CC) $(CFLAGS) src/sniff_http.c -o sniff_http $(PCAP_LIBS)

send_http: src/send_http.c
	$(CC) $(CFLAGS) src/send_http.c -o send_http

clean:
	rm -f sniff_http send_http *.o