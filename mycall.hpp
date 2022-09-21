#ifndef MYCALL_HPP
#define MYCALL_HPP
#include <pjsua2.hpp>
#include <iostream>
#include "myaccount.hpp"

using namespace pj;
class MyAccount;
class MyCall: public Call{
private:
    MyAccount *myAcc;
    std::string logFilename;
    AudioMedia aud_med;
    AudioMedia play_dev_med;
    AudioMediaRecorder recorder;
    AudioMedia cap_dev_med;
    bool sendDTMFmsg = true;
    bool DTMFsent = false;
    
public:
    MyCall(Account &, std::string, int);
    ~MyCall();
    void onCallState(OnCallStateParam &);
    void DTMF(std::string);
    void onDtmfDigit(OnDtmfDigitParam &);
    void onCallMediaState(OnCallMediaStateParam &prm);
    bool isDTMFSent();
};
#endif
