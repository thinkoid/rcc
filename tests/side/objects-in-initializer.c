/* An object in another object's initializer is its load, which is no
   constant expression, 6.6p7: the error is at the initializer's first
   token. */
int y = 1;
int x = y;
