// C program to convert assembly level instructions to
// machine code.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INSTRUCTION_LENGTH 137
#define MAX_NO_OF_INSTRUCTIONS 40
#define NO_OF_KEYWORDS 27
#define NO_OF_REGISTERS 32                               //(25 + 7)

void convertLineToMachineCode(char instr[], FILE * inputFp);

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

const char * twoAddrZero[] = {
"MOV",              "MOD",
"ADD",              "AND",
"SUB",              "OR",
"MUL",              "XOR",
"DIV",              "CMP",
};
const char * opcodeTwoAddrZero[] = {
"10000000",         "10000101",
"10000001",         "10000110",
"10000010",         "10000111",
"10000011",         "10001000",
"10000100",         "10001001",
};

const char * twoAddrOne[] = {
"STA",
};
const char * opcodeTwoAddrOne[] = {
"100100000000",
};

const char * twoAddrTwo[] = {
"LDA",
};
const char * opcodeTwoAddrTwo[] = {
"101000000000",
};

const char * twoAddrThree[] = {
"MOV",               "MOD",
"ADD",               "AND",
"SUB",               "OR",
"MUL",               "XOR",
"DIV",               "CMP",
};
const char * opcodeTwoAddrThree[] = {
"1011000000000000",          "1011000000000101",
"1011000000000001",          "1011000000000110",
"1011000000000010",          "1011000000000111",
"1011000000000011",          "1011000000001000",
"1011000000000100",          "1011000000001001",
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

char* intToBinary(int value,char otpt[],int pad) {

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
  newStr=padStringWithZeroes(output,pad);
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
      // printf("%s %d\n",token,lineNum);
      st.symbName[st.size] = (char *)malloc(sizeof(char)*MAX_INSTRUCTION_LENGTH);  // reserve space of len
      strcpy(st.symbName[st.size],token); // copy string
      // strcpy(st.symbName[st.size],temp);
      st.symbAddr[st.size]=lineNum;
      // printf("LABEL TOKEN:%s\n",token);
      // printf("table contents: %s, %d",st.symbName[st.size],st.symbAddr[st.size]);
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

int findAddrSubtype(char instr[], int noOfAddress)
{
  char * token;
  char instrCopy[MAX_INSTRUCTION_LENGTH];
  strcpy(instrCopy,instr);

  switch (noOfAddress) {
    case 1:{
           token = strtok (instrCopy," :/,");
           token= strtok(NULL, " :/,");
           if(isTokenReg(token))
             return 1;
           else return 0;
           }
           break;

    case 2:{
            token = strtok (instrCopy," :/,");
            if(strcmp("STA",token) == 0)
              return 1;
            else if(strcmp("LDA",token) == 0)
              return 2;
            else
            {
              token= strtok(NULL, " :/,");
              token= strtok(NULL, " :/,");
              if(isTokenReg(token))
                return 3;
              else return 0;
            }
            }
            break;
    default:printf("ERR: MORE THAN TWO ADDR INSTR NOT SUPPORTED");
            break;
  }
  return 999;
}


void evaluateTypeZeroAddress(char instr[], FILE * inputFp)
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
          fprintf(inputFp, "%s\n", opcodeZeroAddrZero[i]);
      }

  }
}

