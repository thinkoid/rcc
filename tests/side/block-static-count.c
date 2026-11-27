/* main counting its calls in a static object. */
int
main(void)
{
        static int count;
        count = count + 1;
        return count;
}
