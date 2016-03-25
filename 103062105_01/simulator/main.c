#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "regfile.c"
//#include "memory.c"
#include "instruction.c"

typedef struct Instruction{
    unsigned int opcode;
    unsigned int rs;
    unsigned int rt;
    unsigned int rd;
    unsigned int shamt;
    unsigned int funct;
}Instruction;

unsigned int inst[256]={0};
unsigned int reg[32]={0};
unsigned int PC=0;
unsigned int sp=0;
unsigned int dnum=0;
unsigned int inum=0;
unsigned int increasing=0;
unsigned char instmemory[256][4]={'\0'};
unsigned char datamemory[256][4]={'\0'};
unsigned char temp_datamemory[1024]={'\0'};
//unsigned char tempmemory[1024]={'\0'};
void print();
void _error();
Instruction execI(Instruction ins);
Instruction execR(Instruction ins);
Instruction execJ(Instruction ins);
int cycle=0;
int error_flag[4]={0};
FILE*iimage,*dimage,*error,*snapshot;

void readfile(){
    
    //open file
    iimage=fopen("./error2/iimage.bin","rb");
    dimage=fopen("./error2/dimage.bin","rb");
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
    while(fread(datamemory[i], sizeof(char), 4, dimage)!=0)
    {
        if(i==0){
            for(k=0;k<4;k++)
            {
                sp=sp<<8|datamemory[0][k];//initial sp
            }
            reg[29]=sp;
        }
        else if(i==1)
        {
            for(k=0;k<4;k++)
            {
                dnum=dnum<<8|datamemory[1][k];//data number
            }
        }
        else if(i>=2)
        {//store i>1 data
                temp_datamemory[(i-2)*4]=datamemory[i][0];
                temp_datamemory[(i-2)*4+1]=datamemory[i][1];
                temp_datamemory[(i-2)*4+2]=datamemory[i][2];
                temp_datamemory[(i-2)*4+3]=datamemory[i][3];
        }
        i++;
    }
}
Instruction cut(){//get rs rt rd shamt funct address opcode
    Instruction ins;
    ins.opcode=inst[increasing]>>26;//opcode here here here here

    if(ins.opcode==halt)
    {
        return ins;
    }
    else if(ins.opcode==Rtype)
    {
        ins.rs=(inst[increasing]<<6)>>27;
        ins.rt=(inst[increasing]<<11)>>27;
        ins.rd=(inst[increasing]<<16)>>27;
        ins.shamt=(inst[increasing]<<21)>>27;
        ins.funct=(inst[increasing]<<26)>>26;
        if((ins.funct!=0x00)&&(ins.funct!=0x08)&&(ins.rd==0))
        {
            printf("0x%02x\n", ins.funct);
            error_flag[0]=1;
        }
        if((ins.rs>32)||(ins.rs<0))
        {
            error_flag[2]=1;
            ins.opcode=halt;
            _error();
            return ins;
        }
        if((ins.rt>32)||(ins.rt<0))
        {
            error_flag[2]=1;
            ins.opcode=halt;
            _error();
            return ins;
        }
        if((ins.rd>32)||(ins.rd<0))
        {
            error_flag[2]=1;
            ins.opcode=halt;
            _error();
            return ins;
        }
        execR(ins);
    }
    else if(ins.opcode==jump||ins.opcode==jal)
    {
        ins.shamt=(inst[increasing]<<6)>>6;
        execJ(ins);
    }
    else
    {
        ins.rs=(inst[increasing]<<6)>>27;
        ins.rt=(inst[increasing]<<11)>>27;
        ins.shamt=(inst[increasing]<<16)>>16;
        if((ins.funct==0x08)||(ins.funct==0x09)||(ins.funct==0x23)||(ins.funct==0x21)
            ||(ins.funct==0x25)||(ins.funct==0x20)||(ins.funct==0x24)||(ins.funct==0x0F)
            ||(ins.funct==0x0C)||(ins.funct==0x0D)||(ins.funct==0x0E)||(ins.funct==0x0A))
        {
            if(ins.rt==0)
            {   
                printf("%d\n", ins.rt);
                error_flag[0]=1;
            }
        }
        execI(ins);
    }
    return ins;
}

