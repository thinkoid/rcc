/* A conditional expression whose arms point to different types. */
int main(void)
{
    int a = 1;
    char c = 2;
    int *p = &a;
    char *q = &c;

    return *(1 ? p : q);
}
