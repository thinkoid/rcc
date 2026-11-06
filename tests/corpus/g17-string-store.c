/* G17: a string literal is a writable object. The store writes 'z'
   into the literal's array, and s[0] read back is 'z': exit status
   122. clang -O2 makes the literal a constant: the load folds to the
   initializer's 'a', 97, and the store into constant memory is
   dropped, so it never reaches the read-only section and nothing
   faults. The fold needs no value from outside; argc is not used. */
int main(int argc)
{
    char *s = "abc";

    s[0] = 'z';

    return s[0];
}
