#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <math.h>


#define COM_BUFFER_MX_SZ 10000


void start_server(int port) {

    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;

    bzero((char*) &server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*) &server, sizeof(server))) {
        perror("Error on binding");
        exit(1);
    }

    listen(server_socket, 10);

    int client_socket, client_status = 1;
    char client_message[COM_BUFFER_MX_SZ];
    char server_response[COM_BUFFER_MX_SZ];

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)NULL, NULL);

        while (client_status > 0) {
            bzero(client_message, COM_BUFFER_MX_SZ * sizeof(char));
            client_status = read(client_socket, client_message, sizeof(client_message));

            printf("%s", client_message);
            bzero(server_response, sizeof(char) * COM_BUFFER_MX_SZ);

            printf("\n> ");
            fgets(server_response, sizeof(server_response), stdin);

            client_status =  write(client_socket, server_response, sizeof(char) * COM_BUFFER_MX_SZ);
        }

        close(client_socket); 
    }
}

void start_client(char*addr, int port) {

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0) {
        perror("Cannot open socket");
        exit(1);
    }

    struct hostent *server = gethostbyname(addr);

    if (server == NULL) {
        perror("Could not find server :(");
        exit(0);
    }

    struct sockaddr_in server_addr;
    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy((char *) server->h_addr_list, (char *)&server_addr.sin_addr.s_addr, server->h_length);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect to server");
        exit(1);
    }

    char client_message[COM_BUFFER_MX_SZ];
    char server_message[COM_BUFFER_MX_SZ];

    int server_status = 1;
    while (server_status > 0) {
        bzero(client_message, sizeof(client_message));
        bzero(server_message, sizeof(server_message));
        printf("\n> ");
        fgets(client_message, sizeof(client_message), stdin);

        server_status = write(client_socket, client_message, sizeof(client_message));
        server_status = read(client_socket, server_message, sizeof(server_message));

        printf("%s", server_message);
    }

}


int atoiconv(char* num) {
    char* chr = num;
    uint8_t idx = 0;
    while (*chr - '0' >= 0 && *chr - '0' <=9) {
      chr++;
      idx++;
    }

    int res = 0;
    for (int i = 0; i < idx; i++) {
        res += (num[i] - '0') * pow(10, idx - i - 1);
    }

    return res;
}

int main(int argc, char* argv[]) {
    

    if (argc == 4) {
        if (strcmp(argv[1], "-c") == 0) {
            char *addr = argv[2];
            int port = atoiconv(argv[3]);

            printf("%s, %d", addr, port);

            start_client(addr, port);
        }
    }

    if (argc == 3) {
        if (strcmp(argv[1], "-s") == 0) {
            int port = atoiconv(argv[2]);
            printf("%d", port);
            start_server(port);
        }
    }

    if (argc != 3 && argc != 4) {
        printf("-c | -s \n-c -> [addr] -> [port]\n-s -> [port]");
    }


    return 0;
}
