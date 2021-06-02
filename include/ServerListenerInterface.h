/*
    ServerListenerInterFace Created 2021/6/1
    By: Ag2S
*/

#ifndef SERVERLISTENERINTERFACE_H
#define SERVERLISTENERINTERFACE_H

#include "CallbackListener.h"

#include <map>
#include <string>

class ServerListenerInterface : public CallbackListener {
    public :
    ServerListenerInterface();
    virtual ~ServerListenerInterface();
    /*
        callback funtion listener
    */
    virtual void onEmitMessage(unsigned int sender,int v_stamp, 
                               const std::string& p_type, const std::string& content) = 0;
};

#endif