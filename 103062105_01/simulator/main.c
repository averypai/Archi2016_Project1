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
unsigned  reg[32]={0};
unsigned int PC=0;
unsigned int sp=0;
unsigned int dnum=0;
unsigned int inum=0;
unsigned int increasing=0;
unsigned char instmemory[256][4]={'\0'};
unsigned char datamemory[256][4]={'\0'};
unsigned char temp_datamemory[1024]={'\0'};
unsigned char tempmemory[1024]={'\0'};
void print();
unsigned int execI(Instruction ins);
unsigned int execR(Instruction ins);
unsigned int execJ(Instruction ins);
int cycle=0;
void readfile(){
    FILE*iimage,*dimage,*error,*snapshot;
    //open file
    iimage=fopen("./recur/iimage.bin","rb");
    dimage=fopen("./recur/dimage.bin","rb");
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
            for(k=0;k<4;k++)
            {
                temp_datamemory[(i-2)*4]=datamemory[i][0];
                temp_datamemory[(i-2)*4+1]=datamemory[i][1];
                temp_datamemory[(i-2)*4+2]=datamemory[i][2];
                temp_datamemory[(i-2)*4+3]=datamemory[i][3];
            }
       /*     printf("%02x\n", temp_datamemory[(i-2)*4]);
            printf("%02x\n", temp_datamemory[(i-2)*4+1]);
            printf("%02x\n", temp_datamemory[(i-2)*4+2]);
            printf("%02x\n", temp_datamemory[(i-2)*4+3]);*/
        }
        i++;
    }
}
void cut(){//get rs rt rd shamt funct address opcode
    Instruction ins;
    ins.opcode=inst[increasing]>>26;//opcode here here here here
    printf("0x%02x----ins.opcode\n",ins.opcode);
    //printf("0x%08x\n",instmemory[increasing][3]);
    printf("%d----increasing\n",increasing);
    if(ins.opcode==halt)
    {
        //print
        exit(1);
    }
    else if(ins.opcode==Rtype)
    {
        ins.rs=(inst[increasing]<<6)>>27;
        ins.rt=(inst[increasing]<<11)>>27;
        ins.rd=(inst[increasing]<<16)>>27;
        ins.shamt=(inst[increasing]<<21)>>27;
        ins.funct=(inst[increasing]<<26)>>26;
     /*   printf("0x%02x----Ropcode\n",ins.opcode);
        printf("%05x----Rrs\n",ins.rs);
        printf("%05x----Rrt\n",ins.rt);
        printf("%05x----Rrd\n",ins.rd);
        printf("%05x----Rshamt\n",ins.shamt);*/
        printf("0x%02x----Rfunct\n",ins.funct);
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

        execI(ins);
    }
}

