#ifndef MYACCOUNT_HPP
#define MYACCOUNT_HPP
#include <pjsua2.hpp>
#include <iostream>
#include "mycall.hpp"

using namespace pj;
class MyCall;
class MyAccount: public Account{
public:
    MyCall* cal;
    std::string client;
    std::string server;
    std::string target;
    MyAccount();
    ~MyAccount();
    virtual void onRegState(OnRegStateParam &);

    void setCall(MyCall *);

};

#endif
