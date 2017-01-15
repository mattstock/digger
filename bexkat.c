/* This is gross, but I'm going to put all of my hardware functions here. */


#include "def.h"
#include "hardware.h"
#include <vga.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <keyboard.h>
#include <timers.h>

extern unsigned char *vgatable[];
extern unsigned char *ascii2vga[];
extern unsigned int ftime;
unsigned int prev;

void doscreenupdate();

extern void gettitle(unsigned char *buf);

unsigned char **sprites = vgatable;
unsigned char **alphas = ascii2vga;

short xratio = 2;
short yratio = 2;
short yoffset = 0;
short hratio = 2;
short wratio = 2;
#define virt2scrx(x) (x*xratio)
#define virt2scry(y) (y*yratio+yoffset)
#define virt2scrw(w) (w*wratio)
#define virt2scrh(h) (h*hratio)

#define KBLEN		30
unsigned short kbuffer[KBLEN];
short klen=0;

typedef struct _sdl_surface {
  int w;
  int h;
  unsigned char *pixels;
} SDL_Surface;

typedef struct _myrect {
  unsigned short int x;
  unsigned short int y;
  unsigned short int w;
  unsigned short int h;
} SDL_Rect;

typedef struct _transactions {
  struct _transactions *p;
  struct _transactions *n;
  SDL_Rect rect;
} Transactions;

typedef struct _sdl_color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} SDL_Color;

// master copy of the screen buffer.  we track the modified rectangles on
// this and periodically dump the result to the frame buffer.
SDL_Surface *screen = NULL;
Transactions *First = NULL, *Last = NULL;
unsigned int pendnum = 0;

/* palette1, normal intensity */
SDL_Color vga16_pal1[] = \
{{0,0,0,0},{0,0,128,0},{0,128,0,0},{0,128,128,0},{128,0,0,0},{128,0,128,0} \
,{128,64,0,0},{128,128,128,0},{64,64,64,0},{0,0,255,0},{0,255,0,0} \
,{0,255,255,0},{255,0,0,0},{255,0,255,0},{255,255,0,0},{255,255,255,0}};
/* palette1, high intensity */
SDL_Color vga16_pal1i[] = \
{{0,0,0,0},{0,0,255,0},{0,255,0,0},{0,255,255,0},{255,0,0,0},{255,0,255,0} \
,{255,128,0,0},{196,196,196,0},{128,128,128,0},{128,128,255,0},{128,255,128,0} \
,{128,255,255,0},{255,128,128,0},{255,128,255,0},{255,255,128,0},{255,255,255,0}};
/* palette2, normal intensity */
SDL_Color vga16_pal2[] = \
{{0,0,0,0},{0,128,0,0},{128,0,0,0},{128,64,0,0},{0,0,128,0},{0,128,128,0} \
,{128,0,128,0},{128,128,128,0},{64,64,64,0},{0,255,0,0},{255,0,0,0} \
,{255,255,0,0},{0,0,255,0},{0,255,255,0},{255,0,255,0},{255,255,255,0}};
/* palette2, high intensity */
SDL_Color vga16_pal2i[] = \
{{0,0,0,0},{0,255,0,0},{255,0,0,0},{255,128,0,0},{0,0,255,0},{0,255,255,0} \
,{255,0,255,0},{196,196,196,0},{128,128,128,0},{128,255,128,0},{255,128,128,0} \
,{255,255,128,0},{128,128,255,0},{128,255,255,0},{255,128,255,0},{255,255,255,0}};

SDL_Color *npalettes[] = {vga16_pal1, vga16_pal2};
SDL_Color *ipalettes[] = {vga16_pal1i, vga16_pal2i};
short	currpal=0;

SDL_Surface *createsurface(short w, short h) {
  SDL_Surface *tmp;

  //  printf("createsurface(%d, %d)\n", w, h);
  
  if (w <= 0 || h <= 0)
    return NULL;

  tmp = (SDL_Surface *) malloc(sizeof(SDL_Surface));
  tmp->h = h;
  tmp->w = w;
  tmp->pixels = malloc(h*w);
  return tmp;
}

