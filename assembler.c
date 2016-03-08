// C program to convert assembly level instructions to
// machine code.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INSTRUCTION_LENGTH 137
#define MAX_NO_OF_INSTRUCTIONS 40
#define NO_OF_KEYWORDS 27
#define NO_OF_REGISTERS 32                               //(25 + 7)

//Global variables:
const char * instructions[]= {
  "MOV",
  "ADD",
  "SUB",
  "MUL",
  "DIV",
  "MOD",
  "AND",
  "OR",
  "XOR",
  "CMP",
  "STA",
  "LDA",

  "JZE",
  "JNE",
  "JMP",
  "CALL",
  "SUBR",
  "PUSH",
  "RET",
  "INC",
  "DEC",
  "NOT",
  "PUSH",
  "POP",

  "START",
  "END",
  "HLT"
};

const char * registers[]={
  "R00",          "R04",          "R08",          "R12",          "R16",          "R20",          "R24",          "FP",
  "R01",          "R05",          "R09",          "R13",          "R17",          "R21",          "EAX",          "IR",
  "R02",          "R06",          "R10",          "R14",          "R18",          "R22",          "SS",           "PC",
  "R03",          "R07",          "R11",          "R15",          "R19",          "R23",          "SP",           "FR"
};

const char * regAddr[]={
  "00000000",    "00000100",    "00001000",    "00001100",    "00010000",    "00010100",    "00011000",    "00011100",
  "00000001",    "00000101",    "00001001",    "00001101",    "00010001",    "00010101",    "00011001",    "00011101",
  "00000010",    "00000110",    "00001010",    "00001110",    "00010010",    "00010110",    "00011010",    "00011110",
  "00000011",    "00000111",    "00001011",    "00001111",    "00010011",    "00010111",    "00011011",    "00011111",
};

const char * zeroAddrZero[] = {
"START",
"END",
"HLT"
};
const char * opcodeZeroAddrZero[] = {
"00000000000000000000000000000000",
"00000000000000000000000000000001",
"00000000000000000000000000000010"
};

const char * oneAddrZero[] = {
  "JZE",
  "JNE",
  "JMP",
  "CALL",
  "SUBR",
  "PUSH",
  "RET"
};
const char * opcodeOneAddrZero[] = {
  "01000000000000000000",
  "01000000000000000001",
  "01000000000000000010",
  "01000000000000000011",
  "01000000000000000100",
  "01000000000000000101",
  "01000000000000000110",
};

const char * oneAddrOne[] = {
  "INC",
  "DEC",
  "NOT",
  "PUSH",
  "POP",
};
const char * opcodeOneAddrOne[] = {
  "010100000000000000000000",
  "010100000000000000000001",
  "010100000000000000000010",
  "010100000000000000000011",
  "010100000000000000000100",
};


typedef struct symbolTable
{
  char * symbName[MAX_NO_OF_INSTRUCTIONS];
  int  symbAddr[MAX_NO_OF_INSTRUCTIONS];
  int  size;
}symbolTable;
symbolTable st;

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
  char padding[]="0000000000000000";
  int computePad=pad-(strlen(output));
  strncpy(otpt,padding,computePad);
  otpt[computePad]='\0';
  strcat(otpt,output);
  strcpy(output,otpt);
  // strncpy(output,padding,pad-(strlen(otpt)+1));
  // strcat(output,otpt);
  return output;

}

char* intToBinary(int value,char otpt[]) {

  int pos = 0;
  char output[MAX_INSTRUCTION_LENGTH];
  while(value > 0) {
    if (value & 1) output[pos++] = '1';
    else           output[pos++] = '0';
    value >>= 1;
  }
  output[pos] = 0;
  char * newStr;
  newStr=reverseString(output);
  strcpy(output,newStr);
  newStr=padStringWithZeroes(output,12);
  strcpy(output,newStr);
  strcpy(otpt,output);
  return otpt;
}

int isTokenInstruction(char token[])
{
  int i;
  for(i = 0; i < NO_OF_KEYWORDS; ++i) {
      if (strcmp(instructions[i], token) == 0) {
          return 1;
      }
  }
  return 0;
}

int isTokenReg(char token[])
{
  int i;
  for(i = 0; i < NO_OF_REGISTERS; ++i) {
      if (strcmp(registers[i], token) == 0) {
          return 1;
      }
  }
  return 0;
}

int isTokenKeyword(char token[])
{
  if( isTokenInstruction(token) || isTokenReg(token) )
    return 1;
  else
    return 0;

}


/*`````````````````firstpass functions```````````````````````*/

