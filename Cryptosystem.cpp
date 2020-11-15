#define _CRT_SECURE_NO_WARNINGS
#define CYPHERLEN 100000
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

//flag_16==true - Feistel
//flag_16==false - SP
int S_box[] = {3,9,15,10,5,11,13,2,6,4,12,0,14,7,1,8};
int P_block_16[] = {1 ,10 ,3 ,12 ,5 ,14 ,7 ,0 ,9 ,2 ,11 ,4 ,13 ,6 ,15 ,8};
int P_block_32[] = {17 ,10 ,3 ,28 ,21 ,14 ,7 ,0 ,25 ,18 ,11 ,4 ,29 ,22 ,15 ,8 ,1 ,26 ,19 ,12 ,5 ,30 ,23 ,16 ,9 ,2 ,27 ,20 ,13 ,6 ,31 ,24};

uint16_t keys_16[3];
uint32_t keys_32[2];
//uint32_t CypherText[CYPHERLEN]={0};

uint32_t read_key(string parametr)
{
    string name="key"+parametr+".in";
}

uint32_t read_key()
{
    string name="example_key.in";
    std::ifstream in(name);
    uint32_t result=0;
    char input[4]={0};
    if (in.is_open())
    {
        in.read(input,4);
        result=(uint8_t)input[0]<<24;
        result+=(uint8_t)input[1]<<16;
        result+=(uint8_t)input[2]<<8;
        result+=(uint8_t)input[3];
    }
    in.close();
    return result;

}

void write_in_file(uint32_t& block)
{
    string name="example1.out";
    ofstream out;
    out.open(name);
    char output[4]={0};
        output[0] = block >> 24;
        output[1] = (block << 8) >> 24;
        output[2] = (block << 16) >> 24;
        output[3] = (block << 24) >> 24;
    out.write(output,4);
}

//void read_open_text(uint32_t * )
//n=3
//0101 0000 1111 1100
//part1= 0100 0000 0000 0000;
//part2= 0000 1010 0001 1111;
//result=0100 1010 0001 1111;
//checked
uint16_t cycle_roll_left(uint16_t value, int n)
{
    uint16_t part1, part2, result;
    part1=value>>(16-n);
    part2=value<<n;

    result=part1|part2;
    return result;
}

//checked
uint16_t cycle_roll_right(uint16_t value, int n)
{
    uint16_t part1, part2, result;
    part1=value<<(16-n);
    part2=value>>n;

    result=part1|part2;
    return result;
}

//checked
void key_scheduler(uint32_t k)
{
//(k1)&(k2<2) k2<11,k1<2 k2<8 k2 k2<1,k2

    //uint32_t k=read_key();
    uint16_t k1,k2;
    k2=k;
    k1=k>>16;
    cout<<hex<<"k1: "<<k1<<endl;
    cout<<hex<<"k2: "<<k2<<endl;
    keys_16[0]=k1&cycle_roll_left(k2,2);
    keys_32[0]=(cycle_roll_left(k2,11)<<16)+cycle_roll_left(k1,2);
    keys_16[1]=cycle_roll_left(k2,8);
    keys_16[2]=k2;
    uint32_t tmp=cycle_roll_left(k2,1);
    tmp<<=16;
    keys_32[1]=tmp+k2;

//    cout<<hex<<"k: "<<k<<endl;
//    cout<<hex<<"k16 0 : "<<keys_16[0]<<endl;
//    cout<<hex<<"k32 0: "<<keys_32[0]<<endl;
//    cout<<hex<<"k16 1 : "<<keys_16[1]<<endl;
//    cout<<hex<<"k16 2 k2 : "<<keys_16[2]<<endl;
//    cout<<hex<<"k32 1 : "<<keys_32[1]<<endl;
}

