/* A call of a function whose parameter a marking attribute marks: the
   declaration is accepted, and the call, which passes the parameter,
   is not yet.  A dropped attribute on a parameter changes nothing. */
int first(int x __attribute__((__unused__)));

int second(int x __attribute__((__aligned__(8))));

int
main(void)
{
        return first(1) + second(2);
}
