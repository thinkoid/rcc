/* The bounds probe's main, compiled by clang -O0: calls every function
   of bounds.c as rcc compiled it and as clang did (ref_), and compares
   the results and the bytes around the object.  Prints the first forty
   differences, or every one when PROBE_ALL is set in the environment,
   and a summary; exits 0 when nothing differs, 1 otherwise.

   The object is placed twice on one page mapped between two that are
   not: at the page's end, its last byte the page's last, and at the
   page's start.  An access wider than its object, or one that starts
   below it, reaches an unmapped page and faults, G10's width made a
   fault no value could show: the low bytes of most rows' results do
   not depend on the bytes above the object.  A fault prints the
   function, the build and the place as the probe's last line and
   exits 1.  The eight bytes beside the object on the mapped side are
   filled alike before each call and compared after it, so a store that
   reaches them shows as a difference.

   A pointer to a record's member is written as the parser writes the
   access, *(T *) ((char *) P + offset): the member_ functions take a
   pointer 8 bytes below the object.  A global's or a local's address
   is the program's to place, not the probe's, so those forms are not
   here: the fit of the comparison checks their width, and the cost
   pins show each operand. */

#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "bounds.h"

typedef long function_t(void *, int);

#define DECLARE(T, N, name, e)                                          \
    long name##_##N(T *, int);                                          \
    long ref_##name##_##N(T *, int);
#define DECLARE_MEMBER(T, N, name, e)                                   \
    long member_##name##_##N(char *, int);                              \
    long ref_member_##name##_##N(char *, int);
#define DECLARE_REGISTER(T, N)                                          \
    DECLARE(T, N, register_add, 0) DECLARE(T, N, register_and, 0)       \
    DECLARE(T, N, register_load, 0)                                     \
    long register_member_##N(char *, int);                              \
    long ref_register_member_##N(char *, int);
#define DECLARE_TYPE(T, N)                                              \
    VALUES(DECLARE, T, N, 0) VALUES(DECLARE_MEMBER, T, N, 0)            \
    DIVIDED(DECLARE, T, N, 0) DIVIDED(DECLARE_MEMBER, T, N, 0)          \
    STATEMENTS(DECLARE, T, N, 0) STATEMENTS(DECLARE_MEMBER, T, N, 0)    \
    CONDITIONS(DECLARE, T, N, 0) CONDITIONS(DECLARE_MEMBER, T, N, 0)    \
    DECLARE_REGISTER(T, N)

TYPES(DECLARE_TYPE)

/* the object's values, truncated to its width, and y's: none is 0 or
   -1, so that a division by y neither traps nor overflows */
static const long values[] = {
    0, 1, 2, 3, 5, 7, 85, 100, 127, 128, 200, 255, 256, 4095, 4096,
    32767, 32768, 65535, 65536, 0x12345, 0x12345678, 0x7fffffff,
    0x80000000L, 0xffffffffL, -1, -2, -3, -7, -128, -129, -4096, -32768,
    -32769, -65536, -2147483647L - 1
};
static const int ys[] = {
    1, 2, 3, 7, 12, 100, 127, 128, 255, 256, 1000, 32767, 65537,
    0x12345678, -2, -3, -7, -128, -129, -1000, -32768, -100000
};

#define COUNT(a) ((int) (sizeof a / sizeof *a))

/* the bytes beside the object on the mapped side, compared after each
   call, and the value they are filled with */
#define BESIDE 8
#define FILL 0xa5

static unsigned char *page;
static long page_size;

static long total;
static long bad;
static long shown = 40;

/* what is running, for the fault's line */
static const char *volatile running_name = "";
static const char *volatile running_build = "";
static const char *volatile running_place = "";

static void
say(const char *s)
{
    if (write(1, s, strlen(s)) < 0)
        _exit(2);
}

static void
on_fault(int signal)
{
    say(signal == SIGBUS ? "bus fault" : "fault");
    say(" in ");
    say(running_build);
    say(running_name);
    say(" with the object at the page's ");
    say(running_place);
    say(": an access outside the object\n");
    _exit(1);
}

