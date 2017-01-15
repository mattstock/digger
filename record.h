void openplay(char *name);
void recstart(void);
void recname(char *name);
void playgetdir(short *dir,bool *fire);
void recinit(void);
void recputrand(unsigned int randv);
unsigned int playgetrand(void);
void recputinit(char *init);
void recputeol(void);
void recputeog(void);
void playskipeol(void);
void recputdir(short dir,bool fire);
void recsavedrf(void);

extern bool playing,savedrf,gotname,gotgame,drfvalid,kludge;
