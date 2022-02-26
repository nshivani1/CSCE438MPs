#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <unistd.h>
#include <grpc++/grpc++.h>
#include "client.h"
#include "sns.grpc.pb.h"

using namespace std;


using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using csce438::Message;
//using csce438::Posting;
//using csce438::ListReply;
using csce438::Request;
//using csce438::ServerRequest;
using csce438::Reply;
using csce438::SNSService;

class Client : public IClient
{
    public:
        Client(const std::string& hname,
               const std::string& uname,
               const std::string& p)
            :hostname(hname), username(uname), port(p)
            {}
    protected:
        virtual int connectTo();
        virtual IReply processCommand(std::string& input);
        virtual void processTimeline();
    private:
        std::string hostname;
        std::string username;
        std::string port;
        
        // You can have an instance of the client stub
        // as a member variable.
        std::unique_ptr<SNSService::Stub> stub_;
        IReply Login();
        IReply Route();
        IReply List();
        IReply Follow(const std::string& temp_username);
        IReply UnFollow(const std::string& temp_username);
        void Timeline(const std::string& username);

        
        
};

int main(int argc, char** argv) {

    std::string hostname = "localhost";
    std::string username = "default";
    std::string port = "3010";
    int opt = 0;
    while ((opt = getopt(argc, argv, "h:u:p:")) != -1){
        switch(opt) {
            case 'h':
                hostname = optarg;break;
            case 'u':
                username = optarg;break;
            case 'p':
                port = optarg;break;
            default:
                std::cerr << "Invalid Command Line Argument\n";
        }
    }

    Client myc(hostname, username, port);
    // You MUST invoke "run_client" function to start business logic
    myc.run_client();

    return 0;
}

int Client::connectTo()
{
    string login_info = hostname + ":" + port;
    stub_ = unique_ptr<SNSService::Stub>(SNSService::NewStub(grpc::CreateChannel(login_info, grpc::InsecureChannelCredentials())));
    //IReply ire = Route();
    IReply ire;
    Request req;
    Reply reply;
    ClientContext context;
    req.set_username(username);
    Status status = stub_->Login(&context, req, &reply);
    
   // ire = Login();
   ire.grpc_status= status;
//     if(!ire.grpc_status.ok()) {
// 		cout << "Failed logging in" << endl;
//         return -1;
//     }
    
    if(reply.msg() == "Username is taken"){
        return -1;
    }
    
	// ------------------------------------------------------------
    // In this function, you are supposed to create a stub so that
    // you call service methods in the processCommand/porcessTimeline
    // functions. That is, the stub should be accessible when you want
    // to call any service methods in those functions.
    // I recommend you to have the stub as
    // a member variable in your own Client class.
    // Please refer to gRpc tutorial how to create a stub.
	// ------------------------------------------------------------

    return 1; // return 1 if success, otherwise return -1
}

IReply Client::processCommand(std::string& input)
{
    
    IReply ire;
    string cmd = "";
    string user = "";
    ClientContext context;
    Request request;
    Reply reply;
    size_t index = input.find_first_of(" ");
    if(index != string::npos){
        cmd = input.substr(0, index);
        //cout << "This is the command:" << cmd << endl;
        user = input.substr(index+1, (input.length()-index));
        //cout << "This is the user to follow: " << user << endl;
        if (cmd == "FOLLOW") {
           return Follow(user);
        } 
        else if(cmd == "UNFOLLOW") {
            return UnFollow(user);
        }
    }
    else{
        cmd = input.substr(0, index);
        if(cmd == "LIST"){
            return List();
        }
        else if(input == "TIMELINE"){
            ire.comm_status = SUCCESS;
        }
    }
 
	// ------------------------------------------------------------
	// GUIDE 1:
	// In this function, you are supposed to parse the given input
    // command and create your own message so that you call an 
    // appropriate service method. The input command will be one
    // of the followings:
	//
	// FOLLOW <username>
	// UNFOLLOW <username>
	// LIST
    // TIMELINE
	//
	// ------------------------------------------------------------
	
    // ------------------------------------------------------------
	// GUIDE 2:
	// Then, you should create a variable of IReply structure
	// provided by the client.h and initialize it according to
	// the result. Finally you can finish this function by returning
    // the IReply.
	// ------------------------------------------------------------
    
	// ------------------------------------------------------------
    // HINT: How to set the IReply?
    // Suppose you have "Follow" service method for FOLLOW command,
    // IReply can be set as follow:
    // 
    //     // some codes for creating/initializing parameters for
    //     // service method
    //     IReply ire;
    //     grpc::Status status = stub_->Follow(&context, /* some parameters */);
    //     ire.grpc_status = status;
    //     if (status.ok()) {
    //         ire.comm_status = SUCCESS;
    //     } else {
    //         ire.comm_status = FAILURE_NOT_EXISTS;
    //     }
    //      
    //      return ire;
    // 
    // IMPORTANT: 
    // For the command "LIST", you should set both "all_users" and 
    // "following_users" member variable of IReply.
    // ------------------------------------------------------------
    return ire;
}

