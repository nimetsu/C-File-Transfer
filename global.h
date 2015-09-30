
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ioctl.h>
//#include <ioctl.h> // uncomment this if running on non raspbian machine
#include <netinet/in.h>
#include <net/if.h>
#include <sys/socket.h>

#define client_Port 8800 //use port 8800 to connect
#define server_Port 8801
#define listenEnq 5 //max rate connections
#define maxReceiveBuffer 256
#define maxSendBuffer 256