int isTokenLabel(char token[],int noOfTokens)
{
  if(noOfTokens==0 && !(isTokenKeyword(token)))
    return 1;
  else return 0;
}

int isTokenSubrName(char token[],int noOfTokens, int flag)
{
  if(( noOfTokens!=0 && !(isTokenKeyword(token)) && flag==1) || (noOfTokens==0 && strcmp(token,"SUBR") == 0))
    return 1;
  else return 0;
}

void extractTokenFromLine(char instr[], int lineNum)
{
  char * token;                                                                //obtain tokens from each instr
  token = strtok (instr," :/,");
  int noOfTokens=0;
  int flag=0;                                                                  //var used in filtering subr names
  while (token != NULL)
  {

    // if(noOfTokens==0 && !(isTokenKeyword(token,noOfTokens)))
    if(isTokenLabel(token, noOfTokens))
    {
      printf("%s %d\n",token,lineNum);
      st.symbName[st.size] = (char *)malloc(sizeof(char)*MAX_INSTRUCTION_LENGTH);  // reserve space of len
      strcpy(st.symbName[st.size],token); // copy string
      // strcpy(st.symbName[st.size],temp);
      st.symbAddr[st.size]=lineNum;
      // printf("LABEL TOKEN:%s\n",token);
      printf("table contents: %s, %d",st.symbName[st.size],st.symbAddr[st.size]);
      st.size++;

    }
    else if(isTokenSubrName(token, noOfTokens, flag))
    {
      if(flag==0)
      {
        flag=1;
      }
      else if(flag==1)
      {
        st.symbName[st.size]=token;
        st.symbAddr[st.size]=lineNum;
        st.size++;
        flag=0;
        // printf("SUBR TOKEN:%s\n",token);
      }

    }
    else
      // printf ("%s\n",token);
    token = strtok (NULL, " :/,");
    noOfTokens++;

  }
}

void firstPass(FILE *fp)
{
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

    extractTokenFromLine(instr,lineNum);
    lineNum++;

  }

  if (line)
      free(line);

}

/*-^^^^^^^^^^^^^^^^^^^^^firstpass functions^^^^^^^^^^^^^^^^^^^^^*/
/*!!!!!!!!!!!!!!!!!!!!!!second pass functions!!!!!!!!!!!!!!!!!!!*/

char *removeLabel(char instr[])
{
  char * token;                                                                //obtain tokens from each instr
  char instrCopy[MAX_INSTRUCTION_LENGTH];
  strcpy(instrCopy,instr);

  token = strtok (instrCopy," :/,");
  if(!isTokenKeyword(token))
  {
    char *newStr = instr;

    while (*newStr != 0 && *(newStr++) != ':') {}
    strcpy(instr,newStr);
  }
  return instr;
}

int findInstrAddrType(char instr[])
{
  char * token;                                                                //obtain tokens from each instr
  char instrCopy[MAX_INSTRUCTION_LENGTH];
  strcpy(instrCopy,instr);

  token = strtok (instrCopy," :/,");
  int noOfAddress=-1;

  while (token != NULL)
  {
    token= strtok(NULL, " :/,");
    noOfAddress++;
  }
  return noOfAddress;

}

int findAddrSubtype(char instr[], int noOafAddress)
{
  char * token;
  char instrCopy[MAX_INSTRUCTION_LENGTH];
  strcpy(instrCopy,instr);

  token = strtok (instrCopy," :/,");

  switch (noOafAddress) {
    case 1:token= strtok(NULL, " :/,");
           if(isTokenReg(token))
             return 1;
           else return 0;
           break;
    case 2:printf("2 addr inst!");
            break;
    default:printf("ERR: MORE THAN TWO ADDR INSTR NOT SUPPORTED");
            break;
  }
  return 999;
}


void evaluateTypeZeroAddress(char instr[])
{

  char * token;
  char instrCopy[MAX_INSTRUCTION_LENGTH];
  strcpy(instrCopy,instr);

  token = strtok (instrCopy," ");
  // printf("evaluation: %s\n", token);

  int i;
  int arrayLength=sizeof(zeroAddrZero)/sizeof(zeroAddrZero[0]);
  for(i = 0; i < arrayLength ; i++) {
      if (strcmp(zeroAddrZero[i], token) == 0) {
          printf("%s\n",opcodeZeroAddrZero[i]);
      }

  }
}

