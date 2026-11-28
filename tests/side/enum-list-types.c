/* While its list is read each constant has the type of its value,
   6.7.3.3p12: after = an int where int holds it, else the
   expression's; without = the type of the one before, or the 64-bit
   type of its sign where that one does not hold the value, so B is a
   long and C is 8.  Once the list is complete the constants are of the
   enumerated type where int does not hold every value, p15: A - 1 is
   then unsigned, and sizeof A is 4. */
enum e { A = 0x7fffffff, B, C = sizeof B };
enum f { D, E = 0x80000000 };

int
list(void)
{
        return C;
}

int
completed(void)
{
        return D - 1 < 0;
}

unsigned long
sizes(void)
{
        return sizeof A + sizeof B + sizeof D;
}
