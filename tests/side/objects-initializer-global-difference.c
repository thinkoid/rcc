/* y - y is not folded, and is no constant: the error is at the
   initializer's first token. */
int y;
int x = y - y;
