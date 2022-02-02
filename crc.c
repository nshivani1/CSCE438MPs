#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <bits/stdc++.h>


#include <errno.h>
#include <pthread.h>

using namespace std;
using std::cout;
using std::endl;


/*
 * TODO: IMPLEMENT BELOW THREE FUNCTIONS
 */
int connect_to(const char *host, const int port);
struct Reply process_command(const int sockfd, char* command);
void process_chatmode(int sockfd);
bool keepRunning = true;
int main(int argc, char** argv) 
{
	if (argc != 3) {
		fprintf(stderr,
				"usage: enter host address and port number\n");
		exit(1);
	}

    display_title();
    
	while (1) {
	
		int sockfd = connect_to(argv[1], atoi(argv[2]));
    
		char command[MAX_DATA];
        get_command(command, MAX_DATA);

		struct Reply reply = process_command(sockfd, command);
		display_reply(command, reply);
		
		touppercase(command, strlen(command) - 1);
		if (strncmp(command, "JOIN", 4) == 0 && reply.status == SUCCESS) {
			printf("Now you are in the chatmode\n");
			process_chatmode(sockfd);
		}
	
		close(sockfd);
    }

    return 0;
}

/*
 * Connect to the server using given host and port information
 *
 * @parameter host    host address given by command line argument
 * @parameter port    port given by command line argument
 * 
 * @return socket fildescriptor
 */
int connect_to(const char *host, const int port)
{
	// ------------------------------------------------------------
	// GUIDE :
	// In this function, you are suppose to connect to the server.
	// After connection is established, you are ready to send or
	// receive the message to/from the server.
	// 
	// Finally, you should return the socket fildescriptor
	// so that other functions such as "process_command" can use it
	// ------------------------------------------------------------

    // below is just dummy code for compilation, remove it.
    
    int sockfd;
    if((sockfd =socket(AF_INET,SOCK_STREAM,0)) < 0){
    	perror("Failed to open socket");
    	exit(-1);
    }
    	
    //struct sockaddr_in serveraddr;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
    
    
 //   serveraddr.sin_family      = AF_INET;
	// serveraddr.sin_port        = htons(port);
	//serveraddr.sin_addr.s_addr = inet_addr(host);
	
	if (inet_pton(AF_INET, host, &addr.sin_addr) < 0){
        perror("Hostname error: ");
        exit(-1);
	}
	
	//connect to server
	int conn = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));
	if(conn < 0){
		printf("Connection error");
	}
    
	return sockfd;
}

/* 
 * Send an input command to the server and return the result
 *
 * @parameter sockfd   socket file descriptor to commnunicate
 *                     with the server
 * @parameter command  command will be sent to the server
 *
 * @return    Reply    
 */
struct Reply process_command(const int sockfd, char* command)
{
	
	// ------------------------------------------------------------
	// GUIDE 1:
	// In this function, you are supposed to parse a given command
	// and create your own message in order to communicate with
	// the server. Surely, you can use the input command without
	// any changes if your server understand it. The given command
    // will be one of the followings:
	//
	// CREATE <name>
	// DELETE <name>
	// JOIN <name>
    // LIST
	//
	// -  "<name>" is a chatroom name that you want to create, delete,
	// or join.
	// 
	// - CREATE/DELETE/JOIN and "<name>" are separated by one space.
	// ------------------------------------------------------------

	//read into buffer
	//printf("Process_command: %s\n", command);
	char buffer[256];
	char recbuf[256];
	strcpy(buffer, command);
	// ------------------------------------------------------------
	// GUIDE 2:
	// After you create the message, you need to send it to the
	// server and receive a result from the server.
	// ------------------------------------------------------------
	int rc = send(sockfd, buffer, sizeof(buffer), 0);
	if(rc < 0) 
		perror("Send Unsuccesful");
		
