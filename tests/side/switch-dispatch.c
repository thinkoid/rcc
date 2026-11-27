/* The rows of each dispatch, backend.md 7.3, on a value in eax and w0:
   the direct form's subtraction of its first value where it is not 0, an
   immediate or held where aarch64's sub does not encode it, the bounds
   check, unsigned, and the table's jump, which on x86-64 extends an index
   of a first value of 0, movl %eax, %eax; the simple form's tests, in the
   order of the values, cbz for 0 and a held constant where aarch64's cmp
   does not encode it.  A case's constant is folded and converted to the
   promoted type, int. */
int
first_one(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: b = 1; break;
        case 2: b = 2; break;
        case 3: b = 3; break;
        }
        return b;
}

int
first_one_defaulted(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: b = 1; break;
        case 2: b = 2; break;
        case 3: b = 3; break;
        default: b = 4;
        }
        return b;
}

int
first_zero(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: b = 1; break;
        case 1: b = 2; break;
        case 2: b = 3; break;
        }
        return b;
}

int
first_negative(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case -1: b = 1; break;
        case 0: b = 2; break;
        case 1: b = 3; break;
        }
        return b;
}

int
first_least(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case -2147483647 - 1: b = 1; break;
        case -2147483647: b = 2; break;
        }
        return b;
}

int
first_greatest(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 2147483646: b = 1; break;
        case 2147483647: b = 2; break;
        }
        return b;
}

int
first_4096(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 4096: b = 1; break;
        case 4097: b = 2; break;
        }
        return b;
}

int
first_4097(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 4097: b = 1; break;
        case 4098: b = 2; break;
        }
        return b;
}

int
first_wide(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 100000: b = 1; break;
        case 100001: b = 2; break;
        }
        return b;
}

int
unsorted(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 3: b = 3; break;
        case 1: b = 1; break;
        case 2: b = 2; break;
        }
        return b;
}

int
gap(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 5;
        default: return 0;
        }
}

int
spread(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: b = 1; break;
        case 2000: b = 2; break;
        case 4000: b = 3; break;
        }
        return b;
}

int
simple_unsorted(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 10: b = 1; break;
        case 0: b = 2; break;
        case -1: b = 3; break;
        case 100: b = 4; break;
        default: b = 5;
        }
        return b;
}

int
simple_not_encoded(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: b = 1; break;
        case 5000: b = 2; break;
        case -5000: b = 3; break;
        }
        return b;
}

int
simple_least(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case -2147483647 - 1: b = 1; break;
        case 2147483647: b = 2; break;
        }
        return b;
}

int
constant_expressions(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1 + 2: b = 1; break;
        case 2 * 3: b = 2; break;
        case -1: b = 3; break;
        case 'a': b = 4; break;
        case (char) 300: b = 5;
        }
        return b;
}

int
converted(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0x100000001: b = 1; break;
        case 4294967295u: b = 2; break;
        case 0x80000000: b = 3;
        }
        return b;
}

int
constant_controlled(int a)
{
        int b = 0;
        int c = 0;
        switch (1) {
        case 1: b = 1; break;
        case 2: b = 2; break;
        }
        return b;
}
