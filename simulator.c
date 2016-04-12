// $$$$$$$$$$$$ This is our myARMSim.c  file 



/* 

The project is developed as part of Computer Architecture class
Project Name: Functional Simulator for subset of ARM Processor

Developer's Name: Sonakshi Grover
Developer's Email id: sonakshi14106@iiitd.ac.in
Date: 31/3/2015

Developer's Name: Pahal Krishnia
Developer's Email id: 
Date: 31/3/2015


*/


/* myARMSim.cpp
   Purpose of this file: implementation file for myARMSim
*/

#include "myARMSim.h"
#include <stdlib.h>
#include <stdio.h>
#include<string.h>



//Register file
static unsigned int R[16];

//flags
static int N,C,V,Z;
//memory
static unsigned char MEM[4000];

char INST[80][9]; // $$$$$$$$$$$$ instruction memory
 
char cond[5];  //$$$$$$$$$$$$  condition 
char F[3];    // $$$$$$$$$$$$ instruction format
char I; //$$$$$$$$$$ immediate
char opcode[5] ; 
char S; //$$$$$$$$$$$$$$$$$   set condition code
char Rn[5]; 
char Rd[5];
char Rm[5];
char Imm[9];
char offset[13];
char b_opcode[3];
char b_offset[25];

char instruction[9];  //for storing 1 instruction

//intermediate datapath and control path signals
static unsigned int instruction_word;
static unsigned int operand1;
static unsigned int operand2;

char arr[5]; //$$$$$$$$$$$$  used by hextobin()
int tot_inst; //$$$$$$$$$$$$$ no of instructions
int n,mr,mi,d,off;  //$$$$$$$$$$$ 1st operand's index ,2nd operand's index, imm value and destination operand's index 
int sum;  //$$$$$$$$$ used by bintodec()
int ex_flag;  // $$$$$$$$$ flag for exit
int control; //for opcode
int result;// for execute()
int result_mem;
int b_off;
int b_flag;
int c_result;

int u=0; ////////////
void run_armsim() 
{
  
   while(ex_flag!=1) 
  {
    b_flag=0;
    fetch();
    decode();
    execute();
    if(ex_flag==1)
      {return ;}
    //if(control==17&&u==5)  //////////
      // return;       /////////////
    if(b_flag==1)
       continue;
    mem();
    write_back();
  }
 

  return;
}


// it is used to set the reset values
//reset all registers and memory content to 0
void reset_proc()
{
   int i;
   for(i=0;i<16;i++)
       { R[i]=0; }
   for(i=0;i<4000;i++)
       { MEM[i]='0';}

}




//load_program_memory reads the input memory, and pupulates the instruction 
// memory
void load_program_memory(/*char *file_name*/)
{
  char c1,c2;
  FILE *fp;
  unsigned int address;
  
  fp = fopen("array.mem", "r");
  if(fp == NULL) {
    printf("Error opening input mem file\n");
    exit(1);
  }
  while(fscanf(fp, "%x %c%c%s", &address, &c1,&c2,instruction) != EOF) 
  {
     tot_inst++;
     printf("%d\n",address);
     //printf("%d\n",address%10);
     write_word(address, instruction);
    
  }
 
  fclose(fp);
  
}



//writes the data memory in "data_out.mem" file
void write_data_memory() 
{
  /*FILE *fp;
  unsigned int i;
  fp = fopen("simple_add.mem", "w");
  if(fp == NULL) {
    printf("Error opening dataout.mem file for writing\n");
    return;
  }
  
  for(i=0; i < 4000; i = i+4){
    fprintf(fp, "%x %x\n", i, read_word(MEM, i));
  }
  fclose(fp);
 
*/}

//$$$$$$$$$$$$$$$$$$$$$$$$$$ for printing the contents of REG and INST array  

void printreg()
{ 
  printf(" \n\nREGISTER CONTENTS ARE :\n");
  int i;
  for(i=0;i<16;i++) 
  { 
    printf("%d %d\n", i, R[i]);
  }
}

void print()
{ 
  printf(" INSTRUCTION MEMORY CONTENTS ARE :\n");
  int i,cnt;
  for(i=0,cnt=0; cnt<tot_inst+20 ; i = i+4,cnt++) 
  { 
    printf("%d %s\n", i, INST[i]);
  }
} 

int prod;
void power( int x,int p )
{
  int i;
  for(i=1;i<=p;i++)
  {
     prod=prod*x;
    
  }

}

