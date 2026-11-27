/* A name declared later in the file is not in scope in an earlier
   initializer, 6.2.1p7. */
int x = y;
int y = 1;
