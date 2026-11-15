/* A function declared inside a function body through a typedef of
   function type, which is not yet. */
typedef int handler(int);

int
main(void)
{
        handler g;

        return 0;
}
