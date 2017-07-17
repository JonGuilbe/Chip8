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
7
void emulateCycle(){
    // Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1] // Or the 2 parts of the opcode (after shifting the first by 8 to make the length 16)
    short x = (opcode >> 8) & 0xf;
    short y = (opcode >> 4) & 0xf;
    // Decode opcode
    switch(opcode & 0xF000){ //Checking first byte of the opcode
        case 0x0000: //3 Potential Opcodes, investigate further
            switch(opcode){
                case 0x00E0: 
                    //Clear Display
                     for(i = 0; i < 64*32; i++){
                        gfx[i] = 0;
                    }
                    pc += 2;
                    break;
                case 0x00EE:
                    //Return
                    break;
                default:
                    //"Calls RCA 1802 program at address NNN."
                    printf("Not Implemented...\n");
                    pc += 2;
            }
            break;
        case 0x1000:
            //Jump to NNN
            pc = (opcode & 0x0FFF);
            break;
        case 0x2000:
            //Call NNN
            break;
        case 0x3000:
            //Skip next instruction (increment PC?) if V[X] == NN
            if(V[x] == (opcode & 0x00FF)){
                pc += 4;
            }
            else{
                pc += 2;
            }
            break;
        case 0x4000:
            //Skip next instruction if V[x] != NN
            if(V[x] != (opcode & 0x00FF)){
                pc += 4;
            }
            else{
                pc += 2;
            }
            break;
        case 0x5000:
            //Skip next instruction if V[x] == V[y]
            if(V[x] == V[y]){
                pc += 4;
            }
            else{
                pc += 2;
            }
            break;
        case 0x6000:
            //Set V[x] to NN
            V[x] = (opcode & 0x00FF);
            pc += 2;
            break;
        case 0x7000:
            //Add NN to V[x]
            V[x] += (opcode & 0x00FF);
            pc += 2;
            break;
        case 0x8000:
            switch(opcode & 0x000F){
                case 0x0000:
                    //V[x] is set to V[y]
                    V[x] = V[y];
                    pc += 2;
                    break;
                case 0x0001:
                    //V[x] = Bitwise OR of V[x] and V[y]
                    V[x] = V[x] | V[y];
                    pc += 2;
                    break;
                case 0x0002:
                    //V[x] = Bitwise AND of V[x] and V[y]
                    V[x] = V[x] & V[y];
                    pc += 2;
                    break;
                case 0x0003:
                    //V[x] = Bitwise XOR of V[x] and V[y]
                    V[x] = V[x] ^ V[y];
                    pc += 2;
                    break;
                case 0x0004:
                    //V[x] = V[x] + V[y], set carry flag as needed
                    V[x] = V[x] + V[y]; //Still need to do the carry flag for all these opcodes!
                    pc += 2;
                    break;
                case 0x0005:
                    //V[x] = V[x] - V[y], set carry flag when there isn't a borrow, 0 when there is
                    V[x] = V[x] - V[y];
                    pc += 2;
                    break;
                case 0x0006:
                    //V[x] is shited to the right by one, carry flag is set to V[x]'s original least significant bit
                    V[15] = V[x] & 1;
                    V[x] = V[x] >> 1;
                    pc += 2;
                    break;
                case 0x0007:
                    //V[x] = V[y] - V[x], set carry flag when there isn't a borrow, 0 when there is
                    V[x] = V[y] - V[x];
                    pc += 2;
                    break;
                case 0x000E:
                    //V[x] is shited to the left by one, carry flag is set to V[x]'s original least significant bit
                    V[15] = V[x] & 1;
                    V[x] = V[x] << 1;
                    pc += 2;
                    break;
            }
            break;
        case 0x9000:
            //Skip next instruction if V[x] != V[y]
            if(V[x] != V[y]){
                pc += 4;
            }
            else{
                pc += 2;
            }
            break;
        case 0xA000: // ANN, Sets I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000:
            // Jump to NNN + V[0]
            pc = (0x0FFF & opcode) + V[0];
            break;
        case 0xC000:
            //Set V[x] to a bitwise and on a random number (0 to 255) and NN
            break;
        case 0xD000:
            //Draw sprite at (V[x], V[y]) with a width of 8 pixels and a height of N pixels. Carry Flag set to 1 if any pixels are flipped from set to unset.
        case 0xE000:
            switch(opcode & 0x00FF){
                case 0x009E:
                    //Skip next instruction if key stored in V[x] is pressed.
                    if(key[V[x]]){ //THIS IS PROBABLY VERY WRONG!!!!
                        pc += 4;
                    }
                    else{
                        pc += 2;
                    }   
                    break;
                case 0x00A1:
                    //Skip next instruction if key stored in V[x] isn't pressed.
                    if(!key[V[x]]){ //THIS IS PROBABLY VERY WRONG!!!!!
                        pc += 4;
                    }
                    else{
                        pc += 2;
                    }   
                    break;
                default:
                    printf("ERROR IN E SWITCH!\n");
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF){
                case 0x0007:
                    //Set V[x] to the value of the delay timer.
                    V[x] = delay_timer;
                    pc += 2;
                    break;
                case 0x000A:
                    //Key press is awaited, then stored in V[x]. Instructions halted until next key event.
                    break;
                case 0x0015:
                    //Set delay timer to V[x]
                    delay_timer = V[x];
                    pc += 2;
                    break;
                case 0x0018:
                    //Set sound timer to V[x]
                    sound_timer = V[x];
                    pc += 2;
                    break;
                case 0x001E:
                    //Add V[x] to I
                    I += V[x];
                    pc += 2;
                    break;
                case 0x0029:
                    //"Sets I to the location of the sprite for the character in VX"
                    break;
                case 0x0033:
                    //"Stores the binary-coded decimal representation of VX" Look at Wikipedia for the description of this
                    break;
                case 0x0055:
                    //Stores V0 to VX (inclusive) in memory starting at address I
                    break;
                case 0x0065:
                    //Fills V0 to VX (inclusive) with values from memory starting at address I
                    break;
            }
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