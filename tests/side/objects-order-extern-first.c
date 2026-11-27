/* extern without an initializer makes no entry, so a's entry is its
   definition's, after b's: .data b, then a. */
extern int a;
int b = 1;
int a = 2;
