/* Bodies that no return ends: an empty body is reached with the
   result register as it is, G5's some value, but main's, which returns
   0 when control reaches its closing brace, 5.1.2.3.4p1. */
int
empty(void)
{
}

int
main(void)
{
}
