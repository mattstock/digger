#include <stdlib.h>
#include "def.h"
#include "sprite.h"
#include "hardware.h"

bool retrflag=TRUE;

bool sprrdrwf[SPRITES+1];
bool sprrecf[SPRITES+1];
bool sprenf[SPRITES];
short sprch[SPRITES+1];
unsigned char *sprmov[SPRITES];
short sprx[SPRITES+1];
short spry[SPRITES+1];
short sprwid[SPRITES+1];
short sprhei[SPRITES+1];
short sprbwid[SPRITES];
short sprbhei[SPRITES];
short sprnch[SPRITES];
short sprnwid[SPRITES];
short sprnhei[SPRITES];
short sprnbwid[SPRITES];
short sprnbhei[SPRITES];

void clearrdrwf(void);
void clearrecf(void);
void setrdrwflgs(short n);
bool collide(short bx,short si);
bool bcollide(short bx,short si);
void putims(void);
void putis(void);
void bcollides(int bx);

void (*ginit)(void)=vgainit;
void (*gclear)(void)=vgaclear;
void (*gpal)(short pal)=vgapal;
void (*ginten)(short inten)=vgainten;
void (*gputi)(short x,short y,unsigned char *p,short w,short h)=vgaputi;
void (*ggeti)(short x,short y,unsigned char *p,short w,short h)=vgageti;
void (*gputim)(short x,short y,short ch,short w,short h)=vgaputim;
short (*ggetpix)(short x,short y)=vgagetpix;
void (*gtitle)(void)=vgatitle;
void (*gwrite)(short x,short y,short ch,short c)=vgawrite;

void setretr(bool f)
{
  retrflag=f;
}

void createspr(short n,short ch,unsigned char *mov,short wid,short hei,short bwid,
               short bhei)
{
  sprnch[n]=sprch[n]=ch;
  sprmov[n]=mov;
  sprnwid[n]=sprwid[n]=wid;
  sprnhei[n]=sprhei[n]=hei;
  sprnbwid[n]=sprbwid[n]=bwid;
  sprnbhei[n]=sprbhei[n]=bhei;
  sprenf[n]=FALSE;
}

void movedrawspr(short n,short x,short y)
{
  sprx[n]=x&-4;
  spry[n]=y;
  sprch[n]=sprnch[n];
  sprwid[n]=sprnwid[n];
  sprhei[n]=sprnhei[n];
  sprbwid[n]=sprnbwid[n];
  sprbhei[n]=sprnbhei[n];
  clearrdrwf();
  setrdrwflgs(n);
  putis();
  ggeti(sprx[n],spry[n],sprmov[n],sprwid[n],sprhei[n]);
  sprenf[n]=TRUE;
  sprrdrwf[n]=TRUE;
  putims();
}

void erasespr(short n)
{
  if (!sprenf[n])
    return;
  gputi(sprx[n],spry[n],sprmov[n],sprwid[n],sprhei[n]);
  sprenf[n]=FALSE;
  clearrdrwf();
  setrdrwflgs(n);
  putims();
}

void drawspr(short n,short x,short y)
{
  short t1,t2,t3,t4;
  x&=-4;
  clearrdrwf();
  setrdrwflgs(n);
  t1=sprx[n];
  t2=spry[n];
  t3=sprwid[n];
  t4=sprhei[n];
  sprx[n]=x;
  spry[n]=y;
  sprwid[n]=sprnwid[n];
  sprhei[n]=sprnhei[n];
  clearrecf();
  setrdrwflgs(n);
  sprhei[n]=t4;
  sprwid[n]=t3;
  spry[n]=t2;
  sprx[n]=t1;
  sprrdrwf[n]=TRUE;
  putis();
  sprenf[n]=TRUE;
  sprx[n]=x;
  spry[n]=y;
  sprch[n]=sprnch[n];
  sprwid[n]=sprnwid[n];
  sprhei[n]=sprnhei[n];
  sprbwid[n]=sprnbwid[n];
  sprbhei[n]=sprnbhei[n];
  ggeti(sprx[n],spry[n],sprmov[n],sprwid[n],sprhei[n]);
  putims();
  bcollides(n);
}

void initspr(short n,short ch,short wid,short hei,short bwid,short bhei)
{
  sprnch[n]=ch;
  sprnwid[n]=wid;
  sprnhei[n]=hei;
  sprnbwid[n]=bwid;
  sprnbhei[n]=bhei;
}

