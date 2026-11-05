/* G1: signed addition wraps. INT_MAX + 1 is INT_MIN, so the
   comparison is false and the exit status 0; under ISO rules the
   optimizer folds a + 1 > a to 1. The value comes through argc so
   nothing folds before the promise applies. */
#include <limits.h>
int main(int argc)
{
    int a = INT_MAX - 1 + argc;
    return a + 1 > a;
}
