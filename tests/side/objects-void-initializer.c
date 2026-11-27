/* A void expression has no value, 6.3.2.2p1: the error is at the
   initializer's first token, before the constant's. */
void g(void);
int x = g();
