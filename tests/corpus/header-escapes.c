/* K&R's enumeration constants, section 2.3 of the second edition: enum
   boolean, enum escapes and enum months as the book writes them, used
   as the book describes them, which enum makes possible.
   escape_name() names the escape a character is, by a switch on an
   enum escapes.  leap() says by an enum boolean whether a year is a
   leap year.  days() is the days of an enum months, from a table of
   DEC + 1 elements, February's one more in a leap year, and
   year_days() sums them over a loop from JAN to DEC.

   The program names the escapes of "a\tb\nc\ad\re\vf\b", counts the
   days of 1988, 1900 and 2000, and checks each result with strcmp or
   ==, and that an enum boolean is unsigned, so one that holds -1 is no
   less than NO.  It writes the line "TAB NEWLINE BELL RETURN VTAB
   BACKSPACE; 1988 has 366 days, 1900 has 365" and exits 35, the 29
   days of February 1988 and the six escapes, or exits 100 plus the
   number of the first check that failed.

   Under rcc and under clang -O2 it prints the line and exits 35:
   nothing here is undefined.

   On x86-64 and on aarch64 the listing must show escape_name()'s
   switch as the direct form (backend.md 7.3), six cases from BELL, 7,
   to RETURN, 13, range 6: the first value subtracted, subl $7, %eax and
   sub w0, w0, #7, the compare with the range, unsigned, cmpl $6, %eax;
   ja and cmp w0, #6; b.hi, and a table of seven addresses; and the
   head of year_days()'s loop, m <= DEC, compared unsigned, since an
   enum months is an unsigned int: the jump out of the loop is
   cmpl $12, -8(%rbp); ja on x86-64 and cmp w0, #12; b.hi on
   aarch64. */
#include <stdio.h>
#include <string.h>

enum boolean { NO, YES };

enum escapes {
        BELL = '\a',
        BACKSPACE = '\b',
        TAB = '\t',
        NEWLINE = '\n',
        VTAB = '\v',
        RETURN = '\r'
};

enum months { JAN = 1, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC };

/* the days of each month of a year that is no leap year; the zeroth is
   no month */
static int month_days[DEC + 1] = { 0,  31, 28, 31, 30, 31, 30,
                                   31, 31, 30, 31, 30, 31 };

/* the text whose escapes the program names, and their names, a blank
   between two */
static char text[] = "a\tb\nc\ad\re\vf\b";
static char names[64];

/* the name of the escape c is, or a null pointer */
static char *
escape_name(enum escapes c)
{
        switch (c) {
        case BELL: return "BELL";
        case BACKSPACE: return "BACKSPACE";
        case TAB: return "TAB";
        case NEWLINE: return "NEWLINE";
        case VTAB: return "VTAB";
        case RETURN: return "RETURN";
        }

        return 0;
}

/* whether year is a leap year */
static enum boolean
leap(int year)
{
        if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
                return YES;

        return NO;
}

/* the days of month m in a year that is a leap year or not */
static int
days(enum months m, enum boolean is_leap)
{
        return month_days[m] + (m == FEB && is_leap == YES);
}

/* the days of the months of year */
static int
year_days(int year)
{
        enum months m;
        int n = 0;

        for (m = JAN; m <= DEC; m++)
                n += days(m, leap(year));

        return n;
}

int
main(void)
{
        enum boolean all = -1;
        char *name;
        int count = 0;
        int i;

        for (i = 0; text[i] != '\0'; i++) {
                name = escape_name(text[i]);
                if (name == 0)
                        continue;

                if (count++ > 0)
                        strcat(names, " ");
                strcat(names, name);
        }

        printf("%s; 1988 has %d days, 1900 has %d\n", names, year_days(1988),
               year_days(1900));

        if (strcmp(names, "TAB NEWLINE BELL RETURN VTAB BACKSPACE") != 0)
                return 101;
        if (year_days(1988) != 366 || year_days(1900) != 365 ||
            year_days(2000) != 366)
                return 102;
        if (days(FEB, leap(1988)) != 29 || days(FEB, NO) != 28 ||
            days(DEC, YES) != 31)
                return 103;
        if (all < NO || sizeof(enum months) != sizeof(int))
                return 104;

        return days(FEB, leap(1988)) + count;
}
