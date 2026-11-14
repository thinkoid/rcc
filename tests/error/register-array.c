/* An array declared register, whose decay is undefined, 6.3.2.1p3:
   not yet. */
int main(void)
{
    register int a[3];

    return 0;
}
