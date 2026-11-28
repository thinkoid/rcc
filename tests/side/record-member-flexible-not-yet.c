/* A last member's [0], which rcc reads as a flexible array member, is
   not yet with the array members, before its size is judged. */
struct s
{
        int n;
        char d[0];
};
