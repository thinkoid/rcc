/* A string literal longer than the array of char it initializes, an
   element of an array of arrays: every byte of the literal but the
   terminating zero is an element of the row, 6.7.11p15, and p3
   refuses an initializer for an element the row does not have. */
char names[2][3] = { "Jan", "Febr" };

int
main(void)
{
        return names[0][0];
}
