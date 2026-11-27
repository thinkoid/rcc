/* A call is no constant expression, 6.6p3. */
int g(void);
int x = g();
