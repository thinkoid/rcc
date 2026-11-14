/* A call of a function declared weak: the declaration is accepted,
   and the use, whose symbol may be absent at the link, is not yet. */
int hook(void) __attribute__ ((__weak__));

int main(void)
{
    return hook();
}
