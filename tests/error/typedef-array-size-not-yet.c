/* A typedef of an array whose size is the size of a struct, as glibc's
   bits/procfs.h declares elf_gregset_t: the typedef is accepted and
   names a type rcc does not use yet, a pointer to it is an ordinary
   pointer, and a local of it is refused with the words of what its size
   needs. */
struct regs
{
        long r[34];
};

typedef long greg_t;
typedef greg_t gregset_t[sizeof(struct regs) / sizeof(greg_t)];

int save(gregset_t *set);

int
main(void)
{
        gregset_t set;

        return 0;
}
