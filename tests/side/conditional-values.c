/* c ? y : n as a value, 6.5.16, rcc's conditional(): every held value
   spilled before the paths part, the two labels taken before the
   condition, the condition's jump to the second arm, the first arm, the
   jump past the second, the second arm.  On x86-64 the arms join in the
   register the consumer names, or in the one the first arm's row left
   its value in, and the second arm is handed that register; on aarch64
   each arm is handed the consumer's register alone, where both arms'
   rows are certain to write it, or always at a return's top.  An arm
   that lands elsewhere moves into the join.

   Each function is one shape, its name the corpus's. */
int g(int x);
int k(int x, int y);
int h(int x);

int
andop(void)
{
        int a;
        int b;
        int c;
        return a && (b ? c : 1);
}

int
arg(void)
{
        int a;
        int b;
        int c;
        return g(a ? b : c);
}

int
arg2(void)
{
        int a;
        int b;
        int c;
        return k(a, b ? c : 1);
}

int
argheld(void)
{
        int a;
        int b;
        int c;
        return a + g(b ? c : 1);
}

int
asgboth(void)
{
        int a;
        int b;
        int c;
        return a ? (b = c) : (c = b);
}

int
asgyes(void)
{
        int a;
        int b;
        int c;
        return a ? (b = 1) : c;
}

int
assignval(void)
{
        int a;
        int b;
        int c;
        return c = a ? b : 1;
}

int
bigconst(void)
{
        int a;
        int b;
        int c;
        return a ? 100000 : 1;
}

int
callboth(void)
{
        int a;
        int b;
        int c;
        return a ? g(b) : h(c);
}

int
callcond(void)
{
        int a;
        int b;
        int c;
        return g(a) ? b : c;
}

int
callno(void)
{
        int a;
        int b;
        int c;
        return a ? b : g(c);
}

int
callrem(void)
{
        int a;
        int b;
        int c;
        c = a ? g(b) : b % c;
        return c;
}

int
callstore(void)
{
        int a;
        int b;
        int c;
        c = a ? g(b) : c;
        return c;
}

int
callyes(void)
{
        int a;
        int b;
        int c;
        return a ? g(b) : c;
}

int
cmpboth(void)
{
        int a;
        int b;
        int c;
        return a ? b < c : c > b;
}

int
cmpd(void)
{
        int a;
        int b;
        int c;
        c += a ? b : 1;
        return c;
}

int
cmpdmul(void)
{
        int a;
        int b;
        int c;
        return a ? (b *= 3) : c;
}

int
cmpdval(void)
{
        int a;
        int b;
        int c;
        return c += a ? b : 1;
}

int
cmpdyes(void)
{
        int a;
        int b;
        int c;
        return a ? (b += 2) : c;
}

int
cmpstore(void)
{
        int a;
        int b;
        int c;
        c = a ? b < c : c;
        return c;
}

int
cmpyes(void)
{
        int a;
        int b;
        int c;
        return a ? b < c : c;
}

int
commacond(void)
{
        int a;
        int b;
        int c;
        return (a, b) ? c : 1;
}

int
commamid(void)
{
        int a;
        int b;
        int c;
        return a ? b, c : a;
}

int
commano(void)
{
        int a;
        int b;
        int c;
        return a ? b : (c, a);
}

int
compl(void)
{
        int a;
        int b;
        int c;
        return ~(a ? b : c);
}

int
consts(void)
{
        int a;
        int b;
        int c;
        return a ? 1 : 2;
}

int
consts0(void)
{
        int a;
        int b;
        int c;
        c = a ? 1 : 0;
        return c;
}

int
deep(void)
{
        int a;
        int b;
        int c;
        return a + (b + (c ? a : b));
}

int
deepstore(void)
{
        int a;
        int b;
        int c;
        c = (a + b) * (c ? a : b);
        return c;
}

int
divno(void)
{
        int a;
        int b;
        int c;
        c = a ? b : b / c;
        return c;
}

int
divstore(void)
{
        int a;
        int b;
        int c;
        c = a ? b / c : c;
        return c;
}

int
divyes(void)
{
        int a;
        int b;
        int c;
        return a ? b / c : c;
}

int
fold0(void)
{
        int a;
        int b;
        int c;
        return 0 ? a : b;
}

