/* An attribute list closed by one parenthesis instead of two. */
int f(int *p) __attribute__ ((nonnull));
int g(int *p) __attribute__ ((nonnull);

int main(void)
{
    return 0;
}
