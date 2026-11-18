/* backend.md 7.2 and aarch64.md 7.2, an if whose body ends in a jump
   and that has an else: the body cannot fall through, so no jump over
   the else part follows it and no label past the else part, as the
   first pass of the Seventh Edition compiler writes it after a bare
   return e.  value's body is a return with a value, broken's a block
   that ends in a break, continued's in a continue, jumped's in a goto,
   and nested's an if whose two parts return.  fall and landed are the
   edge: fall's body is an if with no else, which falls through past
   it, and landed's ends in a label after its return, which a goto
   reaches; each keeps its jump over the else part and the label past
   it.

   On x86-64 the listing must show value's jmp .Lvalue.1 followed at
   once by .Lvalue.2, the else part's label, and no label but these
   two; broken's jmp .Lbroken.3 followed at once by .Lbroken.4,
   continued's jmp .Lcontinued.3 by .Lcontinued.5 and jumped's jmp
   .Ljumped.3 by .Ljumped.2; nested's first two jmp .Lnested.1 followed
   at once by .Lnested.3 and .Lnested.2; and fall's jmp .Lfall.4 and
   landed's jmp .Llanded.4, each over the else part to the label past
   it.

   On aarch64 the listing must show the same, with b for jmp. */
int
value(int c, int x)
{
        if (c)
                return x + 1;
        else
                x = x * 3;
        return x;
}

int
broken(int n)
{
        int s = 0;
        while (n) {
                if (n & 1) {
                        s = s + 1;
                        break;
                }
                else
                        s = s + 2;
                n = n >> 1;
        }
        return s;
}

int
continued(int n)
{
        int s = 0;
        for (int i = 0; i < n; i = i + 1) {
                if (i & 1) {
                        s = s + i;
                        continue;
                }
                else
                        s = s - 1;
                s = s + 1;
        }
        return s;
}

int
jumped(int n)
{
        if (n < 0) {
                n = 0;
                goto done;
        }
        else
                n = n + 1;
        n = n * 2;
done:
        return n;
}

int
nested(int c, int d, int x)
{
        if (c) {
                if (d)
                        return 1;
                else
                        return 2;
        }
        else
                x = 3;
        return x;
}

int
fall(int c, int d, int x)
{
        if (c) {
                if (d)
                        return 1;
        }
        else
                x = 3;
        return x;
}

int
landed(int c, int x)
{
        if (c) {
                if (x > 9)
                        goto out;
                return x;
        out:
        }
        else
                x = 2;
        return x + 1;
}
