/* An assignment is no constant expression, 6.6p3: the error is at
   the '('. */
int y;
int x = (y = 1);
