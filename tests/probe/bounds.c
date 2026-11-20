/* The bounds probe: each row that reads or writes an object in memory,
   in rcc's C, for G10: an access is at its object's width.  The main
   places the object at a page's end and at a page's start beside an
   unmapped page, so a wider access faults.  Each function takes the
   object's address, p, or a pointer 8 bytes below it, q, the object
   then the member-like *(T *) (q + 8), as a record's member is read,
   and an int y.  See bounds.h. */

#include "bounds.h"

/* a value: name_<type> on *p and member_name_<type> on the member */
#define VALUE(T, N, name, e)         \
        long name##_##N(T *p, int y) \
        {                            \
                return e;            \
        }
#define MEMBER_VALUE(T, N, name, e)              \
        long member_##name##_##N(char *q, int y) \
        {                                        \
                return e;                        \
        }

#define STATEMENT(T, N, name, s)     \
        long name##_##N(T *p, int y) \
        {                            \
                s;                   \
                return 0;            \
        }
#define MEMBER_STATEMENT(T, N, name, s)          \
        long member_##name##_##N(char *q, int y) \
        {                                        \
                s;                               \
                return 0;                        \
        }

#define CONDITION(T, N, name, c)     \
        long name##_##N(T *p, int y) \
        {                            \
                if (c)               \
                        return 1;    \
                return 2;            \
        }
#define MEMBER_CONDITION(T, N, name, c)          \
        long member_##name##_##N(char *q, int y) \
        {                                        \
                if (c)                           \
                        return 1;                \
                return 2;                        \
        }

/* the object as a register variable's operand and its compound
   assignment's right operand, loaded into one, and as a member loaded
   into one */
#define REGISTER(T, N)                           \
        long register_add_##N(T *p, int y)       \
        {                                        \
                register int r = y;              \
                r += *p;                         \
                return r;                        \
        }                                        \
        long register_and_##N(T *p, int y)       \
        {                                        \
                register int r = y;              \
                r &= *p;                         \
                return r;                        \
        }                                        \
        long register_load_##N(T *p, int y)      \
        {                                        \
                register T r;                    \
                r = *p;                          \
                return r + y;                    \
        }                                        \
        long register_member_##N(char *q, int y) \
        {                                        \
                register long r;                 \
                r = *(T *) (q + 8);              \
                return r + y;                    \
        }

#define ROWS(T, N)                                    \
        VALUES(VALUE, T, N, POINTED(T))               \
        VALUES(MEMBER_VALUE, T, N, MEMBER(T))         \
        DIVIDED(VALUE, T, N, POINTED(T))              \
        DIVIDED(MEMBER_VALUE, T, N, MEMBER(T))        \
        STATEMENTS(STATEMENT, T, N, POINTED(T))       \
        STATEMENTS(MEMBER_STATEMENT, T, N, MEMBER(T)) \
        CONDITIONS(CONDITION, T, N, POINTED(T))       \
        CONDITIONS(MEMBER_CONDITION, T, N, MEMBER(T)) \
        REGISTER(T, N)

TYPES(ROWS)
