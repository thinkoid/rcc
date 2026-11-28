/* A widening of a signed c ? y : n, rcc's cast() of a conditional: it
   is handed to each arm, converted on its own path, where an arm widens
   free, widens_arms(): a constant, movq $7, %rax; an object in memory,
   its extending load, movslq -8(%rbp), %rax, or a narrow one's under its
   promotion, movsbq; a truth value, valid to 64 bits; and on x86-64 a
   remainder beside a register the consumer names other than rdx, movslq
   %edx, %rax for a return.  Otherwise the join and one extension after
   it, movslq %eax, %rax, sxtw x0, w0. */
int g(int x);
static int gy;
extern char gc;
extern int gn;

long
load_and_sum(void)
{
        int a;
        int y;
        int n;
        return a ? y : n + y;
}

long
sum_and_constant(void)
{
        int a;
        int y;
        int n;
        return a ? y + n : 7;
}

long
sum_and_comparison(void)
{
        int a;
        int y;
        int n;
        return a ? y + n : n < y;
}

long
remainder_and_sum(void)
{
        int a;
        int y;
        int n;
        return a ? y % n : n + y;
}

long
sums(void)
{
        int a;
        int y;
        int n;
        return a ? y + n : n + y;
}

long
step_and_call(void)
{
        int a;
        int y;
        int n;
        return a ? y++ : g(a);
}

long
remainders(void)
{
        int a;
        int y;
        int n;
        return a ? y % n : n % y;
}

int
remainders_stored(void)
{
        int a;
        int y;
        int n;
        long z;
        z = a ? y % n : n % y;
        return 0;
}

int
remainder_and_constant_stored(void)
{
        int a;
        int y;
        int n;
        long z;
        z = a ? y % n : 7;
        return 0;
}

long
narrow_arms(void)
{
        int a;
        char y;
        char n;
        return a ? y : n;
}

long
negated(void)
{
        int a;
        char y;
        char n;
        return a ? -y : -n;
}

long
char_and_unsigned_char(void)
{
        int a;
        char y;
        unsigned char n;
        return a ? y : n;
}

long
loads(void)
{
        int a;
        int y;
        int n;
        return a ? y : n;
}

long
char_and_short(void)
{
        int a;
        char y;
        short n;
        return a ? y : n;
}

unsigned long
cast_to_unsigned_long(void)
{
        int a;
        int y;
        int n;
        return (unsigned long) (a ? y : n);
}

long
narrowed_then_widened(void)
{
        int a;
        int y;
        int n;
        return (char) (a ? y : n);
}

long
remainder_returned(void)
{
        int a;
        int b;
        int c;
        return a ? b % c : c;
}

long
operand(void)
{
        int a;
        long l;
        char c;
        char d;
        return l + (a ? c : d);
}

long
constants(void)
{
        int a;
        return a ? -1 : 2;
}

long
char_constant(void)
{
        int a;
        return a ? (char) -1 : 2;
}

long
unsigned_char_constant(void)
{
        int a;
        return a ? (unsigned char) 255 : -1;
}

long
char_and_constant(void)
{
        int a;
        char c;
        return a ? c : 300;
}

long
unsigned_char_and_minus_one(void)
{
        int a;
        unsigned char c;
        return a ? c : -1;
}

long
folded(void)
{
        return 1 ? (char) -1 : 2;
}

long
identity(void)
{
        int a;
        int b;
        int c;
        return (a ? b : c) + 0;
}

long
nested(void)
{
        int a;
        int b;
        int c;
        int d;
        return a ? (b ? c : d) : 5;
}

long
initialized(void)
{
        int a;
        int b;
        int c;
        long l = a ? b % c : b + c;
        return l;
}

long
objects(void)
{
        int a;
        int n;
        return a ? gy : n + 1;
}

long
objects_declared(void)
{
        int a;
        int n;
        return a ? gc : gn % n;
}

int
objects_stored(void)
{
        int a;
        long z;
        z = a ? gn + 1 : gy + 1;
        return 0;
}
