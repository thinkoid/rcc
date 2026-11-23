/* shape.md chapter 5, a pointer to a function a marking attribute
   marks, converted through void * or an integer: once converted it
   carries no function type to judge, so the casts are taken, and a
   cast between two pointers to function types of the same marks is
   too.  A cast between pointers converts nothing, so each statement is
   the pointer's load and its store, with no instruction between.
   through_void() stores p, through void *, into plain, and plain,
   through void * back to the marked type, into p; through_integer()
   stores r, through unsigned long, into plain_result; same() stores p
   cast to its own marks into marked. */
typedef long word_t __attribute__((mode(word)));

int (*plain)(int);
long (*plain_result)(void);
int (*marked)(int x __attribute__((mode(word))));

void
through_void(int (*p)(int x __attribute__((mode(word)))))
{
        plain = (int (*)(int))(void *) p;
        p = (int (*)(int x __attribute__((mode(word)))))(void *) plain;
}

void
through_integer(word_t (*r)(void))
{
        plain_result = (long (*)(void))(unsigned long) r;
}

void
same(int (*p)(int x __attribute__((mode(word)))))
{
        marked = (int (*)(int x __attribute__((mode(word))))) p;
}
