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

#include "chatroom.h"
using namespace std;

std::vector<chatroom> chatData;
std::vector<int> ports;


struct threadargs{
	int socket;
	int port;
	struct sockaddr_in address;
};

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void to_All(char* messg, char* name, int s_sock){
	//lock data 
	pthread_mutex_lock(&mutex1);
	char buffer[256];
	memset(buffer, 0, 256);
	strcpy(buffer, messg);
	for(int i = 0; i < chatData.size(); i++){
		if(name == chatData[i].room_name){
			for(int j = 0; j < chatData[i].slave_socket.size(); j++){
				int sockets = chatData[i].slave_socket[j];
				if(sockets != s_sock){
					send(chatData[i].slave_socket[j], buffer, strlen(buffer), 0);
				}
			}
		}
	}
	pthread_mutex_unlock(&mutex1);
	
}





int createFunction(char* buffer, int s_sock){
	string msg = "";
	printf("It reached this point");
	pthread_mutex_lock(&mutex1);
	if(chatData.size() >= 25){
		char* sendUnk = "FAILURE UNKNOWN";
		int usrMsg = send(s_sock, sendUnk, sizeof(sendUnk), 0);
	}
	//read name into the buffer
	char subbuff[256];
	memcpy(subbuff, &buffer[7],31);
	subbuff[31] ='\0';
	
	std::string str(subbuff);
	for(int i = 0; i < chatData.size(); i++){
		std::string name(chatData[i].room_name);
		if(str == name){
			//printf("Room exists already: %s\n", name);
			char* sendExists = "FAILURE_ALREADY_EXISTS";
			int usrMsg = send(s_sock, sendExists, strlen(sendExists), 0);
			if(sendExists < 0){
				perror("Send failed");
			}
			pthread_mutex_unlock(&mutex1);
			return 0;
		}
	}
	
	int port = ports[0];
	ports.erase(ports.begin());
	
	int slave_sock = socket(AF_INET, SOCK_STREAM, 0);
	// struct sockaddr_in serveraddr;
 //   memset(&serveraddr, 0, sizeof(serveraddr));
 //   serveraddr.sin_family      = AF_INET;
 //   serveraddr.sin_port        = htons(port);
 //   serveraddr.sin_addr.s_addr = INADDR_ANY;
	chatroom newRoom;
	newRoom.num_members =0;
	newRoom.port_num = port;
	strcpy(newRoom.room_name, subbuff);
	newRoom.slave_socket.push_back(slave_sock);
	chatData.push_back(newRoom);
	
	char* sentSuccess = "SUCCESS";
	int sentmsg = send(s_sock, sentSuccess, sizeof(sentSuccess), 0);
	pthread_mutex_unlock(&mutex1);
	return 1;
}


void deleteFunction(char* buffer, int s_sock){
	string msg = "";
	pthread_mutex_lock(&mutex1);
	int isActive = 0;
	if(chatData.size() == 0){
		char* invalid = "FAILURE_INVALID";
		int sendmsg = send(s_sock, invalid, sizeof(invalid), 0);
	}
	char subbuff[256];
	memcpy(subbuff, &buffer[7],31);
	subbuff[31] ='\0';
	
	std::string str(subbuff);
		for(int i = 0; i < chatData.size(); i++){
			std::string name(chatData[i].room_name);
			if(str == name){
				for(int j = 0; j < chatData[i].slave_socket.size(); j++){
					//close the connection
					close(chatData[i].slave_socket[j]);
				}
				int port = chatData[i].port_num;
				///remove the chat room from the list
				chatData.erase(chatData.begin() + i);
				//make the port available again
				ports.push_back(port);
				std::string msg = "SUCCESS";
	            char msgChar[256];
	            strcpy(msgChar, msg.c_str());
			
				int rc = send(s_sock, msgChar, sizeof(msgChar), 0);
				if(rc < 0){
					printf("Failed to reply: CREATE");
				}
				printf("Chatroom deleted successfully\n");
				isActive = 1;
				pthread_mutex_unlock(&mutex1);
				break;
			}
		}
		if(isActive = 0){
			char* sentFailure = "FAILURE_NOT_EXISTS";
			int sentmsg = send(s_sock, sentFailure, sizeof(sentFailure), 0);
			printf("Failed to delete chatroom\n");
		}
		pthread_mutex_unlock(&mutex1);
	}
	
int listFunction(char* buffer, int s_sock){
	//printf("It is failing here");
	if(chatData.size() > 0){
		//make a reply message
		std::string msg = "";
		for(int i = 0; i < chatData.size(); i++){
			std::string tname(chatData[i].room_name);
			if(i < chatData.size()- 1){
				msg += tname + ", ";
			}
			else{
				msg += tname;
			}
		}
		
		char msgChar[256];
		strcpy(msgChar, msg.c_str());
        
        int rc = send(s_sock, msgChar, sizeof(msgChar), 0);
        if(rc < 0){
        	perror("Failed to reply: LIST command");
        }
	}
        else{

	        std::string msg = "No Active Chat Rooms";
	        char msgChar[256];
	        strcpy(msgChar, msg.c_str());
	
	        int rc = send(s_sock, msgChar, sizeof(msgChar), 0);
	        // test error rc < 0
	        if(rc < 0) {
	            perror("Failed to reply: LIST Command");
	        }

        }
        return 1;
	}
	


