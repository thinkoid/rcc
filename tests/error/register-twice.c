/* Two storage-class specifiers in one declaration, 6.7.2p2. */
int main(void)
{
    register register int x = 1;

    return x;
}
