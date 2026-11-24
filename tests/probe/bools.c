/* The bools probe: _Bool in rcc's C, docs/shape.md chapters 3 and 5.
   Per type of bools.h, its conversion to _Bool as a result, through a
   cast and stored.  Per compound assignment, the _Bool it stores and
   its value; per step, its value and the _Bool it stores.  Then a
   pointer converted, the values already 0 or 1, the arithmetic and the
   conditions on a _Bool, a _Bool copied, members and elements of it, a
   global, calls of the library, its type in a generic selection, and
   its compound assignment and steps through a place with a side
   effect.  See bools.h. */

#include "bools.h"

/* the conversions from T, its letters N */
#define CONVERSION(T, N)          \
        _Bool to_bool_##N(T x)    \
        {                         \
                return x;         \
        }                         \
                                  \
        _Bool cast_bool_##N(T x)  \
        {                         \
                return (_Bool) x; \
        }                         \
                                  \
        int stored_##N(T x)       \
        {                         \
                _Bool b;          \
                                  \
                b = x;            \
                return b;         \
        }

CONVERSIONS(CONVERSION)

/* a compound assignment OP, its name N, on a _Bool holding a and the
   right operand x, as a statement and as a value; a shift's count is x's
   low bits, the assignment's right operand x & 15 */
#define COMPOUND(OP, N)                \
        int stores_##N(_Bool a, int x) \
        {                              \
                _Bool b = a;           \
                                       \
                b OP x;                \
                return b;              \
        }                              \
                                       \
        int yields_##N(_Bool a, int x) \
        {                              \
                _Bool b = a;           \
                                       \
                return b OP x;         \
        }
#define SHIFT(OP, N)                   \
        int stores_##N(_Bool a, int x) \
        {                              \
                _Bool b = a;           \
                                       \
                b OP x & 15;           \
                return b;              \
        }                              \
                                       \
        int yields_##N(_Bool a, int x) \
        {                              \
                _Bool b = a;           \
                                       \
                return b OP x & 15;    \
        }

COMPOUNDS(COMPOUND)
DIVIDING(COMPOUND)
SHIFTS(SHIFT)

/* a step S, its name N, on a _Bool holding a: twice its value and the
   _Bool it stores */
#define STEP(N, S)                    \
        int N(_Bool a)                \
        {                             \
                _Bool b = a;          \
                int value = S;        \
                                      \
                return 2 * value + b; \
        }

STEPS(STEP)

_Bool
from_pointer(char *p)
{
        return p;
}

_Bool
less(int x, int y)
{
        return x < y;
}

_Bool
not(int x)
{
        return !x;
}

_Bool
both(int x, int y)
{
        return x && y;
}

_Bool
either(int x, int y)
{
        return x || y;
}

int
arithmetic(_Bool a, _Bool b)
{
        return a + 2 * b - (a & b) + (a ^ b) * 4;
}

int
condition(_Bool b)
{
        if (b)
                return 3;

        return 4;
}

int
negated(_Bool b)
{
        return !b + 2 * (b == 1) + 4 * (b < 1);
}

_Bool
copied(_Bool b)
{
        _Bool c = b;

        return c;
}

struct flags
{
        _Bool a;
        _Bool b;
        int n;
};

int
members(int x)
{
        struct flags f = { x, !x, x };

        f.a |= f.b;
        f.b++;

        return f.a + 2 * f.b + 4 * (f.n != 0);
}

int
elements(int x)
{
        _Bool v[3] = { x, x > 0, 0 };

        v[2]++;
        --v[1];

        return v[0] + 2 * v[1] + 4 * v[2];
}

_Bool flag;

int
global(int x)
{
        flag = x;
        flag ^= 1;

        return flag;
}

int
calls(int x)
{
        return lib_negate(x) + 2 * lib_identity(lib_negate(x)) +
               4 * lib_identity(x);
}

int
selected(_Bool b)
{
        return _Generic(b, _Bool: 1, unsigned char: 2, default: 0) +
               (int) sizeof b * 4;
}

/* a compound assignment, a prefix step and a postfix step for its
   value, each through a place with a side effect, *p++: p is stepped
   once each, and the _Bool beside stays */
int
through(int x)
{
        _Bool v[4] = { x, !x, x > 0, 1 };
        _Bool *p = v;
        int value;

        *p++ |= x;
        ++*p++;
        value = (*p++)++;

        return v[0] + 2 * v[1] + 4 * v[2] + 8 * v[3] + 16 * value +
               32 * (int) (p - v);
}
