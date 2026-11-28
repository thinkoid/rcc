/* An array of records, whose element no row of an index reaches yet,
   refused at its '[' by the array's element. */
struct s
{
        int a;
};

int
f(void)
{
        struct s a[4];
        return 0;
}
