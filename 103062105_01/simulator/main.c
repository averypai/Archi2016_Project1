#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define halt 0x3F
#define Jump 0x02
#define Jal 0x03
#include "function.h"
int opcode,rs,rt,rd,shamt,funct,immi;
int reg[32]={0};
int PC=0,sp=0,dnum=0,inum=0,cycle=0,newPC=0,change=0;
unsigned int inst[256]={0};
unsigned char instmemory[256][4];
unsigned char datamemory[1024]={'\0'};
unsigned char t_datamemory[256][4]={'\0'};
void print();
FILE*iimage,*dimage,*error,*snapshot;

void readfile(){
    //open file
    iimage=fopen("iimage.bin","rb");
    dimage=fopen("dimage.bin","rb");
    error=fopen("error_dump.rpt","wb");
    snapshot=fopen("snapshot.rpt","wb");

    if(!dimage)
    {
        printf("Can't read dimage.");
        exit(1);
    }
    if(!iimage)
    {
        printf("Can't read iimage.");
        exit(1);
    }
    //read file
    int i=0,k=0,j=0;
    while(fread(instmemory[i], sizeof(char), 4, iimage)!=0)
    {
        if(i==0){

            for(k=0;k<4;k++)
            {
                PC=PC<<8|instmemory[0][k];//initial PC
                j=PC/4;
            }
        }
        else if(i==1)
        {
            for(k=0;k<4;k++)
            {
                inum=inum<<8|instmemory[1][k];//initial PC
            }
        }
        else if(i>1&&i<=inum+1){
            for(k=0;k<4;k++)
            {
                inst[j]=((unsigned)inst[j]<<8)|instmemory[i][k];//inst number
            }
            j++;
        }
        else break;
        i++;
    }
    i=0;
    while(fread(t_datamemory[i], sizeof(char), 4, dimage)!=0)
    {
        if(i==0){
            for(k=0;k<4;k++)
            {
                sp=sp<<8|t_datamemory[0][k];//initial sp
            }
            reg[29]=sp;
        }
        else if(i==1)
        {
            for(k=0;k<4;k++)
            {
                dnum=dnum<<8|t_datamemory[1][k];//data number
            }
        }
        else if(i>=2&&i<=dnum+1)
        {//store i>1 data
                datamemory[(i-2)*4]=t_datamemory[i][0];
                datamemory[(i-2)*4+1]=t_datamemory[i][1];
                datamemory[(i-2)*4+2]=t_datamemory[i][2];
                datamemory[(i-2)*4+3]=t_datamemory[i][3];
        }
        else break;
        i++;
    }
}
void cut(int get){//get rs rt rd shamt funct address opcode
    opcode=inst[get]>>26;//opcode here here here here
    if(opcode==halt)
    {
        exit(1);
    }
    else if(opcode==0x00)//R type
    {
        rs=(inst[get]<<6)>>27;
        rt=(inst[get]<<11)>>27;
        rd=(inst[get]<<16)>>27;
        shamt=(inst[get]<<21)>>27;
        funct=(inst[get]<<26)>>26;
        if(rd==0&&(funct!=0x00)&&(funct!=0x08))
        {
            fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
        }
        else if(rd==0&&funct==0x00&&(rt!=0))
        {
            fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
        }
        else if(rd==0&&funct==0x00&&(rt==0)&&shamt!=0)
        {
            fprintf(error, "In cycle %d: Write $0 Error\n", cycle);
        }

    }
    else if(opcode==Jump||opcode==Jal)//J type
    {
        immi=(inst[get]<<6)>>6;
    }
    else//I type
    {
        rs=(inst[get]<<6)>>27;
        rt=(inst[get]<<11)>>27;
        immi=(inst[get]<<16)>>16;
    }
    reg[0]=0;
}

void function(){

    if(opcode==0x00)
    {
        switch (funct) {
            case 0x20: add();break;
            case 0x21: addu();break;
            case 0x22: sub();break;
            case 0x24: And();break;
            case 0x25: Or();break;
            case 0x26: Xor();break;
            case 0x27: nor();break;
            case 0x28: nand(); break;
            case 0x2A: slt(); break;
            case 0x00: sll(); break;
            case 0x02: srl(); break;
            case 0x03: sra(); break;
            case 0x08: jr(); break;
            default:
                exit(1);
                break;
        }
    }
    else{
            switch (opcode) {
                case 0x3F: exit(1); break;
                case 0x08: addi(); break;
                case 0x09: addiu(); break;
                case 0x23: lw(); break;
                case 0x21: lh(); break;
                case 0x25: lhu(); break;
                case 0x20: lb(); break;
                case 0x24: lbu();break;
                case 0x2B: sw();break;
                case 0x29: sh();break;
                case 0x28: sb();break;
                case 0x0F: lui();break;
                case 0x0C: andi();break;
                case 0x0D: ori();break;
                case 0x0E: nori();break;
                case 0x0A: slti();break;
                case 0x04: beq();break;
                case 0x05: bne();break;
                case 0x07: bgtz();break;
                case 0x02: jump();break;
                case 0x03: jal();break;
                default:
                    exit(1);
                    break;
            }
        }
     reg[0]=0;
    }

void print(){
    int i=0;
    if(newPC < PC&&change!=0)
    {
        change=0;
        while(newPC<PC)
        {
            PC=newPC;
            fprintf(snapshot,"cycle %d\n",cycle);
            for(i=0;i<32;i++)
            {
                fprintf(snapshot,"$%02d: 0x%08X\n",i,reg[i]);
            }
            fprintf(snapshot,"PC: 0x%08X\n\n\n",PC);
            newPC+=4;
        }
    }
    else
    {
        fprintf(snapshot,"cycle %d\n",cycle);
        for(i=0;i<32;i++)
        {
            fprintf(snapshot,"$%02d: 0x%08X\n",i,reg[i]);
        }
        fprintf(snapshot,"PC: 0x%08X\n\n\n",PC);
    }
}

int main()
{
    int getins=0;
    readfile();
    print();
    cycle++;
    int i=0;

    while (cycle<500000) {
     getins=PC/4;
	 cut(getins);
     function();
     print();
     cycle++;
    }
    return 0;
}