void freesurface(SDL_Surface *s) {
  free(s->pixels);
  free(s);
}

SDL_Surface *ch2bmap(unsigned char *sprite, short w, short h)
{
  short realw, realh;
  SDL_Surface *tmp;
  
  realw = virt2scrw(w*4);
  realh = virt2scrh(h);
  tmp = createsurface(realw, realh);
  memcpy(tmp->pixels, sprite, realh*realw);
      
  return tmp;
}

// this code assumes that most of the keyboard keys are
// returned as ascii, which means I need to build a translation
// from ps2 scan codes using a lut.  Since the elements I want aren't
// contiguous, this list is bigger than it needs to be, but I'm aiming for
// simplicity.
short remap[] =
  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x09, 0x60, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 'Q' , '1' , 0x17,
    0x18, 0x19, 'Z' , 'S' , 'A' , 'D' , '2' , 0x1f,
    0x20, 'C' , 'X' , 'D' , 'E' , '4' , '3' , 0x27,
    0x28, ' ' , 'V' , 'F' , 'T' , 'R' , '5' , 0x2f,
    0x30, 'N' , 'B' , 'H' , 'G' , 'Y' , '6' , 0x37,
    0x38, 0x39, 'M' , 'J' , 'U' , '7' , '8' , 0x3f,
    0x40, 0x41, 'K' , 'I' , 'O' , '0' , '9' , 0x47,
    0x48, 0x49, 0x4a, 'L' , 0x4c, 'P' , '-' , 0x4f };
    
// look for keydown events and store
void keydown()
{
  unsigned short ev;

  if (keyboard_count() > 0) {
    ev = keyboard_getevent();
    if ((ev != 0) && ((ev & 0x200) == 0)) {
      if (klen == KBLEN)
	memcpy(kbuffer, kbuffer+1, --klen);
      if (ev < 80) {
	kbuffer[klen++] = remap[ev];
      } else
	kbuffer[klen++] = ev;
    }
  }
}

void initkeyb(void)
{
}

void restorekeyb(void)
{
}

unsigned short getkey(void)
{
  unsigned short result;
  
  while(kbhit() != TRUE)
    gethrt();
  result = kbuffer[0];
  memcpy(kbuffer, kbuffer + 1, --klen);
  // printf("getkey() = %04x\n", result);
  return result;
}

bool kbhit(void)
{
  keydown();
  doscreenupdate();
  
  if (klen > 0)
    return(TRUE);
  else
    return(FALSE);
}

void inittimer(void)
{
}

int getlrt(void)
{
	return(0);
}

unsigned int gethrt(void)
{
  int delta;
  unsigned int tt = timers_ticks();

  doscreenupdate();
  if (prev == 0 || prev > tt) {
    prev = tt;
  } else {
    delta = (ftime/1000 - (tt - prev));
    if (delta > 0) {
      // this is a gross spinwait, but it should work
      while (tt < prev+delta && tt > prev) {
	tt = timers_ticks();
      };
    }
    prev = timers_ticks();
  }
  return(0);
}

int getkips(void)
{
	return(1);
}

void vgainit() {
  vga_set_mode(VGA_MODE_NORMAL);
  screen = createsurface(640,400);
  printf("Digger\n");
}

void vgawrite(short x, short y, short ch, short c)
{
  SDL_Surface *tmp;
  short w=3, h=12, size;

  if (((ch-32) >= 0x5f) || (ch < 32))
    return;

  tmp = ch2bmap(alphas[ch-32], w, h);
  size = tmp->w*tmp->h;

  for (int i = size; i != 0;) {
    i--;
    if (tmp->pixels[i] == 10) {
      if (c==2)
	tmp->pixels[i] = 12;
      if (c==3)
	tmp->pixels[i] = 14;
    } else
      if (tmp->pixels[i] == 12) {
	if (c==1)
	  tmp->pixels[i] = 2;
	if (c==2)
	  tmp->pixels[i] = 4;
	if (c==3)
	  tmp->pixels[i] = 6;
      }
  }

  vgaputi(x,y, (unsigned char *)&tmp, w, h);
  freesurface(tmp);
}

