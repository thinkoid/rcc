/* Two declarations of one function, of an int parameter and then of an
   unsigned one, which are not compatible, 6.7.1p5: the error is at the
   later declaration's name. */
int g(int x);
int g(unsigned x);
