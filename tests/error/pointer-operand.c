/* A pointer as an operand of *, which takes integers only. */
int main(void)
{
    int a = 1;
    int *p = &a;

    return p * 2;
}
