/* An array of unknown size of internal linkage that no declaration
   sizes: a tentative definition of internal linkage is not of an
   incomplete type, 6.9.3p3, where one of external linkage is one
   element at the end of the file, p2. */
static int table[];

int
main(void)
{
        return table[0];
}
