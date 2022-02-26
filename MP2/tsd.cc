#include <ctime>
#include <time.h>
#include <chrono>
#include <thread>
#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/duration.pb.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <google/protobuf/util/time_util.h>
#include <grpc++/grpc++.h>
#include <sys/wait.h>
#include "sns.grpc.pb.h"


using google::protobuf::Timestamp;
using google::protobuf::Duration;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using csce438::Message;
using csce438::Request;
using csce438::Reply;
using csce438::SNSService;


/*
export MY_INSTALL_DIR=$HOME/.grpc

export PATH="$MY_INSTALL_DIR/bin:$PATH"

export PKG_CONFIG_PATH=$MY_INSTALL_DIR/lib/pkgconfig/

export PKG_CONFIG_PATH=$MY_INSTALL_DIR/lib64/pkgconfig:$PKG_CONFIG_PATH
*/

using namespace std;

typedef struct Client{
  
  string username;
  bool connected = false;
  vector<Client> followers;
  vector<Client> following;
  // vector<PostData> posts;
  int following_file_size = 0;
  ServerReaderWriter<Message, Message>* stream = 0;
  
} Client;

vector<Client> client_db;

// int find_user(std::string username){
//   int index = 0;
//   for(Client c : client_db){
//     if(c.username == username && c.connected == true){
//       return index;
//     }
//   }
//   return -1;
// }

class SNSServiceImpl final : public SNSService::Service {
  
  public: 
  
    string serverAddress = "error";
    
    
  private:
  
    int find_user(std::string username){
      int index = 0;
      for(int i = 0; i < client_db.size(); i++){
         if (client_db[i].username == username){
          return i;
        }
      }
      return -1;
    }
    
    Status List(ServerContext* context, const Request* request, Reply* reply) override {
      
      // ------------------------------------------------------------
      // In this function, you are to write code that handles 
      // LIST request from the user. Ensure that both the fields
      // all_users & following_users are populated
      // ------------------------------------------------------------
      //Client user = client_db[find_user(request->username())];
      //cout << "Printing list for " << user.username << endl;
      int index = find_user(request->username());
      Client user = client_db[index];
      for (int i = 0; i < client_db.size(); i++){
        reply->add_all_users(client_db[i].username);
      }
      for(int i = 0; i < user.following.size(); i++){
        reply->add_following_users(user.following[i].username);
      }
      reply->add_following_users(request->username());
      return Status::OK;
    }
  
    Status Follow(ServerContext* context, const Request* request, Reply* reply) override {
      // ------------------------------------------------------------
      // In this function, you are to write code that handles 
      // request from a user to follow one of the existing
      // users
      // ------------------------------------------------------------
      string username = request->username();
      string userTwo = request->arguments(0);
      
      
      
      int user_index = find_user(username);
      int indexTwo = find_user(userTwo);
      
      if(indexTwo < 0){
        reply->set_msg("Follow failed. Invalid user");
      }
      else{
        Client* userTwoClient = &client_db[indexTwo];
        Client* userOneClient = &client_db[user_index];
        
        for(int i = 0; i < client_db[user_index].following.size(); i++){
          if(userTwoClient->username == client_db[user_index].following[i].username){
            reply->set_msg("Follow failed. Already following.");
            return Status::OK;
          }
        }
        userOneClient->following.push_back(*userTwoClient);
        userTwoClient->followers.push_back(*userOneClient);
        reply->set_msg("followed user");
      }
      
      return Status::OK; 
    }
  
    Status UnFollow(ServerContext* context, const Request* request, Reply* reply) override {
      // ------------------------------------------------------------
      // In this function, you are to write code that handles 
      // request from a user to unfollow one of his/her existing
      // followers
      // ------------------------------------------------------------
      string username = request->username();
      string userTwo = request->arguments(0);
      
      int indexOne = find_user(username);
      int indexTwo = find_user(userTwo);
      
      if(indexTwo < 0 || username == userTwo){
        reply->set_msg("Unfollow failed. Invalid user");
      }
      else{
        int indexinOthers = 0;
        for(int i = 0; i < client_db[indexTwo].followers.size(); i++){
          if (client_db[indexTwo].followers[i].username == username){
            indexinOthers = i;
            break;
          }
        }
        
        for(int i = 0; i < client_db[indexOne].following.size(); i++){
          if(userTwo == client_db[indexOne].following[i].username){
            client_db[indexOne].following.erase(client_db[indexOne].following.begin() + i);
            client_db[indexTwo].followers.erase(client_db[indexTwo].followers.begin() + indexinOthers);
            reply->set_msg("Unfollowed Usr.");
            return Status::OK;
          }
        }
        reply->set_msg("Unfollow failed");
      }
      
      return Status::OK;
    }
    
