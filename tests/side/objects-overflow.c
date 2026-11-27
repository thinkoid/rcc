/* A constant expression reached through a signed overflow violates
   6.6p4, and rcc warns of it at the initializer's first token and
   takes the wrapped value: a sum, a difference, a product, a left
   shift whose set bit passes the sign bit and the negative of the most
   negative value, of int and of long, and an operation on such a
   constant, a conversion of it and its operand of && or || that is
   evaluated.  1 << 31 is no overflow, nor an operand && or || does not
   evaluate, nor an unsigned operation, nor a local's initializer, which
   is no constant expression. */
int add = 2147483647 + 1;
int sub = -2147483647 - 2;
int mul = 65536 * 65536;
int neg = -(-2147483647 - 1);
int two = 2147483647 + 1, three = 65536 * 65536;
static int in_static = 2147483647 + 1;
extern int in_extern = 2147483647 + 1;
int an_initializer_on_the_line_after_its_equal_sign_is_warned_of_there =
    2147483647 + 1;
long in_long = 9223372036854775807 + 1;
int shifted = 2 << 31;
int marked_sum = (2147483647 + 1) - 1;
int marked_cast = (char) (2147483647 + 1);
int marked_plus = +(2147483647 + 1);
int marked_not = !(2147483647 + 1);
int marked_and = 1 && 2147483647 + 1;
int marked_or = 0 || 2147483647 + 1;
int marked_comparison = 2147483647 + 1 < 0;
int marked_complement = ~(2147483647 + 1);
int not_shifted = 1 << 31;
int not_evaluated = 1 || 2147483647 + 1;
int not_signed = 4294967295u + 1;
int not_long = 2147483647L + 1;

int
local(void)
{
        int s = 2147483647 + 1;
        return s;
}
