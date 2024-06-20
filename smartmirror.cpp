#include "smartmirror.h"
#include <iostream>

void SmartMirror::initSmartMirror(){
    power=ON;
    socket=true;
    display=0;
    displayMenu();
}

void SmartMirror::displayMenu(){
    std::cout<<"SmartMirror Menu"<<std::endl;
    std::cout<<"1. new user"<<std::endl;
    std::cout<<"2. display"<<std::endl;
    std::cin>> user_menu;
    if(user_menu == 1) {
        std::cout<<"what is your name?"<<std::endl;
        std::cin>> name;
        newUser(char name);
        setUserFunction();
    }
    else if(user_menu == 2) {
        std::cout<<"1. BK"<<std::endl;
        std::cout<<"2. HW"<<std::endl;
        std::cout<<"3. MW"<<std::endl;
        std::cin>> number;
    }
    else {
        std::cout<<"error"<<std::endl;
        std::cout<<"Please choose 1 or 2"<<std::endl;
        displayMenu();
    }
}

void SmartMirror::newUser(char name){
    this->name = name;
    setUserFunction();
}

void SmartMirror::setUserFunction(){
    std::cout<<"1. Olmpic"<<std::endl;
    std::cout<<"2. Weather"<<std::endl;
    std::cin>> function;
}

void SmartMirror::runningSmartMirror(int number){
    this->number = number;
    switch (number)
    {
    case 1:
        /* code */
        break;
    case 2:
        /* code */
        break;
    default:
        break;
    }

}


