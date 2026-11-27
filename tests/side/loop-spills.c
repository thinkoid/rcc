/* Loops whose conditions, init and step spill: a chain of 16
   operators nested on the right is generated in the loop's test, its
   init and its step as anywhere else, and the test's jump reloads a
   spilled left operand before it reads both.  An if of one jump in the
   body jumps on its chain's own flags.

   Each function is one loop, its name the loop and where its chains
   stand. */
int
while_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (
            (c -
             (c -
              (c -
               (c -
                (c -
                 (c -
                  (c -
                   (c -
                    (c - (c - (c - (c - (c - (c - (c - (c - b)))))))))))))))))
                b = b - 1;
        return b;
}

int
do_less_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                a = a + 1;
        while (
            a <
            (c +
             (c +
              (c +
               (c +
                (c +
                 (c +
                  (c +
                   (c +
                    (c + (c + (c + (c + (c + (c + (c + (c + b)))))))))))))))));
        return a;
}

int
for_chain(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = (c +
                  (c +
                   (c +
                    (c +
                     (c +
                      (c +
                       (c +
                        (c +
                         (c +
                          (c + (c + (c + (c + (c + (c + (c + b))))))))))))))));
             a <
             (c *
              (c *
               (c *
                (c *
                 (c *
                  (c *
                   (c *
                    (c *
                     (c * (c * (c * (c * (c * (c * (c * (c * b))))))))))))))));
             a = a +
                 (c -
                  (c -
                   (c -
                    (c -
                     (c -
                      (c -
                       (c -
                        (c -
                         (c -
                          (c - (c - (c - (c - (c - (c - (c - b)))))))))))))))))
                if ((c &
                     (c &
                      (c &
                       (c &
                        (c &
                         (c &
                          (c &
                           (c &
                            (c &
                             (c &
                              (c & (c & (c & (c & (c & (c & b)))))))))))))))))
                        break;
        return a;
}
