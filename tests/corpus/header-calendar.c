/* K&R's day_of_year and month_day, section 5.7 of the second edition,
   and its month_name, section 5.8, written from the book's
   description, which brace-enclosed lists at file scope make possible.
   The tables are globals, since static is not yet.  daytab is the
   days of each month, a row for a common year and one for a leap
   year, each with a zero before January so that a month indexes its
   row: an array of char of two rows of thirteen, braced row by row.
   name is the names of the months after the name of an illegal one,
   an array of pointers to string literals whose size the list gives.
   The program turns every day of 1988, a leap year, and of 2026 into
   its month and day and back, and fails on the first that does not
   come back; checks the names of the illegal months 0 and 13; and
   writes the lines "day 60 of 1988 is February 29" and "day 365 of
   2026 is December 31".  Its exit status, day_of_year(1988, 3, 1),
   says that the row of the leap year was read.

   Under rcc and under clang -O2 it prints the two lines and exits 61:
   nothing here is undefined, and the row is the same on both sides.

   On x86-64 and on aarch64 the data must show the names' thirteen
   literals in the literals' block after the two main names, .Ls2 as
   .string "Illegal month" to .Ls14 as .string "December"; daytab as
   .size daytab, 26 and its 26 elements, each a .byte, under
   .p2align 4 on x86-64 and none on aarch64; name as .size name, 104,
   under .p2align 4 on x86-64 and .p2align 3 on aarch64, and its
   thirteen elements, .quad .Ls2 to .quad .Ls14; each in .data. */
#include <stdio.h>

char daytab[2][13] = { { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
                       { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } };

char *name[] = { "Illegal month", "January",  "February", "March",  "April",
                 "May",           "June",     "July",     "August", "September",
                 "October",       "November", "December" };

/* 1 for a leap year, 0 for a common one: a year divisible by 4 but
   not by 100, or by 400 */
int
leap_year(int year)
{
        return year % 4 == 0 && year % 100 != 0 || year % 400 == 0;
}

/* the day of the year of a month and a day */
int
day_of_year(int year, int month, int day)
{
        int i, leap;

        leap = leap_year(year);
        for (i = 1; i < month; i++)
                day += daytab[leap][i];
        return day;
}

/* the month and the day of a day of the year */
void
month_day(int year, int yearday, int *pmonth, int *pday)
{
        int i, leap;

        leap = leap_year(year);
        for (i = 1; yearday > daytab[leap][i]; i++)
                yearday -= daytab[leap][i];
        *pmonth = i;
        *pday = yearday;
}

/* the name of month n, or of an illegal month */
char *
month_name(int n)
{
        return n < 1 || n > 12 ? name[0] : name[n];
}

/* every day of a year to its month and day and back: 0, or the first
   day that does not come back */
int
round_trip(int year)
{
        int yearday, month, day;

        for (yearday = 1; yearday <= 365 + leap_year(year); yearday++) {
                month_day(year, yearday, &month, &day);
                if (day_of_year(year, month, day) != yearday)
                        return yearday;
        }
        return 0;
}

int
main(void)
{
        int month, day;

        if (round_trip(1988) != 0 || round_trip(2026) != 0)
                return 1;

        if (month_name(0) != name[0] || month_name(13) != name[0])
                return 2;

        month_day(1988, 60, &month, &day);
        printf("day 60 of 1988 is %s %d\n", month_name(month), day);

        month_day(2026, 365, &month, &day);
        printf("day %d of %d is %s %d\n", 365, 2026, month_name(month), day);

        return day_of_year(1988, 3, 1);
}
