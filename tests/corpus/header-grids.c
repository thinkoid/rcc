/* Tables of more than one dimension, in the manner of section 5.7 of
   K&R's second edition, each indexed by variables: a grid of four rows
   of four at file scope and one in a local, a cube of three layers of
   three rows of three, and a row stepped back through a pointer.
   number() hands out 1, 2, 3 and on, one per call.  fill() numbers
   the global grid's cells in reading order, grid[i][j] = number(), so
   that a cell holds 4i + j + 1, and checks each as it reads it back;
   numbered() does the same to a local grid, whose cells go on from
   17, and returns the sum of its diagonal; layer() reads a cell of the
   cube's third layer, cube[i][j][2], after main has filled the cube
   with 9i + 3j + k; and back() reads (p - i + 1)[3] on a pointer into
   the grid, the element two past p when i is 2.  Each check that fails
   returns a status of its own, 1 to 4.  Its exit status, the sum of
   grid[1][2], 7, the cube's cell at 1, 2, 2, 17, and back() at the
   start of the third row, 11, says that every row was reached by both
   of its indices.

   Under rcc and under clang -O2 it exits 35: nothing here is
   undefined, and the row is the same on both sides. */
int grid[4][4];
int cube[3][3][3];
int count;

/* the next number, from 1 */
int
number(void)
{
        return ++count;
}

/* 0 when every cell of grid holds its number in reading order */
int
fill(void)
{
        int i, j;

        for (i = 0; i < 4; i++)
                for (j = 0; j < 4; j++)
                        grid[i][j] = number();

        for (i = 0; i < 4; i++)
                for (j = 0; j < 4; j++)
                        if (grid[i][j] != 4 * i + j + 1)
                                return 1;

        return 0;
}

/* the sum of the diagonal of a local grid numbered in reading order
   from where count stands */
int
numbered(void)
{
        int cells[4][4];
        int i, j, sum;

        for (i = 0; i < 4; i++)
                for (j = 0; j < 4; j++)
                        cells[i][j] = number();

        sum = 0;
        for (i = 0; i < 4; i++)
                sum += cells[i][i];

        return sum;
}

/* the cell of the cube's third layer at row i, column j */
int
layer(int i, int j)
{
        return cube[i][j][2];
}

/* the element 4 - i past p */
int
back(int *p, int i)
{
        return (p - i + 1)[3];
}

int
main(void)
{
        int i, j, k;

        if (fill() != 0)
                return 1;

        /* 17, 22, 27 and 32 on the diagonal */
        if (numbered() != 98)
                return 2;

        for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                        for (k = 0; k < 3; k++)
                                cube[i][j][k] = 9 * i + 3 * j + k;

        if (layer(2, 1) != 23)
                return 3;

        if (back(grid[2], 2) != 11)
                return 4;

        return grid[1][2] + layer(1, 2) + back(grid[2], 2);
}
