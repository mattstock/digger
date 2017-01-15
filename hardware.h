void olddelay(short t);
long getkips(void);
void inittimer(void);
unsigned long gethrt(void);
long getlrt(void);

void s0initint8(void);
void s0restoreint8(void);
void s0soundoff(void);
void s0setspkrt2(void);
void s0settimer0(unsigned short t0v);
void s0timer0(unsigned short t0v);
void s0settimer2(unsigned short t2v);
void s0timer2(unsigned short t2v);
void s0soundinitglob(void);
void s0soundkillglob(void);
void s1initint8(void);
void s1restoreint8(void);

void initkeyb(void);
void restorekeyb(void);
short getkey(void);
bool kbhit(void);

void graphicsoff(void);
void gretrace(void);

void cgainit(void);
void cgaclear(void);
void cgapal(short pal);
void cgainten(short inten);
void cgaputi(short x,short y,unsigned char *p,short w,short h);
void cgageti(short x,short y,unsigned char *p,short w,short h);
void cgaputim(short x,short y,short ch,short w,short h);
short cgagetpix(short x,short y);
void cgawrite(short x,short y,short ch,short c);
void cgatitle(void);

void vgainit(void);
void vgaclear(void);
void vgapal(short pal);
void vgainten(short inten);
void vgaputi(short x,short y,unsigned char *p,short w,short h);
void vgageti(short x,short y,unsigned char *p,short w,short h);
void vgaputim(short x,short y,short ch,short w,short h);
short vgagetpix(short x,short y);
void vgawrite(short x,short y,short ch,short c);
void vgatitle(void);