void vgatitle(void)
{
  SDL_Surface *tmp = NULL;
  
  vgageti(0,0, (unsigned char *)&tmp, 80, 200);
  gettitle(tmp->pixels);
  vgaputi(0,0, (unsigned char *)&tmp, 80, 200);
  freesurface(tmp);
}

short vgagetpix(short x, short y)
{
  return 1;
}

void vgaputim(short x, short y, short ch, short w, short h)
{
  SDL_Surface *tmp;
  SDL_Surface *mask;
  SDL_Surface *scr = NULL;
  unsigned char *tmp_pxl, *mask_pxl, *scr_pxl;
  short realsize;
  short i;

  tmp = ch2bmap(sprites[ch*2], w, h); // grab the bitmap tile
  mask = ch2bmap(sprites[ch*2+1], w, h); // grab the mask for the bitmap
  vgageti(x,y, (unsigned char *)&scr, w, h); // grab what's ont he screen currently
  realsize = scr->w*scr->h;
  tmp_pxl = (unsigned char *)tmp->pixels;
  mask_pxl = (unsigned char *)mask->pixels;
  scr_pxl = (unsigned char *)scr->pixels;

  // process the mask
  for(i=0;i<realsize;i++)
    if(tmp_pxl[i] != 0xff)
      scr_pxl[i] = (scr_pxl[i] & mask_pxl[i]) | tmp_pxl[i];
  vgaputi(x,y, (unsigned char *)&scr, w, h);
  freesurface(tmp);
  freesurface(mask);
  freesurface(scr);
}

void fbwrite(unsigned short x, unsigned short y, unsigned short w, unsigned short h) {
  for (int a=0; a < w; a++)
    for (int b=0; b < h; b++)
      vga_fb[(y+b)*VGA_MAX_X+x+a] = screen->pixels[(y+b)*VGA_MAX_X+x+a];
}

void blitfromscreen(SDL_Rect *bb, unsigned char *to) {
  for (int x=0; x < bb->w; x++) {
    for (int y=0; y < bb->h; y++) {
      to[y*bb->w+x] = screen->pixels[(bb->y+y)*VGA_MAX_X+bb->x+x];
    }
  }
}

void blittoscreen(SDL_Rect *bb, unsigned char *from) {
  for (int x=0; x < bb->w; x++) {
    for (int y=0; y < bb->h; y++) {
      screen->pixels[(bb->y+y)*VGA_MAX_X+bb->x+x] = from[y*bb->w+x];
    }
  }
}

void doscreenupdate() {
  Transactions *node;

  for (node=First; node !=NULL;) {
    fbwrite(node->rect.x, node->rect.y, node->rect.w, node->rect.h);
    First = node->n;
    free(node);
    node = First;
  }
  pendnum = 0;
}

void vgageti(short x, short y, unsigned char *p, short w, short h)
{
  SDL_Surface *tmp;
  SDL_Rect src;

  //  printf("vgageti(%d, %d, %d, %d)\n", x, y, w, h);
  memcpy(&tmp, p, (sizeof(SDL_Surface *)));
  if (tmp != NULL) {
    //    printf("given a non-NULL reference\n");
    freesurface(tmp);
  }

  src.x = virt2scrx(x);
  src.y = virt2scry(y);
  src.h = virt2scrh(h);
  src.w = virt2scrw(w*4);
  
  tmp = createsurface(src.w, src.h);
  blitfromscreen(&src, tmp->pixels);
  memcpy(p, &tmp, (sizeof(SDL_Surface *)));
}

