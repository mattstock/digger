void outtext(char *p,short x,short y,short c);

void creatembspr(void);
void initmbspr(void);
void drawmon(short n,bool nobf,short dir,short x,short y);
void drawdigger(int n,short t,short x,short y,bool f);
void drawgold(short n,short t,short x,short y);
void drawemerald(short x,short y);
void eraseemerald(short x,short y);
void drawbonus(short x,short y);
void drawlives(void);
void savefield(void);
void makefield(void);
void drawstatics(void);
void drawfire(int n,short x,short y,short t);
void eatfield(short x,short y,short dir);
void drawrightblob(short x,short y);
void drawleftblob(short x,short y);
void drawtopblob(short x,short y);
void drawbottomblob(short x,short y);
void drawmondie(short n,bool nobf,short dir,short x,short y);
void drawfurryblob(short x,short y);
void drawsquareblob(short x,short y);

extern short field[];
extern short fireheight;
