/* G9: an access through a null pointer is performed at address 0.
   argc is 1, the branch is taken and the load faults: the program dies
   of SIGSEGV. timeout(1) dies of the same signal, and the shell
   reports 128 + 11, so the oracle records 139. clang -O2 takes the
   load as undefined, the branch as unreachable, and returns 5 without
   testing argc. The condition comes through argc, so the null access
   is the only reason to drop the branch. */
int main(int argc)
{
    if (argc == 1)
        return *(int *) 0;

    return 5;
}
