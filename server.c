#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3000

int main() {
  int socket_fd, client_fd;

  socklen_t client_addr_size;
  struct sockaddr_in client_addr;

  // get file descriptor for socket
  if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("socket");
    close(socket_fd);
    exit(1);
  }

  // set the SO_REUSEADDR option for the socket
  // this lets the local address be reused
  int yes = 1;
  if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes)){
      perror("setsocketopt");
      exit(1);
  }

  // bind socket to localhost:3000
  struct sockaddr_in addr = {AF_INET, htons(PORT), 0};
  if(bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1){
    perror("bind");
    close(socket_fd);
    exit(1);
  }

  // listen for incoming connections
  if(listen(socket_fd, 10) == -1){
    perror("listen");
    close(socket_fd);
    exit(1);
  }

  printf("waiting for connection...\n");

  // accept connection on socket
  if ((client_fd = accept(socket_fd, (struct sockaddr *)&client_addr,
                          &client_addr_size)) == -1) {
    perror("accept");
    close(socket_fd);
    close(client_fd);
    exit(1);
  }

  // get ip address of client in text form
  char client_string_addr[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr, client_string_addr,
            client_addr_size);
  printf("accepted connection from: %s\n", client_string_addr);

  // send success message
  char http_res_msg[] = "HTTP/1.0 200 OK\n\n<html>\n<h1>test</h1>\n</html>";
  send(client_fd, http_res_msg, sizeof(http_res_msg), 0);

  close(client_fd);
  close(socket_fd);
}
