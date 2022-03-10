/*
●コンパイル
| gcc  -g pa-misc.c opt.c optck.c setopt.c hpck.c mtime.c fftw.c anacx.c fftw-cx.c winfunc.c -lm -lportaudio -lsndfile -pg -lfftw3 -pg SavitzkyGolaySample.c -o savitzkygolay.o=
●実行
|
●参考元URL
| ◎【C++】【Eigen】Savitzky-Golay法を実装する
| 　 https://izadori.net/math-savitzky-golay/
*/

#include "SavitzkyGolaySample.h"
#include <stdio.h>
#include <sndfile.h>
#include <math.h>
#include "pa-misc.h"
#include<string.h> //strcmp、strncmp関数を使うために必要
#include <stdlib.h>
#include <string.h>
#include "opt.h"
#include <time.h>
#include "mtime.h"
#include "fftw.h"
#include "cvl.h"
#include <sys/types.h>
#include <dirent.h>


/////////////////////////////////////////////////////////////////////////////////  TransposeMatrix関数
double** TransposeMatrix(double **X, int x, int y){

   
    double **transMat_X = (double**)malloc(sizeof(double*)*y);
    for(int i=0; i<y; i++){
        transMat_X[i] = (double*)malloc(sizeof(double)*x); 
    }
     if( transMat_X == NULL ){
        fputs( "transMat_X の領域確保に失敗しました。\n", stderr );
        exit( EXIT_FAILURE );
    }
    

    for(int i=0; i< y; i++){
        for(int j=0; j<x; j++){
            transMat_X[i][j] = X[j][i];
        }
    }
    
    /*
    //確認用
    printf("転置行列の出力\n");
    for(int i=0;i<y;i++){
        for(int j=0;j<x;j++){
            printf(" %f",transMat_X[i][j]);
        }
        printf("\n");
    }
     printf("\n");
    */

    return transMat_X;
}

///////////////////////////////////////////////////////////////////////////////////// MultiplicationMatrix関数

double** MultiplicationMatrix(double **A, int A_row, int A_col, double **B, int B_row, int B_col){

    if(A_col != B_row){
        fputs( "行列の積の計算を行えません。\n", stderr );
        exit( EXIT_FAILURE );
    }

    int n = A_col;

    
    double **matrixAandB = (double**)malloc(sizeof(double*)*A_row);
    for(int i=0; i< A_row; i++){
        matrixAandB[i] = (double*)malloc(sizeof(double)*B_col);
    }
    if( matrixAandB == NULL ){
        fputs( "matrixAandB の領域確保に失敗しました。\n", stderr );
        exit( EXIT_FAILURE );
    }
    
    /*
   //確認用
    for(int i=0; i<A_row; i++){
        for(int j=0; j<A_col; j++){
            printf("%f  ", A[i][j]);
        }
         printf("\n");
    }
     printf("\n");
    //確認用
    for(int i=0; i<B_row; i++){
        for(int j=0; j<B_col; j++){
            printf("%f  ", B[i][j]);
        }
         printf("\n");
    }
     printf("\n");
    */
   
   for(int i=0;i<A_row;i++){
        for(int j=0;j<B_col;j++){
            matrixAandB[i][j] = 0.0f;
        }
    }

    for(int i=0; i<A_row; i++){
        for(int j=0; j<B_col; j++){
            for(int k=0; k<n; k++){
                matrixAandB[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    /*
     //確認用
    printf("行列の積の出力\n");
    for(int i=0;i<A_row;i++){
        for(int j=0;j<B_col;j++){
            printf(" %f",matrixAandB[i][j]);
        }
        printf("\n");
    }
     printf("\n");
     */
    
   
    return matrixAandB ;
    
}


    
////////////////////////////////////////////////////////////////////////////////////// InverseMatrix　関数
double** InverseMatrix(double **X, int n){

   
    double **invMat_X = (double**)malloc(sizeof(double*)*n);
    for(int i=0; i<n; i++){
        invMat_X[i] = (double*)malloc(sizeof(double)*n); 
    }
    if( invMat_X == NULL ){
        fputs( "invMat_X の領域確保に失敗しました。\n", stderr );
        exit( EXIT_FAILURE );
    }
    
    double **buff_X = (double**)malloc(sizeof(double*)*n);
    for(int i=0; i<n; i++){
        buff_X[i] = (double*)malloc(sizeof(double)*n); 
    }
    if( buff_X== NULL ){
        fputs( "buff_ の領域確保に失敗しました。\n", stderr );
        exit( EXIT_FAILURE );
    }

    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            buff_X[i][j] = X[i][j];
        }
    }
    
    /*
    //確認用
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            printf("%f  ", buff_X[i][j]);
        }
         printf("\n");
    }
    */

    double buf = 0.0f; //一時的なデータを蓄える
    int i,j,k; //カウンタ
 
    //単位行列を作る
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
        invMat_X[i][j]=(i==j)?1.0:0.0;
        }
    }

    //掃き出し法
    for(i=0;i<n;i++){
        buf=1/buff_X[i][i];
        for(j=0;j<n;j++){
            buff_X[i][j] *= buf;
            invMat_X[i][j]*=buf;
        }
        for(j=0;j<n;j++){
            if(i!=j){
                buf = buff_X[j][i];
                for(k=0;k<n;k++){
                    buff_X[j][k] -= buff_X[i][k]*buf;
                    invMat_X[j][k] -= invMat_X[i][k]*buf;
                }
            }
        }
    }


    /*
    //逆行列を出力 確認用
    printf("逆行列の出力\n");
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf(" %f",invMat_X[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    */

    //メモリの開放　ここから
    for(int i=0; i<n; i++){
        free(buff_X[i]);
    }
    free(buff_X);

    //メモリの解放　ここまで

    return invMat_X;
    
}