void hextobin( char a)
{ 
   
   if(a=='0') 
     strcpy(arr,"0000\0");
   else
   if(a=='1')
     strcpy(arr,"0001\0");
   else 
   if(a=='2') 
     strcpy(arr,"0010\0");
   else
   if(a=='3') 
     strcpy(arr,"0011\0");
   else
   if(a=='4') 
     strcpy(arr,"0100\0");
   else
   if(a=='5') 
     strcpy(arr,"0101\0");
   else
   if(a=='6') 
     strcpy(arr,"0110\0");
   else
   if(a=='7') 
      strcpy(arr,"0111\0");
   else
   if(a=='8') 
      strcpy(arr,"1000\0");
   else
   if(a=='9') 
      strcpy(arr,"1001\0");
   else
   if(a=='A') 
      strcpy(arr,"1010\0");
   else
   if(a=='B') 
      strcpy(arr,"1011\0");
   else
   if(a=='C') 
      strcpy(arr,"1100\0");
   else
   if(a=='D') 
      strcpy(arr,"1101\0");
   else
   if(a=='E') 
      strcpy(arr,"1110\0");
   else
   if(a=='F') 
      strcpy(arr,"1111\0");

   

}

void bintodec( char *ptr)
{ 
   
   int size=strlen(ptr);
   int i,p;

   for(i=size-1,p=0;i>=0;i--,p++)
   {  
      prod=1;
      if(ptr[i]=='1')
       { power(2,p);
         sum=sum+prod;
       }      
      
   }
  
}
 

//should be called when instruction is swi_exit
void swi_exit() {
  write_data_memory();
  exit(0);
}



//reads from the instruction memory and updates the instruction register
void fetch()
{
   strcpy(instruction,INST[R[15]]);
   printf("\n\n Fetch instruction 0X%s from address 0X%x \n \n ",instruction,R[15]); 
   R[15]=R[15]+4;
}