	// ------------------------------------------------------------
	// GUIDE 3:
	// Then, you should create a variable of Reply structure
	// provided by the interface and initialize it according to
	// the result.
	//
	// For example, if a given command is "JOIN room1"
	// and the server successfully created the chatroom,
	// the server will reply a message including information about
	// success/failure, the number of members and port number.
	// By using this information, you should set the Reply variable.
	// the variable will be set as following:
	//
	// Reply reply;
	// reply.status = SUCCESS;
	// reply.num_member = number;
	// reply.port = port;
	// 
	// "number" and "port" variables are just an integer variable
	// and can be initialized using the message fomr the server.
	//
	// For another example, if a given command is "CREATE room1"
	// and the server failed to create the chatroom becuase it
	// already exists, the Reply varible will be set as following:
	//
	// Reply reply;
	// reply.status = FAILURE_ALREADY_EXISTS;
    // 
    // For the "LIST" command,
    // You are suppose to copy the list of chatroom to the list_room
    // variable. Each room name should be seperated by comma ','.
    // For example, if given command is "LIST", the Reply variable
    // will be set as following.
    //
    // Reply reply;
    // reply.status = SUCCESS;
    // strcpy(reply.list_room, list);
    // 
    // "list" is a string that contains a list of chat rooms such 
    // as "r1,r2,r3,"
	// ------------------------------------------------------------

	// REMOVE below code and write your own Reply.
	struct Reply reply;
	memset(recbuf, 0, 256);
	//JOIN COMMAND
	//checking if the command is JOIN
	if(strncmp(buffer, "JOIN", 4) == 0){
		//char cs[250];
		//recieve reply into char
		int rc = recv(sockfd, recbuf, sizeof(recbuf), 0);
       
        //printf("process_command JOIN RECEIVE AFTER\n");
        std::string replyInfo(recbuf);
        std::vector <std::string> tokens; 
      
		// stringstream class check1 
		std::stringstream check1(replyInfo); 
		std::string intermediate;  
		//tokenize to parse
		while(getline(check1, intermediate, ',')) 
		{ 
			tokens.push_back(intermediate); 
		}
		
		// Get and set status
		std::string status(tokens.front());
        
        // if (strncmp(status, "FAILURE_NOT_EXISTS", 18) == 0){
        //     reply.status = FAILURE_NOT_EXISTS;
        //     reply.port = -1;
        //     reply.num_member = 0;
        // } else if (strncmp(status, "SUCCESS", 7) == 0){
        //     reply.status = SUCCESS;
        //     reply.port = atoi(port);
        //     reply.num_member = atoi(numM);
        // }
        if(status == "FAILURE_NOT_EXISTS"){
        	 reply.status = FAILURE_NOT_EXISTS;
	         reply.port = -1;
	         reply.num_member = 0;
        }
        else if(status == "SUCCESS") {
        	reply.status = SUCCESS;
			reply.port = std::stoi(tokens.at(1));
			reply.num_member = std::stoi(tokens.at(2));
        }
	}
	
	else if(strncmp(buffer, "CREATE", 6) == 0){
		// printf("it is reaching this point");
		int rc = recv(sockfd, recbuf, sizeof(recbuf), 0);
		if(rc < 0){
			perror("Recieve unsuccesful");
		}
		string status(recbuf);
		// printf("Status %s\n",status);
		if(status == "FAILURE_ALREADY_EXISTS") {
			reply.status = FAILURE_ALREADY_EXISTS;
		}
		else if(status == "FAILURE_INVALID") {
			reply.status = FAILURE_INVALID;
		}
		else if(status == "SUCCESS") {
			reply.status = SUCCESS;
		}
		else {
			reply.status = FAILURE_UNKNOWN;
		}
		
	}
	
	else if(strncmp(buffer, "DELETE", 6) == 0){
		//char cs[250];
		int rc = recv(sockfd, recbuf, sizeof(recbuf), 0);
		if(rc < 0){
			perror("Recieve unsuccesful");
		}
		string status(recbuf);
		if(status == "FAILURE_NOT_EXISTS") {
			reply.status = FAILURE_NOT_EXISTS;
		}
		else if(status == "FAILURE_INVALID") {
			reply.status = FAILURE_INVALID;
		}
		else if(status == "SUCCESS") {
			reply.status = SUCCESS;
		}
		else {
			reply.status = FAILURE_UNKNOWN;
		}
		
	}
	
