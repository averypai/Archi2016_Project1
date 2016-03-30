#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.c"

    int opcode;
    int rs;
    int rt;
    int rd;
    int shamt;
    int funct;
int immi;

int inst[256]={0};
int reg[32]={0};
int PC=0;
int sp=0;
int dnum=0;
int inum=0;
unsigned char instmemory[256][4]={'\0'};
unsigned char datamemory[1024]={'\0'};
void print();

int cycle=0;
void readfile(){
    FILE*iimage,*dimage,*error,*snapshot;
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
            }
        }
        else if(i>1){
            for(k=0;k<4;k++)
            {
                inst[j]=inst[j]<<8|instmemory[i][k];//inst number
            }
            j++;
        }
        i++;
    }
    i=0;
    while(fread(&datamemory[i], sizeof(char), 1, dimage)!=0)
    {
        if(i==0){
            for(k=0;k<4;k++)
            {
                sp=sp<<8|datamemory[0];//initial sp
            }
            reg[29]=sp;
        }
        else if(i==1)
        {
            for(k=0;k<4;k++)
            {
                dnum=dnum<<8|datamemory[1];//data number
            }
        }
        printf("%02X\n",datamemory[i] );
        i++;
    }
}
void cut(int get){//get rs rt rd shamt funct address opcode
    opcode=inst[PC]>>26;//opcode here here here here

    if(halt)
    {
        exit(1);
    }
    else if(opcode==0x00)
    {
        rs=(inst[get]<<6)>>27;
        rt=(inst[get]<<11)>>27;
        rd=(inst[get]<<16)>>27;
        shamt=(inst[get]<<21)>>27;
        funct=(inst[get]<<26)>>26;
    }
    else if(jump||jal)
    {
        immi=(inst[get]<<6)>>6;
    }
    else
    {
        rs=(inst[get]<<6)>>27;
        rt=(inst[get]<<11)>>27;
        immi=(inst[get]<<16)>>16;
    }
}

void print(){
    int i=0;
    printf("cycle %d\n",cycle);
    for(i=0;i<32;i++)
    {
        printf("$%d:0x%08X\n",i,reg[i]);
    }
    printf("PC:0x%08X\n\n",PC);
}

int main()
{
    int getins=0;
    readfile();
    print();
    cycle++;
    while (cycle<500000) {
	 printf("cycle==========%d\n",cycle);
        getins=PC/4;
	 cut(getins);
     
     print();
     cycle++;
    }
    return 0;
}