int flag3; //flag3=0 +ve offset ,flag3=1 -ve offset
//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
int z=0,w;
void decode() 
{
   flag3=0;
   hextobin(instruction[0]);
   strcpy(cond,arr);
   
   hextobin(instruction[1]);
   F[0]=arr[0];
   F[1]=arr[1];
   
   I=arr[2];
   opcode[0]=arr[3];
   b_opcode[0]=arr[2];
   b_opcode[1]=arr[3];
   
   hextobin(instruction[2]);
   opcode[1]=arr[0];
   opcode[2]=arr[1];
   opcode[3]=arr[2];
   b_offset[0]=arr[0];
   b_offset[1]=arr[1];
   b_offset[2]=arr[2];
   
   
   S=arr[3];
   b_offset[3]=arr[3];
   

   hextobin(instruction[3]);
   strcpy(Rn,arr);
   b_offset[4]=arr[0];
   b_offset[5]=arr[1];
   b_offset[6]=arr[2];
   b_offset[7]=arr[3];
   
   
   hextobin(instruction[4]);
   strcpy(Rd,arr);
   b_offset[8]=arr[0];
   b_offset[9]=arr[1];
   b_offset[10]=arr[2];
   b_offset[11]=arr[3];
      




   hextobin(instruction[5]);
   offset[0]=arr[0];
   offset[1]=arr[1];
   offset[2]=arr[2];
   offset[3]=arr[3];
   b_offset[12]=arr[0];
   b_offset[13]=arr[1];
   b_offset[14]=arr[2];
   b_offset[15]=arr[3];
   


   hextobin(instruction[6]);
   Imm[0]=arr[0];
   Imm[1]=arr[1];
   Imm[2]=arr[2];
   Imm[3]=arr[3];
   offset[4]=arr[0];
   offset[5]=arr[1];
   offset[6]=arr[2];
   offset[7]=arr[3];
   b_offset[16]=arr[0];
   b_offset[17]=arr[1];
   b_offset[18]=arr[2];
   b_offset[19]=arr[3];
      

   
   hextobin(instruction[7]);
   strcpy(Rm,arr);
   offset[8]=arr[0];
   offset[9]=arr[1];
   offset[10]=arr[2];
   offset[11]=arr[3];
   b_offset[20]=arr[0];
   b_offset[21]=arr[1];
   b_offset[22]=arr[2];
   b_offset[23]=arr[3];
   
      
   Imm[4]=arr[0];
   Imm[5]=arr[1];
   Imm[6]=arr[2];
   Imm[7]=arr[3];
  
   sum=0; 
   bintodec(Rn);
   n=sum;
      
   sum=0;
   bintodec(Rm);
   mr=sum;
   
   
   sum=0;
   bintodec(Imm);
   mi=sum;
  
   
   sum=0;
   bintodec(Rd);
   d=sum;

   sum=0;
   bintodec(offset);
   off=sum;

   if(instruction[2]=='F')
    {  
       flag3=1;
       if(instruction[7]=='E')
         b_off=-1;
       if(instruction[7]=='D')
         b_off=-2; 
       if(instruction[7]=='C')
         b_off=-3; 
       if(instruction[7]=='B')
         b_off=-4; 
       if(instruction[7]=='A')
         b_off=-5; 
       if(instruction[7]=='9')
         b_off=-6; 
       if(instruction[7]=='8')
         b_off=-7; 
       if(instruction[7]=='7')
         b_off=-8; 
       if(instruction[7]=='6')
         b_off=-9; 
        if(instruction[7]=='5')
         b_off=-10; 
       if(instruction[7]=='4')
         b_off=-11; 
       if(instruction[7]=='3')
         b_off=-12; 
       if(instruction[7]=='2')
         b_off=-13; 
       if(instruction[7]=='1')
         b_off=-14; 
       if(instruction[7]=='0')
         b_off=-15; 
 
    }
   else
   {sum=0;
   bintodec(b_offset);
   b_off=sum;
   }
   //printf("\n f= %s b_opcode= %s, b_off = %d \n",F,b_opcode,b_off);
   
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"1101")==0) && I=='1') 
  { printf(" Operation is MOV , immediate value is %d , Destination register is R%d \n",mi,d); 
    control=1;
  } 
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"1101")==0) && I=='0') 
  { printf(" Operation is MOV , register  is R%d , Destination register is R%d \n",mr,d); 
    control=0;
  }   
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"0100")==0 )&&(I=='0')) 
  { 
    printf(" Operation is ADD , first operand is R%d ,Second operand is R%d , Destination register is R%d \n",n,mr,d); 
    control=2;  

  }
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"0100")==0 )&&(I=='1')) 
  { //printreg();///////////////////////////
    //u++;    ////////////////////////
    printf(" Operation is ADD , first operand is R%d ,immediate value is %d , Destination register is R%d \n",n,mi,d); 
    control=5;  

  }
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"0010")==0 )&&(I=='0')) 
  { 
    printf(" Operation is SUB , first operand is R%d ,Second operand is R%d , Destination register is R%d \n",n,mr,d); 
    control=6;  

  }
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"0010")==0 )&&(I=='1')) 
  { 
    printf(" Operation is SUB , first operand is R%d ,immediate value is %d , Destination register is R%d \n",n,mi,d); 
    control=7;  

  }
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"1010")==0 )&&(I=='0')) 
  { 
    printf(" Operation is CMP , first operand is R%d ,Second operand is R%d \n",n,mr); 
    printf("\n R[4]== %d\n",R[4]);
    control=9;  

  }
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"1010")==0 )&&(I=='1')) 
  { 
    printf(" Operation is CMP , first operand is R%d ,immediate value is %d \n",n,mi); 
    printf("\n R[4]== %d\n",R[4]);
    control=11;  
    if(z==0)
      {w=mi;z=1;} 
  }
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"0100")==0 )&&(I=='0')) 
  { 
    printf(" Operation is AND , first operand is R%d ,Second operand is R%d , Destination register is R%d \n",n,mr,d); 
    control=20;  

  }
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"0100")==0 )&&(I=='1')) 
  { 
    printf(" Operation is AND , first operand is R%d ,immediate value is %d , Destination register is R%d \n",n,mi,d); 
    control=21;  

  }
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"0100")==0 )&&(I=='0')) 
  { 
    printf(" Operation is OR , first operand is R%d ,Second operand is R%d , Destination register is R%d \n",n,mr,d); 
    control=22;  

  }
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"0100")==0 )&&(I=='1')) 
  { 
    printf(" Operation is OR , first operand is R%d ,immediate value is %d , Destination register is R%d \n",n,mi,d); 
    control=23;  

  }
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"0100")==0 )&&(I=='0')) 
  { 
    printf(" Operation is EOR , first operand is R%d ,Second operand is R%d , Destination register is R%d \n",n,mr,d); 
    control=24;  

  }
  else
  if((strcmp(F,"00")==0)&&(strcmp(opcode,"0100")==0 )&&(I=='1')) 
  { 
    printf(" Operation is EOR , first operand is R%d ,immediate value is %d , Destination register is R%d \n",n,mi,d); 
    control=25;  

  }
  
  
  if((strcmp(F,"01")==0)&&(I=='0')&&(strcmp(opcode,"1100")==0)) 
  { 
    printf(" Operation is LDR , base address is %d , offset is %d , Destination register is R%d \n",n,off,d); 
    control=3;  

  }
  
  /*else
  if((strcmp(F,"01")==0)&&(I=='0')&&(strcmp(opcode,"1100")==0))  //$$$$$$$$ for register offsets
  { 
    printf(" Operation is LDR , base address is %d , offset is %d , Destination register is R%d \n",n,off,d); 
    control=26;  

  }
  */
  else
  if((strcmp(F,"01")==0)&&(I=='0')&&(strcmp(opcode,"1100")==0)) 
  { 
    printf(" Operation is STR , base address is %d , offset is %d , register whose value is to be stored is R%d \n",n,off,d); 
    control=4;  

  }
  else
  if((strcmp(F,"01")==0)&&(I=='1')&&(strcmp(opcode,"1100")==0)) //$$$$$$$$$$ for register offsets
  { 
    printf(" Operation is STR , base address is stored in R%d , offset register is R%d , register whose value is to be stored is R%d \n",n,mr,d); 
    control=27;  

  }

  else
  if((strcmp(F,"10")==0)&&(strcmp(b_opcode,"10")==0)&&(strcmp(cond,"0000")==0)) 
  { 
    printf(" Operation is Branch EQ , branch offset is %d\n",b_off); 
    control=8;  

  }
  else
  if((strcmp(F,"10")==0)&&(strcmp(b_opcode,"10")==0)&&(strcmp(cond,"0001")==0)) 
  { 
    printf(" Operation is Branch NE , branch offset is %d\n",b_off); 
    control=12;  

  }
  else
  if((strcmp(F,"10")==0)&&(strcmp(b_opcode,"10")==0)&&(strcmp(cond,"1011")==0)) 
  { 
    printf(" Operation is Branch LT , branch offset is %d\n",b_off); 
    control=13;  

  }
  else
  if((strcmp(F,"10")==0)&&(strcmp(b_opcode,"10")==0)&&(strcmp(cond,"1101")==0)) 
  { 
    printf(" Operation is Branch LE , branch offset is %d\n",b_off); 
    control=14;  

  }
  else
  if((strcmp(F,"10")==0)&&(strcmp(b_opcode,"10")==0)&&(strcmp(cond,"1100")==0)) 
  { 
    printf(" Operation is Branch GT , branch offset is %d\n",b_off); 
    control=15;  

  }
  else
  if((strcmp(F,"10")==0)&&(strcmp(b_opcode,"10")==0)&&(strcmp(cond,"1010")==0)) 
  { 
    printf(" Operation is Branch GE , branch offset is %d\n",b_off); 
    control=16;  

  }
  else
  if((strcmp(F,"10")==0)&&(strcmp(b_opcode,"10")==0)&&(strcmp(cond,"1110")==0)) 
  { 
    //return;                 /////////////////////////////////////////////////////////////////////////
    printf(" Operation is Branch ALL , branch offset is %d\n",b_off); 
    control=17;  

  }
  else
  if(strcmp(opcode,"1000")==0) 
  { printf(" EXIT \n");
    control=10;
      
  }
  /*else
  if(instruction[4]=='5')
      R[5]=
  */
}

