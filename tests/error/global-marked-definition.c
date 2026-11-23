/* The definition of an object an attribute marks on its declaration:
   rcc would have to honour the attribute, which is unsupported. */
extern int count __attribute__((__weak__));
int count = 1;

int
main(void)
{
        return count;
}
