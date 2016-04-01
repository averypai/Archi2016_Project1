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
    printf("add $%d,$%d,$%d\n",rd,rs,rt);
    if(( reg[rs]>0 && reg[rt]>0 && temp<=0) || (reg[rs]<0 && reg[rt]<0 && temp>=0 ))
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    reg[rd] = temp;
    PC += 4;
    reg[rd]=reg[rs]+reg[rt];
}

void addu()
{
    reg[rd]=(unsigned)reg[rs]+(unsigned)reg[rt];
    PC+=4;
}

void sub()
{
    printf("sub $%d,$%d,$%d\n",rd,rs,rt);
    int tempReg = -reg[rt], temp;
    temp = reg[rs] + tempReg;
    if(( reg[rs]>0 && tempReg>0 && temp<=0 )|| (reg[rs]<0 && tempReg<0 && temp>=0) )
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    reg[rd] = temp;
    PC += 4;
}

void And()
{
    printf("and %d %d %d\n",rs ,rt, rd);
    reg[rd]=reg[rs]&reg[rt];
    PC+=4;
}

void Or()
{
    printf("or %d %d %d\n",rs ,rt, rd);
    reg[rd]=reg[rs]|reg[rt];
    PC+=4;
}

void Xor()
{
    printf("xor %d %d %d\n",rs ,rt, rd);
    reg[rd]=reg[rs]^reg[rt];
    PC+=4;
}

void nor()
{
    printf("nor %d %d %d\n",rs ,rt, rd);
    reg[rd]=~(reg[rs]|reg[rt]);
    PC+=4;
}

void nand()
{
    printf("nand %d %d %d\n",rs ,rt, rd);
    reg[rd]=~(reg[rs]&reg[rt]);
    PC+=4;
}

void slt()
{
    printf("slt %d %d %d\n",rs ,rt, rd);
    reg[rd]=(reg[rs]<reg[rt]);
    PC+=4;
}

void sll()
{
    printf("sll %d %d %d\n",rs ,rt, shamt);
    reg[rd]=reg[rt]<<shamt;
    PC+=4;
}

void srl()
{
    printf("sll %d %d %d\n",rs ,rt, shamt);
    reg[rd]=((unsigned)reg[rt]>>shamt);
    PC+=4;
}

void sra()
{
    printf("sra %d %d %d\n",rd ,rt, shamt);
    reg[rd]=reg[rt]>>shamt;
    PC+=4;
}

void jr()
{
    printf("jr %d\n",rs);
    PC=reg[rs];
}
//I type

void addi()
{
     printf("addi %d %d %d\n",rs ,rt, immi);
     if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
     int temp=reg[rs]+immi;
     if((reg[rs]>0&&immi>0&&temp<=0)||(reg[rs]<0&&immi<0&&temp>0))
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
        printf("addiu %d %d %d\n",rs ,rt, immi);
        int temp=reg[rs]+immi;
        reg[rt]=temp;
    }
    PC+=4;
}


int lw(){
    int flag=0;
    int temp=reg[rs]+immi;
    //printf("temp=============%d %d %d\n",temp,reg[rs],immi);
    //printf("lw %d %d %d\n",rt ,rs, immi);

     if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }

    if(( reg[rs]>0 && immi>0 && temp<=0) || (reg[rs]<0 &&immi<0 && temp>=0 ))
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
   /* for(i=0;i<50;i++)
    {
        printf("DATA %08X\n",datamemory[i]);
    }*/
    for(i=0;i<4;i++)
    {
        reg[rt]=reg[rt]<<8|datamemory[temp+i];
        printf("lw %08X===%d",datamemory[temp+i],temp+i);
    }
    PC+=4;
    return 0;
}
int lh(){
    printf("lh %d %d %d\n",rs ,rt, immi);
    int flag=0;
     if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    int temp=reg[rs]+immi;
    if(( reg[rs]>0 && immi>0 && temp<=0) || (reg[rs]<0 &&immi<0 && temp>=0 ))
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
    reg[rt]=(datamemory[temp]>>31);
    printf("reg[rt]======%d\n",reg[rt]);
    printf("temp======%d\n",temp);
    for(i=0;i<2;i++)
    {
        reg[rt]=reg[rt]<<8|datamemory[temp+i];
        printf("Dtemp======%08X\n",datamemory[temp+i]);
    }
    //reg[rt]=(reg[rt]<<16)>>16;
    PC+=4;
    return 0;
}

