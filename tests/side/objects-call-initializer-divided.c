/* A call under an operator whose constant is its identity is still
   no constant. */
int g(void);
int x = g() * 1 + 0;