void evaluateTypeOneAddress(char instr[], FILE * inputFp)
{
  int i;
  int arrayLength;
  char * token;
  char instrCopy[MAX_INSTRUCTION_LENGTH];
  strcpy(instrCopy,instr);

  int subType=findAddrSubtype(instr,1);


  switch(subType)
  {
    case 0:{
            token = strtok (instrCopy," :/,");
            arrayLength=sizeof(oneAddrZero)/sizeof(oneAddrZero[0]);
            for(i = 0; i < arrayLength ; i++) {
              if (strcmp(oneAddrZero[i], token) == 0)
                fprintf(inputFp, "%s",opcodeOneAddrZero[i]);
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
            output=intToBinary(value,otpt,12);
            fprintf(inputFp, "%s", output);
            }
            break;
    case 1:{
            token = strtok (instrCopy," :/,");
            arrayLength=sizeof(oneAddrOne)/sizeof(oneAddrOne[0]);

            for(i = 0; i < arrayLength ; i++) {
              if (strcmp(oneAddrOne[i], token) == 0)
                fprintf(inputFp, "%s",opcodeOneAddrOne[i]);
            }
            token= strtok(NULL, " :/,");

            arrayLength=sizeof(registers)/sizeof(registers[0]);
            for(i = 0; i < arrayLength ; i++) {
              // printf("%s\n",token);
                if (strcmp(registers[i], token) == 0) {
                    fprintf(inputFp, "%s",regAddr[i]);
                }
            }
            }
            break;
    default:printf("ERROR");
            break;

  }

  fprintf(inputFp, "\n");
}

int instrIsCISC(char instr[])
{
  char * token;
  char instrCopy[MAX_INSTRUCTION_LENGTH];
  strcpy(instrCopy,instr);
  token = strtok (instrCopy," :/,");
  token= strtok(NULL, " :/,");
  token= strtok(NULL, " :/,");
  const char * temp=token;
  if(strchr(temp,'-') ||strchr(temp,'+') ||strchr(temp,'*'))
    return 1;
  else
    return 0;

}

void evaluateTwoAddrCISC(char instr[], FILE * inputFp)
{

  char ciscinstr[MAX_INSTRUCTION_LENGTH];
  char firstOp[MAX_INSTRUCTION_LENGTH];
  char secondOp[MAX_INSTRUCTION_LENGTH];
  char secondOp1Var[MAX_INSTRUCTION_LENGTH];
  char midOp[2];
  char secondOp2Var[MAX_INSTRUCTION_LENGTH];
  char * token;
  char instrCopy[MAX_INSTRUCTION_LENGTH];
  strcpy(instrCopy,instr);


  token = strtok (instrCopy," :/,");
  strcpy(ciscinstr,token);
  // printf("hshs %s\n", ciscinstr);

  token = strtok (NULL," :/,");
  strcpy(firstOp,token);
  // printf("hshs %s\n", firstOp);

  token = strtok (NULL," :/,");
  strcpy(secondOp,token);
  if(strchr(secondOp,'-'))
    midOp[0]='-';
  else if(strchr(secondOp,'+'))
    midOp[0]='+';
  else if(strchr(secondOp,'*'))
    midOp[0]='*';
  // printf("hshs %s\n", midOp);

  token = strtok (secondOp," :/,*+-");
  strcpy(secondOp1Var,token);
  // printf("hshs %s\n", secondOp1Var);

  token = strtok (NULL," :/,*+-");
  strcpy(secondOp2Var,token);
  // printf("hshs %s\n", secondOp2Var);


  char instr1[MAX_INSTRUCTION_LENGTH];
  strcat(instr1, ciscinstr);
  strcat(instr1, " EAX, ");
  strcat(instr1, secondOp1Var);

  char instr2[MAX_INSTRUCTION_LENGTH];
  if(strcmp(midOp,"-")==0)
    strcat(instr2, "SUB EAX, #");
  else if(strcmp(midOp,"+")==0)
    strcat(instr2, "ADD EAX, #");
  else if(strcmp(midOp,"*")==0)
    strcat(instr2, "MUL EAX, #");
  strcat(instr2, secondOp2Var);

  char instr3[MAX_INSTRUCTION_LENGTH];
  // printf("instr3: %s\n", instr4);
  // strcat(instr3,ciscinstr);
  strcat(instr3, " ");
  strcat(instr3, firstOp);
  strcat(instr3, ", EAX");


  printf("%s\n%s\n%s\n",instr1,instr2,instr3);

  convertLineToMachineCode(instr1, inputFp);
  convertLineToMachineCode(instr2, inputFp);
  convertLineToMachineCode(instr3, inputFp);


}


void evaluateTypeTwoAddress(char instr[], FILE * inputFp)
{
  int i;
  int arrayLength;
  char * token;
  char instrCopy[MAX_INSTRUCTION_LENGTH];
  strcpy(instrCopy,instr);

  if(instrIsCISC(instr))
  {
    evaluateTwoAddrCISC(instr,inputFp);
    // fprintf(inputFp, "isnstr isCISC\n");
    return;
  }


  int subType=findAddrSubtype(instr,2);
  // printf("subtype: %d\n",subType);


  switch(subType)
  {
    case 0:{
              token = strtok (instrCopy," :/,");
              arrayLength=sizeof(twoAddrZero)/sizeof(twoAddrZero[0]);
              for(i = 0; i < arrayLength ; i++) {
                if (strcmp(twoAddrZero[i], token) == 0)
                  fprintf(inputFp, "%s",opcodeTwoAddrZero[i]);
              }

              token= strtok(NULL, " :/,");
              arrayLength=sizeof(registers)/sizeof(registers[0]);
              for(i = 0; i < arrayLength ; i++) {
                  if (strcmp(registers[i], token) == 0) {
                    fprintf(inputFp, "%s",regAddr[i]);
                  }
              }

              token= strtok(NULL, " :/,");
              char * output;
              char otpt[MAX_INSTRUCTION_LENGTH];
              char addr[MAX_INSTRUCTION_LENGTH];
              strcpy(addr,token);
              char *newStr = addr;
              *(newStr++);
              strcpy(addr,newStr);
              int value=atoi(addr);
              output=intToBinary(value,otpt,16);
              fprintf(inputFp, "%s", output);
            }
            break;
    case 1:{
              token = strtok (instrCopy," :/,");
                  fprintf(inputFp, "%s",opcodeTwoAddrOne[0]);

              token= strtok(NULL, " :/,");
              char * output;
              // printf("gfsa\n");
              char otpt[MAX_INSTRUCTION_LENGTH];
              char addr[MAX_INSTRUCTION_LENGTH];
              strcpy(addr,token);
              char *newStr = addr;
              *(newStr++);
              strcpy(addr,newStr);
              int value=atoi(addr);
              output=intToBinary(value,otpt,12);
              fprintf(inputFp, "%s", output);

              token= strtok(NULL, " :/,");
              arrayLength=sizeof(registers)/sizeof(registers[0]);
              for(i = 0; i < arrayLength ; i++) {
                  if (strcmp(registers[i], token) == 0) {
                      fprintf(inputFp, "%s",regAddr[i]);
                  }
              }
            }
            break;
    case 2:{
              token = strtok (instrCopy," :/,");
                  fprintf(inputFp, "%s",opcodeTwoAddrTwo[0]);

              token= strtok(NULL, " :/,");
              arrayLength=sizeof(registers)/sizeof(registers[0]);
              for(i = 0; i < arrayLength ; i++) {
                  if (strcmp(registers[i], token) == 0) {
                      fprintf(inputFp, "%s",regAddr[i]);
                  }
              }

              token= strtok(NULL, " :/,");
              char * output;
              // printf("gfsa\n");
              char otpt[MAX_INSTRUCTION_LENGTH];
              char addr[MAX_INSTRUCTION_LENGTH];
              strcpy(addr,token);
              char *newStr = addr;
              *(newStr++);
              strcpy(addr,newStr);
              int value=atoi(addr);
              output=intToBinary(value,otpt,12);
              fprintf(inputFp, "%s", output);

            }
            break;
    case 3:{
              char binaryInstr[100];
              token = strtok (instrCopy," :/,");
              arrayLength=sizeof(twoAddrThree)/sizeof(twoAddrThree[0]);
              for(i = 0; i < arrayLength ; i++) {
                if (strcmp(twoAddrThree[i], token) == 0)
                {
                  fprintf(inputFp, "%s",opcodeTwoAddrThree[i]);
                }
              }

              token= strtok(NULL, " :/,");
              arrayLength=sizeof(registers)/sizeof(registers[0]);
              for(i = 0; i < arrayLength ; i++) {
                  if (strcmp(registers[i], token) == 0) {
                      strcat(binaryInstr,regAddr[i]);
                      fprintf(inputFp, "%s",regAddr[i]);
                      // fflush(stdout);
                  }
              }

              token= strtok(NULL, " :/,");
              arrayLength=sizeof(registers)/sizeof(registers[0]);
              for(i = 0; i < arrayLength ; i++) {
                  if (strcmp(registers[i], token) == 0) {
                      strcat(binaryInstr,regAddr[i]);
                      fprintf(inputFp, "%s",regAddr[i]);
                  }
              }
            }
            break;
    default:printf("NO!");
            break;
  }

  fprintf(inputFp, "\n");
}

void convertLineToMachineCode(char instr[], FILE * inputFp)
{
  instr=removeLabel(instr);
  // printf("%s\n",instr);

  int noOfAddress;
  noOfAddress=findInstrAddrType(instr);
  // printf("addrtype for line %d: %d \n",lineNum, noOfAddress);
  switch(noOfAddress)
  {
    case 0:evaluateTypeZeroAddress(instr, inputFp);
          break;

    case 1:evaluateTypeOneAddress(instr, inputFp);
          break;

    case 2:evaluateTypeTwoAddress(instr, inputFp);
           break;

    default:printf("default case\n");
          break;
  }

}

void secondPass(FILE * fp, FILE * inputFp)
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

    convertLineToMachineCode(instr,inputFp);
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

  FILE * inputFp;

  st.size=0;

  fp1 = fopen("file2.asm", "r");
  if (fp1 == NULL)
      exit(EXIT_FAILURE);

  inputFp=fopen("binary.txt", "w");

  firstPass(fp1);

  fp2 = fopen("file2.asm", "r");
  if (fp2 == NULL)
      exit(EXIT_FAILURE);


  // printf("%d, %s\n",st.symbAddr[0],st.symbName[0]);

  secondPass(fp2,inputFp);
  // printf("sfdhghyhdj\n");

  fclose(inputFp);
  fclose(fp1);
  fclose(fp2);



}
