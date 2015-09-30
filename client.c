
#include <stdio.h>
#include "global.h"
/* global var */
int recv_file(int ,char*);
/* end of global var */

int main(int argc, char* argv[]) // accepting variable upon execution
{
        int sock_fd;
        struct sockaddr_in srv_addr;
        if (argc < 2)
        {
                printf("Usage: %s <file.extension> <IP> <port> \n", argv[0]);
                exit(EXIT_FAILURE);
        }
        memset(&srv_addr, 0, sizeof(srv_addr));

        /* create socket */
        sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        srv_addr.sin_family = AF_INET;

        /* ip from execution parameter convert to numeric */
        if ( inet_pton(AF_INET, argv[2], &(srv_addr.sin_addr)) < 1 )
        {
                printf("Invalid IP address\n");
                exit(EXIT_FAILURE);
        }
        /* if port number supplied, use it, otherwise use Port */
        srv_addr.sin_port = (argc > 3) ? htons(atoi(argv[3])) : htons(client_Port);
        if( connect(sock_fd, (struct sockaddr*) &srv_addr, sizeof(srv_addr)) < 0 )
        {
                perror("Error! Connect failed ");
                exit(EXIT_FAILURE);
        }
        printf("Connected to :%s:%d ..\n",argv[2],client_Port);
        recv_file(sock_fd, argv[1]);

        if(close(sock_fd) < 0) // to close socket
        {
                perror("Error! Can not close socket. Ctrl+Z to terminate.");
                exit(EXIT_FAILURE);
        }
        return 0;
}

/* function to sent file */
int recv_file(int sock, char* file_name)
{
        char send_str [maxSendBuffer];
        int f;
        ssize_t sent_bytes, rcvd_bytes, rcvd_file_size;
        int recv_count;
        char recv_str[maxReceiveBuffer];
        size_t send_strlen;
        sprintf(send_str, "%s\n", file_name);
        send_strlen = strlen(send_str);

        if( (sent_bytes = send(sock, file_name, send_strlen, 0)) < 0 )
        {
                perror("Error! Send Failed ");
                return -1;
        }
        
        if ( (f = open(file_name, O_WRONLY|O_CREAT, 0644)) < 0 )
        {
                perror("Error creating file, check permission!");
                return -1;
        }
        recv_count = 0;
        rcvd_file_size = 0;

        while ( (rcvd_bytes = recv(sock, recv_str, maxReceiveBuffer, 0)) > 0 )
        {
                recv_count++;
                rcvd_file_size += rcvd_bytes;
                if (write(f, recv_str, rcvd_bytes) < 0 )
                {
                        perror("Error: Cannot append file.");
                        return -1;
                }
        }
        close(f);
        printf("Client received: %d bytes in %d recv(s)\n", rcvd_file_size,
        recv_count);
        return rcvd_file_size;
}
