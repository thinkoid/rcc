/* The lists the copies probe is generated from.  copies.c makes the
   functions of each record, compiled by rcc and, as the reference, by
   clang -O0 under the prefix ref_; copies-main.c calls both and
   compares the bytes each leaves.  A record's assignment copies the
   right operand's object into the left's, 6.5.17.2p2, padding
   included, which the copy rule takes as bytes like any other: every
   byte of the object is compared, and the bytes around it, which no
   copy writes.  The records are defined here, so the main, rcc's build
   and clang's see one set: an array of char, short, int or long, of
   each size from 1 to 64 bytes that is a multiple of its element's,
   so of each alignment from 1 to 8, and arrays of long of 9 and 25
   elements, past eight units of 8; records with padding between their
   members or after the last, one of them aligned to 16 by a long
   double, which the copy takes in units of 8 as it takes any record's
   bytes, no member read; and unions. */

/* the records of one array member: the name, by alignment and size,
   the element's type and the count of elements */
/* clang-format off */
#define SIZED(X)                                                        \
    X(a1_1, char, 1) X(a1_2, char, 2) X(a1_3, char, 3)                  \
    X(a1_4, char, 4) X(a1_5, char, 5) X(a1_6, char, 6)                  \
    X(a1_7, char, 7) X(a1_8, char, 8) X(a1_9, char, 9)                  \
    X(a1_10, char, 10) X(a1_11, char, 11) X(a1_12, char, 12)            \
    X(a1_13, char, 13) X(a1_14, char, 14) X(a1_15, char, 15)            \
    X(a1_16, char, 16) X(a1_17, char, 17) X(a1_18, char, 18)            \
    X(a1_19, char, 19) X(a1_20, char, 20) X(a1_21, char, 21)            \
    X(a1_22, char, 22) X(a1_23, char, 23) X(a1_24, char, 24)            \
    X(a1_25, char, 25) X(a1_26, char, 26) X(a1_27, char, 27)            \
    X(a1_28, char, 28) X(a1_29, char, 29) X(a1_30, char, 30)            \
    X(a1_31, char, 31) X(a1_32, char, 32) X(a1_33, char, 33)            \
    X(a1_34, char, 34) X(a1_35, char, 35) X(a1_36, char, 36)            \
    X(a1_37, char, 37) X(a1_38, char, 38) X(a1_39, char, 39)            \
    X(a1_40, char, 40) X(a1_41, char, 41) X(a1_42, char, 42)            \
    X(a1_43, char, 43) X(a1_44, char, 44) X(a1_45, char, 45)            \
    X(a1_46, char, 46) X(a1_47, char, 47) X(a1_48, char, 48)            \
    X(a1_49, char, 49) X(a1_50, char, 50) X(a1_51, char, 51)            \
    X(a1_52, char, 52) X(a1_53, char, 53) X(a1_54, char, 54)            \
    X(a1_55, char, 55) X(a1_56, char, 56) X(a1_57, char, 57)            \
    X(a1_58, char, 58) X(a1_59, char, 59) X(a1_60, char, 60)            \
    X(a1_61, char, 61) X(a1_62, char, 62) X(a1_63, char, 63)            \
    X(a1_64, char, 64) X(a2_2, short, 1) X(a2_4, short, 2)              \
    X(a2_6, short, 3) X(a2_8, short, 4) X(a2_10, short, 5)              \
    X(a2_12, short, 6) X(a2_14, short, 7) X(a2_16, short, 8)            \
    X(a2_18, short, 9) X(a2_20, short, 10) X(a2_22, short, 11)          \
    X(a2_24, short, 12) X(a2_26, short, 13) X(a2_28, short, 14)         \
    X(a2_30, short, 15) X(a2_32, short, 16) X(a2_34, short, 17)         \
    X(a2_36, short, 18) X(a2_38, short, 19) X(a2_40, short, 20)         \
    X(a2_42, short, 21) X(a2_44, short, 22) X(a2_46, short, 23)         \
    X(a2_48, short, 24) X(a2_50, short, 25) X(a2_52, short, 26)         \
    X(a2_54, short, 27) X(a2_56, short, 28) X(a2_58, short, 29)         \
    X(a2_60, short, 30) X(a2_62, short, 31) X(a2_64, short, 32)         \
    X(a4_4, int, 1) X(a4_8, int, 2) X(a4_12, int, 3)                    \
    X(a4_16, int, 4) X(a4_20, int, 5) X(a4_24, int, 6)                  \
    X(a4_28, int, 7) X(a4_32, int, 8) X(a4_36, int, 9)                  \
    X(a4_40, int, 10) X(a4_44, int, 11) X(a4_48, int, 12)               \
    X(a4_52, int, 13) X(a4_56, int, 14) X(a4_60, int, 15)               \
    X(a4_64, int, 16) X(a8_8, long, 1) X(a8_16, long, 2)                \
    X(a8_24, long, 3) X(a8_32, long, 4) X(a8_40, long, 5)               \
    X(a8_48, long, 6) X(a8_56, long, 7) X(a8_64, long, 8)               \
    X(a8_72, long, 9) X(a8_200, long, 25)
/* clang-format on */

/* the records with padding: the name and the members */
#define PADDED(X)                                \
        X(p_sc, short s; char c;)                \
        X(p_csc, char c; short s; char d;)       \
        X(p_ci, char c; int i;)                  \
        X(p_ic5, int i; char c[5];)              \
        X(p_lc, long l; char c;)                 \
        X(p_cl, char c; long l;)                 \
        X(p_cil, char c; int i; char d; long l;) \
        X(p_ldi, long double d; int x;)          \
        X(p_nest, char c; struct p_ci in; short s;)

/* the unions: the name and the members */
#define UNIONS(X)                    \
        X(u_cs, char c[3]; short s;) \
        X(u_ci, char c[5]; int i;)   \
        X(u_cl, char c[9]; long l;)  \
        X(u_rl, struct p_lc r; long l;)

#define SIZED_RECORD(N, T, COUNT) \
        struct N                  \
        {                         \
                T m[COUNT];       \
        };

#define PADDED_RECORD(N, MEMBERS) \
        struct N                  \
        {                         \
                MEMBERS           \
        };

#define UNION_RECORD(N, MEMBERS) \
        union N                  \
        {                        \
                MEMBERS          \
        };

SIZED(SIZED_RECORD)
PADDED(PADDED_RECORD)
UNIONS(UNION_RECORD)
