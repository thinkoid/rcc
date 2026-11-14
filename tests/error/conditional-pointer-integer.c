/* A conditional expression with a pointer arm and an integer arm other
   than a null pointer constant, 6.5.16p3. */
int main(void)
{
    int a = 1;
    int *p = &a;

    return *(1 ? p : 2);
}
