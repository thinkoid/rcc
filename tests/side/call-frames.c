/* The frames of functions that call.  On x86-64 a function that calls
   moves rsp below its slots whatever their size, subq $16, %rsp for the
   4 bytes of one argument's slot, so that rsp is a multiple of 16 at
   the call; with no slot at all it writes no subq.  The slots are the
   parameters' and the locals', and 4 bytes for each argument of each
   call.  On aarch64 the frame is the record's 16 bytes and the slots,
   rounded up to 16, as for any function. */
int g(void);
int h(int a);
int h3(int a, int b, int c);
int h4(int a, int b, int c, int d);

int
no_slots(void)
{
        return g();
}

void
void_caller(void)
{
        g();
}

int
parameter(int a)
{
        return g() + a;
}

int
one_argument(int a)
{
        return h(a);
}

int
three_arguments(int a)
{
        return h3(a, a, a);
}

int
four_arguments(int a)
{
        return h4(a, a, a, a);
}

int
six_parameters(int a, int b, int c, int d, int e, int f)
{
        return g();
}

int
six_parameters_locals(int a, int b, int c, int d, int e, int f)
{
        int x, y, z;
        return g();
}

int
nine_parameters(
    int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9)
{
        return g();
}
