/* #pragma GCC visibility push(hidden) hides each symbol the region
   declares: clang writes .hidden for counter and reaches it directly,
   not through the GOT, so a shared object does not export it, where
   rcc writes it visible.  rcc refuses the pragma by its name, as not
   yet. */
#pragma GCC visibility push(hidden)
int counter;
#pragma GCC visibility pop

int
main(void)
{
        return counter;
}