IReply Client::List(){
    Request request;
    
    
    ClientContext context;
    Reply reply;
    IReply ire;
    request.set_username(username);
    Status status = stub_->List(&context, request, &reply);
    ire.grpc_status = status;
    if(status.ok()){
        ire.comm_status = SUCCESS;
        for(string user: reply.all_users()){
            ire.all_users.push_back(user);
        }
        for(string user: reply.following_users()){
            ire.following_users.push_back(user);
        }
    }
    else{
        ire.comm_status = FAILURE_UNKNOWN;
    }
    
    return ire;
    
    
}

// IReply Client::Login() {
//     //IReply ire;
//     Request request;
//     request.set_username(username);
//     Reply reply;
//     ClientContext context;
//     Status status = stub_->Login(&context, request, &reply);
    
    
//     IReply ire;
//     ire.grpc_status = status;
//     if (reply.msg() == "Invalid Username") {
//         ire.comm_status = FAILURE_ALREADY_EXISTS;
//     } else {
//         ire.comm_status = SUCCESS;
//     }
    
//     return ire;
// }
IReply Client::Follow(const string& temp_username){
       Request request;
       ClientContext context;
       Reply reply;
       IReply ire;
       request.set_username(username);
       request.add_arguments(temp_username);
       Status status = stub_->Follow(&context, request, &reply);
        ire.grpc_status = status;
        if (reply.msg() == "Follow failed. Invalid user.") {
                ire.comm_status = FAILURE_INVALID_USERNAME;
            } else if (reply.msg() == "Follow failed. Already following.") {
                ire.comm_status = FAILURE_ALREADY_EXISTS;
            } else if (reply.msg() == "followed user") {
                ire.comm_status = SUCCESS;
            } else {
                ire.comm_status = FAILURE_UNKNOWN;
            }
    return ire;
}

IReply Client::UnFollow(const std::string& temp_username){
        Request request;
       ClientContext context;
       Reply reply;
       IReply ire;
       request.set_username(username);
       request.add_arguments(temp_username);
       Status status = stub_->UnFollow(&context, request, &reply);
        ire.grpc_status = status;
   if (reply.msg() == "Unfollow failed. Invalid user" || reply.msg() == "Unfollow failed") {
        ire.comm_status = FAILURE_INVALID_USERNAME;
    } else if (reply.msg() == "Unfollowed Usr.") {
        ire.comm_status = SUCCESS;
    } else {
        ire.comm_status = FAILURE_UNKNOWN;
    }
}


void Client::processTimeline()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to get into timeline mode.
    // You may need to call a service method to communicate with
    // the server. Use getPostMessage/displayPostMessage functions
    // for both getting and displaying messages in timeline mode.
    // You should use them as you did in hw1.
	// ------------------------------------------------------------

    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    //
    // Once a user enter to timeline mode , there is no way
    // to command mode. You don't have to worry about this situation,
    // and you can terminate the client program by pressing
    // CTRL-C (SIGINT)
	// ------------------------------------------------------------
	Timeline(username);
}

void Client::Timeline(const std::string& username){
    ClientContext context;
    
    
    shared_ptr<ClientReaderWriter<Message, Message>> stream(stub_->Timeline(&context));
    
    
    thread writer([username, stream]() {
        Message m;
        string input = "start list";
        m.set_username(username);
        m.set_msg(input);
        google::protobuf::Timestamp* timestamp = new google::protobuf::Timestamp();
        timestamp->set_seconds(time(NULL));
        timestamp->set_nanos(0);
        m.set_allocated_timestamp(timestamp);
        stream->Write(m);
        while (1) {
            input = getPostMessage();
            m.set_username(username);
            m.set_msg(input);
            google::protobuf::Timestamp* timestamp = new google::protobuf::Timestamp();
            timestamp->set_seconds(time(NULL));
            timestamp->set_nanos(0);
            m.set_allocated_timestamp(timestamp);
            stream->Write(m);
        }
        stream->WritesDone();
    });
    
    
    // thread writer([username, stream](){
        
    //     string sendMsg = " ";
    //     while(true){
    //         sendMsg = getPostMessage();
    //         Message m;
    //         m.set_username(username);
    //         m.set_msg(sendMsg);
    //         google::protobuf::Timestamp* timestamp = new google::protobuf::Timestamp();
    //         timestamp->set_seconds(time(NULL));
    //         timestamp->set_nanos(0);
    //         m.set_allocated_timestamp(timestamp);
	   //     stream->Write(m);
	   // }
	   // stream->WritesDone();
    // });
    
    
    thread reader([username, stream](){
	    Message m;
	    while(stream->Read(&m)){
	        google::protobuf::Timestamp timestamp = m.timestamp();
	        time_t timeinsec = timestamp.seconds();
	        displayPostMessage(m.username(), m.msg(), timeinsec);
	    }
	});
	
	if (writer.joinable()){
	    writer.join();
	}
	if (reader.joinable()){
	    reader.join();    
	}

}
