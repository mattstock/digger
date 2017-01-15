/* This is gross, but I'm going to put all of my hardware functions here. */


#include "def.h"
#include "hardware.h"
#include <vga.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <keyboard.h>

extern Uint3 *vgatable[];
extern Uint3 *ascii2vga[];
extern void gettitle(unsigned char *buf);

Uint3 **sprites = vgatable;
Uint3 **alphas = ascii2vga;

Sint4 xratio = 2;
Sint4 yratio = 2;
Sint4 yoffset = 0;
Sint4 hratio = 2;
Sint4 wratio = 2;
#define virt2scrx(x) (x*xratio)
#define virt2scry(y) (y*yratio+yoffset)
#define virt2scrw(w) (w*wratio)
#define virt2scrh(h) (h*hratio)

#define KBLEN		30
Sint4 kbuffer[KBLEN];
Sint4 klen=0;

typedef struct _sdl_surface {
  int w;
  int h;
  unsigned char *pixels;
} SDL_Surface;

typedef struct _sdl_color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} SDL_Color;

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
Sint4	currpal=0;

typedef struct _myrect {
  unsigned short int x;
  unsigned short int y;
  unsigned short int w;
  unsigned short int h;
} SDL_Rect;

SDL_Surface *createsurface(Sint4 w, Sint4 h) {
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

SDL_Surface *ch2bmap(Uint3 *sprite, Sint4 w, Sint4 h)
{
  Sint4 realw, realh;
  SDL_Surface *tmp;
  
  realw = virt2scrw(w*4);
  realh = virt2scrh(h);
  tmp = createsurface(realw, realh);
  for (int y=0; y < realh; y++) {
    for (int x=0; x < realw; x++) {
      tmp->pixels[y*realw+x] = sprite[y*realw+x];
    }
  }
  printf("\n");
    
  return tmp;
}

// look for keydown events and store
void keydown()
{
  unsigned int ev;
  
  if (keyboard_count() > 0) {
    ev = keyboard_getevent();
    if ((ev != 0) && ((ev & 0x200) == 0)) {
      if (klen == KBLEN)
	memcpy(kbuffer, kbuffer+1, --klen);
      kbuffer[klen++] = ev & 0xff;
    }
  }
}

void initkeyb(void)
{
}

void restorekeyb(void)
{
}

Sint4 getkey(void)
{
	Sint4 result;
	
	while(kbhit() != TRUE)
		gethrt();
	result = kbuffer[0];
	memcpy(kbuffer, kbuffer + 1, --klen);

	return(result);
}

bool kbhit(void)
{
  keydown();

  if (klen > 0)
    return(TRUE);
  else
    return(FALSE);
}

void inittimer(void)
{
}

Sint5 getlrt(void)
{
	return(0);
}

Uint5 gethrt(void)
{
	return(0);
}

Sint5 getkips(void)
{
	return(1);
}

void vgainit() {
  vga_set_mode(VGA_MODE_NORMAL);
  printf("Digger\n");
}

void vgawrite(Sint4 x, Sint4 y, Sint4 ch, Sint4 c)
{
  SDL_Surface *tmp;
  Sint4 w=3, h=12, size;

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

  vgaputi(x,y, (Uint3 *)&tmp, w, h);
  freesurface(tmp);
}

void vgatitle(void)
{
  SDL_Surface *tmp = NULL;
  
  vgageti(0,0, (Uint3 *)&tmp, 80, 200);
  gettitle(tmp->pixels);
  vgaputi(0,0, (Uint3 *)&tmp, 80, 200);
  freesurface(tmp);
}

Sint4 vgagetpix(Sint4 x, Sint4 y)
{
  return 1;
}

void vgaputim(Sint4 x, Sint4 y, Sint4 ch, Sint4 w, Sint4 h)
{
  SDL_Surface *tmp;
  SDL_Surface *mask;
  SDL_Surface *scr = NULL;
  unsigned char *tmp_pxl, *mask_pxl, *scr_pxl;
  Sint4 realsize;
  Sint4 i;

  tmp = ch2bmap(sprites[ch*2], w, h); // grab the bitmap tile
  mask = ch2bmap(sprites[ch*2+1], w, h); // grab the mask for the bitmap
  vgageti(x,y, (Uint3 *)&scr, w, h); // grab what's ont he screen currently
  realsize = scr->w*scr->h;
  tmp_pxl = (unsigned char *)tmp->pixels;
  mask_pxl = (unsigned char *)mask->pixels;
  scr_pxl = (unsigned char *)scr->pixels;

  // process the mask
  for(i=0;i<realsize;i++)
    if(tmp_pxl[i] != 0xff)
      scr_pxl[i] = (scr_pxl[i] & mask_pxl[i]) | tmp_pxl[i];
  vgaputi(x,y, (Uint3 *)&scr, w, h);
  freesurface(tmp);
  freesurface(mask);
  freesurface(scr);
}

void blitfromfb(SDL_Rect *bb, unsigned char *to) {
  for (int x=0; x < bb->w; x++) {
    for (int y=0; y < bb->h; y++) {
      to[y*bb->w+x] = vga_fb[(bb->y+y)*VGA_MAX_X+bb->x+x];
    }
  }
}

void blittofb(SDL_Rect *bb, unsigned char *from) {
  for (int x=0; x < bb->w; x++) {
    for (int y=0; y < bb->h; y++) {
      vga_fb[(bb->y+y)*VGA_MAX_X+bb->x+x] = from[y*bb->w+x];
    }
  }
}

void vgageti(Sint4 x, Sint4 y, Uint3 *p, Sint4 w, Sint4 h)
{
  SDL_Surface *tmp;
  SDL_Rect src;

  printf("vgageti(%d, %d, %d, %d)\n", x, y, w, h);
  memcpy(&tmp, p, (sizeof(SDL_Surface *)));
  if (tmp != NULL) {
    printf("given a non-NULL reference\n");
    freesurface(tmp);
  }

  src.x = virt2scrx(x);
  src.y = virt2scry(y);
  src.h = virt2scrh(h);
  src.w = virt2scrw(w*4);
  
  tmp = createsurface(src.w, src.h);
  blitfromfb(&src, tmp->pixels);
  memcpy(p, &tmp, (sizeof(SDL_Surface *)));
}

void vgaputi(Sint4 x, Sint4 y, Uint3 *p, Sint4 w, Sint4 h)
{
  SDL_Surface *tmp;
  SDL_Rect src;

  //  printf("vgaputi(%d, %d, %d, %d)\n", x, y, w, h);
  
  src.x = virt2scrx(x);
  src.y = virt2scry(y);
  src.h = virt2scrh(h);
  src.w = virt2scrw(w*4);
  
  memcpy(&tmp, p, (sizeof(SDL_Surface *)));
  blittofb(&src, tmp->pixels);
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

void vgainten(Sint4 inten)
{
  if(inten == 1)
    setpal(ipalettes[currpal]);
  else
    setpal(npalettes[currpal]);
}

void vgapal(Sint4 pal)
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

void s0settimer0(Uint4 t0v)
{
}

void s0settimer2(Uint4 t0v)
{
}

void s0timer0(Uint4 t0v)
{
}

void s0timer2(Uint4 t0v)
{
}

void s0soundkillglob(void)
{
}

bool initsounddevice(void)
{
	return TRUE;
}

bool setsounddevice(int base, int irq, int dma, Uint4 samprate, Uint4 bufsize)
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
void cgawrite(Sint4 x, Sint4 y, Sint4 ch, Sint4 c) {}
void cgaputim(Sint4 x, Sint4 y, Sint4 ch, Sint4 w, Sint4 h) {}
void cgageti(Sint4 x, Sint4 y, Uint3 *p, Sint4 w, Sint4 h) {}
void cgaputi(Sint4 x, Sint4 y, Uint3 *p, Sint4 w, Sint4 h) {}
void cgapal(Sint4 pal) {}
void cgainten(Sint4 inten) {}
Sint4 cgagetpix(Sint4 x, Sint4 y) {return(0);}
void graphicsoff(void) {}