	else if(strncmp(buffer, "LIST", 4) == 0){
		//char list[250];
		int rc = recv(sockfd, recbuf, sizeof(recbuf), 0);
		if(rc < 0){
			perror("Recieve unsuccesful");
		}
		if(strcmp(recbuf, "No list") == 0){
			reply.status = FAILURE_INVALID;
			char* lnf = "List not found";
			strcpy(reply.list_room, lnf);
		}
		else{
			reply.status = SUCCESS;
			strcpy(reply.list_room, recbuf);
		}
		
	}
	
	else{
		reply.status = FAILURE_INVALID;
	}
	
	
	// reply.status = SUCCESS;
	// reply.num_member = 5;
	// reply.port = 1024;
	// return reply;
	close(sockfd);
	return reply;
}

//UNUSED

//https://codereview.stackexchange.com/questions/151044/socket-client-in-c-using-threads
// struct socketStruct{
// 	int sock;
	
// };

// void* recvThreadFunction(void* socket){
// 	char recvMsg[250];
// 	while(1){
// 		if(recv(sockfd->sock, recvMsg, sizeof(recvMsg),0) < 0){
// 			perror("Recieve Unsuccesful");
// 		}
// 		display_message(recvMsg);
// 		string msg(recvMsg);
// 		if(msg == "Warning the chat room is now closing...\n"){
// 			keepRunning = false;
// 			close(sockfd -> sock);
// 			pthread_exit(NULL);
// 		}
// 	}
// }

// void* sendThreadFunction(void* socket) {
// 	char sendMsg[250];
// 	while(1) {
// 		get_message(sendMsg, 250);
// 		if(send(sockfd->sock, sendMsg, sizeof(sendMsg), 0) < 0) {
// 			perror("Failed to send msg\n");
// 		}
// 	} 
// }

/* 
 * Get into the chat mode
 * 
 * @parameter host     host address
 * @parameter port     port
 */
void process_chatmode(int sockfd)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In order to join the chatroom, you are supposed to connect
	// to the server using host and port.
	// You may re-use the function "connect_to".
	// ------------------------------------------------------------
	// pthread_t recvThread;
	// pthread_t sendThread;
	
	// int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	// if (sockfd < 0) 
	// 	perror("ERROR opening socket");
	
	// struct sockaddr_in serveraddr;

	// memset(&serveraddr, 0, sizeof(serveraddr));
	// serveraddr.sin_family      = AF_INET;
	// serveraddr.sin_port        = htons(port);
	// serveraddr.sin_addr.s_addr = inet_addr(host);

	// int err = connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
	// if(err == 0) { 
	//int sockfd = connect_to(host, port);
	printf("Connection Successfull\n");
	char sendbuffer[256];
    char recbuffer[256];
    while(1){
    	memset(sendbuffer, 0, 256);
    	memset(recbuffer, 0 , 256);
    	printf("Enter a message:\n");
    	get_message(sendbuffer, 256);
    	int sendmsg = send(sockfd, sendbuffer, sizeof(sendbuffer), 0);
        int recmsg = recv(sockfd, recbuffer, sizeof(recbuffer), 0);
        display_message(recbuffer);
        printf("\n");
    }
    close(sockfd);
    printf("Exiting chat..\n");
    return;
	// ------------------------------------------------------------
	// GUIDE 2:
	// Once the client have been connected to the server, we need
	// to get a message from the user and send it to server.
	// At the same time, the client should wait for a message from
	// the server.
	// ------------------------------------------------------------
	// socketStruct *data = new socketStruct();
	// data->sock = sockfd;
	
	// pthread_create(&recvThread, NULL, recvThreadFunction, (void*)data);
	// pthread_create(&sendThread, NULL, sendThreadFunction, (void*)data);
	// while(keepRunning) {
	// }
	// pthread_cancel(sendThread);
	// return;
	
    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    // 1. To get a message from a user, you should use a function
    // "void get_message(char*, int);" in the interface.h file
    // 
    // 2. To print the messages from other members, you should use
    // the function "void display_message(char*)" in the interface.h
    //
    // 3. Once a user entered to one of chatrooms, there is no way
    //    to command mode where the user  enter other commands
    //    such as CREATE,DELETE,LIST.
    //    Don't have to worry about this situation, and you can 
    //    terminate the client program by pressing CTRL-C (SIGINT)
	// ------------------------------------------------------------
}

