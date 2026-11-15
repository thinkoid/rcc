/* restrict on a pointer to a function: restrict qualifies a pointer to
   an object type only, 6.7.4.1p2. */
void (*restrict handler)(int);

int
main(void)
{
        return 0;
}