//executes the ALU operation based on ALUop
void execute() 
{

/*$$$$$$$$$$  Opcode 

0 for AND
1 for EOR (xor operation)
2 for SUB
4 for ADD
5 for ADC (add with carry)
10 for CMP
12 for ORR
13 for MOV
15 for MNV (bitwise not)
*/

/*
I (bit 25) : Value 0 if second operand is register, 1 if second operand is immediate
*/

  if(control==0) 
  { result=R[mr];
  }
  else
  if(control==1) 
  { result=mi;
  }
  else
  if(control==2) 
  { result=R[n]+R[mr];
    
  }
  else
  if(control==3) 
  { result=R[n]+off;
    
  }
  else
  if(control==4) 
  { result=R[n]+off;
    
  }
  else
  if(control==7) 
  { result=R[n]-mi;
    
  }
  else
  if(control==6) 
  { result=R[n]-R[mr];
    
  }
  else
  if(control==5) 
  { 
    //printreg();///////////////
    result=R[n]+mi;
    
  }
  else
  if(control==11) 
  { 
    
    c_result=R[n]-mi;
    printf(" c_result === %d",c_result);
    if(c_result==0)
      Z=0;                
       
  }
  else
  if(control==9) 
  { 
    
    c_result=R[n]-R[mr];
    printf(" c_result === %d",c_result);
    if(c_result==0)          
      Z=0;
       
  }
  else
  if(control==8) 
  { 
    printf("\n control 8");
    if(c_result==0)
      {
        printf("\nbranch offset==%d\n",b_off);
        printf(" earlier pc = %d ",R[15]);
        if(flag3==1)
           {R[15]=R[15]+((b_off)*4)-8; 
           printf(" R[15]  = %d \n",R[15]);}  
        else
           R[15]=R[15]+((b_off)*4)+4;
        
        printf(" now pc = %d ",R[15]);

        b_flag=1;

      }
  }
  else
  if(control==12) 
  { 
    
    if(c_result!=0)
      { printf(" earlier pc = %d ",R[15]);
        
        if(flag3==1)
           {R[15]=R[15]+((b_off)*4);
           printf(" R[15]  = %d \n",R[15]);}   
        else
           R[15]=R[15]+((b_off)*4)+4;
        
        printf(" now pc = %d ",R[15]);
        b_flag=1;

      }
  }
  else
  if(control==13) 
  { 
    
    if(c_result<0)
      {
        R[15]=R[15]+((b_off)*4)+4;
        b_flag=1;

      }
  }
  else
  if(control==14) 
  { 
    
    if(c_result<=0)
      {
        R[15]=R[15]+((b_off)*4)+4;
        b_flag=1;

      }
  }
  else
  if(control==15) 
  { 
    
    if(c_result>0)
      {
        R[15]=R[15]+((b_off)*4)+4;
        b_flag=1;

      }
  }
  else
  if(control==16) 
  { 
    
    if(c_result>=0)
      {
        R[15]=R[15]+((b_off)*4)+4;
        b_flag=1;

      }
  }
  else
  if(control==17) 
  { 
      //  return;  ////////////////////////////////
        R[15]=R[15]+((b_off)*4)+4;
        b_flag=1;

     
  }
  else
  if(control==27) 
  { result=R[n]+R[d];
    
  }
  else
  if(control==10) 
  { 
    
    ex_flag=1;
    return; 
  }
 
}



