#ifndef CHATROOM_H_INCLUDED
#define CHATROOM_H_INCLUDED

#include <vector>

using namespace std;

typedef struct {
    char room_name[256];
    int port_num;
    int num_members;
    vector<int> slave_socket;
} chatroom;

#endif