/* A declaration of a function through a typedef of function type,
   which is not yet: the typedef is accepted, and so is a pointer
   through it; the function declared through it is refused. */
typedef int handler(int);

handler *pointer_to_one(handler *h);
handler on_signal;

int main(void)
{
    return 0;
}