void evaluateTypeOneAddress(char instr[])
{
  int i;
  int arrayLength;
  char * token;
  char instrCopy[MAX_INSTRUCTION_LENGTH];
  strcpy(instrCopy,instr);

  // token = strtok (instrCopy," :/,");
  // arrayLength=sizeof(oneAddrOne)/sizeof(oneAddrOne[0]);
  //
  // for(i = 0; i < arrayLength ; i++) {
  //   if (strcmp(oneAddrOne[i], token) == 0)
  //     printf("%s",opcodeOneAddrOne[i]);
  // }
  // // printf("%s\n",token);
  // token= strtok(NULL, " :/,");
  //
  // // printf("tgfsrdgs %s %d\n",token, subType);
  // arrayLength=sizeof(registers)/sizeof(registers[0]);
  // for(i = 0; i < arrayLength ; i++) {
  //   // printf("%s\n",token);
  //     if (strcmp(registers[i], token) == 0) {
  //         printf("%s",regAddr[i]);
  //     }
  // }


  int subType=findAddrSubtype(instr,1);


  switch(subType)
  {
    case 0:{
            token = strtok (instrCopy," :/,");
            arrayLength=sizeof(oneAddrZero)/sizeof(oneAddrZero[0]);
            for(i = 0; i < arrayLength ; i++) {
              if (strcmp(oneAddrZero[i], token) == 0)
                printf("%s",opcodeOneAddrZero[i]);
            }

            token= strtok(NULL, " :/,");
            char * output;
            char otpt[MAX_INSTRUCTION_LENGTH];
            int value=-1;
            for(i=0;i<st.size;i++)
            {
              if(strcmp(token,st.symbName[i]) == 0)
                value=st.symbAddr[i];
            }
            // if(isTokenLabel(token))
            // {
            //   for(i=0;i<st.size;i++)
            //   {
            //     if(strcmp(token,st.symbName[i]) == 0)
            //       value=st.symbAddr[i];
            //   }
            // }
            if(value==-1)
            {
              char addr[MAX_INSTRUCTION_LENGTH];
              strcpy(addr,token);
              char *newStr = addr;
              *(newStr++);
              addr[strlen(addr)-1]='\0';
              strcpy(addr,newStr);
              value=atoi(addr);
            }
            output=intToBinary(value,otpt);
            printf("%s", output);
            }
            break;
    case 1:{
            token = strtok (instrCopy," :/,");
            arrayLength=sizeof(oneAddrOne)/sizeof(oneAddrOne[0]);

            for(i = 0; i < arrayLength ; i++) {
              if (strcmp(oneAddrOne[i], token) == 0)
                printf("%s",opcodeOneAddrOne[i]);
            }
            token= strtok(NULL, " :/,");

            arrayLength=sizeof(registers)/sizeof(registers[0]);
            for(i = 0; i < arrayLength ; i++) {
              // printf("%s\n",token);
                if (strcmp(registers[i], token) == 0) {
                    printf("%s",regAddr[i]);
                }
            }
            }
            break;
    default:printf("ERROR");
            break;

  }

  printf("\n");

}

void convertLineToMachineCode(char instr[], int lineNum)
{
  instr=removeLabel(instr);
  printf("%s\n",instr);

  int noOfAddress;
  noOfAddress=findInstrAddrType(instr);
  printf("addrtype for line %d: %d \n",lineNum, noOfAddress);
  switch(noOfAddress)
  {
    case 0:evaluateTypeZeroAddress(instr);
          break;

    case 1:evaluateTypeOneAddress(instr);
          break;

    default:printf("default case\n");
          break;
  }

}

void secondPass(FILE * fp)
{

  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int lineNum=0;

  while ((read = getline(&line, &len, fp)) != -1) {
    // printf("hgfshhh");
    char instr[MAX_INSTRUCTION_LENGTH];                                        //copy each line into a var
    strcpy(instr,line);                                                        //so that original is not changed

    char *newLine;                                                             //removes newline from
    if ((newLine=strchr(instr, '\n')) != NULL)                                 //the instruction statment
      *newLine = '\0';                                                         //

    convertLineToMachineCode(instr,lineNum);
    lineNum++;

  }

  if (line)
      free(line);
}

/*^^^^^^^^^^^^^^secondpass functions^^^^^^^^^^^^^^^*/

int main(void)
{

  FILE * fp1;
  FILE * fp2;

  st.size=0;

  fp1 = fopen("file2.asm", "r");
  if (fp1 == NULL)
      exit(EXIT_FAILURE);

  firstPass(fp1);

  fp2 = fopen("file2.asm", "r");
  if (fp2 == NULL)
      exit(EXIT_FAILURE);


  // printf("%d, %s\n",st.symbAddr[0],st.symbName[0]);

  secondPass(fp2);
  printf("sfdhghyhdj\n");

  fclose(fp1);
  fclose(fp2);



}
