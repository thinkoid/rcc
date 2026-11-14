/* A pointer qualified volatile after its '*', which is not yet. */
int main(void)
{
    int a = 1;
    int *volatile p = &a;

    return *p;
}
