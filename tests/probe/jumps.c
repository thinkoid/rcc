/* The jumps probe: do, break, continue, goto and labels, in rcc's C, for
   docs/backend.md 7.2 and docs/aarch64.md 7.2.  A do's condition of
   every kind the control probe has, per type; break and continue in
   each loop and in loops in loops; goto forward, back, out of loops,
   into a block and into a loop, past declarations, with register
   variables live on both sides; back into a block whose variable a
   later declaration outlives; a label before a declaration and before
   a closing brace.  See jumps.h. */

#include "jumps.h"

/* a do whose condition is tested after the body; the body counts the
   passes and leaves at the fourth, so a condition that holds gives 4 and
   one that fails gives 1 */
#define COUNTED(name, parameters, condition) \
        int name parameters                  \
        {                                    \
                int n = 0;                   \
                do {                         \
                        if (++n == 4)        \
                                break;       \
                } while (condition);         \
                return n;                    \
        }

/* docs/backend.md 7.1 and 7.2, per type: a do's condition of each kind
   the control probe has, also on the operand of a narrowing cast, whose
   upper bits are stale, and with an operand's side effect */
#define CONDITIONS(T, N)                               \
        COUNTED(dlt_##N, (T a, T b), a < b)            \
        COUNTED(dle_##N, (T a, T b), a <= b)           \
        COUNTED(dgt_##N, (T a, T b), a > b)            \
        COUNTED(dge_##N, (T a, T b), a >= b)           \
        COUNTED(deq_##N, (T a, T b), a == b)           \
        COUNTED(dne_##N, (T a, T b), a != b)           \
        COUNTED(dtruth_##N, (T a), a)                  \
        COUNTED(dnot_##N, (T a), !a)                   \
        COUNTED(dnotlt_##N, (T a, T b), !(a < b))      \
        COUNTED(dand_##N, (T a, T b), (a && b))        \
        COUNTED(dor_##N, (T a, T b), a || b)           \
        COUNTED(dneither_##N, (T a, T b), !(a || b))   \
        COUNTED(dstale_##N, (long a), (T) a)           \
        int dmixed_##N(T a, T b)                       \
        {                                              \
                T x = a;                               \
                int n = 0;                             \
                do {                                   \
                        if (++n == 4)                  \
                                break;                 \
                } while (a < b && (x += b) || a == b); \
                return n + (x != a) * 8;               \
        }

TYPES(CONDITIONS)

/* a do's condition that compares with a constant */
#define KCMP(T, N, name, op, constant) \
        COUNTED(d##N##_##name, (T a), a op constant)
#define KCMP_ALL(T, N) KCMPS(KCMP, T, N)

WIDE(KCMP_ALL)

/* conditions that change in the loop: a count, a value, a comma; a
   constant condition is still a test, and the body runs once.  k bounds
   the passes, and the empty body counts down an unsigned char, which
   wraps, so a jump back on the wrong sense shows in the result and does
   not hang. */
int
count_up(int n)
{
        int i = 0;
        int s = 0;
        int k = 0;

        do {
                if (++k > 100)
                        break;
                s += i;
        } while (++i < n);

        return s * 100 + i;
}

int
digits(unsigned n)
{
        int d = 0;

        do {
                if (d > 100)
                        break;
                n /= 10;
                d++;
        } while (n);

        return d;
}

int
commas(int n)
{
        int i = n;
        int s = 0;

        do {
                if (s > 300)
                        break;
                s += 3;
        } while (i--, i > 0);

        return s + i;
}

int
constants(int a)
{
        int r = 0;

        do {
                if (r > 40)
                        break;
                r += 1;
        } while (0);
        do {
                r += 2;
                if (r > 40)
                        break;
        } while (1);

        return r + a;
}

int
empty_body(int n)
{
        unsigned char c = n;

        do
                ;
        while (--c > 3);

        return c;
}

/* break and continue in each loop: continue in a while jumps to the
   condition, in a do to the condition, in a for to the step; k counts
   the passes, which a continue to the wrong place would change */
int
while_jumps(int n)
{
        int i = 0;
        int s = 0;

        while (i < n) {
                i++;
                if (i % 3 == 0)
                        continue;
                if (i > 20)
                        break;
                s += i;
        }

        return s * 100 + i;
}

int
do_jumps(int n)
{
        int i = 0;
        int s = 0;

        do {
                i++;
                if (i % 3 == 0)
                        continue;
                if (i > 20)
                        break;
                s += i;
        } while (i < n);

        return s * 100 + i;
}

int
for_jumps(int n)
{
        int s = 0;
        int k = 0;
        int i;

        for (i = 0; i < n; i++) {
                k++;
                if (k % 3 == 0)
                        continue;
                if (i > 20)
                        break;
                s += i;
        }

        return s * 1000 + k * 10 + i;
}

/* a for with no condition, left by a break; continue reaches the
   step.  k bounds the passes, so a continue that missed the step would
   show in the result, not hang. */
int
for_ever(int n)
{
        int s = 0;
        int k = 0;

        for (int i = 0;; i += 2) {
                if (++k > 100)
                        break;
                if (i & 4)
                        continue;
                if (i >= n)
                        break;
                s += i;
        }

        return s * 1000 + k;
}

/* loops in loops: the inner loop's break and continue leave or go on
   with the inner loop alone.  The outer continue depends on the passes,
   not on i, so one that missed the step would show, not hang. */
long
nested_jumps(int n, int m)
{
        long s = 0;
        int passes = 0;

        for (int i = 0; i < n; i++) {
                int j = 0;
                int d;

                while (1) {
                        if (j >= m)
                                break;
                        j++;
                        if ((i + j) % 4 == 0)
                                continue;
                        d = 0;
                        do {
                                if (j == 2)
                                        break;
                                s += i * j;
                                if (j & 1)
                                        continue;
                                s += 1;
                        } while (++d == 1);
                        if (i * j > 50)
                                break;
                        s += 1;
                }

                if (++passes % 3 == 0)
                        continue;
                s += j * 1000;
        }

        return s;
}

int
nested_do(int n)
{
        int i = 0;
        int s = 0;
        int k = 0;

        do {
                int j = 0;

                if (++k > 100)
                        break;

                do {
                        j++;
                        if (j == 2)
                                continue;
                        if (j > i)
                                break;
                        s += j;
                } while (j < 5);

                i++;
                if (i == 4)
                        continue;
                s += 100;
        } while (i < n);

        return s;
}

/* goto forward, past statements */
int
skip(int a)
{
        int r = 1;

        if (a > 5)
                goto big;
        r += 10;
big:
        r += a;

        return r;
}

/* a loop built of goto: the label is laid out as written */
int
goto_loop(int n)
{
        int s = 0;
        int i = 0;

top:
        if (i >= n)
                goto out;
        s += i * i;
        i++;
        goto top;
out:
        return s;
}

/* out of loops in loops at once, to a label after a return */
int
find(int n, int target)
{
        int i;
        int j;

        for (i = 0; i < n; i++)
                for (j = 0; j < n; j++)
                        while (1) {
                                if (i * j == target)
                                        goto found;
                                break;
                        }

        return -1;
found:
        return i * 100 + j;
}

/* into a block, past a declaration with an initializer: the object is
   assigned before it is read */
int
into_block(int a)
{
        int r = 0;

        if (a & 1)
                goto inside;
        {
                int t = a * 2;

                r += t;
        inside:
                t = a + 1;
                r += t;
        }

        return r;
}

/* into a loop's body, which then repeats, 6.8.6.1p3 */
int
into_loop(int n)
{
        int i = 5;
        int s = 0;

        goto inside;
        while (i < n) {
                s += 100;
        inside:
                s += i;
                i++;
        }

        return s;
}

/* register variables live on both sides of every jump: out of a block
   that declares them, into one, and back */
int
registers(int n)
{
        register int a = n;
        register int s = 0;

        if (a < 0)
                goto negative;
        {
                register int i = 0;
                register int t;

        next:
                if (i >= a)
                        goto done;
                t = i * 3;
                s = s + t;
                i = i + 1;
                goto next;
        }
negative:
        {
                register int u;

                u = -a;
                s = u * 7;
        }
done:
        return s * 10 + a;
}

int
register_into(int n)
{
        register int a = n;
        register int s = 1;

        if (n & 1)
                goto in;
        s = 2;
        {
                register int b = a * 2;

                s = s + b;
        in:
                b = a + 5;
                s = s + b;
        }

        register int c = s * 3;

        return a + s + c;
}

/* a register variable declared after an inner block, in the enclosing
   one: its object is live from the entry into its block, 6.2.4p6, so
   through the inner block, and the two take two registers, backend.md
   chapter 4.  A goto back into the inner block writes the inner
   variable while the later one is live, and the later one is read
   after.  later_block is the shape itself; later_for has a for that
   declares one in its clause and one in its braced body in place of the
   inner block; later_deep has the inner block two levels down, and a
   declaration after it in the middle block as well; later_loop has the
   inner block and the later declaration in a loop's body, with a
   continue; later_case enters the inner block by a case label of a
   switch whose body declares the later one. */
int
later_block(int n)
{
        register int x = 1;
        int once = 0;

        {
                register int y;

        L:
                y = 3;
                if (once)
                        goto M;
        }
        register int z = 7;

        once = 1;
        if (n)
                goto L;
M:
        return x + z;
}

int
later_for(int n)
{
        register int x = n;
        int once = 0;

        for (register int i = 0; i < 1; i = i + 1) {
                register int t;

        L:
                t = 20;
                i = 30 + t;
                if (once)
                        goto M;
        }
        register int z = 7;

        once = 1;
        if (n)
                goto L;
M:
        return x + z;
}

int
later_deep(int n)
{
        register int x = n;
        int once = 0;

        {
                {
                        register int y;

                L:
                        y = 3;
                        if (once)
                                goto M;
                }
                register int w = 50;

                x = x + w;
        }
        register int z = 7;

        once = 1;
        if (n)
                goto L;
M:
        return x + z;
}

int
later_loop(int n)
{
        register int s = 0;
        int again = 0;

        for (register int i = 0; i < 6; i = i + 1) {
                {
                        register int y;

                back:
                        y = i + 10;
                        s = s + y;
                        if (again) {
                                again = 0;
                                goto forward;
                        }
                        if (i == n)
                                continue;
                }
                register int z = i * 100 + n;

                if (i & 1) {
                        again = 1;
                        goto back;
                }
        forward:
                s = s + z;
        }

        return s;
}

int
later_case(int n)
{
        register int x = n;
        int once = 0;

        switch (n & 3) {
        case 0:
                return x;
                {
                        register int y;

                case 1:
                case 2:
                L:
                        y = 3;
                        if (once)
                                goto M;
                }
                register int z = 7;

                once = 1;
                goto L;
        M:
                return x + z;
        default: return -x;
        }
}

/* a label before a declaration, C23's 6.8.2p4 */
int
label_declaration(int n)
{
        int s = 0;

        if (n > 3)
                goto here;
        s = 100;
here:
        int k = n * 2;
        s += k;

        return s;
}

/* a label before a block's closing brace, in a loop's body and at the
   end of a function */
int
label_end(int n)
{
        int s = 0;

        for (int i = 0; i < n; i++) {
                if (i & 1)
                        goto next;
                s += i;
        next:
        }

        return s;
}

void
clamp(int *p)
{
        if (*p >= 0)
                goto end;
        *p = 0;
end:
}

/* a label and an object of one name, in their two name spaces */
int
names(int n)
{
        int n2 = n;

        goto n2;
n2:
        return n2 + 1;
}
