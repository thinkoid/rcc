/* A widening reached through a comma, an assignment, a switch's index
   or a conditional arm, of a signed c ? y : n whose second arm, a truth
   value, lands where the first arm's remainder left the join, and of an
   unsigned one whose call's result a mov brought there: the width the
   join records is the widening's, as rcc's held value is. */
int g(int x);

long
comma_stored(void)
{
        int a;
        int b;
        int c;
        int d;
        int e;
        long l;
        l = (b, a ? b % c : d < e);
        return l;
}

long
comma_returned(void)
{
        int a;
        int b;
        int c;
        int d;
        int e;
        return (b, a ? b % c : d < e);
}

long
assignment(void)
{
        int a;
        int b;
        int c;
        int d;
        int e;
        int u;
        long l;
        l = (u = a ? b % c : d < e);
        return l;
}

long
stored(void)
{
        int a;
        int b;
        int c;
        int d;
        int e;
        long l;
        l = a ? b % c : d < e;
        return l;
}

int
switch_index(void)
{
        int a;
        int b;
        int c;
        int d;
        int e;
        switch (a ? b % c : d < e) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        }
        return 0;
}

long
unsigned_comma_returned(void)
{
        int a;
        unsigned y;
        unsigned n;
        return (a, a ? y % n : g(a));
}

long
nested_unsigned(void)
{
        int a;
        unsigned y;
        unsigned n;
        long l;
        l = (a ? (a ? y % n : g(a)) : y);
        return l;
}

long
comma_nested(void)
{
        int a;
        int b;
        int c;
        long l;
        l = (b, a ? (c ? b % c : g(a)) : 5);
        return l;
}