//perform the memory operation
void mem()
{
   if(control==3) 
  { result_mem=MEM[result]-48;
   
  }
  else
  if(control==4) 
  { MEM[result]=R[d]+48;
   
  }
  else
  if(control==27) 
  { MEM[result]=R[d]+48;
   
  }
  
  
}
//writes the results back to register file
void write_back()
{
  if(control==3)
     R[d]=result_mem;

  if(control==1||control==2||control==0||control==5||control==6||control==7)
     { printf("\n\n\n hhhh\n");
        R[d]=result;
       printreg();
       
     }
}



int read_word(char *mem, unsigned int address)
{
  int *data;
  data =  (int*) (mem + address);
  return *data;
}


void write_word(unsigned int address, char * instruction)  // $$$$$$$ copies instructions onto  the INST[]
{
   /*int rem;
   int dec=0;  
   int h=0; 
   printf(" inst[address] ======== %s ",INST[address]);
   while(address>0)
   {
     printf("\n intermed dec= %d",dec);
     rem=address%10;
     prod=1;
     power(16,h);
     dec=dec+(rem*prod);
     address=address/10;
     h++;
   

   } 

   printf("\ndec in strcpy is %d \n\n\n",dec);
   */strcpy(INST[address],instruction);
   
}


int main() 
{
  int i; 
  //reset the processor
  reset_proc();
  
  //load the program memory
  load_program_memory();
  print();
  
  
  
  //$$$$$$$$$$$$$$ set pc=0X0
  R[15]=0;
  ex_flag=0;
  //run the simulator  
  run_armsim();
  printreg();

  // $$$$$$$$$$$  printing the memory array
  MEM[w+1]=R[7]+48; 
 // printf("\n\n%d",R[7]);
  printf("\n\nPrinting the elements of MEM array\n\n");
    for(i=0;i<w+2;i++)
      printf("%d ",MEM[i]-48);
  printf("\n"); 
  swi_exit();
  
  return 0;
}
