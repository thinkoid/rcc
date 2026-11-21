/* A function declared with a pointer to const char and then with a
   pointer to char: the parameters are not compatible, 6.7.4.1p11, so
   neither are the function types, 6.7.7.4p14. */
int length(const char *);
int length(char *);

int
main(void)
{
        return 0;
}
