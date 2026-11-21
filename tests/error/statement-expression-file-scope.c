/* A statement expression outside a function's body: its statements
   run where it stands, so it stands where statements do, in a body,
   and an initializer at file scope is none, an error as clang has
   it. */
int g = ({ 1; });

int
main(void)
{
        return g;
}
