/* A typedef of function type followed by a body: a function
   definition's storage class is extern or static, 6.9.2p4, so a typedef
   defines no function. */
typedef int
answer(void)
{
        return 42;
}

int
main(void)
{
        return 0;
}
