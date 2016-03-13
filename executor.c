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

int countInstructions(FILE *fp);
char *reverseString(char output[]);
char *padStringWithZeroes(char output[],int pad);
char* intToBinary(unsigned int value,int pad);
int power(int base, unsigned int exp);
unsigned int binaryToInt(char instr[]);
int *findType(char instr[]);
void convertInstructions(FILE * fp);

typedef struct instruction
{
  char binaryInstr[MAX_INSTRUCTION_LENGTH];
  int type;
  int subType;
} instruction;

instruction * instrArray=NULL;
int lines=0;

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
  // printf("value : %u",value);
  int pos = 0;
  char * otpt=(char *)malloc(sizeof(char) * MAX_INSTRUCTION_LENGTH+1);
  char output[MAX_INSTRUCTION_LENGTH+1];
  while(value > 0) {
    if (value & 1) output[pos++] = '1';
    else           output[pos++] = '0';
    value >>= 1;
  }
  output[pos] = 0;
  // printf("output:%s pad:%d\n",output, pad);
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
  unsigned int temp=int_instr & int_mask;
  // printf("unsigned string :%s\n", output);
  type[0]=binaryToInt(reverseString(intToBinary(int_instr & int_mask,32)));
  if     (strcmp("00000000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[0]=0;
  else if(strcmp("01000000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[0]=1;
  else if(strcmp("10000000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[0]=2;

  strcpy(mask,"00110000000000000000000000000000");
  int_mask= binaryToInt(mask);


  type[1]=binaryToInt(reverseString(intToBinary(int_instr & int_mask,32)));
  if     (strcmp("00000000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[1]=0;
  else if(strcmp("00010000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[1]=1;
  else if(strcmp("00100000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[1]=2;
  else if(strcmp("00110000000000000000000000000000",intToBinary(int_instr & int_mask,32))==0)
    type[1]=3;

  return type;

  return 0;
}

void convertInstructions(FILE * fp)
{
  instrArray=(instruction *)calloc(lines, sizeof(instruction));
  strcpy(instrArray[3].binaryInstr,"4455");
  strcpy(instrArray[50].binaryInstr,"4fgdsg 455");

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
    printf("%d %d",instrArray[lineNum].type,instrArray[lineNum].subType);
    printf("ASD %s\n", instrArray[lineNum].binaryInstr);

    lineNum++;
  }

  if (line)
      free(line);
}

int main()
{
  FILE * fp1;
  fp1 = fopen("output.txt", "r");
  if (fp1 == NULL)
    exit(EXIT_FAILURE);

  lines = countInstructions(fp1);
  // printf("%d", lines);
  fp1 = fopen("output.txt", "r");

  convertInstructions(fp1);                                                    //each instraction is converted to a
  fclose(fp1);                                                                 //struct object; easier to handle
  return 0;
}