int joinFunction(char* buffer, int s_sock){
		// char subbuff[256];
		// memcpy(subbuff, &buffer[5],31);
		// subbuff[31] ='\0';
		vector<char*> splitcommand;
	    char* token = strtok(buffer, " ");
	    while (token != NULL){
	        splitcommand.push_back(token);
	        token = strtok(NULL, " ");
	    }
	    char* name = splitcommand[1];
		printf("Name of the room is: and %s\n", name);
		// std::string str(subbuff);
		//printf("Room trying to join: %s\n", str);
		int nameTarget = -1;
		printf("Number of rooms: %d\n", chatData.size());
		for(int i = 0; i < chatData.size(); i++) {
			//std::string chatroomName(chatData[i].room_name);
			printf("Name or room: %s\n",name);
			if(strcmp(name, chatData[i].room_name) == 0) {
				nameTarget = i;
			}
		}
		printf("Value of nameTarget: %d\n", nameTarget);
		string msg ="";
		printf("Handle join\n");
		
		
		if(nameTarget > -1){
				printf("It reaches the success");
				chatData.at(nameTarget).num_members+=1;
				msg = "SUCCESS";
				msg += " ";
				//chatData[nameTarget].num_members++;
				msg += to_string(chatData[nameTarget].num_members);
				printf("Number of memebers: %d\n", chatData[nameTarget].num_members);
				msg += " ";
				msg += to_string(chatData[nameTarget].port_num);
				
				chatData[nameTarget].slave_socket.push_back(s_sock);
				
				int sentmsg = send(s_sock, msg.c_str(), strlen(msg.c_str()), 0);
				
				// pthread_t handler_thread;
				// threadargs tinfo;
				// tinfo.socket = s_sock;
				// tinfo.port = chatData[i].port_num;
				// return 1;
				
				//return 0;
		}
		else{
			printf("It reaches the fail");
			char* sendFail = "FAILURE_NOT_EXISTS";
			int sendmsg = send(s_sock, sendFail, strlen(sendFail), 0);
			
		}
	
		return 1;
	
		
}
int parse_command(char* buffer, int s_sock){
    //printf("Parse_command: Received Buffer: %s\n", buffer);
    if (strncmp(buffer, "CREATE", 5) == 0){
        createFunction(buffer, s_sock);
        return 1;
    } else if (strncmp(buffer, "DELETE", 6) == 0){
        deleteFunction(buffer, s_sock);
        return 1;
    } else if (strncmp(buffer, "JOIN", 4) == 0){
        return joinFunction(buffer, s_sock);
    } else if (strncmp(buffer, "LIST", 4) == 0){
        listFunction(buffer, s_sock);
        return 1;
    }
    return 1;
}

void *connection_handler(void *threadinfo){
	//printf("It is failing here");
	threadargs* t = (threadargs*) threadinfo;
	int sock = t->socket;
	int port = t->port;
	char client_message[256];
	char command_buffer[256];
	int rc = 0;
	int rs = 0;
	
	while(1){
		rc = recv(sock, client_message, sizeof(client_message), 0);
		if(rc < 0){
			perror("Recieve failed");
		}
		//memcpy(command_buffer, client_message, sizeof(client_message));
		if(!rc){
			continue;
		}
		memcpy(command_buffer, client_message, sizeof(client_message));
		if(!parse_command(client_message, sock)){
			break;
		}
	}

		
	char subbuff[256];
	memcpy(subbuff, &command_buffer[7],31);
	subbuff[31] ='\0';
	
	std::string name(subbuff);
	char nametoChar[256];
	strcpy(nametoChar, name.c_str());
	memset(client_message, 0, 256);
	while(1){
		rs = recv(sock, client_message, 256, 0);
		if(rs < 0){
			perror(" this Recieve failed");
		}
		if(!rs){
			continue;
		}
		to_All(client_message, nametoChar, sock);
		memset(client_message, 0, 256);
	}
	return 0;
	
}

int passiveTCPsock(int port, int backlog){
    
    //creating a master socket
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family      = AF_INET;
	serveraddr.sin_port        = htons(port);
	serveraddr.sin_addr.s_addr = INADDR_ANY;
    int m_sock = socket(AF_INET, SOCK_STREAM, 0);
   
    if(m_sock < 0){
        perror("Failed to create a socket");
    }
    
	
	
	
	int rc = bind(m_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if(rc < 0) {
		perror("Failed to bind socket");
	}
	rc = listen(m_sock, backlog);
	if(rc < 0) {
		perror("Failed to listen on socket");
	} 
	
	
	return m_sock;
    
}


int main(int argc, char** argv){
    int m_sock, s_sock;      /* master and slave socket     */
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_size = sizeof(struct sockaddr_in);
    int rc = 1;
    int port = atoi(argv[1]);
    m_sock = passiveTCPsock(port, 32);
    char buffer[256];
    for (int i = port; i <= port + 25; i++){
        ports.push_back(i);
    }
    printf("Ready for connection: \n");

	while(true){

        s_sock = accept(m_sock, (struct sockaddr*)&peer_addr, &peer_addr_size);
        if (s_sock < 0){
            perror("Socket not accepted");
        }
        
        pthread_t handler_thread;
        threadargs *threadinfo = (threadargs*)malloc(sizeof(threadargs));
        memcpy(&threadinfo->address, &peer_addr, sizeof(struct sockaddr_in));
        threadinfo->socket = s_sock;
        threadinfo->port = port;
        pthread_create(&handler_thread, NULL, connection_handler, (void*) threadinfo);
        
    }
}