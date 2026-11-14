/* The use of an object declared extern at file scope: the declaration
   is accepted, the use is not yet. */
extern int count;

int main(void)
{
    return count;
}
