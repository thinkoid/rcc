/* A conditional expression whose arms are both void, valid C but not
   yet. */
void f(void);

int main(void)
{
    1 ? f() : f();
    return 0;
}
