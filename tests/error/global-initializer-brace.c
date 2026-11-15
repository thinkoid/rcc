/* A list whose initializers are not separated by a comma: the list is
   initializers separated by commas and closed by a brace, 6.7.11p1, so
   the parse wants the '}' where the second stands. */
int table[3] = { 1, 2 3 };

int
main(void)
{
        return table[0];
}
