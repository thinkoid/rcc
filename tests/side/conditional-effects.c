/* c ? y : n for its effect, rcc's conditional_effect(): each arm for
   its effect, a constant assigned in an arm stored in one instruction,
   and two void arms the same.

   Each function is one shape, its name the corpus's. */
int g(int x);
void v(void);
void w(void);

int
asgeff(void)
{
        int a;
        int b;
        int c;
        a ? (b = 1) : (c = 2);
        return b;
}

int
asgeff0(void)
{
        int a;
        int b;
        int c;
        a ? (b = 0) : (c = 0);
        return b;
}

int
eff(void)
{
        int a;
        int b;
        int c;
        a ? b : c;
        return 0;
}

int
effcall(void)
{
        int a;
        int b;
        int c;
        a ? g(b) : g(c);
        return 0;
}

int
effcmpd(void)
{
        int a;
        int b;
        int c;
        a ? (b += 1) : (c -= 2);
        return b;
}

int
effcomma(void)
{
        int a;
        int b;
        int c;
        a ? (b = 1, c = 2) : (c = 3);
        return c;
}

int
effnest(void)
{
        int a;
        int b;
        int c;
        a ? (b ? (c = 1) : (c = 2)) : (c = 3);
        return c;
}

int
effpar(void)
{
        int a;
        int b;
        int c;
        (a ? b : c);
        return 0;
}

int
effstep(void)
{
        int a;
        int b;
        int c;
        a ? b++ : c--;
        return b;
}

int
effstore(void)
{
        int a;
        int b;
        int c;
        c = a ? b : c;
}

int
voidarms(void)
{
        int a;
        a ? v() : w();
        return 0;
}
