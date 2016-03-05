// C program to convert assembly level instructions to
// machine code.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INSTRUCTION_LENGTH 137
#define MAX_NO_OF_INSTRUCTIONS 40
#define NO_OF_KEYWORDS 27                                   //(12 + 12 + 3)

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

int isTokenKeyword(char token[])
{
  int i;
  for(i = 0; i < NO_OF_KEYWORDS; ++i) {
      if (strcmp(instructions[i], token) == 0) {
          return 1;
      }
  }
  return 0;
}

symbolTable firstPass(FILE *fp, symbolTable st)
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

    char * token;                                                              //obtain tokens from each instr
    token = strtok (instr," :/");
    int noOfTokens=0;
    while (token != NULL)
    {
      if(noOfTokens==0 && !(isTokenKeyword(token)))
      {
        st.symbName[st.size]=token;
        st.size++;
        printf("LABEL TOKEN:%s\n",token);
      }
      else
        printf ("%s\n",token);
      token = strtok (NULL, " :/,");
      noOfTokens++;
    }
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
  symbolTable st;
  st.size=0;

  fp = fopen("file.txt", "r");
  if (fp == NULL)
      exit(EXIT_FAILURE);

  st=firstPass(fp,st);


}
