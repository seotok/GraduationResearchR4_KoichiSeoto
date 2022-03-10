
#ifndef optck_h
#define optck_h

extern  void optck_entry(Opt* op);
extern  int  optck_title(char* name);
extern  int  optck_key(Opt* op, char* name, int kind);
extern  void optck_delete(Opt* op);

#endif
