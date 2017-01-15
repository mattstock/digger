/* Generic sound device header */

typedef unsigned char samp;      /* 8 bit unsigned samples */
#define MIN_SAMP 0
#define MAX_SAMP 0xff

extern samp *buffer;
extern unsigned short firsts,last,size;

bool setsounddevice(int base,int irq,int dma,unsigned short samprate,unsigned short bufsize);
bool initsounddevice(void);
void killsounddevice(void);
