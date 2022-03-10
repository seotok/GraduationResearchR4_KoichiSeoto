/*
●コンパイル
　gcc  -pg gaussianTest.c -g pa-misc.c opt.c optck.c setopt.c hpck.c -lm -lportaudio -lsndfile -pg -lfftw3 -o gaussian.out

●実行
　./gaussian.out
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>  
#include <time.h>
#include <string.h>


//[0:1]の一様乱数を生成
double uniform( void ){
    //return rand()/(RAND_MAX+1.0);
    return rand()/(double)RAND_MAX; //sum = infになる
}

//要素数nのoを正規分布で初期化
void Gaussian_init(int n, double *in, double *o, double mu, double sigma){
    int i;
    for(i=0;i<n;i++){
        //Good
        //double z =sqrt(-2.0*log(uniform()) ) * cos( 2.0*M_PI*uniform()); //OK
        double z =sqrt( -2.0*log( rand()/(double)RAND_MAX ) ) * cos( 2.0*M_PI* rand()/(double)RAND_MAX ); //OK

        //Bad
        //double z =sqrt( -2.0*log( in[i] ) ) * cos( 2.0*M_PI* in[i] ); //Bad Why?
        /*This is Bad. Why
         double buff = (double)rand()/RAND_MAX;
        double z =sqrt( -2.0*log( buff ) ) * cos( 2.0*M_PI* buff ); 
        */
        
        o[i] =  sigma * z + mu;
    }
}


int main() {
    srand(time(NULL));
    int n = 10000;
    double *x = malloc(sizeof(double) * n);
    double *y = malloc(sizeof(double) * n);
    

    //textpointer preparation
    char *outputFileName = (char*)malloc(sizeof(char)*strlen("Gussian.txt"));
     if(outputFileName == NULL){
      printf("malloc rdata error\n");
      return -1;
    }
    sprintf(outputFileName, "Gussian.txt");
    FILE *outputTextpointer = fopen(outputFileName, "w");

    // Gaussian calculation 
    for(int i=0; i<n; i++){
        //x[i] = rand()/(RAND_MAX+1.0);
        x[i] = (double)rand()/RAND_MAX;
        //y[i] = 0.010*(1.0*(sqrt(-2.0*log(x[i])) * cos(2.0*M_PI*x[i])) +0.0);
        // y[i] = (1.0*(sqrt(-2.0*log(x[i])) * cos(2.0*M_PI*x[i])) +0.0);
        y[i] = sqrt( -2.0*log( rand()/(double)RAND_MAX ) ) * cos( 2.0*M_PI* (rand()/(double)RAND_MAX) );
    }
    
    // Gaussian_init(n, x, y, 0.0, 1.0);


    for (int i = 0; i < n; i++) {   
        /*//結果変わらなかった。　つまり正しい。
        //fprintf(outputTextpointer, "%d %f\n",i,(double)rand()/RAND_MAX) ;
        //fprintf(outputTextpointer, "%d %f\n",i, x[i]); 
        */
        
        //fprintf(outputTextpointer, "%d %f\n",i, x[i]); 
        fprintf(outputTextpointer, "%d %f\n",i, y[i]);

        //Write x to text
         //fprintf(outputTextpointer, "%f %f\n", x[i], y[i]);
    }

    //gnuplot preparation
    // Output the text with gunuplolt
    FILE *pipe_pointer = popen("gnuplot -persist", "w");
    fprintf(pipe_pointer, "set xlabel 'x'\n");
    fprintf(pipe_pointer, "set ylabel 'y'\n");
    fprintf(pipe_pointer, "plot \"%s\" u 1:2  \n", outputFileName);
    fprintf(pipe_pointer, "q\n");

    //textpointer close

    free(x);
    free(y);
    free(outputFileName);
    fclose(outputTextpointer);
    fclose(pipe_pointer);

    return 0;
}