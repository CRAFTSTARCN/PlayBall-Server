#include "Game.h"
#include "iostream"

int main(int argc, char *argv[]) {
    if(SDLNet_Init() == 0) {
        std::cout<<"Success to init SDL network"<<std::endl;
    } else {
        return 1;
    }
    Game mygame;
    std::string order;
    while(1) {
        std::cin>>order;
        if(order == "quit");
        break;
    }
    return 0;
}