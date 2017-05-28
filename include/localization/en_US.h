#ifndef _ENUSKB_H
#define _ENUSKB_H 1

const char *numrow = "1234567890-=";
const char *toprow = "qwertyuiop[]";
const char *midrow = "asdfghjkl;\'`";
const char *bottomrow = "\\zxcvbnm,./";
const char *shiftnumrow = "!@#$%^&*()_+";
const char *shifttoprow = "QWERTYUIOP{}";
const char *shiftmidrow = "ASDFGHJKL:\"~";
const char *shiftbottomrow = "|ZXCVBNM<>?";

const unsigned char LSHIFT=0x2A;
const unsigned char RSHIFT=0x36;
const unsigned char CAPSLOCK=0x3A;
const unsigned char SPACE=0x39;
const unsigned char ENTER=0x1C;
const unsigned char BACKSPACE=0x0E;

const unsigned char numrowstart=0x02;
const unsigned char numrowend=0x0E;
const unsigned char toprowstart=0x10;
const unsigned char toprowend=0x1B;
const unsigned char midrowstart=0x1E;
const unsigned char midrowend=0x29;
const unsigned char bottomrowstart=0x2B;
const unsigned char bottomrowend=0x35;

#endif