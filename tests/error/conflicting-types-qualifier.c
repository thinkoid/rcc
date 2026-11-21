/* An object declared const and then without the qualifier: two
   qualified versions of a type are compatible only when identically
   qualified, 6.7.4.1p11, so the declarations of the one object do not
   specify compatible types, 6.7.1p5. */
extern const int limit;
int limit;

int
main(void)
{
        return 0;
}
