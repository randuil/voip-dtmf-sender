#include <pjsua2.hpp>
#include "myaccount.hpp"
#include <iostream>
#include "mycall.hpp"

MyAccount::MyAccount(){};
MyAccount::~MyAccount(){};
void MyAccount::onRegState(OnRegStateParam &prm){
    AccountInfo accInfo = getInfo();
    std::cout<< (accInfo.regIsActive ? "LOG: Register" :"LOG: Unregister" ) << " Code:"<<prm.code << std::endl;
    if (accInfo.regIsActive){

    }
}


void MyAccount::setCall(MyCall *cal){
    this->cal = cal;
}
