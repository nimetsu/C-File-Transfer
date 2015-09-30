#include <stdio.h>
#include "global.h"
/* start of global variable declaration */
void get_fileName(int, char*);
int send_file(int , char*);
void sig_chld(int);
/* End */

int main(int argc, char* argv[])
{
        int listen_fd, conn_fd, ip_fd;
        struct sockaddr_in srv_addr, cli_addr;
        socklen_t cli_len;
        pid_t child_pid;
        //ifreq ifr;
        char fileName [maxReceiveBuffer];
        char ipAddress [INET_ADDRSTRLEN];

        memset(&srv_addr, 0, sizeof(srv_addr));
        memset(&cli_addr, 0, sizeof(cli_addr));
        srv_addr.sin_family = AF_INET;
        srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // any interface can accept connection

        /* create socket */
        srv_addr.sin_port = (argc > 1) ? htons(atoi(argv[1])) : htons(server_Port);
        if ( (listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                perror("Error! Create socket error.");
                exit(EXIT_FAILURE);
        }
        /* bind to socket */
        if( bind(listen_fd, (struct sockaddr*) &srv_addr, sizeof(srv_addr)) < 0 ){
        perror("Error! Bind failed.");
        exit(EXIT_FAILURE);
        }

        /* server msg */
        //ip_fd = socket(AF_INET, SOCK_DGRAM, 0);
        //ifr.ifr_addr.sa_family = AF_INET;
        //strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
        //ioctl(ip_fd, SIOCGIFADDR, &ifr);
        //printf("Server IP address is %s \n, inet_ntoa(((struct sockaddr_in *)&ifr.ifr$;
        printf("Listening on port %d ...\n", ntohs(srv_addr.sin_port));
        if( listen(listen_fd, listenEnq) < 0 ) {
                perror("Error! Listen failed.");
                exit(EXIT_FAILURE);
        }
        /* install signal handler */
        signal (SIGCHLD, sig_chld);

        for( ; ; ) /* will run until terminated (to wait for connection) */
        {
                cli_len = sizeof(cli_addr);
                printf ("Waiting for a connection...\n\n");

                if ( (conn_fd = accept(listen_fd, (struct sockaddr*) &cli_addr,&cli_len)) < 0 )
                        {
                                perror("accept error");
                                break;
                        }
                inet_ntop(AF_INET, &(cli_addr.sin_addr), ipAddress, INET_ADDRSTRLEN); // numeric ip to n$
                printf("Client connected from %s:%d\n",ipAddress, ntohs(cli_addr.sin_port) );

                if ( (child_pid = fork()) == 0 ) /* forking for multiple connection */
                {
                        close (listen_fd);

                        get_fileName(conn_fd, fileName);
                        send_file(conn_fd, fileName);
                        printf("Closing connection\n");

                        close(conn_fd);
                        exit(0);
                }
                close(conn_fd);
        }
        close(listen_fd);
        return 0;
}

/* get file name func */
void get_fileName(int sock, char* fileName)
{
        char recv_str[maxReceiveBuffer];
        ssize_t rcvd_bytes;

        if ( (rcvd_bytes = recv(sock, recv_str, maxReceiveBuffer, 0)) < 0) {
                perror("Error! Receive error.");
                return;
        }
        sscanf (recv_str, "%s\n", fileName);
}


int send_file(int sock, char *fileName)
{
        int sent_count;
        ssize_t read_bytes, sent_bytes, sent_file_size;
        char send_buf[maxSendBuffer];
        char * errmsg_notfound = "File not found\n";
        int f;
        sent_count = 0;
        sent_file_size = 0;

        if( (f = open(fileName, O_RDONLY)) < 0) /* file not found */
        {
                perror(fileName);
                if( (sent_bytes = send(sock, errmsg_notfound, strlen(errmsg_notfound), 0)) < 0 )
                {
                        perror("Error! Send error.");
                        return -1;
                }
        }
        else /* file found */
        {
                printf("Sending file : %s\n", fileName);
                while( (read_bytes = read(f, send_buf, maxReceiveBuffer)) > 0 )
                {
                        if( (sent_bytes = send(sock, send_buf, read_bytes, 0))< read_bytes )
                        {
                                perror("send error");
                                return -1;
                        }
                        sent_count++;
                        sent_file_size += sent_bytes;
                }
                close(f);
        printf("Transfer completed. Sent %d bytes in %d send(s)\n\n",sent_file_size, sent_count);
        execl("/bin/cat", "cat", fileName, (char*)0);
        //printf("--END--\n");
        return sent_count;
}
/* signal handler */
void sig_chld(int signo)
{
        pid_t procid;
        int stat;

        while ( (procid = waitpid(-1, &stat, WNOHANG)) > 0) //no wait
        printf("Child %d killed\n", procid);
        return;
}


