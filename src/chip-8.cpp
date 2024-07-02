#include <chrono>
#include <cstddef>
#include <cstring>
#include <iosfwd>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <random>

using namespace std;

//fde cycle
//opcode pointer table

//PC instructions start at memory value 0x200 as 0x000 to 0x1FF is reserved
//Fonts are loaded into memory starting at 0x50
const unsigned int START_ADDRESS= 0x200;
const unsigned int START_ADDRESS_FONTS= 0x50;

class chip8{
    public:
        //functions
        chip8();
        void loadROM(char const* fileName);
        void FDEcycle();

        default_random_engine rando;
        uniform_int_distribution<> randNum;

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

        /*
        Registers are labeled V0 - VF for the 16 registers available
        As they are 8-bit, they can hold values from 0x00 - 0xFF
        Register VF is used to hold flag values for instructions
        */
        /*
        How the 4k bytes of memory are allocated
        0x000 - 0x1FF: Not used in coded interpretors as this is where the interpretor was held in the actual CHIP-8
        0x050 - 0x0A0: Storage area for fontset
        0x200 - 0xFFF: Space for instructions
        */

    private:
        //tables for functions
        void Table0();
        void Table8();
        void TableE();
        void TableF();

        //opcodes
        void OP_null();

        void OP_00E0();

        void OP_00EE();

        void OP_1nnn();

        void OP_2nnn();

        void OP_3xkk();

        void OP_4xkk();

        void OP_5xy0();

        void OP_6xkk();

        void OP_7xkk();

        void OP_8xy0();

        void OP_8xy1();

        void OP_8xy2();

        void OP_8xy3();

        void OP_8xy4();

        void OP_8xy5();

        void OP_8xy6();

        void OP_8xy7();

        void OP_8xyE();

        void OP_9xy0();

        void OP_Annn();

        void OP_Bnnn();

        void OP_Cxkk();

        void OP_Dxyn();

        void OP_Ex9E();

        void OP_ExA1();

        void OP_Fx07();

        void OP_Fx0A();

        void OP_Fx15();

        void OP_Fx18();

        void OP_Fx1E();

        void OP_Fx29();

        void OP_Fx33();

        void OP_Fx55();

        void OP_Fx65();

        typedef void (chip8::*chip8Func)();
        chip8Func table[0xF + 1];
        chip8Func table0[0xE +1];
        chip8Func table8[0xE + 1];
        chip8Func tableE[0xE + 1];
        chip8Func tableF[0x65 + 1];
};

/*
Fonts are stored in array and are loaded into memory
Programs use fonts by using specific memory locations
Example: the number 0 is utilized as follows:
        0xF0 = 11110000 = ****
        0X90 = 10010000 = *  *
        0X90 = 10010000 = *  *
        0X90 = 10010000 = *  *
        0xF0 = 11110000 = ****
*/
uint8_t fonts[80]= {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80, //F
};