    Status Login(ServerContext* context, const Request* request, Reply* reply) override {
      // ------------------------------------------------------------
      // In this function, you are to write code that handles 
      // a new user and verify if the username is available
      // or already taken
      // ------------------------------------------------------------
      Client c;
      string username = request->username();
      int index = find_user(username);
      if(index < 0){
        c.username = username;
        c.connected = true;
        client_db.push_back(c);
        cout << "Username available. Creating user and logging in..." << endl;
        reply->set_msg("Creating user and logging in...");
      }
      else{
        if(!client_db[index].connected){
          cout << "Attempting to login to existing user..." << endl;
          reply->set_msg("Attempting to login to existing user...");
        }else{
          cout << "Username taken" << endl;
          reply->set_msg("Username taken");
        }
      }
      return Status::OK;
    }
    
    
  
    Status Timeline(ServerContext* context, ServerReaderWriter<Message, Message>* stream) override {
      // ------------------------------------------------------------
      // In this function, you are to write code that handles 
      // receiving a message/post from a user, recording it in a file
      // and then making it available on his/her follower's streams
      // ------------------------------------------------------------
      Message m;
      int user_index;
      string username = "";
      string filename = "";
      bool linesRemain = true;
      while(stream->Read(&m)){
        username = m.username();
        user_index = find_user(username);
        
        //filename = username + ".txt";
        //ofstream user_file(filename);
        
        google::protobuf::Timestamp timestamp = m.timestamp();
        
        string timeStr = google::protobuf::util::TimeUtil::ToString(timestamp);
        string fullmsg = m.username() + "(" + timeStr + ") >> " + m.msg();
        
        if (client_db[user_index].stream == 0){
          client_db[user_index].stream = stream;
        }
        
        if(m.msg() == "start list"){
           if (linesRemain){
            string line = "";
            ifstream read_file(username + "_following.txt");
            int count = 0;
            Message sent_msg;
            while (getline(read_file, line) && count < 20){
              sent_msg.set_msg(line);
              stream->Write(sent_msg);
              count++;
              // newest_twenty.push_back(line);
            }
            linesRemain = false;
            Message new_msg; 
            
          }
        }
        
       
        
        
        // //Send the newest messages to the client to be displayed
       
        
        for (int i = 0; i < client_db[user_index].followers.size(); i++){
          int indexTwo = find_user(client_db[user_index].followers[i].username);
          string userTwo = client_db[indexTwo].username;
          string otherfilename = userTwo + "_following.txt";
          if (client_db[indexTwo].stream != 0){
            client_db[indexTwo].stream->Write(m);
          }
          ofstream other_file(otherfilename, ofstream::out | ofstream::in | ofstream::app);
          other_file << fullmsg;
        }
      }
      return Status::OK;
    }
  
  };
  
  
  void RunServer(std::string port_no) {
    // ------------------------------------------------------------
    // In this function, you are to write code 
    // which would start the server, make it listen on a particular
    // port number.
    // ------------------------------------------------------------
    string server_addr = "localhost:" + port_no;
    SNSServiceImpl service;
    service.serverAddress = server_addr;
    
    ServerBuilder builder;
    builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_addr << std::endl;
    server->Wait();
    
   
    
  }

int main(int argc, char** argv) {
  
  std::string port = "3010";
  int opt = 0;
  while ((opt = getopt(argc, argv, "p:")) != -1){
    switch(opt) {
      case 'p':
          port = optarg;
          break;
      default:
	         std::cerr << "Invalid Command Line Argument\n";
    }
  }
  RunServer(port);
  return 0;
}
