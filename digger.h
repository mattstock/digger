void dodigger(void);
void erasediggers(void);
void killfire(int n);
void erasebonus(void);
short countem(void);
void makeemfield(void);
void drawemeralds(void);
void initdigger(void);
void drawexplosion(int n);
void updatefire(int n);
void createbonus(void);
short reversedir(short d);
bool hitemerald(short x,short y,short rx,short ry,short dir);
void killdigger(int n,short bp6,short bp8);
bool checkdiggerunderbag(short h,short v);
void killemerald(short bpa,short bpc);
void newframe(void);
int diggerx(int n);
int diggery(int n);
void digresettime(int n);
void sceatm(int n);
bool isalive(void);
bool digalive(int n);
int getlives(int pl);
void addlife(int pl);
void initlives(void);
void declife(int pl);

extern bool bonusvisible,digonscr,bonusmode;
extern unsigned int ftime,curtime,cgtime;

#ifdef INTDRF
extern unsigned int frame;
#endif