Instruction execJ(Instruction ins)
{
    unsigned int newhead=((PC+increasing*4+4)>>28)<<28;
    unsigned int newaddress=ins.shamt<<2;
    if(ins.opcode==jump)
    {
        increasing=((newhead|newaddress)-PC)/4-1;
        if(increasing>1023||increasing<0)
        {
            error_flag[2]=1;
            ins.opcode=halt;
            _error();
            //print();
            return ins;
        }
    }
    else
    {
        reg[31]=PC+increasing*4+4;
        increasing=((newhead|newaddress)-PC)/4-1;
        if(increasing>1023||increasing<0)
        {
            error_flag[2]=1;
            ins.opcode=halt;
            _error();
            //print();
            return ins;
        }
    }
    increasing++;
    print();
    return ins;
}
Instruction execI(Instruction ins)
{
    int flag=0;
    unsigned int sign=0;
    switch (ins.opcode) {
        case 0x08:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=ins.shamt|0xFFFF0000;
            }
            reg[ins.rt]=reg[ins.rs]+ins.shamt;
            if((ins.shamt>>31)==(reg[ins.rs]>>31))
            {
                if((reg[ins.rt]>>31)!=(reg[ins.rs]>>31))
                {
                    error_flag[1]=1;
                }
            }
            break;
        case 0x09://no overflow detection
            reg[ins.rt]=reg[ins.rs]+ins.shamt;
            break;
        case 0x23:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=ins.shamt|0xFFFF0000;
            }

            if (((reg[ins.rs]+ins.shamt)<0)||((reg[ins.rs]+ins.shamt)>1023))
            {
                error_flag[1]=1;
                ins.opcode=halt;
                _error();
                return ins;
            }
            if(((reg[ins.rs]+ins.shamt)%4)!=0)
            {
                error_flag[1]=1;
                ins.opcode=halt;
            }
             reg[ins.rt]=temp_datamemory[reg[ins.rs]+ins.shamt];
        
            for(int i=1;i<4;i++){    
                reg[ins.rt]=((reg[ins.rt]<<8)|temp_datamemory[reg[ins.rs]+ins.shamt+i]);
            }
            break;
        case 0x21:
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            if (((reg[ins.rs]+ins.shamt)<0)||((reg[ins.rs]+ins.shamt)>1023))
            {
                error_flag[1]=1;
                ins.opcode=halt;
                _error();
                return ins;
            }
            if(((reg[ins.rs]+ins.shamt)%2)!=0)
            {
                error_flag[1]=1;
                ins.opcode=halt;
            }
            sign=temp_datamemory[reg[ins.rs]+ins.shamt]>>7;
            reg[ins.rt]=sign;
            if(sign==1)
            {
                reg[ins.rt]=0xFFFFFF00|temp_datamemory[reg[ins.rs]+ins.shamt];
                reg[ins.rt]=reg[ins.rt]<<8|temp_datamemory[reg[ins.rs]+ins.shamt+1];
            }
            else{
                reg[ins.rt]=((reg[ins.rt]>>8)<<8)|temp_datamemory[reg[ins.rs]+ins.shamt];
                reg[ins.rt]=reg[ins.rt]<<8|temp_datamemory[reg[ins.rs]+ins.shamt+1];
            }
            break;
        case 0x25:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            if (((reg[ins.rs]+ins.shamt)<0)||((reg[ins.rs]+ins.shamt)>1023))
            {
                error_flag[1]=1;
                ins.opcode=halt;
                _error();
                return ins;
            }
            if(((reg[ins.rs]+ins.shamt)%2)!=0)
            {
                error_flag[1]=1;
                ins.opcode=halt;
            }
            reg[ins.rt]=reg[ins.rt]<<16|temp_datamemory[reg[ins.rs]+ins.shamt];
            reg[ins.rt]=reg[ins.rt]<<8|temp_datamemory[reg[ins.rs]+ins.shamt+1];
            break;
        case 0x20:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            sign=temp_datamemory[reg[ins.rs]+ins.shamt]>>7;
            if (((reg[ins.rs]+ins.shamt)<0)||((reg[ins.rs]+ins.shamt)>1023))
            {
                error_flag[1]=1;
                ins.opcode=halt;
                //error();
                //return ins;
            }
            if(sign==1)
            {
                reg[ins.rt]=0xFFFFFF00|temp_datamemory[reg[ins.rs]+ins.shamt];
            }
            else{
                reg[ins.rt]=reg[ins.rt]<<8|temp_datamemory[reg[ins.rs]+ins.shamt];
            }
            break;
        case 0x24:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            if (((reg[ins.rs]+ins.shamt)<0)||((reg[ins.rs]+ins.shamt)>1023))
            {
                error_flag[2]=1;
                ins.opcode=halt;
                _error();
                return ins;
            }
             reg[ins.rt]=reg[ins.rt]<<8|temp_datamemory[reg[ins.rs]+ins.shamt];
            break;
        case 0x2B:
            sign=ins.shamt>>31;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            if((ins.shamt>>31)==(reg[ins.rs]>>31))
            {
                if((reg[ins.rt]>>31)!=(reg[ins.rs]>>31))
                {
                    error_flag[1]=1;
                }
            }
            if (((reg[ins.rs]+ins.shamt)<0)||((reg[ins.rs]+ins.shamt)>1023))
            {
                error_flag[2]=1;
                ins.opcode=halt;
                _error();
                return ins;
            }
            if(((reg[ins.rs]+ins.shamt)%4)!=0)
            {
                error_flag[1]=1;
                ins.opcode=halt;
            }
            temp_datamemory[reg[ins.rs]+ins.shamt]=(char)(reg[ins.rt]>>24);
            temp_datamemory[reg[ins.rs]+ins.shamt+1]=(char)(reg[ins.rt]>>16);
            temp_datamemory[reg[ins.rs]+ins.shamt+2]=(char)(reg[ins.rt]>>8);
            temp_datamemory[reg[ins.rs]+ins.shamt+3]=(char)(reg[ins.rt]);
            break;
        case 0x29:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            if((ins.shamt>>31)==(reg[ins.rs]>>31))
            {
                if((reg[ins.rt]>>31)!=(reg[ins.rs]>>31))
                {
                    error_flag[1]=1;
                }
            }
            if (((reg[ins.rs]+ins.shamt)<0)||((reg[ins.rs]+ins.shamt)>1023))
            {
                error_flag[2]=1;
                ins.opcode=halt;
                _error();
                return ins;
            }
            if(((reg[ins.rs]+ins.shamt)%2)!=0)
            {
                error_flag[1]=1;
                ins.opcode=halt;
            }
            temp_datamemory[reg[ins.rs]+ins.shamt]=(char)(reg[ins.rt]&0x0000FFFF)>>24;
            temp_datamemory[reg[ins.rs]+ins.shamt+1]=((reg[ins.rt]&0x0000FFFF)<<16)>>16;
            
            break;
        case 0x28:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            if((ins.shamt>>31)==(reg[ins.rs]>>31))
            {
                if((reg[ins.rt]>>31)!=(reg[ins.rs]>>31))
                {
                    error_flag[1]=1;
                }
            }
            if (((reg[ins.rs]+ins.shamt)<0)||((reg[ins.rs]+ins.shamt)>1023))
            {
                error_flag[2]=1;
                ins.opcode=halt;
                _error();
                return ins;
            }
            temp_datamemory[reg[ins.rs]+ins.shamt]=(char)((reg[ins.rt]&0x000000FF)>>24);
            break;
        case 0x0F:
            reg[ins.rt]=ins.shamt<<16;
            break;
        case 0x0C:
            reg[ins.rt]=reg[ins.rs]&ins.shamt;
            break;
        case 0x0D:
            reg[ins.rt]=reg[ins.rs]|ins.shamt;
            break;
        case 0x0E:
            reg[ins.rt]=~(reg[ins.rs]|ins.shamt);
            break;
        case 0x0A:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            reg[ins.rt]=((int)reg[ins.rs]<ins.shamt);
            break;
        case 0x04:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            if(reg[ins.rs]==reg[ins.rt])
            {
                flag=1;
                increasing+=ins.shamt+1;
                print();
            }
            if(increasing>1023||increasing<0)
            {
                error_flag[2]=1;
            }
            break;
        case 0x05:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            if(reg[ins.rs]!=reg[ins.rt])
            {
                flag=1;
                increasing+=ins.shamt+1;
                print();

            }
            if(increasing>1023||increasing<0)
            {
                error_flag[2]=1;
            }
            break;
        case 0x07:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            if(reg[ins.rs]>0)
            {
                flag=1;
                increasing+=ins.shamt+1;
                print();
            }
            break;
        default:
            break;
    }
    //error handle
    if(!flag&&(ins.opcode!=halt))
    {
        increasing++;
        print();
    }
}
Instruction execR(Instruction ins)
{
    switch (ins.funct) {
        case 0x20:
            reg[ins.rd]=reg[ins.rs]+reg[ins.rt];
            if((reg[ins.rs]>>31)==(reg[ins.rt]>>31))
            {
                if((reg[ins.rd]>>31)!=(reg[ins.rs]>>31))
                {
                    error_flag[1]=1;
                }
            }
            break;
        case 0x21://no overflow detection
            reg[ins.rd]=reg[ins.rs]+reg[ins.rt];
            break;
        case 0x22:
            reg[ins.rd]=reg[ins.rs]-reg[ins.rt];
            if(((~reg[ins.rs]+1)>>31)==(~reg[ins.rt]+1)>>31)
            {
                if(((~reg[ins.rs]+1)>>31)!=(reg[ins.rs]>>31))
                {
                    error_flag[1]=1;
                }
            }
            break;
        case 0x24:
            reg[ins.rd]=reg[ins.rs]&reg[ins.rt];
            break;
        case 0x25:
            reg[ins.rd]=reg[ins.rs]|reg[ins.rt];
            break;
        case 0x26:
            reg[ins.rd]=reg[ins.rs]^reg[ins.rt];
            break;
        case 0x27:
            reg[ins.rd]=~(reg[ins.rs]|reg[ins.rt]);
            break;
        case 0x28:
            reg[ins.rd]=~(reg[ins.rs]&reg[ins.rt]);
            break;
        case 0x2A:
            reg[ins.rd]=reg[ins.rs]<reg[ins.rt];
            break;
        case 0x00:
            reg[ins.rd]=reg[ins.rt]<<ins.shamt;
            break;
        case 0x02:
            reg[ins.rd]=reg[ins.rt]>>ins.shamt;
            break;
        case 0x03:
            unsigned int sign;
            sign=reg[ins.rt]>>31;
            if(sign==1)
            {
                unsigned dec=0xFFFFFFFF;
                dec=(dec>>(32-ins.shamt))<<(32-ins.shamt);
                reg[ins.rt]=dec|reg[ins.rt];
            }
            else reg[ins.rd]=reg[ins.rt]>>ins.shamt;
            break;
        case 0x08:
            increasing=(reg[ins.rs]-PC)/4-1;
            increasing++;
            break;
        default:
            break;
    }
    //error handle
    if (ins.funct!=0x08&&(ins.opcode!=halt)) {
        increasing++;//pc increasing amount
    }
    print();
    _error();
}

void print(){
    int i=0;
    fprintf(snapshot,"cycle %d\n",cycle);
    for(i=0;i<32;i++)
    {
        fprintf(snapshot,"$%02d: 0x%08X\n",i,reg[i]);
    }
    fprintf(snapshot,"PC: 0x%08X\n\n\n",PC+increasing*4);
}

void _error(){
    int i=0;
    if(error_flag[0]==1)
        fprintf(error, "In cycle %d: Write $0 Error\n",cycle);
    if(error_flag[1]==1)
        fprintf(error,"In cycle %d: Number Overflow\n",cycle);
    if(error_flag[2]==1)
        fprintf(error,"In cycle %d: Address Overflow\n",cycle);
    if(error_flag[3]==1)
        fprintf(error,"In cycle %d: Misalignment Error\n",cycle);
    for(i=0;i<4;i++)
        error_flag[i]=0;
}

int main()
{
    readfile();
    int i=0;
    while (cycle<500000) {
       if(cycle==0){
        print();
        _error();
        cycle++;
        continue;
    }
    if(cut().opcode==halt)return 0;
    _error();
    cycle++;
    }
    return 0;
}