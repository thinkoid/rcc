/* An enumerated type is compatible with its integer type alone,
   6.7.3.3p13: unsigned, not int. */
enum e { A };
int g(enum e x);
int g(int x);
