#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <winsock.h>
#include <string.h>
#define BACKLOG_MAX 5
#define BUFFER_SIZE 128
#define EXIT_CALL_STRING "#quit"

char* ip="192.168.0.110";
unsigned short port = 8080;

int server_socket = 0, client_socket = 0,client_socket_ = 0, client_length = 0, message_length = 0, message_client_length=0;
char message[BUFFER_SIZE];
char message_client[BUFFER_SIZE];
struct sockaddr_in server_address,client_address;
WSADATA wsa_data;

void msg_err_exit(char *msg) {
    fprintf(stderr, msg);
    system("PAUSE");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    //start Winsock 2.0 (DLL), Only for Windows
    if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0)
    	msg_err_exit("WSAStartup() failed\n");
    	
    //creating serverhost socket
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    //if socket is invalid
    if (server_socket == INVALID_SOCKET || client_socket == INVALID_SOCKET) {
        WSACleanup();
        msg_err_exit("socket() failed\n");
    }
    
    fflush(stdin);
    
    //connect server
    memset(&server_address, 0, sizeof (server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);
    
    //connecting socket with serverhost
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof (server_address)) == SOCKET_ERROR) {
        WSACleanup();
        closesocket(server_socket);
        msg_err_exit("bind() failed\n");
    }
    
    //listening connections
    if (listen(server_socket, BACKLOG_MAX) == SOCKET_ERROR) {
        WSACleanup();
        closesocket(server_socket);
        msg_err_exit("listen() failed\n");
    }
    client_length = sizeof (client_address);
    printf("waiting some connection...\n");
    
    //connecting client to server
    memset(&client_address, 0, sizeof (client_address));
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = inet_addr(ip);
    client_address.sin_port = htons(port);
    printf("connecting with server %s...\n", ip);
    
    //connect
    if (connect(client_socket, (struct sockaddr *) &client_address, sizeof (client_address)) == SOCKET_ERROR) {
        WSACleanup();
        msg_err_exit("connect() failed\n");
    }
    
    //server accepts client
    client_socket_ = accept(server_socket, (struct sockaddr *) &client_address, &client_length);
    if (client_socket_ == INVALID_SOCKET) {
        WSACleanup();
        closesocket(server_socket);
        msg_err_exit("accept() failed\n");
    }
    
    //SUCCESS
    printf("connected with %s\n", inet_ntoa(client_address.sin_addr));
    printf("waiting messages...\n");
    
    //infinite loop to send/receive messages
    do {
    	//send message
    	memset(&message_client, 0, BUFFER_SIZE);
   		printf("msg: ");
    	gets(message_client);
   		fflush(stdin);    
    	message_client_length = strlen(message_client);
    	if (send(client_socket_, message_client, message_client_length, 0) == SOCKET_ERROR) {
        	WSACleanup();
        	closesocket(client_socket_);
        	msg_err_exit("send() failed\n");
    	}
    	
    	//receive mesage
        memset(&message, 0, BUFFER_SIZE);
        message_length = recv(client_socket, message, BUFFER_SIZE, 0);
        if (message_length == SOCKET_ERROR)
            msg_err_exit("recv() failed\n");
        printf("%s: %s\n", inet_ntoa(client_address.sin_addr), message);
        
    } while (strcmp(message, EXIT_CALL_STRING));
    
    printf("closed\n");
    WSACleanup();
    closesocket(server_socket);
    closesocket(client_socket);
    system("PAUSE");
    return 0;
}
