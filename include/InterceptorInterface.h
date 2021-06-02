/*
    InterceptorInterface.h Created 2021/5/28
    By: Ag2s
*/

#ifndef INTERCEPTORINTERFACE_H
#define INTERCEPTORINTERFACE_H

#include <SDL2/SDL_net.h>

class InterceptorInterface {
    private:

    public:
    InterceptorInterface();
    virtual ~InterceptorInterface();

    virtual bool failCertify(TCPsocket);
    virtual bool failVarify(TCPsocket);

    virtual bool banned(TCPsocket);
};


#endif