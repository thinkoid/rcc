/* c ? y : n as a condition: jump code on the arms, rcc's
   conditional_branch(), where an arm is a truth value, and no join;
   otherwise the join's value compared with 0.

   Each function is one shape, its name the corpus's. */

int
cmpheld(void)
{
        int a;
        int b;
        int c;
        if (a < (b ? c : 1))
                return 1;
        return 0;
}

int
commaifcond(void)
{
        int a;
        int b;
        int c;
        if ((a = 1, b) ? c < 1 : c)
                return 1;
        return 0;
}

int
docmp(void)
{
        int a;
        int b;
        int c;
        do
                a = a - 1;
        while (a ? b < c : c);
        return a;
}

int
doval(void)
{
        int a;
        int b;
        int c;
        do
                a = a - 1;
        while (a ? b : c);
        return a;
}

int
forclauses(void)
{
        int a;
        int b;
        int c;
        for (a ? (b = 1) : (c = 1); b < 3; a ? b++ : c++)
                ;
        return b;
}

int
forcmp(void)
{
        int a;
        int b;
        int c;
        for (; a ? b < 3 : c;)
                b = b + 1;
        return b;
}

int
ifand(void)
{
        int a;
        int b;
        int c;
        if ((a ? b < c : c) && b)
                return 1;
        return 0;
}

int
ifbrk(void)
{
        int a;
        int b;
        int c;
        while (a) {
                if (b ? c < 1 : a)
                        break;
                a = a - 1;
        }
        return a;
}

int
ifcmp1(void)
{
        int a;
        int b;
        int c;
        if (a ? b < c : c)
                return 1;
        return 0;
}

int
ifcmp2(void)
{
        int a;
        int b;
        int c;
        if (a ? b : b < c)
                return 1;
        return 0;
}

int
ifcmpboth(void)
{
        int a;
        int b;
        int c;
        if (a ? b < c : b > c)
                return 1;
        return 0;
}

int
ifcondand(void)
{
        int a;
        int b;
        int c;
        if ((a && b) ? c : b)
                return 1;
        return 0;
}

int
ifcondor(void)
{
        int a;
        int b;
        int c;
        if ((a || b) ? c < 1 : c)
                return 1;
        return 0;
}

int
ifconstarms(void)
{
        int a;
        int b;
        int c;
        if (a ? 1 : 0)
                return 1;
        return 0;
}

int
ifelse(void)
{
        int a;
        int b;
        int c;
        if (a ? b < c : c)
                b = 1;
        else
                b = 2;
        return b;
}

int
ifeq0(void)
{
        int a;
        int b;
        int c;
        if ((a ? b : c) == 0)
                return 1;
        return 0;
}

int
ifflag(void)
{
        int a;
        int b;
        int c;
        if (a ? b + c : c - b)
                return 1;
        return 0;
}

int
iflt(void)
{
        int a;
        int b;
        int c;
        if ((a ? b : c) < 5)
                return 1;
        return 0;
}

int
ifnested(void)
{
        int a;
        int b;
        int c;
        if (a ? (b ? c < 1 : c) : b)
                return 1;
        return 0;
}

int
ifnot(void)
{
        int a;
        int b;
        int c;
        if (!(a ? b < c : c))
                return 1;
        return 0;
}

int
ifnotval(void)
{
        int a;
        int b;
        int c;
        if (!(a ? b : c))
                return 1;
        return 0;
}

int
ifor(void)
{
        int a;
        int b;
        int c;
        if ((a ? b < c : c) || b)
                return 1;
        return 0;
}

int
ifplus(void)
{
        int a;
        int b;
        int c;
        if ((a ? b : c) + 1)
                return 1;
        return 0;
}

int
ifval(void)
{
        int a;
        int b;
        int c;
        if (a ? b : c)
                return 1;
        return 0;
}

int
whilecmp(void)
{
        int a;
        int b;
        int c;
        while (a ? b < c : 0)
                a = a - 1;
        return a;
}

int
whileval(void)
{
        int a;
        int b;
        int c;
        while (a ? b : c)
                a = a - 1;
        return a;
}