unsigned int execJ(Instruction ins)
{
    printf("exeJJJJJJJJJJJJJJJJJJJJJJJJJJ\n");
    unsigned int newhead=((PC+increasing*4+4)>>28)<<28;
    unsigned int newaddress=ins.shamt<<2;
   // printf("newhead=%04x newaddress=%08x\n\n",newhead,newaddress);
    if(ins.opcode==jump)
    {
        increasing=((newhead|newaddress)-PC)/4-1;
        //printf("%08x\n",PC );
    }
    else
    {
        reg[31]=PC+increasing*4+4;
        increasing=((newhead|newaddress)-PC)/4-1;
        //printf("PC=%08x\n",PC );
    }
    increasing++;
    print();
}
unsigned int execI(Instruction ins)
{
    int flag=0;
    unsigned int sign=0;
    //printf("0x%02x----Iopcode\n",ins.opcode);
    switch (ins.opcode) {
        case 0x08:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=ins.shamt|0xFFFF0000;
            }
            reg[ins.rt]=reg[ins.rs]+ins.shamt;
            /*
            printf("shamt:%04x\n",ins.shamt);
            printf("rs:%02x\n",ins.rs);
            printf("rt:%02x\n",ins.rt);
            printf("reg[rs]:%08x\n",reg[ins.rs]);
            printf("reg[rt]:%08x\n",reg[ins.rt]);*/
            break;
        case 0x09://no overflow detection
            reg[ins.rt]=reg[ins.rs]+ins.shamt;
            break;
        case 0x23:
            sign=ins.shamt>>15;
            //printf("%d\n",sign);
            if(sign==1)
            {
                ins.shamt=ins.shamt|0xFFFF0000;
            }
            /*printf("%08x\n",ins.shamt);
            printf("%05x\n",ins.rs);
            printf("%05x\n",ins.rt);*/
            for(int i=0;i<4;i++){    
                reg[ins.rt]=reg[ins.rt]<<8|temp_datamemory[ins.rs+ins.shamt+i];
                //printf("%02x\n----%d\n",reg[ins.rt],i);
            }
            break;
        case 0x21:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            sign=((reg[ins.rs+ins.shamt]<<16)>>15);
            if(sign==1)
            {
                reg[ins.rt]=0xFFFF0000|temp_datamemory[ins.rs+ins.shamt];
                reg[ins.rt]=reg[ins.rt]<<8|temp_datamemory[ins.rs+ins.shamt+1];
            }
            else{
                reg[ins.rt]=reg[ins.rt]<<16|temp_datamemory[ins.rs+ins.shamt];
                reg[ins.rt]=reg[ins.rt]<<8|temp_datamemory[ins.rs+ins.shamt+1];
            }
            break;
        case 0x25:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            reg[ins.rt]=reg[ins.rt]<<16|temp_datamemory[ins.rs+ins.shamt];
            reg[ins.rt]=reg[ins.rt]<<8|temp_datamemory[ins.rs+ins.shamt+1];
            break;
        case 0x20:
            sign=ins.shamt>>7;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            sign=reg[ins.rs+ins.shamt]>>31;
            if(sign==1)
            {
                reg[ins.rt]=0xFFFFFF00|temp_datamemory[ins.rs+ins.shamt];
            }
            else{
                reg[ins.rt]=temp_datamemory[ins.rs+ins.shamt];
            }
            break;
        case 0x24:
            sign=ins.shamt>>7;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
             reg[ins.rt]=temp_datamemory[ins.rs+ins.shamt];
            break;
        case 0x2B:
            //printf("I AM RAAAAAAAAA\n");
            sign=ins.shamt>>31;
            //printf("%d\n",sign );
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            datamemory[ins.rs+ins.shamt][0]=(char)(reg[ins.rt]>>24);
            datamemory[ins.rs+ins.shamt][1]=(char)(reg[ins.rt]>>16);
            datamemory[ins.rs+ins.shamt][2]=(char)(reg[ins.rt]>>8);
            datamemory[ins.rs+ins.shamt][3]=(char)(reg[ins.rt]);

            temp_datamemory[ins.rs+ins.shamt]=(char)(reg[ins.rt]>>24);
            temp_datamemory[ins.rs+ins.shamt+1]=(char)(reg[ins.rt]>>16);
            temp_datamemory[ins.rs+ins.shamt+2]=(char)(reg[ins.rt]>>8);
            temp_datamemory[ins.rs+ins.shamt+3]=(char)(reg[ins.rt]);
/*
            printf("%08x\n",(int)(char)(reg[ins.rt] >> 24) );
            printf("%08x\n",(int)(char)(reg[ins.rt] >> 16) );
            printf("%08x\n",(int)(char)(reg[ins.rt] >> 8) );
            printf("%08x\n",(int)(char)(reg[ins.rt] ) );

            printf("%08x\n",datamemory[ins.rs+ins.shamt][0]);
            printf("%08x\n",datamemory[ins.rs+ins.shamt][1]);
            printf("%08x\n",datamemory[ins.rs+ins.shamt][2]);
            printf("%08x\n",datamemory[ins.rs+ins.shamt][3]);    
*/
            break;
        case 0x29:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            datamemory[ins.rs+ins.shamt][0]=(unsigned char)(reg[ins.rt]&0x0000FFFF)>>24;
            datamemory[ins.rs+ins.shamt][1]=((reg[ins.rt]&0x0000FFFF)<<8)>>24;

            temp_datamemory[ins.rs+ins.shamt]=((reg[ins.rt]&0x0000FFFF)>>24);
            temp_datamemory[ins.rs+ins.shamt+1]=((reg[ins.rt]&0x0000FFFF)<<8)>>16;
            break;
        case 0x28:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            datamemory[ins.rs+ins.shamt][0]=(reg[ins.rt]&0x000000FF)>>24;
//datamemory[ins.rs+ins.shamt][1]=((reg[ins.rt]&0x000000FF)<<8)>>24;
  //          datamemory[ins.rs+ins.shamt][2]=((reg[ins.rt]&0x000000FF)<<16)>>24;
    //        datamemory[ins.rs+ins.shamt][3]=((reg[ins.rt]&0x000000FF)<<24)>>24;

            temp_datamemory[ins.rs+ins.shamt]=(char)((reg[ins.rt]&0x000000FF)>>24);
      //      temp_datamemory[ins.rs+ins.shamt+1]=(char)((reg[ins.rt]&0x000000FF)>>16);
        //    temp_datamemory[ins.rs+ins.shamt+2]=(char)((reg[ins.rt]&0x000000FF)>>8);
          //  temp_datamemory[ins.rs+ins.shamt+3]=(char)(reg[ins.rt]&0x000000FF);
            break;
        case 0x0F:
            reg[ins.rt]=ins.shamt<<16;
            break;
        case 0x0C:
            reg[ins.rt]=reg[ins.rs]&ins.shamt;
            break;
        case 0x0D:
            reg[ins.rd]=reg[ins.rs]|ins.shamt;
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
            reg[ins.rt]=(reg[ins.rs]<ins.shamt);
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
            break;
        case 0x05:
            sign=ins.shamt>>15;
            if(sign==1)
            {
                ins.shamt=0xFFFF0000|ins.shamt;
            }
            if(reg[ins.rs]!=reg[ins.rt])
            {
                printf("In BNE======%d %d=====increasing======%d\n",ins.rs,ins.rt,increasing);
                flag=1;
                increasing+=ins.shamt+1;
                printf("shamt======%d  =====increasing======%d\n",ins.shamt,increasing);
                print();

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
    if(!flag)
    {
        increasing++;
        printf("%02x\n",ins.opcode );
        print();
    }
}
unsigned int execR(Instruction ins)
{
    switch (ins.funct) {
            printf("%d---funct\n",ins.funct);
        case 0x20:
            reg[ins.rd]=reg[ins.rs]+reg[ins.rt];
            break;
        case 0x21://no overflow detection
            reg[ins.rd]=reg[ins.rs]+reg[ins.rt];
            break;
        case 0x22:
            reg[ins.rd]=reg[ins.rs]-reg[ins.rt];
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
        case 0x08://QAQ
            increasing=(reg[ins.rs]-PC)/4-1;
            increasing++;
            break;
        default:
            break;
    }
    //error handle
    if (ins.funct!=0x08) {
        increasing++;//pc increasing amount
    }
    print();
}

void print(){
    int i=0;
    printf("cycle %d\n",cycle);
    for(i=0;i<32;i++)
    {
        printf("$%d:0x%08X\n",i,reg[i]);
    }
    printf("PC:0x%08X\n\n",PC+increasing*4);
}

int main()
{
    readfile();

    while (cycle<37) {
       if(cycle==0){
    	print();
    	cycle++;
    	continue;
	}
	 cut();
     printf("the end of cycle==========%d\n",cycle);
        cycle++;
    }
    return 0;
}