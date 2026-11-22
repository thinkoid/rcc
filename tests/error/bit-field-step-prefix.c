/* A prefix step of a bit-field that does not fill its access unit,
   which rcc does not compile yet: refused at the '--'. */
struct counter
{
        unsigned low : 4, high : 4;
};

int
drop(struct counter *p)
{
        return --p->high;
}

int
main(void)
{
        return 0;
}
