/* The controlling expression's value, rcc's rvalue(), in the register
   its row leaves it in: edx for a remainder, where the direct form's
   index is extended, movl %edx, %edx, and the table's address takes rax,
   and which the hash form moves to eax for its division; eax for a
   call; a truth value valid to 64 bits, which is not extended; an object
   of static storage read by its label or through the GOT; a narrow
   object read by its extending load, and a narrowing, both promoted to
   int. */
int g(int x);
int o;
extern int e;

int
remainder(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (a % b) {
        case 1: c = 1; break;
        case 2: c = 2; break;
        }
        return c;
}

int
remainder_first_zero(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (a % b) {
        case 0: c = 1; break;
        case 1: c = 2; break;
        }
        return c;
}

int
remainder_simple(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (a % b) {
        case 0: return 1;
        case 10: return 2;
        }
        return 0;
}

int
remainder_hashed(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (a % b) {
        case 0: return 1;
        case 10: return 2;
        case 20: return 3;
        case 30: return 4;
        case 40: return 5;
        case 50: return 6;
        case 60: return 7;
        case 70: return 8;
        case 80: return 9;
        case 90: return 10;
        }
        return 0;
}

int
quotient(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (a / b) {
        case 0: c = 1; break;
        case 1: c = 2; break;
        }
        return c;
}

int
sum(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (a + b) {
        case 1: c = 1; break;
        case 2: c = 2; break;
        }
        return c;
}

int
truth_value(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (a && b) {
        case 0: c = 1; break;
        case 1: c = 2; break;
        }
        return c;
}

int
comparison(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (a < b) {
        case 0: c = 1; break;
        case 1: c = 2; break;
        }
        return c;
}

int
assignment(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (b = a) {
        case 0: c = 1; break;
        case 1: c = 2; break;
        }
        return c;
}

int
call(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (g(a)) {
        case 1: b = 1; break;
        case 2: b = 2; break;
        }
        return b;
}

int
call_hashed(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (g(a)) {
        case 0: return 1;
        case 10: return 2;
        case 20: return 3;
        case 30: return 4;
        case 40: return 5;
        case 50: return 6;
        case 60: return 7;
        case 70: return 8;
        case 80: return 9;
        case 90: return 10;
        }
        return 0;
}

int
defined_object(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (o) {
        case 0: b = 1; break;
        case 1: b = 2; break;
        }
        return b;
}

int
declared_object(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (e) {
        case 1: b = 1; break;
        case 2: b = 2; break;
        }
        return b;
}

int
declared_object_simple(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (e) {
        case 1: b = 1; break;
        case 200: b = 2; break;
        }
        return b;
}

int
narrow(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (n) {
        case -1: b = 1; break;
        case 0: b = 2; break;
        case 1: b = 3; break;
        }
        return b;
}

int
narrow_unsigned(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch (u) {
        case 0: b = 1; break;
        case 1: b = 2; break;
        case 255: b = 3;
        }
        return b;
}

int
narrowed(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch ((char) a) {
        case 0: b = 1; break;
        case 1: b = 2; break;
        }
        return b;
}

int
converted_unsigned(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch ((int) w) {
        case 0: b = 1; break;
        case 1: b = 2; break;
        }
        return b;
}

int
converted_long(int a)
{
        int b = 0;
        int c = 0;
        char n = a;
        unsigned char u = a;
        unsigned w = a;
        long l = a;
        switch ((int) l) {
        case 0: b = 1; break;
        case 1: b = 2; break;
        }
        return b;
}