int lhu(){
   printf("lhu %d %d %d\n",rs ,rt, immi);
    int flag=0;
     if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    int temp=reg[rs]+immi;
    if(( reg[rs]>0 && immi>0 && temp<=0) || (reg[rs]<0 &&immi<0 && temp>=0 ))
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
    printf("lb %d %d %d\n",rs ,rt, immi);
    int flag=0;
     if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    int temp=reg[rs]+immi;
    if(( reg[rs]>0 && immi>0 && temp<=0) || (reg[rs]<0 &&immi<0 && temp>=0 ))
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
    printf("lb %d %d %d\n",rs ,rt, immi);
    int flag=0;
     if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    int temp=reg[rs]+immi;
    if(( reg[rs]>0 && immi>0 && temp<=0) || (reg[rs]<0 &&immi<0 && temp>=0 ))
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
    printf("sw $%d,%d($%d)\n",rt,temp,rs);
    if(( reg[rs]>0 && immi>0 && tempadd<=0) || (reg[rs]<0 &&immi<0 && tempadd>=0 ))
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
    printf("sw $%d,%d($%d)\n",rt,temp,rs);
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
    datamemory[tempadd]=((unsigned)reg[rt]>>24);
    datamemory[tempadd+1]=((unsigned)reg[rt]<<8)>>24;
    PC+=4;
    return 0;
}

int sb(){
    int flag=0;
    int temp=(immi<<16)>>16;
    int tempadd=reg[rs]+temp;
    printf("sw $%d,%d($%d)\n",rt,temp,rs);
    if(( reg[rs]>0 && immi>0 && tempadd<=0) || (reg[rs]<0 &&immi<0 && tempadd>=0 ))
    {
        fprintf(error , "In cycle %d: Number Overflow\n", cycle);
    }
    if(tempadd>1023 || tempadd<0)
    {
        flag=1;
        fprintf(error , "In cycle %d: Address Overflow\n", cycle);
    }
    datamemory[tempadd]=((unsigned)reg[rt]>>24);
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
    printf("lui %d %d\n",rt, immi);
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
    printf("andi %d %d %d\n",rs ,rt, immi);
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
        printf("ori %d %d %d\n",rs ,rt, immi);
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
        printf("nori %d %d %d\n",rs ,rt, immi);
        reg[rt]=~(reg[rs]|((unsigned)immi));
    }
    PC+=4;
}

void slti(){
    if(rt==0)
    {
        fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
    }
    else
    {
        reg[rt]=(reg[rs]<immi)?1:0;
        printf("slti %d %d %d\n",rs ,rt, immi);
    }
    PC+=4;
}
////////////////////PC related
void beq(){
    int temp=PC+4+(4*immi);
    if(reg[rs]==reg[rt])
    {
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
    printf("BNE:%05X %05X %16X\n",rs,rt,immi);
    int temp=PC+4+(4*immi);

    if(reg[rs]!=reg[rt])
    {
        if(temp<PC)
        {
            change=1;
        }
         else
            PC=temp;
            newPC=temp;
        // printf("%08X\n",temp);
    }else{
        PC+=4;
        newPC=PC;
       // printf("BNE PC:%08X\n",PC);
    }

}


void bgtz(){
    int temp=PC+4+(4*immi);
    if(reg[rs]>0)
     {
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
       // printf("BNE PC:%08X\n",PC);
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
