/* An object's entry is made by its first declaration that defines it,
   a tentative one, so a's is before b's, and a's later initializer
   gives it its value: .data a, 2, then b, 1, and .bss c. */
int a;
int b = 1;
int a = 2;
int c;
int b;
