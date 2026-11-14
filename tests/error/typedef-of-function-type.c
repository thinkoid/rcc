/* A declaration through a typedef of function type, which is not
   yet: the typedef is accepted, its use refused. */
typedef int handler(int);

handler on_signal;

int main(void)
{
    return 0;
}
