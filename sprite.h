void setretr(bool f);
void movedrawspr(short n,short x,short y);
void erasespr(short n);
void createspr(short n,short ch,unsigned char *mov,short wid,short hei,short bwid,
               short bhei);
void initspr(short n,short ch,short wid,short hei,short bwid,short bhei);
void drawspr(short n,short x,short y);
void initmiscspr(short x,short y,short wid,short hei);
void getis(void);
void drawmiscspr(short x,short y,short ch,short wid,short hei);

extern void (*ginit)(void);
extern void (*gclear)(void);
extern void (*gpal)(short pal);
extern void (*ginten)(short inten);
extern void (*gputi)(short x,short y,unsigned char *p,short w,short h);
extern void (*ggeti)(short x,short y,unsigned char *p,short w,short h);
extern void (*gputim)(short x,short y,short ch,short w,short h);
extern short (*ggetpix)(short x,short y);
extern void (*gtitle)(void);
extern void (*gwrite)(short x,short y,short ch,short c);

extern int first[TYPES],coll[SPRITES];
