/* Pointers to different types ordered against each other. */
int main(void)
{
    int a = 1;
    char c = 2;
    int *p = &a;
    char *q = &c;

    return p < q;
}
