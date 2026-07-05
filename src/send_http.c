#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  const char* server_ip = argv[1];
  int server_port = atoi(argv[2]);

  int sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));

  server.sin_family = AF_INET;
  server.sin_port = htons(server_port);
  inet_pton(AF_INET, server_ip, &server.sin_addr);

  connect(sock, (struct sockaddr*)&server, sizeof(server));

  const char* body = "Hello World!";

  char request[512];
  int request_len = snprintf(request, sizeof(request),
                             "POST / HTTP/1.1\r\n"
                             "Host: %s:%d\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: %zu\r\n"
                             "Connection: close\r\n"
                             "\r\n"
                             "%s",
                             server_ip, server_port, strlen(body), body);

  send(sock, request, request_len, 0);

  close(sock);
  return 0;
}