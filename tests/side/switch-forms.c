/* The form of a switch's dispatch, backend.md 7.3, by the rule of the
   Seventh Edition compiler on the sorted values alone: none without a
   case; direct where the range is above 0 and at most three times the
   count; simple below ten cases; hash otherwise, its table's size the one
   of least product.  Each pair is the smallest on either side of a
   threshold.  Each function is one shape, its name the shape. */
int
none(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        }
        return 0;
}

int
one(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: b = 1;
        }
        return b;
}

int
one_zero(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: b = 1;
        }
        return b;
}

int
two_range_6(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: return 1;
        case 6: return 2;
        }
        return 0;
}

int
two_range_7(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: return 1;
        case 7: return 2;
        }
        return 0;
}

int
three_range_9(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: return 1;
        case 4: return 2;
        case 9: return 3;
        }
        return 0;
}

int
three_range_10(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: return 1;
        case 4: return 2;
        case 10: return 3;
        }
        return 0;
}

int
nine_range_27(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
        case 4: return 5;
        case 5: return 6;
        case 6: return 7;
        case 7: return 8;
        case 27: return 9;
        }
        return 0;
}

int
nine_range_28(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
        case 4: return 5;
        case 5: return 6;
        case 6: return 7;
        case 7: return 8;
        case 28: return 9;
        }
        return 0;
}

int
ten_range_30(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
        case 4: return 5;
        case 5: return 6;
        case 6: return 7;
        case 7: return 8;
        case 8: return 9;
        case 30: return 10;
        }
        return 0;
}

int
ten_range_31(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
        case 4: return 5;
        case 5: return 6;
        case 6: return 7;
        case 7: return 8;
        case 8: return 9;
        case 31: return 10;
        }
        return 0;
}

int
nine_sparse(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: return 1;
        case 10: return 2;
        case 20: return 3;
        case 30: return 4;
        case 40: return 5;
        case 50: return 6;
        case 60: return 7;
        case 70: return 8;
        case 80: return 9;
        }
        return 0;
}

int
ten_sparse(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
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
ten_negative(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case -50: return 1;
        case -40: return 2;
        case -30: return 3;
        case -20: return 4;
        case -10: return 5;
        case 0: return 6;
        case 10: return 7;
        case 20: return 8;
        case 30: return 9;
        case 40: return 10;
        }
        return 0;
}

int
ten_defaulted(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: return 1;
        case 100: return 2;
        case 200: return 3;
        case 300: return 4;
        case 400: return 5;
        case 500: return 6;
        case 600: return 7;
        case 700: return 8;
        case 800: return 9;
        case 900: return 10;
        default: return 99;
        }
}

int
twelve(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 200: return 1;
        case 201: return 2;
        case 204: return 3;
        case 301: return 4;
        case 302: return 5;
        case 304: return 6;
        case 400: return 7;
        case 401: return 8;
        case 403: return 9;
        case 404: return 10;
        case 500: return 11;
        case 503: return 12;
        default: return 0;
        }
}

int
twenty(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: return 1;
        case 3: return 2;
        case 7: return 3;
        case 15: return 4;
        case 31: return 5;
        case 63: return 6;
        case 127: return 7;
        case 255: return 8;
        case 511: return 9;
        case 1023: return 10;
        case 2047: return 11;
        case 4095: return 12;
        case 8191: return 13;
        case 16383: return 14;
        case 32767: return 15;
        case 65535: return 16;
        case 131071: return 17;
        case 262143: return 18;
        case 524287: return 19;
        case 1048575: return 20;
        }
        return 0;
}
