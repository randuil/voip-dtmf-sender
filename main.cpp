#include <pjsua2.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "mycall.hpp"
#include "myaccount.hpp"
using namespace pj;
using std::cout;
using std::endl;


void log(string msg){
    cout<<"LOG: "<<msg<<"\n";
}
//-r registrant
//-u sip username
//-p sip password
//-n name
//-t abonent target number
//-T time when was scheduled
//-h help
//-d DTMF string
//-v be verbose
//-P Local port

void show_help(){
    std::string buffer = "";
    std::string delim = "";
    std::cout<<buffer<<"Usage : ./prog [-vh] [-r hostname] [-u username] [-p password]\n";
    std::cout<<buffer<<"    [-T time] [-n account] [-t target@remote.server] [-d dtmf]\n";
    buffer += "\t";
    delim += "\t";
    std::cout<<buffer<<"-h"<<delim<<"Show this help"<<std::endl;
    std::cout<<buffer<<"-v"<<delim<<"Be verbose"<<std::endl;
    std::cout<<buffer<<"-r"<<delim<<"Sip server address for registration."<<std::endl;
    std::cout<<buffer<<"-u"<<delim<<"Sip username for registration."<<std::endl;
    std::cout<<buffer<<"-p"<<delim<<"Sip password for registration"<<std::endl;
    std::cout<<buffer<<"-P"<<delim<<"Local SIP port"<<std::endl;
    std::cout<<buffer<<"-n"<<delim<<"Sip acoount on server"<<std::endl;
    std::cout<<buffer<<"-t"<<delim<<"Target. Example: user123@somesip.nowhere"<<std::endl;
    std::cout<<buffer<<"-T"<<delim<<"Time when task wash scheduled"<<std::endl;
    std::cout<<buffer<<"-d"<<delim<<"DTMF code, where '#' is 1 sec. delay."<<std::endl;
    std::cout<<buffer<<"  "<<delim<<"Example: '#123##'  will sleep for 1 second"<<std::endl;
    std::cout<<buffer<<"  "<<delim<<"after call established, send '123' code, and sleep for another 2 sec."<<std::endl;
}
std::vector<string> parse_args(int argc, char *args[]){
    int opt;
    char * paramValue = NULL;
    int argCounter = 6;
    bool registerCheck = false;
    bool userCheck = false;
    bool passwordCheck = false;
    bool nameCheck = false;
    bool targetCheck = false;
    bool timeCheck = false;
    bool dtmfCheck = false;
    bool helpCheck = false;
    bool portCheck = false;

    std::vector<string> retValues(8);
    while ((opt = getopt(argc, args, "r:u:p:n:P:t:T:d:vh")) != -1){
        //printf("opt is %c param is %s\n", opt, (char *)optarg);
        char *oa = (char *)optarg;
        switch (opt){
            case 'r':
                if ( oa[0] == '-'){
                    std::cout<<"Incorrect parameter for argument: r\n";
                    helpCheck = true;
                } 
                retValues[0].assign(optarg);
                registerCheck = true;
                std::cout<<"Register at "<<(char *)optarg<<std::endl;
                break;
            case 'u':
                if ( oa[0] == '-'){
                    std::cout<<"Incorrect parameter for argument: u\n";
                    helpCheck = true;
                } 
                retValues[1].assign(optarg);
                userCheck = true;
                std::cout<<"Auth Username is "<<(char *)optarg<<std::endl;
                break;
            case 'p':
                if ( oa[0] == '-'){
                    std::cout<<"Incorrect parameter for argument: p\n";
                    helpCheck = true;
                } 
                retValues[2].assign(optarg);
                passwordCheck = true;
                //std::cout<<"Password is "<<(char *)optarg<<std::endl;
                break;
            case 'P':
                if ( oa[0] == '-'){
                    std::cout<<"Incorrect parameter for argument: P\n";
                } 
                retValues[7].assign(optarg);
                portCheck = true;
                std::cout<<"Local port is"<<(char *)optarg<<std::endl;
                break;
            case 'n':
                if ( oa[0] == '-'){
                    std::cout<<"Incorrect parameter for argument: n\n";
                    helpCheck = true;
                } 
                retValues[3].assign(optarg);
                nameCheck = true;
                std::cout<<"Name on server is "<<(char *)optarg<<std::endl;
                break;
            case 't':
                if ( oa[0] == '-'){
                    std::cout<<"Incorrect parameter for argument: t\n";
                    helpCheck = true;
                } 
                retValues[4].assign(optarg);
                targetCheck = true;
                std::cout<<"Target is "<<(char *)optarg<<std::endl;
                break;
            case 'T':
                if ( oa[0] == '-'){
                    std::cout<<"Incorrect parameter for argument: T\n";
                    helpCheck = true;
                } 
                retValues[6].assign(optarg);
                timeCheck = true;
                std::cout<<"Target is "<<(char *)optarg<<std::endl;
                break;
            case 'd':
                if ( oa[0] == '-'){
                    std::cout<<"Incorrect parameter for argument: d\n";
                    helpCheck = true;
                } 
                retValues[5].assign(optarg);
                dtmfCheck = true;
                std::cout<<"DTMF string is"<<(char *)optarg<<std::endl;
                break;
            case 'v':
                std::cout<<"Be more verbose"<<std::endl;
                break;
            case 'h':
                show_help();
                helpCheck = true;
                exit(0);
                break;
        }
    }
    if (registerCheck && userCheck && passwordCheck && nameCheck && targetCheck && dtmfCheck &&timeCheck && portCheck){
        int c =0;
        for (string var : retValues){
            std::cout<<c<<" : "<<var<<std::endl;
            c++;
        }
        return retValues;
    }
    std::cout<<"To few params\n";
    helpCheck = true;
    if (helpCheck){
        show_help();
        exit(0);
    }
    //exit(-7);
    return retValues;  //To make compiler happy
}

