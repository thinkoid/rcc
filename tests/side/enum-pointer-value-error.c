/* A constant of a pointer type is no integer constant expression. */
enum e { A = (int *) 0 };
