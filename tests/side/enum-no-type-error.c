/* No integer type holds -1 and 2 to the 63, 6.7.3.3p4. */
enum e { A = -1, B = 0x8000000000000000 };
