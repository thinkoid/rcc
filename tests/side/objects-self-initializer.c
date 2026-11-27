/* An object is in scope in its own initializer, 6.2.1p7, so x = x
   reads the object, which is no constant expression. */
int x = x;