class MyLogWritter : public LogWriter{
private:
    std::ofstream logFile;

public:
    MyLogWritter(std::string logname) : LogWriter(){
        logFile.open(logname);
    }
    void write(const LogEntry &entry){
        std::cout<<"MYLOG:"
            //<<entry.level
            <<":"<<entry.msg
            //<<std::endl;
            ;
        logFile<< entry.msg;
    }
    ~MyLogWritter(){
        logFile.close();
    }

};

Endpoint *ep = new Endpoint;
MyCall *call;
void my_handler(int s){
        printf("Caught signal %d\n",s);
        ep->libDestroy();
        delete ep;
        exit(125); 
}

int main(int argc, char * argv[]){
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    
    std::vector<string> params = parse_args(argc, argv);
    std::string REGISTERADDR=params[0];
    std::string USERNAME=params[1];
    std::string PASSWORD=params[2];
    std::string CLIENT_NAME=params[3];
    std::string TARGET=params[4];
    std::string DTMDSTRING=params[5];
    std::string TIMESTRING=params[6];
    std::string PORT=params[7];

    try{
        ep->libCreate();
    } catch (Error &err){
        cout<<"Startup error" << err.info() <<endl;
        return 3;
    }

    try{
        EpConfig ep_cfg;
        MyLogWritter *logw = new MyLogWritter("log"+TIMESTRING+".log");
        ep_cfg.logConfig.level = 4;
        ep_cfg.logConfig.writer = logw;
        ep->libInit(ep_cfg);

    } catch (Error &err){
        cout<<"Init error" << err.info() <<endl;
        return 2;
    }

    try{
        TransportConfig tcfg;
        tcfg.port = std::stoi(PORT);
        TransportId tid = ep->transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
        std::cout<<"Local port is:"<<tcfg.port<<std::endl;
    } catch (Error &err){
        cout<<"Transport init error" << err.info() <<endl;
        return 1;
    }

    try {
       ep->libStart(); 
       log("STARTED");
    } catch (Error &err){
        cout<<"Libstart error" << err.info() <<endl;
        return 4;
    }
    MyAccount *acc = new MyAccount;
    acc->client = "sip:" + CLIENT_NAME + "@" + REGISTERADDR;
    acc->server = "sip:" + REGISTERADDR;
    acc->target = "sip:" + TARGET;
    std::cout<<"Reg server is : "<<acc->server<<std::endl;
    std::cout<<"Client is : "<<acc->client<<std::endl;
    std::cout<<"Target is : "<<acc->target<<std::endl;
    AccountConfig acfg;
    acfg.idUri = acc->client;
    acfg.regConfig.timeoutSec =10;
    acfg.regConfig.registrarUri = acc->server;
    AuthCredInfo creds("digest", "*", USERNAME, 0, PASSWORD);
    acfg.sipConfig.authCreds.push_back(creds);
    try{
        acc->create(acfg);
    } catch (Error &err){
        cout<<err.info()<<endl;
        return 1;
    }
    CallOpParam callParam(true);
    callParam.opt.audioCount = 1;
    callParam.opt.videoCount = 0;

    call = new MyCall(*acc, TIMESTRING, PJSUA_INVALID_ID);
    log("CALLING");
    call->makeCall(acc->target, callParam);
    bool send = true;
    while (call->isActive()){
        if (call->isDTMFSent() || call->getInfo().state == PJSIP_INV_STATE_DISCONNECTED) break;
            if (call->getInfo().state == PJSIP_INV_STATE_CONFIRMED){
                //std::cout<<"IF CONFIRMED\n";
                if (send){
                    //std::cout<<"IF SEND\n";
                    try{
                        std::cout<<"Sending";
                            call->DTMF(DTMDSTRING);
                        send = false;
                    } catch (Error &err){
                        cout<<err.info();
                    }
                }
            }
            pj_thread_sleep(1);

    }
    ep->libDestroy();
    delete ep;
    return 0;
}
