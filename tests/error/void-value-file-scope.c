/* The result of a void function initializing an object at file scope:
   a void expression has no value, 6.3.2.2p1, and the error is the
   initializer's, said at its first token. */
void g(void);

int x = g();

int
main(void)
{
        return x;
}
