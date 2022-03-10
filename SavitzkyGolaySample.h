#ifndef SavitzkyGolaySample_h
#define SavitzkyGolaySample_h


extern double** TransposeMatrix(double **X, int x, int y);
extern double** MultiplicationMatrix(double **A, int A_row, int A_col, double **B, int B_row, int B_col);
extern double** InverseMatrix(double **X, int n);
extern double* CalcSavGolCoeff(int window, int polyorder, int derive);


#endif