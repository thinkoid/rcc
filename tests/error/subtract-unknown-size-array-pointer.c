/* The difference of a pointer to an array of three ints and a pointer
   to an array of unknown size: the pointees are compatible, 6.7.7.3p6,
   and the second is incomplete, which 6.5.7p3 does not take. */
int (*rows)[3];
int (*any)[];

int
main(void)
{
        return rows - any;
}