int
fold11(void)
{
        int a;
        int b;
        int c;
        return 1 ? a : b;
}

int
foldall(void)
{
        int a;
        int b;
        int c;
        return 1 ? 2 : 3;
}

int
foldall0(void)
{
        int a;
        int b;
        int c;
        return 0 ? 2 : 3;
}

int
foldcond(void)
{
        int a;
        int b;
        int c;
        return a ? 2 + 3 : 4;
}

int
foldnest(void)
{
        int a;
        int b;
        int c;
        return (1 ? 2 : 3) ? a : b;
}

int
heldcall(void)
{
        int a;
        int b;
        int c;
        return a + (b ? g(c) : c);
}

int
heldrem(void)
{
        int a;
        int b;
        int c;
        return a + (b ? c % a : c);
}

int
heldremst(void)
{
        int a;
        int b;
        int c;
        c = a + (b ? c % a : c);
        return c;
}

int
heldtwo(void)
{
        int a;
        int b;
        int c;
        return a * b + (c ? a : b);
}

int
ident(void)
{
        int a;
        int b;
        int c;
        return a ? b + 0 : c;
}

int
init(void)
{
        int a;
        int b;
        int c;
        int d = a ? b : c;
        return d;
}

int
leftdiv(void)
{
        int a;
        int b;
        int c;
        return (a ? b : c) / a;
}

int
leftlt(void)
{
        int a;
        int b;
        int c;
        return (a ? b : c) < 5;
}

int
leftlt2(void)
{
        int a;
        int b;
        int c;
        return 5 < (a ? b : c);
}

int
leftop(void)
{
        int a;
        int b;
        int c;
        return (a ? b : c) + 1;
}

int
leftrem(void)
{
        int a;
        int b;
        int c;
        return (a ? b : c) % a;
}

int
logstore(void)
{
        int a;
        int b;
        int c;
        c = a ? b || c : b;
        return c;
}

int
logyes(void)
{
        int a;
        int b;
        int c;
        return a ? b && c : c;
}

int
mulimm(void)
{
        int a;
        int b;
        int c;
        return a ? b * 3 : c;
}

int
neg(void)
{
        int a;
        int b;
        int c;
        return -(a ? b : c);
}

int
negconst(void)
{
        int a;
        int b;
        int c;
        return a ? -1 : 1;
}

int
negyes(void)
{
        int a;
        int b;
        int c;
        return a ? -b : c;
}

int
nestboth(void)
{
        int a;
        int b;
        int c;
        return a ? (b ? a : c) : (c ? b : a);
}

int
nestcond(void)
{
        int a;
        int b;
        int c;
        return (a ? b : c) ? 1 : 2;
}

int
nestcondt(void)
{
        int a;
        int b;
        int c;
        return (a ? b < c : c) ? 1 : 2;
}

int
nestdeep(void)
{
        int a;
        int b;
        int c;
        return a ? b ? c ? 1 : 2 : 3 : 4;
}

int
nestno(void)
{
        int a;
        int b;
        int c;
        return a ? 1 : b ? 2 : 3;
}

int
nestst(void)
{
        int a;
        int b;
        int c;
        c = a ? (b ? 1 : 2) : 3;
        return c;
}

int
nestyes(void)
{
        int a;
        int b;
        int c;
        return a ? (b ? 1 : 2) : 3;
}

int
nm_asg(void)
{
        int a;
        int b;
        int c;
        return c = a ? b % c : c;
}

int
nm_comma(void)
{
        int a;
        int b;
        int c;
        return (b, a ? b % c : c);
}

int
nm_left(void)
{
        int a;
        int b;
        int c;
        return (a ? b % c : c) + 1;
}

int
nm_lt(void)
{
        int a;
        int b;
        int c;
        return (a ? b % c : c) < 5;
}

int
nm_neg(void)
{
        int a;
        int b;
        int c;
        return -(a ? b % c : c);
}

int
nm_not(void)
{
        int a;
        int b;
        int c;
        return !(a ? b % c : c);
}

int
not(void)
{
        int a;
        int b;
        int c;
        return !(a ? b : c);
}

int
notyes(void)
{
        int a;
        int b;
        int c;
        return a ? !b : c;
}

int
postyes(void)
{
        int a;
        int b;
        int c;
        return a ? b++ : c;
}

