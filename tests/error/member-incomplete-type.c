/* A struct that holds an instance of itself, which is incomplete until
   its list ends, 6.7.3.2p3. */
struct node
{
        int value;
        struct node next;
};

int
main(void)
{
        return 0;
}
