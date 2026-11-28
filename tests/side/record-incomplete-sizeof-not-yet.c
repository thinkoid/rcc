/* sizeof of *p, through a pointer to an incomplete record, is rcc's not
   yet of the indirection. */
struct s;

unsigned long
f(struct s *p)
{
        return sizeof *p;
}
