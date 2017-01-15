#include "def.h"

void WriteINIString(char *section,char *key,char *value,char *filename);
void GetINIString(char *section,char *key,char *def,char *dest,int destsize,
                  char *filename);
long GetINIInt(char *section,char *key,long def,char *filename);
void WriteINIInt(char *section,char *key,long value,char *filename);
bool GetINIBool(char *section,char *key,bool def,char *filename);
void WriteINIBool(char *section,char *key,bool value,char *filename);
