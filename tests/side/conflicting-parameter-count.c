/* Two declarations of one function of different counts of parameters,
   whose types are not compatible, 6.7.7.4p14: the error is at the
   later declaration's name. */
int g(int a);
int g(int a, int b);
