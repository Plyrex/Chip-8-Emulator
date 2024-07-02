#include "chip-8.cpp"
#include "platform.cpp"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv){

    if(argc!= 4){
        cerr<<"Usage: "<<argv[0]<<" <Scale> <Delay> <ROM>\n";
        exit(EXIT_FAILURE);
    }

    int videoScale= stoi(argv[1]);
    int cycleDelay= stoi(argv[2]);
    char const* romFilename= argv[3];

    platform platform("Chip-8", 64* videoScale, 32* videoScale, 64, 32);
    chip8 chip8;
    chip8.loadROM(romFilename);

    int videoPitch= sizeof(chip8.video[0])* 64;
    auto lastCycleTime= chrono::high_resolution_clock::now();
    bool quit= false;

    while(!quit){
        quit= platform.input(chip8.keypad);
        auto currentTime= chrono::high_resolution_clock::now();
        float dt= chrono::duration<float, chrono::milliseconds::period>(currentTime- lastCycleTime).count();

        if(dt> cycleDelay){
            lastCycleTime= currentTime;
            chip8.FDEcycle();
            platform.update(chip8.video, videoPitch);
        }
    }
    return 0;
}