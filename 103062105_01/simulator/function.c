#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function.h"

extern int rs,rt,rd,shamt,opcode,immi,PC,cycle,newPC,change;
extern unsigned char datamemory[1024];
extern FILE *error;
extern int reg[32];
void add()
{
    int temp = reg[rs] + reg[rt];
    int sign1=((unsigned)reg[rs]>>31);
    int sign2=((unsigned)reg[rt]>>31);
    int sign3=((unsigned)temp>>31);
    if((sign1==sign2)&&(sign1!=sign3))
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    reg[rd] = temp;
    PC += 4;
}

void addu()
{
    reg[rd]=(unsigned)reg[rs]+(unsigned)reg[rt];
    PC+=4;
}

void sub()
{
    int tempReg = -reg[rt], temp;
    temp = reg[rs] + tempReg;
    int sign1=((unsigned)reg[rs]>>31);
    int sign2=((unsigned)tempReg>>31);
    int sign3=((unsigned)temp>>31);

    if((sign1==sign2)&&(sign1!=sign3))
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    reg[rd] = temp;
    PC += 4;
}

void And()
{
    reg[rd]=reg[rs]&reg[rt];
    PC+=4;
}

void Or()
{
    reg[rd]=reg[rs]|reg[rt];
    PC+=4;
}

void Xor()
{
    reg[rd]=reg[rs]^reg[rt];
    PC+=4;
}

void nor()
{
    reg[rd]=~(reg[rs]|reg[rt]);
    PC+=4;
}

void nand()
{
    reg[rd]=~(reg[rs]&reg[rt]);
    PC+=4;
}

void slt()
{
    reg[rd]=(reg[rs]<reg[rt]);
    PC+=4;
}

void sll()
{
    reg[rd]=reg[rt]<<shamt;
    PC+=4;
}

void srl()
{
    reg[rd]=((unsigned)reg[rt]>>shamt);
    PC+=4;
}

void sra()
{
    reg[rd]=reg[rt]>>shamt;
    PC+=4;
}

void jr()
{
    PC=reg[rs];
}
//I type

void addi()
{
     if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
     int temp=reg[rs]+((immi<<16)>>16);
     if((reg[rs]>0&&((immi<<16)>>16)>0&&temp<0)||(reg[rs]<0&&((immi<<16)>>16)<0&&temp>0))
     {
         fprintf(error , "In cycle %d: Number Overflow\n", cycle);
     }
    reg[rt]=temp;
    PC+=4;

}

void addiu()
{
    if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    else
    {
        int temp=reg[rs]+((immi<<16)>>16);
        reg[rt]=temp;
    }
    PC+=4;
}


