/* A function declared again with long long for long: one machine type
   on LP64 and two C types, 6.2.5p4, so the declarations of the one
   function do not specify compatible types, 6.7.1p5. */
long f(void);
long long f(void);

int
main(void)
{
        return 0;
}
