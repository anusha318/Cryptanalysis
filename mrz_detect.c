#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int calculate_missing_value(int,char *,int);

/*
TODO cases - Anusha:
12345678<8<<<11??182<11??167<<<<<<<<<<<<<<<2"; This is when month is missing but chcksum is given
After resolving the case, the output should be th expected MRZ for seeding, which is 12345678<811101821111167
*/
void main()
{
	char *mrz_missing_info = "12345678<8<<<1110182<111116?<<<<<<<<<<<<<<<2";
	char *mrz_to_seed = (char*)calloc(25,sizeof(char));
	//Extract fields for passport information, date of birth and date of expiry
	char passport_info[11];
	char date_of_birth[8];
	char date_of_expiry[8];
	int missing_value=0;
	int field_with_missing_value = '\0'; //use P - passport field, B - date of birth field, E - date of expiry field
	strncpy(passport_info,mrz_missing_info,10);
	passport_info[10]='\0';
	strncpy(date_of_birth,mrz_missing_info+13,7);
	date_of_birth[7]='\0';
	strncpy(date_of_expiry,mrz_missing_info+21,7);
	date_of_expiry[7]='\0';

	//Find the location of ? to determine the field to which it belongs
	char *position = strchr(mrz_missing_info,'?');
	if(position != NULL)
	{
		int position_offset = position-mrz_missing_info;
		int internal_offset = 0;

		if(position_offset <=9) //Missing information in passport number field
		{
			internal_offset = position_offset;
			field_with_missing_value = 'P';
			missing_value = calculate_missing_value(field_with_missing_value,passport_info,internal_offset);
			passport_info[internal_offset] = missing_value+'0';
		}
		else if(position_offset <= 19) //Missing information in date of birth field
		{
			internal_offset = position_offset - 13;
			field_with_missing_value = 'B';
			missing_value = calculate_missing_value(field_with_missing_value,date_of_birth,internal_offset);
			date_of_birth[internal_offset] = missing_value+'0';
		}
		else //Missing information in expiry field
		{
			internal_offset = position_offset - 21;
			field_with_missing_value = 'E';
			missing_value = calculate_missing_value(field_with_missing_value,date_of_expiry,internal_offset);
			date_of_expiry[internal_offset] = missing_value+'0';
			
		}

	}
		
	memcpy(mrz_to_seed,passport_info,sizeof(passport_info));
	memcpy(mrz_to_seed+10,date_of_birth,sizeof(date_of_birth));
	memcpy(mrz_to_seed+17,date_of_expiry,sizeof(date_of_expiry));
	
	printf("The MRZ information that should be used as seed for key calculation is %s\n",mrz_to_seed);
}

int calculate_missing_value(int field_with_missing_value,char *field_of_info,int offset)
{
	int weights[]={7,3,1};
	if(offset == strlen(field_of_info)-1) // Case 1 - where the checksum is the missing element. This is the original challenge question
	{
		int i=0;
		int sum = 0;
		while(i < strlen(field_of_info)-1)
		{
			int value_of_element = (field_of_info[i] == '<')?0:field_of_info[i]-'0';
			sum += (value_of_element * weights[i % 3]);
			i++;
		}
		return sum % 10;
	}
	else if(field_with_missing_value != 'P') //Optimization techniques can only be used if the missing value is in fields other than passport number. Guessing passport number is exhaustive.
	{
		// If any value in month of birth is missing, use the optimization that month value can only be between 01 to 31. Also, if anything missing in year field, compare with year field in expiry.
			// TODO
		

	}
	
}