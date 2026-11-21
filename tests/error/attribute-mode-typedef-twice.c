/* A typedef declared again through a marking attribute of other
   arguments: mode (word) and mode (byte) make two types, so the second
   typedef is a redefinition with another type, 6.7.1p4. */
typedef int word_t __attribute__((mode(word)));
typedef int word_t __attribute__((mode(byte)));

int
main(void)
{
        return 0;
}
