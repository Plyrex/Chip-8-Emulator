#include <iosfwd>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <random>

using namespace std;

//add rng gen
//fde cycle
//fonts
//opcodes

const unsigned int START_ADDRESS= 0x200;
class chip8{
    public:
        //functions
        chip8();
        void loadROM(char const* fileName);
        void FDEcycle();

        //components of the Chip-8
        uint8_t registers[16]{}; //16 8-bit registers
        uint8_t memory[4096]{}; //4k bytes of memory
        uint16_t index{}; //16-bit index register
        uint16_t pc{}; //16-bit program counter
        uint16_t stack[16]{}; //16 level stack
        uint8_t sp{}; //8-bit stack pointer
        uint8_t delayTimer{}; //8-bit delay timer
        uint8_t soundTimer{}; //8-bit sound timer
        uint8_t keypad[16]{}; //16 input keys
        uint32_t video[64* 32]{}; //64 x 32 video output
        uint16_t opcode; //for opcodes (instructions)
};

//Loads a ROM for the emulator to run
void chip8::loadROM(char const* fileName){
    ifstream file(fileName, std::ios::binary | std::ios::ate);

    if(file.is_open()){
        //get size of ROM and allocate buffer
        streampos size= file.tellg();
        char* buffer= new char[size];

        //fill buffer with ROM
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        //load ROM into chip-8 memory (4k bytes) starting at location 0x200
        for(long i=0; i<size; i++){
            memory[START_ADDRESS+ i]= buffer[i];
        }

        //free buffer memory
        delete[] buffer;
    }
}

//OPCODES
//does nothing
void OP_null(){}

//clear screen
void OP_00E0(){}

//return from a subroutine
void OP_00EE(){}

//jump to an address, no return
void OP_1nnn(){}

//call a subroutine with return
void OP_2nnn(){}

//SE Vx, byte (skip next instruction if Vx = kk)
void OP_3xkk(){}

//SNE Vx, byte (skip next instruction if Vx != kk)
void OP_4xkk(){}

//SE Vx, Vy (skip next instruction if Vx = Vy)
void OP_5xy0(){}

//LD Vx, byte (set Vx = kk)
void OP_6xkk(){}

//ADD Vx, byte (Vx= Vx + kk)
void OP_7xkk(){}

//LD Vx, Vy (set Vx = Vy)
void OP_8xy0(){}

//OR Vx, Vy (set Vx= Vx OR Vy)
void OP_8xy1(){}

//AND Vx, Vy (set Vx= Vx AND Vy)
void OP_8xy2(){}

//XOR Vx, Vy (set Vx= Vx XOR Vy)
void OP_8xy3(){}

//ADD Vx, Vy (set Vx= Vx + Vy, set VF as the carry if sum is larger that 8-bits)
void OP_8xy4(){}

//SUB Vx, Vy (set Vx= Vx - Vy, set VF to 1 if Vx > Vy)
void OP_8xy5(){}

//SHR Vx (set Vx = Vx SHR 1, right non-circular shift occurs and the least significant bit is stored in VF)
void OP_8xy6(){}

//SUBN Vx, Vy (set Vy - Vx, set VF to 1 if Vy > Vx)
void OP_8xy7(){}

//SHL Vx (set Vx = Vx SHL 1, left non-circular shift occurs and most significant bit is stored in VF)
void OP_8xyE(){}

//SNE Vx, Vy (skip next instruction if Vx != Vy)
void OP_9xy0(){}

//LD I, addr (set I= nnn)
void OP_Annn(){}

//JP V0, addr (jump to nnn + V0)
void OP_Bnnn(){}

//RND Vx, byte (set Vx= random byte AND kk)
void OP_Cxkk(){}

//DRW Vx, Vy, nibble (display n-byte at location (Vx, Vy) and set VF= collision)
void OP_Dxyn(){}

//SKP Vx (skip next instruction if key with value of Vx is pressed)
void OP_Ex9E(){}

//SKNP Vx (skip next instruction if key with value of Vx is not pressed)
void OP_ExA1(){}

//LD Vx, DT (set Vx = delay timer value)
void OP_Fx07(){}

//LD Vx, K (wait for key press and store value in Vx)
void OP_Fx0A(){}

//LD DT, Vx (set delay timer = Vx)
void OP_Fx15(){}

//LD St, Vx (set sound timer = Vx)
void OP_Fx18(){}

//ADD I, Vx (Set I= I + Vx)
void OP_Fx1E(){}

//LD F, Vx (set I= location of sprite for digit Vx)
void OP_Fx29(){}

//LD B, Vx (Store BCD representation of Vx in memory location I, I+1 and I+2)
void OP_Fx33(){}

//LD [I], Vx (store registers V0 to Vx in memory starting at location I)
void OP_Fx55(){}

//LD Vx, [I] (read registers V0 to Vx in memory starting at location I)
void OP_Fx65(){}