void vgaputi(short x, short y, unsigned char *p, short w, short h)
{
  SDL_Surface *tmp;
  Transactions *new, *ptr;

  //  printf("vgaputi(%d, %d, %d, %d)\n", x, y, w, h);
  new = malloc(sizeof(Transactions));
  new->rect.x = virt2scrx(x);
  new->rect.y = virt2scry(y);
  new->rect.w = virt2scrw(w*4);
  new->rect.h = virt2scrh(h);
  new->n = NULL;
  new->p = NULL;
  
  memcpy(&tmp, p, (sizeof(SDL_Surface *)));
  blittoscreen(&(new->rect), tmp->pixels);

  for (ptr=First;ptr!=NULL;ptr=ptr->n) {
    if ((new->rect.x >= ptr->rect.x) &&
	(new->rect.y >= ptr->rect.y) &&
	((new->rect.x+new->rect.w) <= (ptr->rect.x+ptr->rect.w)) &&
	((new->rect.y+new->rect.h) <= (ptr->rect.y+ptr->rect.h))) {
      free(new);
      return;
    } else if ((new->rect.x <= ptr->rect.x) &&
	       (new->rect.y <= ptr->rect.y) &&
	       ((new->rect.x+new->rect.w) >= (ptr->rect.x+ptr->rect.w)) &&
	       ((new->rect.y+new->rect.h) >= (ptr->rect.y+ptr->rect.h))) {
      ptr->rect.x = new->rect.x;
      ptr->rect.y = new->rect.y;
      ptr->rect.w = new->rect.w;
      ptr->rect.h = new->rect.h;
      free(new);
      return;
    }
  }
  
  if (pendnum == 0)
    First = new;
  else {
    Last->n = new;
    new->p = Last;
  }

  Last = new;
  pendnum++;
}

void vgaclear(void)
{
  for (int a=0; a < VGA_MAX_X*VGA_MAX_Y; a++)
    vga_fb[a] = 0;
}

void setpal(SDL_Color *pal)
{
  for (int i=0; i < 16; i++) {
    unsigned int c = (pal[i].r << 16) | (pal[i].g << 8) | pal[i].b;
    vga_palette(0, i, c);
  }
  vga_palette(0, 16, 0xffffff);
  
}

void vgainten(short inten)
{
  if(inten == 1)
    setpal(ipalettes[currpal]);
  else
    setpal(npalettes[currpal]);
}

void vgapal(short pal)
{
  setpal(npalettes[pal]);
  currpal = pal;
}

/* Audio if we get there */
void s0initint8(void)
{
}

void s0restoreint8(void)
{
}

void s1initint8(void)
{
}

void s1restoreint8(void)
{
}

void s0soundoff(void)
{
}

void s0setspkrt2(void)
{
}

void s0settimer0(unsigned short t0v)
{
}

void s0settimer2(unsigned short t0v)
{
}

void s0timer0(unsigned short t0v)
{
}

void s0timer2(unsigned short t0v)
{
}

void s0soundkillglob(void)
{
}

bool initsounddevice(void)
{
	return TRUE;
}

bool setsounddevice(int base, int irq, int dma, unsigned short samprate, unsigned short bufsize)
{
  return TRUE;
}

void fill_audio(void *udata, unsigned char *stream, int len)
{
}

void killsounddevice(void)
{
}

/* Junk we don't care about */
void gretrace(void) {}
void cgainit(void) {}
void cgaclear(void) {}
void cgatitle(void) {}
void cgawrite(short x, short y, short ch, short c) {}
void cgaputim(short x, short y, short ch, short w, short h) {}
void cgageti(short x, short y, unsigned char *p, short w, short h) {}
void cgaputi(short x, short y, unsigned char *p, short w, short h) {}
void cgapal(short pal) {}
void cgainten(short inten) {}
short cgagetpix(short x, short y) {return(0);}
void graphicsoff(void) {}

