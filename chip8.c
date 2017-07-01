#include stdio.h
#include stdlib.h

unsigned short opcode;
unsigned char memory[4096];
unsigned char V[16]; //Registers, 16th is the Carry flag
unsigned short I; //Index Register
unsigned short pc; //Program Counter

unsigned char gfx[64 * 32]; //Display

unsigned char delay_timer;
unsigned char sound_timer;

unsigned short stack[16];
unsigned short sp; //Stack Pointer

unsigned char key[16]; //Keypad

unsigned char fontset[80];


int main(int argc, char **argv){
    initialize();
}

void loadGame(char[] name){
    //Open file here
}

void initialize(){
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;
    
    int i;
    //Clear Memory
    for(i = 0; i < 4096; i++){
        memory[i] = 0; 
    }
    //Clear Registers
    for(i = 0; i < 16; i++){
        V[i] = 0;
    }
    //Clear Screen
    for(i = 0; i < 64*32; i++){
        gfx[i] = 0;
    }
    for(i = 0; i < 16; i++){
        stack[i] = 0;
    }

    //Load fontset
    for(i = 0; i < 80; i++){
        memory[i] = fontset[i];
    }

    //reset timers
    delay_timer = 0;
    sound_timer = 0;
}

void emulateCycle(){
    // Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1] // Or the 2 parts of the opcode (after shifting the first by 8 to make the length 16)
    // Decode opcode
    switch(opcode & 0xF000){
        case 0xA000:
            I = opcode & 0x0FFF;
            pc += 2;
        break;
        default:
            printf("U wot m8? Code is 0x%X\n", opcode)
    }

    if(delay_timer > 0)
        --delay_timer;
    
    if(sound_timer > 0){
        if(sound_timer == 1){
            printf("BEEP!\n");
        }
        --sound_timer;
    }
    // Execute Opcode
    // Update timers
}