int lw(){
    int flag=0;
    int temp=reg[rs]+((immi<<16)>>16);
    if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }

    int sign1=((unsigned)reg[rs]>>31);
    int sign2=((unsigned)((immi<<16)>>16)>>31);
    int sign3=((unsigned)temp>>31);
    if((sign1==sign2)&&(sign1!=sign3))
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    if(temp>1020 || temp<0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Address Overflow\n", cycle);
    }
    if((temp%4)!=0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Misalignment Error\n", cycle);
    }
    if(flag==1)
        exit(1);
    int i=0;
    for(i=0;i<4;i++)
    {
        reg[rt]=reg[rt]<<8|datamemory[temp+i];
    }
    PC+=4;
    return 0;
}
int lh(){

    int flag=0;
     if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    int temp=reg[rs]+((immi<<16)>>16);
    int sign1=((unsigned)reg[rs]>>31);
    int sign2=((unsigned)((immi<<16)>>16)>>31);
    int sign3=((unsigned)temp>>31);
    if((sign1==sign2)&&(sign1!=sign3)){
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    if(temp>1022 || temp<0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Address Overflow\n", cycle);
    }
    if((temp%2)!=0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Misalignment Error\n", cycle);
    }
    if(flag==1)
        exit(1);
    int i=0;
    reg[rt]=(datamemory[temp]>>31);
    for(i=0;i<2;i++)
    {
        reg[rt]=reg[rt]<<8|datamemory[temp+i];
    }
    reg[rt]=(reg[rt]<<16)>>16;
    PC+=4;
    return 0;
}

int lhu(){
    int flag=0;
     if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    int temp=reg[rs]+((immi<<16)>>16);
    int sign1=((unsigned)reg[rs]>>31);
    int sign2=((unsigned)((immi<<16)>>16)>>31);
    int sign3=((unsigned)temp>>31);
    if((sign1==sign2)&&(sign1!=sign3))
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    if(temp>1022 || temp<0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Address Overflow\n", cycle);
    }
    if((temp%2)!=0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Misalignment Error\n", cycle);
    }
    if(flag==1)
        exit(1);
        int i=0;
    for(i=0;i<2;i++)
    {
        reg[rt]=reg[rt]<<8|datamemory[temp+i];
    }
    reg[rt]=((unsigned)reg[rt]<<16)>>16;
    PC+=4;
    return 0;
}

int lb(){

    int flag=0;
     if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    int temp=reg[rs]+((immi<<16)>>16);
    int sign1=((unsigned)reg[rs]>>31);
    int sign2=((unsigned)((immi<<16)>>16)>>31);
    int sign3=((unsigned)temp>>31);
    if((sign1==sign2)&&(sign1!=sign3))
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    if(temp>1023 || temp<0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Address Overflow\n", cycle);
    }
    if(flag==1)
        exit(1);
    reg[rt]=reg[rt]<<8|datamemory[temp];
    reg[rt]=(reg[rt]<<24)>>24;
    PC+=4;
    return 0;
}

int lbu(){

    int flag=0;
     if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    int temp=reg[rs]+((immi<<16)>>16);
    int sign1=((unsigned)reg[rs]>>31);
    int sign2=((unsigned)((immi<<16)>>16)>>31);
    int sign3=((unsigned)temp>>31);
    if((sign1==sign2)&&(sign1!=sign3))
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    if(temp>1023 || temp<0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Address Overflow\n", cycle);
    }
    if(flag==1)
        exit(1);
    reg[rt]=reg[rt]<<8|datamemory[temp];
    reg[rt]=((unsigned)reg[rt]<<24)>>24;
    PC+=4;
    return 0;
}

int sw(){
    int flag=0;
    int temp=(immi<<16)>>16;
    int tempadd=reg[rs]+temp;

    int sign1=((unsigned)reg[rs]>>31);
    int sign2=((unsigned)temp>>31);
    int sign3=((unsigned)tempadd>>31);
    if((sign1==sign2)&&(sign1!=sign3))
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    if(tempadd>1020 || tempadd<0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Address Overflow\n", cycle);
    }
    if((tempadd%4)!=0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Misalignment Error\n", cycle);
    }
    if(flag==1)
        exit(1);
    datamemory[tempadd]=((unsigned)reg[rt]>>24);
    datamemory[tempadd+1]=((unsigned)reg[rt]<<8)>>24;
    datamemory[tempadd+2]=((unsigned)reg[rt]<<16)>>24;
    datamemory[tempadd+3]=((unsigned)reg[rt]<<24)>>24;
    PC+=4;
    return 0;
}

int sh(){
    int flag=0;
    int temp=(immi<<16)>>16;
    int tempadd=reg[rs]+temp;
    if(( reg[rs]>0 && immi>0 && tempadd<=0) || (reg[rs]<0 &&immi<0 && tempadd>=0 ))
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    if(tempadd>1022 || tempadd<0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Address Overflow\n", cycle);
    }
    if((tempadd%2)!=0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Misalignment Error\n", cycle);
    }
    if(flag==1)
        exit(1);
    datamemory[tempadd]=((unsigned)reg[rt]&0x0000FF00)>>8;
    datamemory[tempadd+1]=((unsigned)reg[rt]&0x000000FF);
    PC+=4;
    return 0;
}

int sb(){
    int flag=0;
    int temp=(immi<<16)>>16;
    int tempadd=reg[rs]+temp;
    if(( reg[rs]>0 && immi>0 && tempadd<=0) || (reg[rs]<0 &&immi<0 && tempadd>=0 ))
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    if(tempadd>1023 || tempadd<0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Address Overflow\n", cycle);
        exit(1);
    }
    datamemory[tempadd]=((unsigned)reg[rt]&0x000000FF);
    PC+=4;
    return 0;
}

void lui()
{
    if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    else
    {
    reg[rt]=immi<<16;
    }
    PC+=4;
}

void andi()
{
    if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    else
    {
    reg[rt]=reg[rs]&((unsigned)immi);
    }
    PC+=4;
}

void ori()
{
    if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    else
    {
        reg[rt]=reg[rs]|((unsigned)immi);
    }
    PC+=4;
}

void nori(){
    if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    else
    {
        reg[rt]=~(reg[rs]|((unsigned)immi));
    }
    PC+=4;
}

void slti(){
    immi=(immi<<16)>>16;
    if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    else
    {
        reg[rt]=(reg[rs]<immi)?1:0;
    }
    PC+=4;
}
////////////////////PC related
void beq(){
    if(reg[rs]==reg[rt])
    {
        immi=(immi<<16)>>16;
        int temp=PC+4+(4*immi);
        if(temp<PC)
        {
            change=1;
        }
         else
            PC=temp;
            newPC=temp;
    }else{
        PC+=4;
        newPC=PC;
    }
}

void bne(){
    if(reg[rs]!=reg[rt])
    {
        immi=(immi<<16)>>16;
        int temp=PC+4+(4*immi);
        if(temp<PC)
        {
            change=1;
        }
         else
            PC=temp;
            newPC=temp;
    }else{
        PC+=4;
        newPC=PC;
    }

}


void bgtz(){
    if(reg[rs]>0)
     {
         immi=(immi<<16)>>16;
        int temp=PC+4+(4*immi);
         if(temp<PC)
        {
            change=1;
        }
         else
            PC=temp;
         newPC=temp;
    }else
        {
        PC+=4;
        newPC=PC;
    }
}

void jump(){
    unsigned int newadd=(PC+4);
    immi=immi<<2;
    int temp=((newadd>>28)<<28)|immi;
    if(temp<PC){
            change=1;
            newPC=temp;
        }
    else{
        PC=temp;
        newPC=PC;
    }
}

void jal(){
    reg[31]=PC+4;
    unsigned int newadd=(PC+4);
    immi=immi<<2;
    int temp=((newadd>>28)<<28)|immi;
    if(temp<PC){
            change=1;
            newPC=temp;
        }
    else{
        PC=temp;
        newPC=PC;
    }
}
