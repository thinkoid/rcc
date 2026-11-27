/* A division by 0 has no value, 6.5.6p6, and is no constant: the
   error is at the initializer's first token. */
int x = 1 / 0;
