/* restrict among the declaration specifiers of an int: restrict
   qualifies a pointer to an object type only.  Behind a '*' it is
   dropped. */
int main(void)
{
    int a = 1;
    int *restrict p = &a;
    restrict int x = *p;

    return x;
}
