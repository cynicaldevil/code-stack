// C program to convert assembly level instructions to
// machine code.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INSTRUCTION_LENGTH 137
#define MAX_NO_OF_INSTRUCTIONS 40
#define NO_OF_KEYWORDS 27
#define NO_OF_REGISTERS 32                               //(12 + 12 + 3)

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
  "R01",          "R05",          "R09",          "R13",          "R17",          "R21",          "R25",          "IR",
  "R02",          "R06",          "R10",          "R14",          "R18",          "R22",          "SS",           "PC",
  "R03",          "R07",          "R11",          "R15",          "R19",          "R23",          "SP",           "FR"
};

typedef struct symbolTable
{
  char * symbName[MAX_NO_OF_INSTRUCTIONS];
  int  symbAddr[MAX_NO_OF_INSTRUCTIONS];
  int  size;
}symbolTable;
symbolTable st;

int isTokenKeyword(char token[])
{
  int i;
  for(i = 0; i < NO_OF_KEYWORDS; ++i) {
      if (strcmp(instructions[i], token) == 0) {
          return 1;
      }
  }
  for(i = 0; i < NO_OF_REGISTERS; ++i) {
      if (strcmp(registers[i], token) == 0) {
          return 1;
      }
  }
  return 0;
}

void extractTokenFromLine(char instr[], int lineNum)
{
  char * token;                                                              //obtain tokens from each instr
  token = strtok (instr," :/");
  int noOfTokens=0;
  int flag=0;
  while (token != NULL)
  {
    if(noOfTokens==0 && !(isTokenKeyword(token)))
    {
      st.symbName[st.size]=token;
      st.symbAddr[st.size]=lineNum;
      st.size++;
      printf("LABEL TOKEN:%s\n",token);
    }
    else if(( noOfTokens!=0 && !(isTokenKeyword(token)) && flag==1) || (noOfTokens==0 && strcmp(token,"SUBR") == 0))
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
        printf("SUBR TOKEN:%s\n",token);
      }

    }
    else
      printf ("%s\n",token);
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

  fclose(fp);
  if (line)
      free(line);
  exit(EXIT_SUCCESS);
}
int main(void)
{
  FILE * fp;

  st.size=0;

  fp = fopen("file.asm", "r");
  if (fp == NULL)
      exit(EXIT_FAILURE);

  firstPass(fp);
  printf("jg\n");


}
