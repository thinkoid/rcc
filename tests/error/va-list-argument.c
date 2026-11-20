/* An argument 0 for a parameter of type __builtin_va_list, the type
   behind va_list, as the target has it, 7.16p4: on x86-64 a pointer
   to the list's record, by the array's adjustment, which the null
   pointer constant converts to, so the program compiles; on aarch64
   the record, which no integer converts to, an error. */
int vcount(__builtin_va_list ap);

int
main(void)
{
        return vcount(0);
}
