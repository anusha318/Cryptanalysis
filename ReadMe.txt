MRZ find missing digit
Conpile: gcc mrz_detect.c -o mrz_detect
Usage: ./mrz_detect
The MRZ information is hardcoded in the program as "12345678<8<<<1110182<111116?<<<<<<<<<<<<<<<2"



Attack on MRZ based AES key system:

Compile: gcc attack.c -g
Usage:  ./a.out
The MRZ information is hardcoded in the program which is "12345678<8<<<?1?0182<1111167<<<<<<<<<<<<<<<2"
The program can find the missing digits from the date fields - Date of Birth and Date of expiry

ORYX stream cipher attack

Compile: gcc oryx_attack.c -o oryx_attack
Usage: ./oryx_attack

Outputs 8 possible values for X||A||B

ORYX stream cipher part 3

Compile: gcc oryx_attack_part3.c -o part3
Usage: ./part3
Outputs thr L [] values for H(A) and the shifts in B register
