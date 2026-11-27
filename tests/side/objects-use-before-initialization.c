/* An object used before a later definition with an initializer: the
   label's read, and the object in .data. */
extern int x;
int
f(void)
{
        return x;
}
int x = 5;