/* the page between two unmapped ones */
static void
map_page(void)
{
    page_size = sysconf(_SC_PAGESIZE);

    unsigned char *area = mmap(0, 3 * page_size, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (area == MAP_FAILED || mprotect(area, page_size, PROT_NONE) != 0 ||
        mprotect(area + 2 * page_size, page_size, PROT_NONE) != 0) {
        perror("bounds: mmap");
        exit(2);
    }

    page = area + page_size;

    struct sigaction action;
    memset(&action, 0, sizeof action);
    action.sa_handler = on_fault;
    sigaction(SIGSEGV, &action, 0);
    sigaction(SIGBUS, &action, 0);
}

/* the object at the page's end, at = 0, or at its start, at = 1, and
   the window of it and the bytes beside it on the mapped side */
static unsigned char *
object_at(int at, int size, unsigned char **window)
{
    unsigned char *object = at ? page : page + page_size - size;

    *window = at ? object : object - BESIDE;

    return object;
}

static long
call(function_t *f, unsigned char *object, int member, int y)
{
    return f(member ? (void *) (object - 8) : (void *) object, y);
}

/* the row on each value and each y, at the page's end and its start,
   rcc's build against clang's; divided keeps the object's value from 0 */
static void
check(const char *name, int size, function_t *rcc, function_t *ref,
      int member, int divided)
{
    static const char *place[] = { "end", "start" };

    for (int i = 0; i < COUNT(values); ++i) {
        unsigned char value[8];
        memcpy(value, &values[i], sizeof value);

        if (divided) {
            int zero = 1;
            for (int k = 0; k < size; ++k)
                zero = zero && value[k] == 0;
            if (zero)
                continue;
        }

        for (int j = 0; j < COUNT(ys); ++j) {
            for (int at = 0; at < 2; ++at) {
                unsigned char *window;
                unsigned char *object = object_at(at, size, &window);
                unsigned char got[BESIDE + 4], want[BESIDE + 4];

                running_name = name;
                running_place = place[at];

                memset(window, FILL, BESIDE + size);
                memcpy(object, value, size);
                running_build = "rcc's ";
                const long r = call(rcc, object, member, ys[j]);
                memcpy(got, window, BESIDE + size);

                memset(window, FILL, BESIDE + size);
                memcpy(object, value, size);
                running_build = "clang's ";
                const long c = call(ref, object, member, ys[j]);
                memcpy(want, window, BESIDE + size);

                ++total;
                if (r == c && memcmp(got, want, BESIDE + size) == 0)
                    continue;

                ++bad;
                if (shown > 0) {
                    --shown;
                    printf("%s(%ld, %d) at the page's %s: rcc %ld, "
                           "clang %ld%s\n",
                           name, values[i], ys[j], place[at], r, c,
                           r == c ? ", the bytes differ" : "");
                }
            }
        }
    }
}

#define CHECK(T, N, name, e)                                            \
    check(#name "_" #N, sizeof(T), (function_t *) name##_##N,           \
          (function_t *) ref_##name##_##N, 0, 0);
#define CHECK_MEMBER(T, N, name, e)                                     \
    check("member_" #name "_" #N, sizeof(T),                            \
          (function_t *) member_##name##_##N,                           \
          (function_t *) ref_member_##name##_##N, 1, 0);
#define CHECK_DIVIDED(T, N, name, e)                                    \
    check(#name "_" #N, sizeof(T), (function_t *) name##_##N,           \
          (function_t *) ref_##name##_##N, 0, 1);
#define CHECK_MEMBER_DIVIDED(T, N, name, e)                             \
    check("member_" #name "_" #N, sizeof(T),                            \
          (function_t *) member_##name##_##N,                           \
          (function_t *) ref_member_##name##_##N, 1, 1);
#define CHECK_REGISTER(T, N)                                            \
    CHECK(T, N, register_add, 0) CHECK(T, N, register_and, 0)           \
    CHECK(T, N, register_load, 0)                                       \
    check("register_member_" #N, sizeof(T),                             \
          (function_t *) register_member_##N,                           \
          (function_t *) ref_register_member_##N, 1, 0);
#define CHECK_TYPE(T, N)                                                \
    VALUES(CHECK, T, N, 0) VALUES(CHECK_MEMBER, T, N, 0)                \
    DIVIDED(CHECK_DIVIDED, T, N, 0)                                     \
    DIVIDED(CHECK_MEMBER_DIVIDED, T, N, 0)                              \
    STATEMENTS(CHECK, T, N, 0) STATEMENTS(CHECK_MEMBER, T, N, 0)        \
    CONDITIONS(CHECK, T, N, 0) CONDITIONS(CHECK_MEMBER, T, N, 0)        \
    CHECK_REGISTER(T, N)

int
main(void)
{
    if (getenv("PROBE_ALL"))
        shown = LONG_MAX;

    map_page();

    TYPES(CHECK_TYPE)

    printf("%ld checks, %ld differ\n", total, bad);

    return bad != 0;
}
