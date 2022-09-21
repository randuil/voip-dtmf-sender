#include <pjsua2.hpp>
#include <iostream>
#include "mycall.hpp"

using namespace pj;

MyCall::MyCall(Account &acc, std::string logTime, int call_id = PJSUA_INVALID_ID): Call (acc, call_id){
    logFilename = "record-"+logTime + ".wav";
    myAcc = (MyAccount *)&acc;
}
MyCall::~MyCall(){

}
void MyCall::onCallState(OnCallStateParam &prm){
        CallInfo info = getInfo();
        std::cout<<"OnCallStateParam::INFO:"<<info.stateText<<std::endl;
        if (info.state == PJSIP_INV_STATE_DISCONNECTED){
            DTMFsent = true;
        delete this;
        }
        //if (info.state == PJSIP_INV_STATE_CONFIRMED) dialDtmf("123");
}

void MyCall::DTMF(std::string msg){
    if (sendDTMFmsg){
        std::cout<<"BEGIN";
        for (char c : msg){
            if (c == '+'){
                std::cout<<"Sleepping 1000 ms\n";
                pj_thread_sleep(1600);
            } else {
                CallSendRequestParam *prm = new CallSendRequestParam();
                prm->method = "INFO";
                SipTxOption *txo = new SipTxOption();
                txo->contentType ="application/dtmf-relay";
                std::string beg = "Signal=";
                std::string end = "\nDuration=160";
                txo->msgBody = beg + c + end;
                prm->txOption =*txo;
                sendRequest(*prm);
                pj_thread_sleep(160);
            }
        }
        pj_thread_sleep(1600);
        std::cout<<"Message sent\n";
        DTMFsent = true;
    }
}

bool MyCall::isDTMFSent(){
    return DTMFsent;
}

void MyCall::onDtmfDigit(OnDtmfDigitParam &prm){
    std::cout<<"Recived DTMF: "<<prm.digit<<std::endl;
}

void MyCall::onCallMediaState(OnCallMediaStateParam &prm){
    CallInfo ci = getInfo();
    //AudioMediaRecorder recorder;
    if (ci.state == PJSIP_INV_STATE_DISCONNECTED){
        DTMFsent = true;
        std::cout << "Disconnected" << std::endl;
        //try {
            //cap_dev_med.stopTransmit(recorder);
            //std::cout << "======= Stop recording" << std::endl;
        //} catch(Error& err) {
            //std::cout << "======= Stop recording FAILED" << std::endl;
        //}
    }
    if (ci.state == PJSIP_INV_STATE_CONFIRMED)
    {
        //AudioMedia& cap_dev_med = Endpoint::instance().audDevManager().getCaptureDevMedia();

        //AudioMedia aud_med;
        //AudioMedia play_dev_med = Endpoint::instance().audDevManager().getPlaybackDevMedia();
        try
        {
            play_dev_med = Endpoint::instance().audDevManager().getPlaybackDevMedia();
            cap_dev_med = Endpoint::instance().audDevManager().getCaptureDevMedia();
            aud_med = getAudioMedia(-1);
            recorder.createRecorder(logFilename);
            std::cout << "======= Got audio media!" << std::endl;
            //cap_dev_med.startTransmit(recorder);
        }
        catch(...)
        {
            std::cout << "======= Failed to get audio media" << std::endl;
            return;
        }
        aud_med.startTransmit(play_dev_med);
        aud_med.startTransmit(recorder);
    }
}
