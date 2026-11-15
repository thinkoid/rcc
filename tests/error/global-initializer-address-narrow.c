/* The address of an object cast to an integer narrower than a pointer:
   the address is known only when the program is loaded, and the loader
   writes it in 64 bits, never in an int. */
int count;
int address = (int) &count;

int
main(void)
{
        return address;
}
