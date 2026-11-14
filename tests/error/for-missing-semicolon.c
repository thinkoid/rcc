/* A for whose condition is not followed by ';'. */
int main(void)
{
    int i;

    for (i = 0; i < 3)
        i = i + 1;

    return i;
}
