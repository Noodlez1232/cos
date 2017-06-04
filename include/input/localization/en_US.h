#ifndef _ENUSKB_H
#define _ENUSKB_H 1

char *numrow = "1234567890-=";
char *toprow = "qwertyuiop[]";
char *midrow = "asdfghjkl;\'`";
char *bottomrow = "\\zxcvbnm,./";
char *shiftnumrow = "!@#$%^&*()_+";
char *shifttoprow = "QWERTYUIOP{}";
char *shiftmidrow = "ASDFGHJKL:\"~";
char *shiftbottomrow = "|ZXCVBNM<>?";

unsigned char numrowstart=0x02;
unsigned char numrowend=0x0E;
unsigned char toprowstart=0x10;
unsigned char toprowend=0x1B;
unsigned char midrowstart=0x1E;
unsigned char midrowend=0x29;
unsigned char bottomrowstart=0x2B;
unsigned char bottomrowend=0x35;

#endif