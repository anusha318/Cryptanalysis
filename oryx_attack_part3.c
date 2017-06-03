#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// polynomials
#define POLYNOMIAL_PX  0x8ce56011L
#define POLYNOMIAL_PA0 0xdb710641L
#define POLYNOMIAL_PA1 0xc3e887e1L
#define POLYNOMIAL_PB  0xa4031803L

unsigned long int Ainitial = 0xdeadbeefL;
unsigned long int Xinitial = 0x1c6f2726L;
unsigned long int Binitial = 0x00000000L;
int pushindex = -1;
int L_indexes[256];

unsigned char putative_L_box_value[256];
int B_stepping_sequence[50];
unsigned char L_box_value[256];

unsigned char keystreamByte[50] = {0x3f,0xff,0xdc,0x91,0xcd,0x06,0xff,0x5f,0x44,0x7d,0x83,0x5a,0x96,0x4c,0x44,0x2d,0xc8,0xf3,0x22,0x9e,0x4f,0xa1,0x21,0x1d,0xd1,0x8e,0xa4,0xe2,0x1f,0x76,0xda,0x12,0xba,0x68,0xa9,0x13,0xe1,0x87,0x12,0x7c,0x40,0x57,0xc7,0x89,0x84,0xf3,0xa0,0xb5,0x08,0x01};
unsigned char expected_L_B[50] = {0xb4,0x5d,0xdf,0x68,0x8b,0xc6,0x5b,0x7c,0x8f,0x0a,0xb1,0xf9,0x8f,0xfb,0xb1,0xa6,0x2e,0xfa,0x0a,0x12,0x11,0x07,0x4e,0x5a,0x2f,0x3e,0xdd,0x21,0xe3,0x2d,0x73,0xd7,0x1a,0x2a,0x01,0x1c,0x26,0xfd,0x0e,0x53,0x43,0xf0,0x95,0x8e,0x3d,0xce,0xad,0xed,0xce,0x9f};

unsigned long int stepA(unsigned long int, unsigned long int);
unsigned long int stepX(unsigned long int);
unsigned long int stepB(unsigned long int, unsigned long int);
unsigned char high(unsigned long int);
void push(int,unsigned char);
int find_lindex(int[],int);

void main()
{
    //initialize all values in L_indexes[] with -1
    for(int i=0;i<256;i++)
    {
        L_indexes[i]=-1;
    }

    unsigned long int X_next = Xinitial;
    unsigned long int A_next = Ainitial;

    for(int i=0;i<50;i++)
    {
        //step to generate keystream bytes and find L[H(A)] using key byte,H(X) and L[H(B)]
         X_next = stepX(X_next);
         A_next = stepA(A_next,X_next);
         int steptimesB = ((X_next & 0x20) >> 5) & 0x1;
         B_stepping_sequence[i]=(steptimesB==0)?1:2;
         int lindex = high(A_next);
         unsigned char lvalue = (keystreamByte[i]-high(X_next)-expected_L_B[i]) & 0xff;
         if(!find_lindex(L_indexes,lindex)) //if it returns value > -1, the L index has already been stored and there is no need to store again
         {
             push(lindex,lvalue);
         }
    }
    printf("The stepping sequence of register B for all 50 keystream bytes is:\n");
    for(int i=0;i<50;i++)
    {
        printf("%d\t",B_stepping_sequence[i]);
    }
    printf("\nL_index\tL_value\n");


    for(int i=0;i<=pushindex;i++)
    {
        printf("%d\t%02x\n",L_indexes[i],L_box_value[i]);
    }

}

unsigned long int stepA(unsigned long int A, unsigned long int X)
{
	static unsigned long int PA0 = POLYNOMIAL_PA0;
	static unsigned long int PA1 = POLYNOMIAL_PA1;

	unsigned long int t;

	int x;

	// x = X29 determines the polynomial
	x = ((X & 0x4) >> 2) & 0x1;


	// if x == 0 step A using PA0
	// else step A using PA1
	if(x == 0)
	{
		t = A & PA0;
	}
	else
	{
		t = A & PA1;
	}

	// new bit == poppar(A & PA)
	t = (t >> 16) ^ t;
	t = (t >> 8) ^ t;
	t = (t >> 4) ^ t;
	t = (t >> 2) ^ t;
	t = ((t >> 1) ^ t) & 0x1;

	// shift new bit to position 0
	t = (A >> 1) | (t << 31);

	return(t);

}

unsigned long int stepX(unsigned long int X)
{
	static unsigned long int PX = POLYNOMIAL_PX;

	unsigned long int t;

	// new bit == poppar(X & PX)
	t = X & PX;

	t = (t >> 16) ^ t;
	t = (t >> 8) ^ t;
	t = (t >> 4) ^ t;
	t = (t >> 2) ^ t;
	t = ((t >> 1) ^ t) & 0x1;

	// put new bit in position 0
	t = (X >> 1) | (t << 31);

	return(t);

}

unsigned long int stepB(unsigned long int B, unsigned long int X)
{
	static unsigned long int PB = POLYNOMIAL_PB;

	unsigned long int t0,
					  t1;

	int x;

	// x = X26 determines whether B steps once or twice
	x = ((X & 0x20) >> 5) & 0x1;

	// step B once

	// new bit == poppar(B & PB)
	t0 = B & PB;

	t0 = (t0 >> 16) ^ t0;
	t0 = (t0 >> 8) ^ t0;
	t0 = (t0 >> 4) ^ t0;
	t0 = (t0 >> 2) ^ t0;
	t0 = ((t0 >> 1) ^ t0) & 0x1;

	// shift new bit to position 0
	t0 = (B >> 1) | (t0 << 31);

	// if x == 1, step B again
	if(x == 1)
	{
		// new bit == poppar(t0 & PB)
		t1 = t0 & PB;

		t1 = (t1 >> 16) ^ t1;
		t1 = (t1 >> 8) ^ t1;
		t1 = (t1 >> 4) ^ t1;
		t1 = (t1 >> 2) ^ t1;
		t1 = ((t1 >> 1) ^ t1) & 0x1;

		// shift new bit to position 0
		t1 = (t0 >> 1) | (t1 << 31);

		return(t1);
	}
	else
	{
		return(t0);
	}

}


unsigned char high(unsigned long int reg)
{
	return(reg & 0xff);

}

void push(int lindex,unsigned char lvalue)
{
    pushindex++;
    L_indexes[pushindex] = lindex;
    L_box_value[pushindex] = lvalue;
}


int find_lindex(int L[],int index)
{
    int i=0;
    int found = 0;
    while((i<256) &&(L[i] != -1))
    {
        if(L[i] == index)
        {
            found = 1;
            return found;
        }
        else
        {
            found =0;
        }
        i++;
    }

    return found;
}