//////////////////////////////////////////////////////////////////////////////////////////// CalcSavGolCoeff関数
// Savitzky-Golay法の係数を求める
//
// window   : データ点数（2N+1点）
// polyorder: 近似多項式の次数
// derive   : 微分の次数
double* CalcSavGolCoeff(int window, int polyorder, int derive)
{
    
    int n = (int)window / 2 * -1;
    double p = 1;
    
    // 行列Xの生成
    //Eigen::VectorXd v = Eigen::VectorXd::LinSpaced(window, -n, n);　//Eigen::VectorXd::LinSpaced(size, low, high) →　初期化 low～highをsizeで分割した連続値

    int * v = (int*)malloc(sizeof(int)*window);
    //メモリ確保できたかの確認入れる
    if( v == NULL ){
        fputs( "v の領域確保に失敗しました。\n", stderr );
        exit( EXIT_FAILURE );
    }
    for(int i=0; i<window; i++){
        v[i] = n;
        n++;
        //printf("%d ", v[i]);
    }
     //printf("\n");
     
    double **x = (double**)malloc(sizeof(double*)*window);
    for (int i = 0; i < window; i++){
        x[i] = (double*)malloc(sizeof(double)*(polyorder+1)); 

    }
    if( x == NULL ){
        fputs( "x の領域確保に失敗しました。\n", stderr );
        exit( EXIT_FAILURE );
    }
    
    for(int i = 0; i < window; i++){
        for(int j=0; j< polyorder+1; j++){    
            x[i][j] = 1.0f;  
        }
    }

    for(unsigned int i=1; i<= polyorder; i++){
        for(int j=0; j<window; j++){
            for(int k=0; k<i; k++){
                x[j][i] *= v[j];
            }
        }
    }
    
    /*
    //確認用
    for(int i=0; i<window; i++){
      for(int j=0; j< polyorder+1; j++){
           printf("%lf  ", x[i][j]);
       }
       printf("\n");
   }
     printf("\n");
    */

    // (X^T * X)^-1 * X^Tを計算する
    //Eigen::MatrixXd coeff_mat = (x.transpose() * x).inverse() * x.transpose(); //mat.transpose()　→　転置行列
    // coeff_mat[][] = (xの転置行列*x)の逆行列 * ｘの転置行列

    double **transMat_x = TransposeMatrix(x, window, polyorder+1);
    
    double **trans_xANDx = MultiplicationMatrix(transMat_x, polyorder+1, window,  x, window, polyorder+1);

    double **invMat_trans_xANDx = InverseMatrix(trans_xANDx,  polyorder+1);
    
    double **coeff_mat = MultiplicationMatrix(invMat_trans_xANDx, polyorder+1, polyorder +1, transMat_x, polyorder+1, window);
   
    // 階乗の計算
    if(derive > 0){
      for(unsigned int i = 1; i <= derive; i++){
        p *= i;
      }
    }
    
    double *coeff_mat_last = (double*)malloc(sizeof(double)*window);
    if( coeff_mat_last == NULL ){
        fputs( "coeff_mat_last の領域確保に失敗しました。\n", stderr );
        exit( EXIT_FAILURE );
    }

    for(int i=0; i<window; i++){
     coeff_mat_last[i] = coeff_mat[derive][i] * p;
    }

    // メモリの開放　ここから //

    //coeff_mat
     for(int i=0; i<polyorder+1; i++){
        free(coeff_mat[i]); 
    }
    free(coeff_mat);

     //inv_trans_xANDx
    for (int i = 0; i < polyorder+1; i++){
        free(invMat_trans_xANDx[i]);       
    }
    free(invMat_trans_xANDx);

    //transxANDx
    for (int i = 0; i < polyorder+1; i++){
        free(trans_xANDx[i]);       
    }
    free(trans_xANDx);
     
    //transMat_x
    for(int i=0; i<polyorder+1; i++){
        free(transMat_x[i]); 
    }
    free(transMat_x);

     //x
    for (int i = 0; i < window; i++){
        free(x[i]);       
    }
    free(x);

     //v
    free(v);
    //メモリの開放　ここまで //

    return  coeff_mat_last; //mat.row(ii)	参照：ii行
}


