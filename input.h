void detectjoy(void);
bool teststart(void);
void readdir(int n);
short getdir(int n);
void checkkeyb(void);
void flushkeybuf(void);
void findkey(int kn);
void clearfire(int n);

extern bool firepflag,fire2pflag,escape;
extern char keypressed;
extern short akeypressed;

extern int keycodes[17][5];
extern bool krdf[17];
extern bool pausef;
