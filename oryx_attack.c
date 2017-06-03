#include <stdio.h>
#include <stdlib.h>
#include <string.h>


unsigned char L_permutation[256];
unsigned char keystreamByte[30];
unsigned char extensions[12][4] = {{1,1,0,0},{1,1,0,128},{1,1,128,0},{1,1,128,128},{1,2,128,0},{1,2,128,64},{1,2,128,128},{1,2,128,192},{1,2,0,0},{1,2,0,64},{1,2,0,128},{1,2,0,192}};

void find_next_putative_fill(int,unsigned char,unsigned char,unsigned char);

unsigned char chosenX[32],chosenA[32],chosenB[32];
int temp_extension_sequence[32];

int main(int argc, const char *argv[])
{

	unsigned char tempX,tempA,tempB;

	unsigned char highX,highA,highB;

	//Fetch L permutation value from file

	FILE *lfile = fopen("Lpermutation.txt","r");
	for(int i=0;i<256;i++)
    {
        fscanf(lfile,"%x\n",&L_permutation[i]);
    }
    fclose(lfile);
    //Fetch keystream bytes from file
    FILE *keyfile = fopen("keystreamdata.txt","r");
    for(int j=0;j<30;j++)
    {
        fscanf(keyfile,"%x\n",&keystreamByte[j]);
    }
    fclose(keyfile);

    for(int i=0;i<256;i++)
    {
        for(int k=0;k<256;k++)
        {
            highA = i & 0xff;
            highB = k & 0xff;

            highX = (keystreamByte[0]-L_permutation[highA]-L_permutation[highB])% 0xff;

            tempX = highX;
            tempA = highA;
            tempB = highB;

            chosenX[0] = tempX;
            chosenA[0] = tempA;
            chosenB[0] = tempB;
            temp_extension_sequence[0] = -1;

            find_next_putative_fill(1,tempX,tempA,tempB);

        }
    }

    //printf("X = %02x\nA = %02x\nB = %02x\nThe length of chain is %d\n",chosenX,chosenA,chosenB,chainlength);


} // end main

void find_next_putative_fill(int keynum,unsigned char Xp,unsigned char Ap,unsigned char Bp)
{
    unsigned char possibleXext0,possibleXext1;
    unsigned char possibleA,possibleB;
    unsigned long int X = 0L;
    unsigned long int A = 0L;
    unsigned long int B = 0L;

    unsigned char Tx;
    possibleXext0 = (Xp) >> 1;
    possibleXext1 = ((1 << 7) &0xff | (Xp) >> 1) & 0xff;
    chosenX[keynum] = Xp;
    chosenA[keynum] = Ap;
    chosenB[keynum] = Bp;

    if(keynum >= 30)
    {
        A = chosenA[1];
        B = chosenB[1];
        int offsetA = 0;
        int offsetB = 0;
        for(int count=1;count<25;count++)
        {
            unsigned long int extra_bit_A = 0L | extensions[temp_extension_sequence[count]][2];
            offsetA += extensions[temp_extension_sequence[count]][0];
            offsetB += extensions[temp_extension_sequence[count]][1];
            A = A | (extra_bit_A << offsetA);
            unsigned long int extra_bit_B = 0L | extensions[temp_extension_sequence[count]][3];
            B = B | (extra_bit_B << offsetB);

        }
        X = X | chosenX[1];
        X = X | ((0L |chosenX[9]) << 8);
        X = X | ((0L |chosenX[17]) << 16);
        X = X | ((0L |chosenX[25]) << 24);
        printf("A just after initial seed = %08lx\nB just after initial seed = %08lx\n X after initial seed = %08lx\n",A,B,X);

        //find if B steps once or twice
        int steptimes = ((X & 0x20) >> 5) & 0x1;


        if(steptimes == 0)
        {
            printf("X <<1, A<<1, B<<1 with 000 = %08x%08x%08x\n",(X<<1)|0L,(A<<1)|0L,(B<<1)|0L);
            printf("X <<1, A<<1, B<<1 with 001 = %08x%08x%08x\n",(X<<1)|0L,(A<<1)|0L,(B<<1)|1L);
            printf("X <<1, A<<1, B<<1 with 010 = %08x%08x%08x\n",(X<<1)|0L,(A<<1)|1L,(B<<1)|0L);
            printf("X <<1, A<<1, B<<1 with 011 = %08x%08x%08x\n",(X<<1)|0L,(A<<1)|1L,(B<<1)|1L);
            printf("X <<1, A<<1, B<<1 with 100 = %08x%08x%08x\n",(X<<1)|1L,(A<<1)|0L,(B<<1)|0L);
            printf("X <<1, A<<1, B<<1 with 101 = %08x%08x%08x\n",(X<<1)|1L,(A<<1)|0L,(B<<1)|1L);
            printf("X <<1, A<<1, B<<1 with 110 = %08x%08x%08x\n",(X<<1)|1L,(A<<1)|1L,(B<<1)|0L);
            printf("X <<1, A<<1, B<<1 with 111 = %08x%08x%08x\n",(X<<1)|1L,(A<<1)|1L,(B<<1)|1L);
        }

        //if B<<2
        else
        {
            for(unsigned long int xbit=0;xbit<2;xbit++)
            {
                for(unsigned long int abit=0;abit<2;abit++)
                {
                    for(unsigned long int bbit=0;bbit<4;bbit++)
                    {
                        printf("X <<1, A<<1, B<<2 = %08x%08x%08x\n",(X<<1)|xbit,(A<<1)|abit,(B<<2)|bbit);
                    }
                }
            }
        }


        return;
    }

    for(int j=0;j<12;j++)
    {
        int shiftA = (int)extensions[j][0];
        int shiftB = (int)extensions[j][1];
        possibleA = (((Ap) >> shiftA) | extensions[j][2]) & 0xff;
        possibleB = (((Bp) >> shiftB) | extensions[j][3]) & 0xff;
        Tx = (keystreamByte[keynum]-L_permutation[possibleA]-L_permutation[possibleB]) & 0xff;

        if((Tx == possibleXext0) || (Tx == possibleXext1))
        {
            temp_extension_sequence[keynum] = j;
            find_next_putative_fill(keynum+1,Tx,possibleA,possibleB);
        }
    }
}
