/* An enumeration constant, 6.4.4.4p2, is the literal of its value
   wherever it stands: an operand, a case label, an initializer of
   static storage and another enumerator's value, 6.7.3.3p4.  The first
   without = is 0 and each other the one before it plus 1, p10; the
   constants are of type int where int holds every value, p15, and
   each is an immediate in the listings, never a load. */
enum color { red, green, blue = 10, cyan, magenta = -3, yellow };
enum { first = 3, second = first * 2 };

int g = second + 1;

int
sum(void)
{
        return red + green + blue + cyan + magenta + yellow;
}

int
classify(int x)
{
        switch (x) {
        case red: return 1;
        case blue: return 2;
        case second: return 3;
        }
        return 0;
}

unsigned long
sizes(void)
{
        return sizeof red + sizeof(enum color);
}

int
compared(int x)
{
        return x == cyan || x < magenta;
}