//////////////////////////////////////////////////////////////////////////////　main関数
//確認用
/*
int main()
{
    double *a2 = CalcSavGolCoeff(9, 5, 2);
    printf( "CalcSavGolCoeff(9, 5, 2) = " );
   for(int i=0; i<9; i++){
    printf( "%lf ",a2[i]);
    }
    printf("\n");

    free(a2);

      double *a1 = CalcSavGolCoeff(9, 5, 1);
     printf( "CalcSavGolCoeff(9, 5, 1) = " );
   for(int i=0; i<9; i++){
    printf( "%lf ",a1[i]);
    }
     printf("\n");
     free(a1);

    double *a0 = CalcSavGolCoeff(9, 5, 0);
     printf( "CalcSavGolCoeff(9, 5, 0) = " );
    for(int i=0; i<9; i++){
    printf( "%lf ",a0[i]);
    }
    printf("\n");
    
    free(a0);
   
    
    //逆行列,転置行列確認用
    double **a= (double**)malloc(sizeof(double*)*4);
    for(int i=0; i<4; i++){
        a[i] = (double*)malloc(sizeof(double)*4);
    }
    double b[4][4]={{1,2,0,-1},{-1,1,2,0},{2,0,1,1},{1,-2,-1,1}}; 

    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            a[i][j] = b[i][j];
        }
    }
    double **inv_a =  InverseMatrix(a, 4);//ここに逆行列が入る
    //double **inv_a =  TransposeMatrix(a, 4, 4);//ここに転置行列が入る

  
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            printf("%f  ", a[i][j]);
        }
         printf("\n");
    }

    for(int i=0; i<4; i++){
        free(inv_a[i]); 
    }
    free(inv_a);

      for(int i=0; i<4; i++){
        free(a[i]); 
    }
    free(a);

    return 0;
}

/*
上記の実行結果
CalcSavGolCoeff(9, 5, 0) = 
 0.034965 -0.128205 0.0699301  0.314685  0.417249  0.314685 0.0699301 -0.128205  0.034965

CalcSavGolCoeff(9, 5, 1) = 
-0.0296037   0.160956  -0.264452  -0.335548          0   0.335548   0.264452  -0.160956  0.0296037

CalcSavGolCoeff(9, 5, 2) = 
-0.0734266     0.2162  0.0879953   -0.12296  -0.215618   -0.12296  0.0879953     0.2162 -0.0734266
*/