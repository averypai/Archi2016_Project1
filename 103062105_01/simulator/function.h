#ifndef FUNCTION_H
#define FUNCTION_H
#include <stdio.h>
#include <stdlib.h>
extern int rs,rt,rd,shamt,opcode,immi,PC,cycle;
extern FILE *error;
extern int reg[32];
void add();
void addu();
void sub();
void And();
void Or();
void Xor();
void nor();
void nand();
void slt();
void sll();
void srl();
void sra();
void jr();
void addi();
void addiu();
int lw();
int lh();
int lhu();
int lb();
int lbu();
int sw();
int sh();
int sb();
void lui();
void andi();
void ori();
void nori();
void slti();
void beq();
void bne();
void bgtz();
void jump();
void jal();

#endif// FUNCTION_H
