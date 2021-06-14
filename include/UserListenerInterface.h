/*
    UserListenerInterFace Created 2021/6/1
    By: Ag2S
*/

#ifndef USERLISTENERINTERFACE_H
#define USERLISTENERINTERFACE_H

#include "CallbackListener.h"

class UserListenerInterface{

    public:
    UserListenerInterface();
    virtual ~UserListenerInterface() = 0;

    virtual void UserCertified(unsigned int id) = 0;
    virtual void UserQuit(unsigned int id) = 0;
};




#endif