//Constructor for chip8 class
//the ': rando...' after the chip8() constructor is how you initialize a member, aka the rand generator
//I seeded gen with system date
chip8::chip8(): rando(chrono::system_clock::now().time_since_epoch().count()){
    //init program counter
    pc= START_ADDRESS;

    //load fonts
    for(unsigned int i= 0; i< 80; i++ ){ //may have to change to ++i
        memory[START_ADDRESS_FONTS+ i]= fonts[i];
    }

    //init RNG
    randNum= uniform_int_distribution<>(0, 255);

    //function pointer table
    table[0x0]= &chip8::Table0;
    table[0x1]= &chip8::OP_1nnn;
    table[0x2]= &chip8::OP_2nnn;
    table[0x3]= &chip8::OP_3xkk;
    table[0x4]= &chip8::OP_4xkk;
    table[0x5]= &chip8::OP_5xy0;
    table[0x6]= &chip8::OP_6xkk;
    table[0x7]= &chip8::OP_7xkk;
    table[0x8]= &chip8::Table8;
    table[0x9]= &chip8::OP_9xy0;
    table[0xA]= &chip8::OP_Annn;
    table[0xB]= &chip8::OP_Bnnn;
    table[0xC]= &chip8::OP_Cxkk;
    table[0xD]= &chip8::OP_Dxyn;
    table[0xE]= &chip8::TableE;
    table[0xF]= &chip8::TableF;

    for(size_t i= 0; i<= 0x65; i++){
        table0[i]= &chip8::OP_null;
        table8[i]= &chip8::OP_null;
        tableE[i]= &chip8::OP_null;
        tableF[i]= &chip8::OP_null;
    }

    table0[0x0]= &chip8::OP_00E0;
    table0[0xE]= &chip8::OP_00EE;

    table8[0x0]= &chip8::OP_8xy0;
    table8[0x1]= &chip8::OP_8xy1;
    table8[0x2]= &chip8::OP_8xy2;
    table8[0x3]= &chip8::OP_8xy3;
    table8[0x4]= &chip8::OP_8xy4;
    table8[0x5]= &chip8::OP_8xy5;
    table8[0x6]= &chip8::OP_8xy6;
    table8[0x7]= &chip8::OP_8xy7;
    table8[0xE]= &chip8::OP_8xyE;

    tableE[0x0]= &chip8::OP_Ex9E;
    tableE[0xE]= &chip8::OP_ExA1;

    tableF[0x07]= &chip8::OP_Fx07;
    tableF[0x0A]= &chip8::OP_Fx0A;
    tableF[0x15]= &chip8::OP_Fx15;
    tableF[0x18]= &chip8::OP_Fx18;
    tableF[0x1E]= &chip8::OP_Fx1E;
    tableF[0x29]= &chip8::OP_Fx29;
    tableF[0x33]= &chip8::OP_Fx33;
    tableF[0x55]= &chip8::OP_Fx55;
    tableF[0x65]= &chip8::OP_Fx65;

}

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
        for(long i=0; i<size; i++){ //may have to change to ++i
            memory[START_ADDRESS+ i]= buffer[i];
        }

        //free buffer memory
        delete[] buffer;
    }
}

void chip8::FDEcycle(){
    //Fetch
    opcode= (memory[pc]<< 8u) | memory[pc+ 1];
    pc+= 2;

    //Decode & Execute
    ((*this).*(table[(opcode & 0x000Fu)>> 12u]))();

    //decrement timers if set
    if(delayTimer> 0){
        delayTimer--;
    }

    if(soundTimer> 0){
        soundTimer--;
    }
}