void initmiscspr(short x,short y,short wid,short hei)
{
  sprx[SPRITES]=x;
  spry[SPRITES]=y;
  sprwid[SPRITES]=wid;
  sprhei[SPRITES]=hei;
  clearrdrwf();
  setrdrwflgs(SPRITES);
  putis();
}

void getis(void)
{
  short i;
  for (i=0;i<SPRITES;i++)
    if (sprrdrwf[i])
      ggeti(sprx[i],spry[i],sprmov[i],sprwid[i],sprhei[i]);
  putims();
}

void drawmiscspr(short x,short y,short ch,short wid,short hei)
{
  sprx[SPRITES]=x&-4;
  spry[SPRITES]=y;
  sprch[SPRITES]=ch;
  sprwid[SPRITES]=wid;
  sprhei[SPRITES]=hei;
  gputim(sprx[SPRITES],spry[SPRITES],sprch[SPRITES],sprwid[SPRITES],
         sprhei[SPRITES]);
}

void clearrdrwf(void)
{
  short i;
  clearrecf();
  for (i=0;i<SPRITES+1;i++)
    sprrdrwf[i]=FALSE;
}

void clearrecf(void)
{
  short i;
  for (i=0;i<SPRITES+1;i++)
    sprrecf[i]=FALSE;
}

void setrdrwflgs(short n)
{
  short i;
  if (!sprrecf[n]) {
    sprrecf[n]=TRUE;
    for (i=0;i<SPRITES;i++)
      if (sprenf[i] && i!=n) {
        if (collide(i,n)) {
          sprrdrwf[i]=TRUE;
          setrdrwflgs(i);
        }
      }
  }
}

bool collide(short bx,short si)
{
  if (sprx[bx]>=sprx[si]) {
    if (sprx[bx]>(sprwid[si]<<2)+sprx[si]-1)
      return FALSE;
  }
  else
    if (sprx[si]>(sprwid[bx]<<2)+sprx[bx]-1)
      return FALSE;
  if (spry[bx]>=spry[si]) {
    if (spry[bx]<=sprhei[si]+spry[si]-1)
      return TRUE;
    return FALSE;
  }
  if (spry[si]<=sprhei[bx]+spry[bx]-1)
    return TRUE;
  return FALSE;
}

bool bcollide(short bx,short si)
{
  if (sprx[bx]>=sprx[si]) {
    if (sprx[bx]+sprbwid[bx]>(sprwid[si]<<2)+sprx[si]-sprbwid[si]-1)
      return FALSE;
  }
  else
    if (sprx[si]+sprbwid[si]>(sprwid[bx]<<2)+sprx[bx]-sprbwid[bx]-1)
      return FALSE;
  if (spry[bx]>=spry[si]) {
    if (spry[bx]+sprbhei[bx]<=sprhei[si]+spry[si]-sprbhei[si]-1)
      return TRUE;
    return FALSE;
  }
  if (spry[si]+sprbhei[si]<=sprhei[bx]+spry[bx]-sprbhei[bx]-1)
    return TRUE;
  return FALSE;
}

void putims(void)
{
  int i;
  for (i=0;i<SPRITES;i++)
    if (sprrdrwf[i])
      gputim(sprx[i],spry[i],sprch[i],sprwid[i],sprhei[i]);
}

void putis(void)
{
  int i;
  for (i=0;i<SPRITES;i++)
    if (sprrdrwf[i])
      gputi(sprx[i],spry[i],sprmov[i],sprwid[i],sprhei[i]);
}

int first[TYPES],coll[SPRITES];
int firstt[TYPES]={FIRSTBONUS,FIRSTBAG,FIRSTMONSTER,FIRSTFIREBALL,FIRSTDIGGER};
int lastt[TYPES]={LASTBONUS,LASTBAG,LASTMONSTER,LASTFIREBALL,LASTDIGGER};

void bcollides(int spr)
{
  int spc,next,i;
  for (next=0;next<TYPES;next++)
    first[next]=-1;
  for (next=0;next<SPRITES;next++)
    coll[next]=-1;
  for (i=0;i<TYPES;i++) {
    next=-1;
    for (spc=firstt[i];spc<lastt[i];spc++)
      if (sprenf[spc] && spc!=spr)
        if (bcollide(spr,spc)) {
          if (next==-1)
            first[i]=next=spc;
          else
            coll[next=(coll[next]=spc)]=-1;
	}
  }
}
