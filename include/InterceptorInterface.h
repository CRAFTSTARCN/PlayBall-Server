/*
    InterceptorInterface.h Created 2021/5/28
    By: Ag2s
*/

#ifndef INTERCEPTORINTERFACE_H
#define INTERCEPTORINTERFACE_H

class InterceptorInterface {
    private:
    public:

    InterceptorInterface();
    virtual ~InterceptorInterface();

    virtual bool failCertify();
    virtual bool failVarify();

    virtual bool banned();
};


#endif