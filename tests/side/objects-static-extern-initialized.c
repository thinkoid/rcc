/* extern after static takes its internal linkage, 6.2.2p4, and its
   initializer defines the object: .data, no .globl. */
static int x;
extern int x = 3;
