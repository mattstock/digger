#include "device.h"

void soundinitglob(int port,int irq,int dma,unsigned short bufsize,unsigned short samprate);
void s1setupsound(void);
void s1killsound(void);
void s1fillbuffer(void);
void s1settimer2(unsigned short t2);
void s1soundoff(void);
void s1setspkrt2(void);
void s1settimer0(unsigned short t0);
void s1timer0(unsigned short t0);
void s1timer2(unsigned short t2);

samp getsample(void);
