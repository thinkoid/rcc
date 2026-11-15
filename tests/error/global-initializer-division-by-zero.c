/* A division by zero has no value, 6.5.6p6, so the evaluator leaves it
   unfolded and the initializer is no constant expression,
   6.7.11p5. */
int count = 1 / 0;

int
main(void)
{
        return count;
}