//function tables
void chip8::Table0(){
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void chip8::Table8(){
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void chip8::TableE(){
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void chip8::TableF(){
    ((*this).*(tableF[opcode & 0x000Fu]))();
}

//OPCODES
//0x0FFFu is AND' with opcode to remove the initial instruction aka 1nnn & 0x0FFFu removes the 1 and leaves nnn aka the address
//does nothing
void chip8::OP_null(){}

//clear screen
void chip8::OP_00E0(){
    memset(video, 0, sizeof(video));
}

//return from a subroutine
void chip8::OP_00EE(){
    sp--; //maybe --sp idk
    pc= stack[sp];
}

//jump to an address, no return
void chip8::OP_1nnn(){
    uint16_t address= opcode & 0x0FFFu;
    pc= address;
}

//call a subroutine with return
void chip8::OP_2nnn(){
    uint16_t address= opcode & 0x0FFFu;
    stack[sp]= pc;
    sp++; 
    pc= address;
}

//SE Vx, byte (skip next instruction if Vx = kk)
void chip8::OP_3xkk(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t byte= opcode & 0x00FFu;

    if(registers[Vx]== byte){
        pc+= 2;
    }
}

//SNE Vx, byte (skip next instruction if Vx != kk)
void chip8::OP_4xkk(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t byte= opcode & 0x00FFu;

    if(registers[Vx]!= byte){
        pc+= 2;
    }
}

//SE Vx, Vy (skip next instruction if Vx = Vy)
void chip8::OP_5xy0(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u; 
    uint8_t Vy= (opcode & 0x00F0u) >> 4u;

    if(registers[Vx]== registers[Vy]){
        pc+= 2;
    } 
}

//LD Vx, byte (set Vx = kk)
void chip8::OP_6xkk(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t byte= opcode & 0x00FFu;
    registers[Vx]= byte;
}

//ADD Vx, byte (Vx= Vx + kk)
void chip8::OP_7xkk(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t byte= opcode & 0x00FFu;
    registers[Vx]+= byte;
}

//LD Vx, Vy (set Vx = Vy)
void chip8::OP_8xy0(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t Vy= (opcode & 0x00F0u) >> 4u;
    registers[Vx]= registers[Vy];
}

//OR Vx, Vy (set Vx= Vx OR Vy)
void chip8::OP_8xy1(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t Vy= (opcode & 0x00F0u) >> 4u;
    registers[Vx] |= registers[Vy];
}

//AND Vx, Vy (set Vx= Vx AND Vy)
void chip8::OP_8xy2(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t Vy= (opcode & 0x00F0u) >> 4u;
    registers[Vx] &= registers[Vy];
}

//XOR Vx, Vy (set Vx= Vx XOR Vy)
void chip8::OP_8xy3(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t Vy= (opcode & 0x00F0u) >> 4u;
    registers[Vx] ^= registers[Vy];
}

//ADD Vx, Vy (set Vx= Vx + Vy, set VF as the carry if sum is larger that 8-bits)
void chip8::OP_8xy4(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t Vy= (opcode & 0x00F0u) >> 4u;
    uint16_t sum= registers[Vx]+ registers[Vy];

    if(sum> 255u){
        registers[0xF]= 1;
    }else{
        registers[0xF]= 0;
    }

    registers[Vx]= sum & 0xFFu; //AND with 0xFFu to store the lowest 8 bits
}

//SUB Vx, Vy (set Vx= Vx - Vy, set VF to 1 if Vx > Vy)
void chip8::OP_8xy5(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t Vy= (opcode & 0x00F0u) >> 4u;
    
    if(registers[Vx]> registers[Vy]){
        registers[0xF]= 1;
    }else{
        registers[0xF]= 0;
    }

    registers[Vx]-= registers[Vy];
}

//SHR Vx (set Vx = Vx SHR 1, right non-circular shift occurs and the least significant bit is stored in VF)
void chip8::OP_8xy6(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;

    registers[0xF]= registers[Vx] & 0x1u; //store least significant bit
    registers[Vx]>>= 1; //shift 1 to the right
}

//SUBN Vx, Vy (set Vx= Vy - Vx, set VF to 1 if Vy > Vx)
void chip8::OP_8xy7(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t Vy= (opcode & 0x00F0u) >> 4u;

    if(registers[Vy]> registers[Vx]){
        registers[0xF]= 1;
    }else{
        registers[0xF]= 0;
    }

    registers[Vx]= registers[Vy]- registers[Vx];
}

//SHL Vx (set Vx = Vx SHL 1, left non-circular shift occurs and most significant bit is stored in VF)
void chip8::OP_8xyE(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;

    registers[0xF]= (registers[Vx] & 0x80u) >> 7u; //store most significant bit

    registers[Vx]<<= 1; //shift 1 to the left
}

//SNE Vx, Vy (skip next instruction if Vx != Vy)
void chip8::OP_9xy0(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t Vy= (opcode & 0x00F0u) >> 4u;

    if(registers[Vx]!= registers[Vy]){
        pc+= 2;
    }
}

//LD I, addr (set I= nnn)
void chip8::OP_Annn(){
    uint8_t address= opcode & 0x0FFFu;
    index= address;
}

//JP V0, addr (jump to nnn + V0)
void chip8::OP_Bnnn(){
    uint8_t address= opcode & 0x0FFFu;
    pc= registers[0]+ address;
}

//RND Vx, byte (set Vx= random byte AND kk)
void chip8::OP_Cxkk(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t byte= opcode & 0x00FFu;

    registers[Vx]= randNum(rando) & byte;
}

//DRW Vx, Vy, nibble (display n-byte at location (Vx, Vy) and set VF= collision)
void chip8::OP_Dxyn(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t Vy= (opcode & 0x00F0u) >> 4u;
    uint8_t height= opcode & 0x000Fu;

    //wrap if going beyond screen
    uint8_t xPos= registers[Vx]% 64; //vid width
    uint8_t yPos= registers[Vy]% 32; //vid height

    registers[0xF]= 0; //collision

    for(unsigned int row= 0; row< height; row++){ //maybe ++row
        uint8_t spriteByte= memory[index+ row];
        for(unsigned int col= 0; col< 8; col++){
            uint8_t spritePixel= spriteByte & (0x80u>> col);
            uint32_t* screenPixel= &video[(yPos+ row)* 64+ (xPos+ col)];

            if(spritePixel){
                if(*screenPixel== 0xFFFFFFFF){
                    registers[0xF]= 1;
                }
                *screenPixel^= 0xFFFFFFFF;
            }
        }
    }
}

//SKP Vx (skip next instruction if key with value of Vx is pressed)
void chip8::OP_Ex9E(){
    uint8_t Vx= (opcode & 0x0F00u)>> 8u;
    uint8_t key= registers[Vx];
    if(keypad[key]){
        pc+= 2;
    }
}

//SKNP Vx (skip next instruction if key with value of Vx is not pressed)
void chip8::OP_ExA1(){
    uint8_t Vx= (opcode & 0x0F00u)>> 8u;
    uint8_t key= registers[Vx];
    if(!keypad[key]){
        pc+= 2;
    }
}

//LD Vx, DT (set Vx = delay timer value)
void chip8::OP_Fx07(){
    uint8_t Vx= (opcode & 0x0F00u)>> 8u;
    registers[Vx]= delayTimer;
}

//LD Vx, K (wait for key press and store value in Vx)
void chip8::OP_Fx0A(){
    uint8_t Vx= (opcode & 0x0F00u)>> 8u;

    // for(uint8_t i= 0; i< 16; i++){
    //     if(keypad[i]){
    //         registers[Vx]= i;
    //     }else{
    //         pc-= 2;
    //     }
    // }

    if(keypad[0]){
        registers[Vx]= 0;
    }else if(keypad[1]){
        registers[Vx]= 1;
    }else if(keypad[2]){
        registers[Vx]= 2;
    }else if(keypad[3]){
        registers[Vx]= 3;
    }else if(keypad[4]){
        registers[Vx]= 4;
    }else if(keypad[5]){
        registers[Vx]= 5;
    }else if(keypad[6]){
        registers[Vx]= 6;
    }else if(keypad[7]){
        registers[Vx]= 7;
    }else if(keypad[8]){
        registers[Vx]= 8;
    }else if(keypad[9]){
        registers[Vx]= 9;
    }else if(keypad[10]){
        registers[Vx]= 10;
    }else if(keypad[11]){
        registers[Vx]= 11;
    }else if(keypad[12]){
        registers[Vx]= 12;
    }else if(keypad[13]){
        registers[Vx]= 13;
    }else if(keypad[14]){
        registers[Vx]= 14;
    }else if(keypad[15]){
        registers[Vx]= 15;
    }else{
        pc-= 2;
    }
}

//LD DT, Vx (set delay timer = Vx)
void chip8::OP_Fx15(){
    uint8_t Vx= (opcode & 0x0F00u)>> 8u;
    delayTimer= registers[Vx];
}

//LD St, Vx (set sound timer = Vx)
void chip8::OP_Fx18(){
    uint8_t Vx= (opcode & 0x0F00u)>> 8u;
    soundTimer= registers[Vx];
}

//ADD I, Vx (Set I= I + Vx)
void chip8::OP_Fx1E(){
    uint8_t Vx= (opcode & 0x0F00u)>> 8u;
    index+= registers[Vx];
}

//LD F, Vx (set I= location of sprite for digit Vx)
void chip8::OP_Fx29(){
    uint8_t Vx= (opcode & 0x0F00u)>> 8u;
    uint8_t num= registers[Vx];

    index= START_ADDRESS_FONTS+ (5* num); 
}

//LD B, Vx (Store BCD representation of Vx in memory location I, I+1 and I+2)
void chip8::OP_Fx33(){
    uint8_t Vx= (opcode & 0x0F00u)>> 8u;
    uint8_t val= registers[Vx];

    memory[index+ 2]= val% 10;
    val/= 10;

    memory[index+ 1]= val% 10;
    val/= 10;

    memory[index]= val% 10;
}

//LD [I], Vx (store registers V0 to Vx in memory starting at location I)
void chip8::OP_Fx55(){
    uint8_t Vx= (opcode & 0x0F00u)>> 8u;

    for(uint8_t i=0; i<= Vx; i++){
        memory[index+ i]= registers[i];
    }
}

//LD Vx, [I] (read registers V0 to Vx in memory starting at location I)
void chip8::OP_Fx65(){
    uint8_t Vx= (opcode & 0x0F00u)>> 8u;

    for(uint8_t i=0; i<= Vx; i++){
        registers[i]= memory[index+ i];
    }
}