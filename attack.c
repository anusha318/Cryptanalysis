// Author: Anusha Sridharan
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ctype.h>

void calculate_missing_month(char *);

/*
   TODO cases - Anusha:
   12345678<8<<<11??182<11??167<<<<<<<<<<<<<<<2"; This is when month is missing but chcksum is given
   After resolving the case, the output should be th expected MRZ for seeding, which is 12345678<811101821111167
 */
void main()
{
	char *mrz_missing_info = "12345678<8<<<?1?0182<1111167<<<<<<<<<<<<<<<2";
	// example output should be "12345678<8111018211111672"
	calculate_missing_month(mrz_missing_info);
}
void calculate_missing_month(char *mrz)
{
	int weights[]={7,3,1};
	printf("\nGiven MRZ: %s\n",mrz);
	int mrzlen = strlen(mrz);
	char *str1 = (char *)(malloc(sizeof(char) * mrzlen));
	memset(str1, 0 , mrzlen);
	strncpy(str1, mrz, mrzlen);
	char *str2 = (char *)(malloc(sizeof(char) * mrzlen));
	memset(str2, 0 , mrzlen);
	int i=0,j=0;
	int cksum=0,count=0,cksum_dt=0;
	int pos1, pos2, dt_pos1, dt_pos2 = 0;	
	int missing_val1, missing_val2,rem_pos =0;
	j=0;
	//Remove the < after passport number field as it is not required in Kseed
	for(i=0;i<mrzlen;i++)
	{
		if(i>9)
		{
			if(str1[i] != '<')
			{
				str2[j]=str1[i];
				j++;
			}
		}
		else
		{	
			str2[j]=str1[i];
			j++;
		}
	}
	str2[j]='\0';
//calculate total check digit and identify the position of missing data elements
	for(i=0;i<strlen(str2)-1;i++)
	{
		if(str2[i]=='?')
		{
			if(count==0) 
			{
				missing_val1=weights[i % 3];
				pos1=i;
				count++;
			}
			else 
			{
				missing_val2=weights[i % 3];
				pos2=i;
			}
		}
		else if(str2[i]=='<')
		{
			continue;
		}
		else if(isdigit(str2[i]))
		{
			cksum +=(str2[i] - '0')*weights[i % 3];
		}
		else
		{
			cksum += (str2[i] -'A' +10)*weights[i % 3];
		}
	}
	count=0;
	//X and Y missing in DOB field
	if(pos1>=10 && pos1<16)
	{
		rem_pos=16;
		for(i=10;i<16;i++)
		{
			if(str2[i]!='?')
			{
				cksum_dt +=(str2[i]-'0')*weights[(i-1) % 3];
			}
			else if(count==0)
			{
				dt_pos1=weights[(i-1)%3];
				count++;
			}
			else dt_pos2=weights[(i-1)%3];
		}
	}
	//X and Y missing in DOE field
	else if(pos1>=17 && pos1<23)
	{	
		rem_pos=23;
		for(i=17;i<23;i++)
		{
			if(str2[i]!='?')
			{
				cksum_dt +=(str2[i]-'0')*weights[(i+1) % 3];
			}
			else if(count==0)
			{
				dt_pos1=weights[(i+1)%3];
				count++;
			}
			else dt_pos2=weights[(i+1)%3];
		}
	}
	int flag=0;
	//solve two linear equations
	for(i=0;i<10;i++)
	{
		if(flag==1)
		{
				break;
		}
		for(j=0;j<10;j++)
		{
			if(((cksum+(missing_val1*i)+(missing_val2*j))%10==str2[strlen(str2)-1]-'0') && ((cksum_dt+(dt_pos1*i)+(dt_pos2*j))%10==str2[rem_pos]-'0'))
			{
				str2[pos1]=i+'0';
				str2[pos2]=j+'0';
				printf("\n Missing values are %d and %d\n",i,j);
				flag=1;
				break;
			}
		}	
	}
	printf("\nThe MRZ used for Kseed is %s\n",str2);
}