//checked
uint16_t SP_16(int stage, uint16_t& X)
{
    uint16_t y=0, k=keys_16[stage];
    uint16_t xored=X^k;
    uint8_t S=0;
    uint16_t p_block[16]={0};
    for (int i=0; i<4; i++)
    {
        int index=16-(4*(i+1));
        //1111 1111 0000 0000
        S=xored>>index;
        xored-=S<<index;
        S=S_box[S];

        for (int k=0; k<4; k++) {
            p_block[i * 4 + k] = S >> (3-k);
            if (k==3)
                p_block[i*4+3]=S;
            else
                S -= p_block[i * 4 + k] << (3-k);
        }
    }
//    cout<<endl;
    for (int i=0; i<16; i++)
    {
//        cout<< hex << p_block[i]<<", ";
        y+=p_block[i]<<(P_block_16[15-i]);
    }
//    cout<<endl;
//    cout << "SP_16: " << hex << y << endl;
    return y;
}

//checked
uint32_t SP_32(int stage, uint32_t& X)
{
    uint32_t y=0, k=keys_32[stage];
    uint32_t xored=X^k;
    uint8_t S=0;
    uint32_t p_block[32]={0};
    cout<<"SP 32 with key "<< k << endl;
    for (int i=0; i<8; i++)
    {
        int index=32-(4*(i+1));
        //1111 1111 0000 0000
        S=xored>>index;
        xored-=S<<index;
        S=S_box[S];

        for (int k=0; k<4; k++) {
            p_block[i * 4 + k] = S >> (3-k);
            if (k==3)
                p_block[i*4+3]=S;
            else
                S -= p_block[i * 4 + k] << (3-k);
        }
    }
    for (int i=0; i<32; i++)
    {
        cout<< hex << p_block[i]<<", ";
        y+=p_block[i]<<(P_block_32[31-i]);
    }
    cout<<endl;
    cout << "SP_32: " << hex << y << endl;
    return y;
}

//checked
uint32_t Feistel(uint32_t& X, int stage)
{
    uint16_t x1,x2;
    x1=X>>16;
    x2=X;
//    cout << "X: " << hex << X << endl;
//    cout << "x1: " << hex << x1 << endl;
//    cout << "x2: " << hex << x2 << endl;
    uint32_t tmp=x2<<16;
    uint32_t block=tmp+(x1^SP_16(stage, x2));

    cout<<"Feistel: "<< hex << block<< endl;
    return block;
}


uint32_t Tau(uint32_t block)
{
    uint32_t y1,y2;
    uint32_t result;
    y1=block>>16;
    y2=block-(y1<<16);
    result=(y2<<16)+y1;
    return result;

}

uint32_t Whitening(uint32_t block, int stage)
{
    return block^keys_32[stage];
}

void encryption(uint32_t block)
{
    //fsftfw
    block=Feistel(block, 0);
    block=SP_32(0, block);
    block=Feistel(block, 1);
    block=Tau(block);
    block=Feistel(block, 2);
    block=Whitening(block, 1);
    write_in_file(block);
}

void cipher_body()
{
    bool TestMode=true;
    if (TestMode)
    {
        key_scheduler(read_key());
    }
    string name="example1.in";
    std::ifstream in(name);
    if (!in.is_open()) {
        cout << "File can not be opened!" << endl;
        EXIT_FAILURE;
    }

    while(!in.eof())
    {
        char input[4]={0};
        uint32_t block;
        in.read(input, 4);

        if (in.eof())
        {
            switch(in.gcount())
            {
                case(0):
                {
                    input[0]=0x80;
                    break;
                }
                case(1):
                {
                    input[1]=0x80;
                    break;
                }
                case(2):
                {
                    input[2]=0x80;
                    break;
                }
                case(3):
                {
                    input[3]=0x80;
                    break;
                }
            }

        }

        block=(uint8_t)input[0]<<24;
        block+=(uint8_t)input[1]<<16;
        block+=(uint8_t)input[2]<<8;
        block+=(uint8_t)input[3];
        encryption(block);

    }
    in.close();





}


int main() {
    std::cout << "Hello, World!" << std::endl;
    cipher_body();
    return 0;
}