int
preyes(void)
{
        int a;
        int b;
        int c;
        return a ? ++b : c;
}

int
remboth(void)
{
        int a;
        int b;
        int c;
        return a ? b % c : c % b;
}

int
remcall(void)
{
        int a;
        int b;
        int c;
        c = a ? b % c : g(b);
        return c;
}

int
remcallret(void)
{
        int a;
        int b;
        int c;
        return a ? b % c : g(b);
}

int
remdiv(void)
{
        int a;
        int b;
        int c;
        c = a ? b % c : b / c;
        return c;
}

int
remno(void)
{
        int a;
        int b;
        int c;
        return a ? b : b % c;
}

int
remnostore(void)
{
        int a;
        int b;
        int c;
        c = a ? b : b % c;
        return c;
}

int
remrem(void)
{
        int a;
        int b;
        int c;
        c = a ? b % c : b % a;
        return c;
}

int
remshcount(void)
{
        int a;
        int b;
        int c;
        return a << (b ? c : c % a);
}

int
remyes(void)
{
        int a;
        int b;
        int c;
        return a ? b % c : c;
}

int
remyesstore(void)
{
        int a;
        int b;
        int c;
        c = a ? b % c : c;
        return c;
}

int
ret(void)
{
        int a;
        int b;
        int c;
        return a ? b : c;
}

int
retdivrem(void)
{
        int a;
        int b;
        int c;
        return a ? b / c : c % b;
}

int
retneg(void)
{
        int a;
        int b;
        int c;
        return a ? b : -c;
}

int
rightdiv(void)
{
        int a;
        int b;
        int c;
        return a / (b ? c : 1);
}

int
rightlt(void)
{
        int a;
        int b;
        int c;
        return a < (b ? c : 1);
}

int
rightmul(void)
{
        int a;
        int b;
        int c;
        return a * (b ? c : 1);
}

int
rightnamed(void)
{
        int a;
        int b;
        int c;
        return a - (b ? c % a : c);
}

int
rightop(void)
{
        int a;
        int b;
        int c;
        return a + (b ? c : 1);
}

int
rightrem(void)
{
        int a;
        int b;
        int c;
        return a % (b ? c : 1);
}

int
rightstore(void)
{
        int a;
        int b;
        int c;
        c = a + (b ? c : 1);
        return c;
}

int
rightsub(void)
{
        int a;
        int b;
        int c;
        return a - (b ? c : 1);
}

int
selfcond(void)
{
        int a;
        int b;
        int c;
        return a ? a : b;
}

int
shcallcount(void)
{
        int a;
        int b;
        int c;
        return a << (b ? g(c) : 1);
}

int
shcount(void)
{
        int a;
        int b;
        int c;
        return a << (b ? c : 1);
}

int
shleft(void)
{
        int a;
        int b;
        int c;
        return (a ? b : c) << 2;
}

int
shno(void)
{
        int a;
        int b;
        int c;
        c = a ? b : b >> c;
        return c;
}

int
shremcount(void)
{
        int a;
        int b;
        int c;
        return a << (b ? c % a : 1);
}

int
shstore(void)
{
        int a;
        int b;
        int c;
        c = a ? b << c : c;
        return c;
}

int
shyes(void)
{
        int a;
        int b;
        int c;
        return a ? b << c : c;
}

int
store(void)
{
        int a;
        int b;
        int c;
        c = a ? b : 5;
        return c;
}

int
storediv2(void)
{
        int a;
        int b;
        int c;
        c = a ? b / c : c / b;
        return c;
}

int
sumstore(void)
{
        int a;
        int b;
        int c;
        c = a ? b + c : c - b;
        return c;
}

int
sumyes(void)
{
        int a;
        int b;
        int c;
        return a ? b + c : c - b;
}

int
two(void)
{
        int a;
        int b;
        int c;
        return (a ? b : c) * (b ? a : c);
}

int
twoadd(void)
{
        int a;
        int b;
        int c;
        return (a ? b : c) + (b ? c : a);
}

int
valcmpcmp(void)
{
        int a;
        int b;
        int c;
        return (a ? b < c : c) + 1;
}

int
zeroarm(void)
{
        int a;
        int b;
        int c;
        c = a ? 0 : b;
        return c;
}
