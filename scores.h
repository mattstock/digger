void loadscores(void);
void showtable(void);
void zeroscores(void);
void writecurscore(int col);
void drawscores(void);
void initscores(void);
void endofgame(void);
void scorekill(int n);
void scorekill2(void);
void scoreemerald(int n);
void scoreoctave(int n);
void scoregold(int n);
void scorebonus(int n);
void scoreeatm(int n,int msc);
void addscore(int n,short score);

#ifdef INTDRF
long getscore0(void);
#endif

extern unsigned short bonusscore;
extern unsigned int scoret;

extern char scoreinit[11][4];
