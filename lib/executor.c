//C program which takes binary isntructions as input and executes them
//after exection, displays content of registers

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INSTRUCTION_LENGTH 137
#define BINARY_INSTR_LENGTH 32
#define MAX_NO_OF_INSTRUCTIONS 40
#define NO_OF_KEYWORDS 29
#define NO_OF_REGISTERS 32                               //(25 + 7)
#define REG_ADDR_LENGTH 8
#define MEMORY_ADDR_LENGTH 12
#define TOTAL_MEMORY_ADDR 4096                            //2^12

#define EAX_ADDR       25                   //register addresses for special registers
#define STCKSGMNT_ADDR 26
#define STCKPTR_ADDR   27
#define FRAMEPTR_ADDR  28
#define INSTRREG_ADDR  29
#define PROGCTR_ADDR   30
#define FLAGREG_ADDR   31


int countInstructions(FILE *fp);
char *reverseString(char output[]);
char *padStringWithZeroes(char output[],int pad);
char* intToBinary(unsigned int value,int pad);
int power(int base, unsigned int exp);
unsigned int binaryToInt(char instr[]);
int *findType(char instr[]);
void convertInstructions(FILE * fp);
void executeInstructions();
int evaluateZeroAddress(int line);
int evaluateOneAddress(int line);
int evaluateTwoAddress(int line);

//Global variables
typedef struct instruction
{
  char binaryInstr[MAX_INSTRUCTION_LENGTH];
  int type;
  int subType;
} instruction;

//holds all the instructions
instruction * instrArray;
int lines;

int registers[NO_OF_REGISTERS];
int memory[TOTAL_MEMORY_ADDR];

int countInstructions(FILE *fp)
{
  int lines=0;
  char ch;
  while(!feof(fp))
  {
    ch = fgetc(fp);
    if(ch == '\n')
    {
      lines++;
    }
  }
  return lines;
}

char *reverseString(char output[])
{
  int i=0;
  int j = strlen(output) - 1;
  char temp;
   while (i < j) {
      temp = output[i];
      output[i] = output[j];
      output[j] = temp;
      i++;
      j--;
   }
   return output;
}

char *padStringWithZeroes(char output[],int pad)
{
  char otpt[MAX_INSTRUCTION_LENGTH];
  strcpy(otpt,output);
  char padding[]="000000000000000000000000000000000000";
  int computePad=pad-(strlen(output));
  strncpy(otpt,padding,computePad);
  otpt[computePad]='\0';
  strcat(otpt,output);
  strcpy(output,otpt);
  return output;

}

char* intToBinary(unsigned int value,int pad) {
  int pos = 0;
  char * otpt=(char *)malloc(sizeof(char) * MAX_INSTRUCTION_LENGTH+1);
  char output[MAX_INSTRUCTION_LENGTH+1];
  while(value > 0) {
    if (value & 1) output[pos++] = '1';
    else           output[pos++] = '0';
    value >>= 1;
  }
  output[pos] = 0;
  char * newStr;
  newStr=reverseString(output);
  strcpy(output,newStr);
  newStr=padStringWithZeroes(output,pad);
  strcpy(output,newStr);
  strcpy(otpt,output);
  return otpt;
}

int power(int base, unsigned int exp) {
    unsigned int i, result = 1;
    for (i = 0; i < exp; i++)
        result *= base;
    return result;
 }

unsigned int binaryToInt(char instr[])
{
  unsigned int res=0;
  int i;
  for(i=0;i<32;i++)
  {
    if(instr[i]=='1')
      res=res+power(2, (32-1-i));
  }
  return res;
}

