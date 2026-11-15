/* An array type as the type name of a cast: a cast converts to void or
   to a scalar type, 6.5.5p2, and an array is neither. */
int main(void)
{
    return (int) (int[3]) 0;
}
