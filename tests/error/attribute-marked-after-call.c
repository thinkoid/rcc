/* A function declared weak after a call of it: GNU C lets the later
   declaration mark the call before it too, which rcc has taken as a
   call of a strong symbol, so the attribute, written too late, is not
   yet. */
int hook(void);

int
main(void)
{
        return hook();
}

int hook(void) __attribute__((__weak__));
