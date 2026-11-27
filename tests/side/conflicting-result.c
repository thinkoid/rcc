/* Two declarations of one function of different result types, which
   are not compatible, 6.7.1p5, 6.7.7.4p14: the error is at the later
   declaration's name. */
int g(void);
void g(void);
