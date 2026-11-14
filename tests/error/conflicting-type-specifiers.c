/* Type specifiers that make no type together, 6.7.3.1p2: int and char
   in one declaration. */
int main(void)
{
    int char x = 1;

    return x;
}
