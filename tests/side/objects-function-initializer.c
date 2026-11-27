/* A function designator converts to a pointer, which converts to no
   integer by assignment, 6.5.17.2p1: the error is at the '='. */
int g(void);
int x = g;
