#include <SDL.h>

#include "def.h"
#ifdef _SDL
#include "sdl_vid.h"
#endif

unsigned char2 prev = 0;
extern unsigned long ftime;

void inittimer(void)
{
}

long getlrt(void)
{
	return(0);
}

unsigned long gethrt(void)
{
	char2 diff;

	doscreenupdate();

	/* Speed controlling stuff */
	if (prev == 0) {
		prev = SDL_GetTicks();
	} else {
		diff = (ftime/1000 - (SDL_GetTicks() - prev));
		if (diff > 0) {
			SDL_Delay(diff);
		}
		prev = SDL_GetTicks();
	}
	return(0);
}

long getkips(void)
{
	return(1);
}

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

void s0settimer0(short t0v)
{
}

void s0settimer2(short t0v)
{
}

void s0timer0(short t0v)
{
}

void s0timer2(short t0v)
{
}

void s0soundkillglob(void)
{
}