int *findType(char instr[])
{
  int *type = (int *)malloc(sizeof (int) * 2);
  unsigned int int_instr=binaryToInt(instr);
  char mask[33];
  unsigned int int_mask;

  strcpy(mask,"11000000000000000000000000000000");
  int_mask= binaryToInt(mask);
  char output[MAX_INSTRUCTION_LENGTH];
  strcpy(output,intToBinary(int_instr & int_mask,32));
  if     (strcmp("00000000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[0]=0;
  else if(strcmp("01000000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[0]=1;
  else if(strcmp("10000000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[0]=2;
  else
  {
    fprintf(stderr, "ERROR: INVALID BINARY INSTRUCTION OF FAULTY TYPE!\n");
    exit(EXIT_FAILURE);
  }

  strcpy(mask,"00110000000000000000000000000000");
  int_mask= binaryToInt(mask);

  if     (strcmp("00000000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[1]=0;
  else if(strcmp("00010000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[1]=1;
  else if(strcmp("00100000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[1]=2;
  else if(strcmp("00110000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[1]=3;
  else
  {
    fprintf(stderr, "ERROR: INVALID BINARY INSTRUCTION OF FAULTY TYPE!\n");
    exit(EXIT_FAILURE);
  }

  return type;
}

void convertInstructions(FILE * fp)
{
  instrArray=(instruction *)calloc(lines, sizeof(instruction));

  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int lineNum=0;
  while ((read = getline(&line, &len, fp)) != -1) {

    char instr[MAX_INSTRUCTION_LENGTH];                                        //copy each line into a var
    strcpy(instr,line);                                                        //so that original is not changed

    char *newLine;                                                             //removes newline from
    if ((newLine=strchr(instr, '\n')) != NULL)                                 //the instruction statment
      *newLine = '\0';                                                         //

    strcpy(instrArray[lineNum].binaryInstr,instr);
    int * typeArr=findType(instr);
    instrArray[lineNum].type=typeArr[0];
    instrArray[lineNum].subType=typeArr[1];

    lineNum++;
  }

  if (line)
      free(line);
}

int isInstrHalt(char instr[])
{
  if(strcmp(instr,"00000000000000000000000000000010")==0)
    return 1;
  else
    return 0;
}

int isInstrStart(char instr[])
{
  if(strcmp(instr,"00000000000000000000000000000000")==0)
    return 1;
  else
    return 0;
}

int evalRETZeroAddr(int line)
{
                                                                               //shifts stack pointer to current location of
                                                                              //frame pointer and shifts frame pointer to location
    line= memory[ registers[STCKPTR_ADDR ] -1 ];                              //of old frame pointer, function returns return address
    registers[EAX_ADDR]=registers[STCKPTR_ADDR];                              //stored in stack frame
    registers[STCKPTR_ADDR]=registers[FRAMEPTR_ADDR];
    registers[FRAMEPTR_ADDR]=memory[registers[EAX_ADDR] - 2];
    return ++line;
}

int evaluateZeroAddress(int line)
{
  if(strcmp(instrArray[line].binaryInstr,"00000000000000000000000000000011")==0)
    line=evalRETZeroAddr(line);
  else
    ++line;
    return line;
}

int evalINR(int line, int reg)
{
  registers[reg]+=1;
  return ++line;
}

int evalDEC(int line, int reg)
{
  registers[reg]-=1;
  return ++line;
}

int evalNOT(int line, int reg)
{
  registers[reg]=~registers[reg];
  return ++line;
}

int evalPUSH(int line, int opr)                                                //handles both 0 and 1 subtypes
{
  memory[ registers[STCKPTR_ADDR] ]= registers[opr];
  registers[STCKPTR_ADDR]++;
  return ++line;
}

int evalPOP(int line, int reg)
{
  registers[reg]= memory[ registers[STCKPTR_ADDR ] -1];
  registers[STCKPTR_ADDR]--;
  return ++line;
}

int evalJZE(int line, int lineAddr)
{
  unsigned int zero_mask=1;                                                    //zero flag is the first bit in flag reg,
  unsigned int res= zero_mask & registers[FLAGREG_ADDR];                       //so masking it with one to get its value
  if(res==1)
    return lineAddr;
  else
    return ++line;

}

int evalJNE(int line, int lineAddr)
{
  unsigned int zero_mask=1;
  unsigned int res= zero_mask & registers[FLAGREG_ADDR];
  if(res==0)                                                                   //opposite of what we did in JZE
    return lineAddr;
  else
    return ++line;
}

int evalJMP(int line, int lineAddr)
{
  return lineAddr;
}

int evalCALL(int line, int lineAddr)                                           //does two things:pushes address in frame pointer
{                                                                              //and pushes return address to stack
  memory[ registers[STCKPTR_ADDR] ]= registers[FRAMEPTR_ADDR];
  registers[STCKPTR_ADDR]++;
  memory[ registers[STCKPTR_ADDR] ]= line;
  registers[STCKPTR_ADDR]++;
  return lineAddr;

}

int evalRET(int line, int lineAddr)                                            //shifts stack pointer to current location of
{                                                                              //frame pointer and shifts frame pointer to location
                                                                               //of old frame pointer
  registers[EAX_ADDR]=registers[STCKPTR_ADDR];
  registers[STCKPTR_ADDR]=registers[FRAMEPTR_ADDR];
  registers[FRAMEPTR_ADDR]=registers[STCKPTR_ADDR] - 2;
  fprintf(stderr,"ONE ADDRESS RET SHOULD NOT BE USED");
  exit(EXIT_FAILURE);
  return lineAddr;
}

int evalJGT(int line, int lineAddr)
{
  unsigned int zero_mask=3;                                                    //we want to check content of zero and sign flag
  unsigned int res= zero_mask & registers[FLAGREG_ADDR];                       //so we mask with first two bits as 1(3)
  if(res==0)                                                                   //both sign and zero flag should be zero
    return lineAddr;
  else
    return ++line;
}

int evalJLT(int line, int lineAddr)
{
  unsigned int zero_mask=3;                                                    //we want to check content of zero and sign flag
  unsigned int res= zero_mask & registers[FLAGREG_ADDR];                       //so we mask with first two bits as 1(3)
  if(res==2)                                                                   //sign flag should be one, zero flag should be zero
    return lineAddr;
  else
    return ++line;
}

int evalSUBR(int line, int lineAddr)
{
  return ++line;
}


int evaluateOneAddress(int line)
{
  unsigned int int_instr=binaryToInt(instrArray[line].binaryInstr);
  char mask    [BINARY_INSTR_LENGTH+1];
  char opr_mask[BINARY_INSTR_LENGTH+1];
  unsigned int int_mask;
  unsigned int int_opr_mask;

  switch(instrArray[line].subType)
  {
    case 0: strcpy(mask,    "11111111111111111111000000000000");               //mask for instruction
            int_mask=      binaryToInt(mask);
            strcpy(opr_mask,"00000000000000000000111111111111");               //mask for operand
            int_opr_mask= binaryToInt(opr_mask);
            if      (strcmp("01000000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalJZE(line,int_opr_mask & int_instr);
            else if (strcmp("01000000000000000001000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalJNE(line,int_opr_mask & int_instr);
            else if (strcmp("01000000000000000010000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalJMP(line,int_opr_mask & int_instr);
            else if (strcmp("01000000000000000011000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalCALL(line,int_opr_mask & int_instr);
            else if (strcmp("01000000000000000100000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalSUBR(line,int_opr_mask & int_instr);
            else if (strcmp("01000000000000000101000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalPUSH(line,int_opr_mask & int_instr);
            else if (strcmp("01000000000000000110000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalRET(line,int_opr_mask & int_instr);
            else if (strcmp("01000000000000000111000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalJGT(line,int_opr_mask & int_instr);
            else if (strcmp("01000000000000001000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalJLT(line,int_opr_mask & int_instr);
            break;

    case 1: strcpy(mask,    "11111111111111111111111100000000");               //mask for instruction
            int_mask=      binaryToInt(mask);
            strcpy(opr_mask,"00000000000000000000000011111111");               //mask for operand
            int_opr_mask= binaryToInt(opr_mask);
            if     (strcmp("01010000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalINR(line,int_opr_mask & int_instr);
            else if(strcmp("01010000000000000000000100000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalDEC(line,int_opr_mask & int_instr);
            else if(strcmp("01010000000000000000001000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalNOT(line,int_opr_mask & int_instr);
            else if(strcmp("01010000000000000000001100000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalPUSH(line,int_opr_mask & int_instr);
            else if(strcmp("01010000000000000000010000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalPOP(line,int_opr_mask & int_instr);
            break;
  }
  return line;
}

int evalMOV(int line,int opr1,int opr2)
{
  switch(instrArray[line].subType)
  {
    case 0: registers[opr1]=opr2;
            break;

    case 3: registers[opr1]=registers[opr2];
            break;
  }

  return ++line;
}

int evalADD(int line,int opr1,int opr2)
{
  switch(instrArray[line].subType)
  {
    case 0: registers[opr1]+=opr2;
            break;

    case 3: registers[opr1]+=registers[opr2];
            break;
  }
  return ++line;
}

int evalSUB(int line,int opr1,int opr2)
{
  switch(instrArray[line].subType)
  {
    case 0: registers[opr1]-=opr2;
            break;

    case 3: registers[opr1]-=registers[opr2];
            break;
  }
  return ++line;
}

int evalMUL(int line,int opr1,int opr2)
{
  switch(instrArray[line].subType)
  {
    case 0: registers[opr1]*=opr2;
            break;

    case 3: registers[opr1]*=registers[opr2];
            break;
  }
  return ++line;
}

int evalDIV(int line,int opr1,int opr2)
{
  switch(instrArray[line].subType)
  {
    case 0: registers[opr1]/=opr2;
            break;

    case 3: registers[opr1]/=registers[opr2];
            break;
  }
  return ++line;
}

int evalMOD(int line,int opr1,int opr2)
{
  switch(instrArray[line].subType)
  {
    case 0: registers[opr1]%=opr2;
            break;

    case 3: registers[opr1]%=registers[opr2];

            break;
  }
  return ++line;
}

int evalAND(int line,int opr1,int opr2)
{
  switch(instrArray[line].subType)
  {
    case 0: registers[opr1]&=opr2;
            break;

    case 3: registers[opr1]&=registers[opr2];
            break;
  }
  return ++line;
}

int evalOR(int line,int opr1,int opr2)
{
  switch(instrArray[line].subType)
  {
    case 0: registers[opr1]|=opr2;
            break;

    case 3: registers[opr1]|=registers[opr2];
            break;
  }
  return ++line;
}

int evalXOR(int line,int opr1,int opr2)
{
  switch(instrArray[line].subType)
  {
    case 0: registers[opr1]^=opr2;
            break;

    case 3: registers[opr1]^=registers[opr2];
            break;
  }
  return ++line;
}

int evalCMP(int line,int opr1,int opr2)
{
  int case_opr2;
  switch(instrArray[line].subType)
  {
    case 0:  case_opr2=opr2;
            break;

    case 3:  case_opr2=registers[opr2];
            break;
  }
  char mask1[BINARY_INSTR_LENGTH+1];
  char mask2[BINARY_INSTR_LENGTH+1];
  unsigned int temp;
  unsigned int int_mask1;
  unsigned int int_mask2;
  strcpy(mask1,"11111111111111111111111111111100");
  int_mask1=binaryToInt(mask1);
  temp=registers[FLAGREG_ADDR] & int_mask1;
  if(registers[opr1]==case_opr2)
    strcpy(mask2,"00000000000000000000000000000001");
  else if(registers[opr1] > case_opr2)
    strcpy(mask2,"00000000000000000000000000000000");
  else if(registers[opr1] < case_opr2)
    strcpy(mask2,"00000000000000000000000000000010");
  int_mask2=binaryToInt(mask2);
  temp=temp | int_mask2;
  registers[FLAGREG_ADDR]=temp;
  return ++line;
}

int evalSTA(int line, int opr1, int opr2)
{
  switch(instrArray[line].subType)
  {
    case 1: memory[opr1]=registers[opr2];
            break;

    case 3: memory[ registers[opr1] ] = registers[opr2];
            break;
  }
  return ++line;
}

int evalLDA(int line, int opr1,int opr2)
{
  switch(instrArray[line].subType)
  {
    case 2: registers[opr1]=memory[opr2];
            break;

    case 3: registers[opr1] = memory[ registers[opr2] ];
            break;
  }
  return ++line;
}

int evaluateTwoAddress(int line)
{
  unsigned int int_instr=binaryToInt(instrArray[line].binaryInstr);
  char mask     [BINARY_INSTR_LENGTH+1];
  char opr1_mask[BINARY_INSTR_LENGTH+1];
  char opr2_mask[BINARY_INSTR_LENGTH+1];
  unsigned int int_mask;
  unsigned int int_opr1_mask;
  unsigned int int_opr2_mask;

  switch(instrArray[line].subType)
  {
    case 0: strcpy(mask,     "11111111000000000000000000000000");               //mask for instruction
            int_mask=      binaryToInt(mask);
            strcpy(opr1_mask,"00000000111111110000000000000000");               //mask for first operand
            int_opr1_mask= binaryToInt(opr1_mask);
            strcpy(opr2_mask,"00000000000000001111111111111111");               //mask for first operand
            int_opr2_mask= binaryToInt(opr2_mask);

            if      (strcmp("10000000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalMOV(line,(int_opr1_mask & int_instr)/65536,int_opr2_mask & int_instr);               //divide by 2^16
            else if (strcmp("10000001000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalADD(line,(int_opr1_mask & int_instr)/65536,int_opr2_mask & int_instr);
            else if (strcmp("10000010000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalSUB(line,(int_opr1_mask & int_instr)/65536,int_opr2_mask & int_instr);
            else if (strcmp("10000011000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalMUL(line,(int_opr1_mask & int_instr)/65536,int_opr2_mask & int_instr);
            else if (strcmp("10000100000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalDIV(line,(int_opr1_mask & int_instr)/65536,int_opr2_mask & int_instr);
            else if (strcmp("10000101000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalMOD(line,(int_opr1_mask & int_instr)/65536,int_opr2_mask & int_instr);
            else if (strcmp("10000110000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalAND(line,(int_opr1_mask & int_instr)/65536,int_opr2_mask & int_instr);
            else if (strcmp("10000111000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalOR(line,(int_opr1_mask & int_instr)/65536,int_opr2_mask & int_instr);
            else if (strcmp("10001000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalXOR(line,(int_opr1_mask & int_instr)/65536,int_opr2_mask & int_instr);
            else if (strcmp("10001001000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalCMP(line,(int_opr1_mask & int_instr)/65536,int_opr2_mask & int_instr);
            break;

    case 1: strcpy(mask,     "11111111111100000000000000000000");               //mask for instruction
            int_mask=      binaryToInt(mask);
            strcpy(opr1_mask,"00000000000011111111111100000000");               //mask for first operand
            int_opr1_mask= binaryToInt(opr1_mask);
            strcpy(opr2_mask,"00000000000000000000000011111111");               //mask for first operand
            int_opr2_mask= binaryToInt(opr2_mask);
            line=evalSTA(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);                      //divide by 2^8
            break;

    case 2: strcpy(mask,     "11111111111100000000000000000000");               //mask for instruction
            int_mask=      binaryToInt(mask);
            strcpy(opr1_mask,"00000000000011111111000000000000");               //mask for first operand
            int_opr1_mask= binaryToInt(opr1_mask);
            strcpy(opr2_mask,"00000000000000000000111111111111");               //mask for first operand
            int_opr2_mask= binaryToInt(opr2_mask);
            line=evalLDA(line,(int_opr1_mask & int_instr)/4096,int_opr2_mask & int_instr);                     //divide by 2^12
            break;

    case 3: strcpy(mask,     "11111111111111110000000000000000");               //mask for instruction
            int_mask=      binaryToInt(mask);
            strcpy(opr1_mask,"00000000000000001111111100000000");               //mask for first operand
            int_opr1_mask= binaryToInt(opr1_mask);
            strcpy(opr2_mask,"00000000000000000000000011111111");               //mask for first operand
            int_opr2_mask= binaryToInt(opr2_mask);

            if      (strcmp("10110000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalMOV(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);               //divide by 2^16
            else if (strcmp("10110000000000010000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalADD(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);
            else if (strcmp("10110000000000100000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalSUB(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);
            else if (strcmp("10110000000000110000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalMUL(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);
            else if (strcmp("10110000000001000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalDIV(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);
            else if (strcmp("10110000000001010000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalMOD(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);
            else if (strcmp("10110000000001100000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalAND(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);
            else if (strcmp("10110000000001110000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalOR(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);
            else if (strcmp("10110000000010000000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalXOR(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);
            else if (strcmp("10110000000010010000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalCMP(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);
            else if (strcmp("10110000000010100000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalSTA(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);
            else if (strcmp("10110000000010110000000000000000",intToBinary(int_instr & int_mask,32))==0)
              line=evalLDA(line,(int_opr1_mask & int_instr)/256,int_opr2_mask & int_instr);
            break;
  }

  return line;
}

void executeInstructions()
{
  int currLine=0;
  while(isInstrStart(instrArray[currLine].binaryInstr)==0)                   //always start at START instruction
    currLine++;
  if(currLine>lines)
  {
    fprintf(stderr,"ERROR: NO START STATEMENT FOUND!\n");
    exit(EXIT_FAILURE);
  }
  while(isInstrHalt(instrArray[currLine].binaryInstr)==0 && currLine<lines)
  {

    switch(instrArray[currLine].type)                                        //each instr returns the line address of next instr
    {
      case 0:currLine=evaluateZeroAddress(currLine);
             break;

      case 1:currLine=evaluateOneAddress(currLine);
             break;

      case 2:currLine=evaluateTwoAddress(currLine);
            break;
    }
  }
}

void resetCPU()
{
  instrArray=NULL;
  lines=0;
  int size_reg_array=sizeof(registers)/sizeof(int);
  int size_mem_array=sizeof(memory)/sizeof(int);
  int i;
  for(i=0;i<size_reg_array;i++)
  {
    registers[i]=0;
  }
  for(i=0;i<size_mem_array;i++)
  {
    memory[i]=0;
  }
  registers[STCKSGMNT_ADDR]=0;
  registers[STCKPTR_ADDR]=registers[STCKSGMNT_ADDR]+1;
  registers[FRAMEPTR_ADDR]=0;
}

int main(int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        /* We print argv[0] assuming it is the program name */
        printf( "USAGE: gcc executor.c -o exe\n./exe file.txt");
    }
  resetCPU();
  FILE * fp1;
  fp1 = fopen(argv[1], "r");
  if (fp1 == NULL)
    exit(EXIT_FAILURE);

  lines = countInstructions(fp1);
  fp1 = fopen(argv[1], "r");

  convertInstructions(fp1);                                                    //each instraction is converted to a
  fclose(fp1);                                                                 //struct object; easier to handle

  executeInstructions();
  int i;
  printf("\nREGISTERS:");
  for(i=0;i<32;i++)
  {
    if(i%8==0)
      printf("\n");
    printf("REG %d:%d      ",i,registers[i]);
  }
  printf("\n\nNONEMPTY MEMORY:\n");
  for(i=0;i<TOTAL_MEMORY_ADDR;i++)
  {
    if(memory[i]!=0)
      printf("content at %d: %d\n",i,memory[i]);
  }

  return 0;
}
