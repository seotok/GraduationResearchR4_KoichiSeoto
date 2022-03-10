/*
|●機能
|●コンパイル
|gcc -pg powerdifference.c -g pa-misc.c opt.c optck.c setopt.c hpck.c mtime.c fftw.c anacx.c fftw-cx.c winfunc.c -lm -lportaudio -lsndfile -pg -lfftw3 -o pwdif.out
|
|●実行例
|./pwdif.out -blk 1024 -x soundTest1220/sinseep_2_l.wav -y soundTest1220/sinseep_2_r.wav -txt pw_sinseep.txt
*/
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

int N = 0;
int blk = 0;

/////////////////////////////////////////////////////////////////////　Hamming_w関数
double Hamming_w(double* out){

  //Hamming窓の処理を加える
  for(int i=0;i < blk; i++){
    out[i] = out[i]*(0.54-0.46*cos(2*M_PI*i/(blk-1)));
    //out[i] = in[i+blk/2*n];
  }
  return 0;
}

//////////////////////////////////////////////////////////////////　pwsp関数
int pwsp(double* x, double* X){

  FFTW *v1 = fftw_Open(blk);
  double  *x_win = (double*)malloc(sizeof(double)*blk);
  Cmplx *Z = (Cmplx*)malloc(sizeof(Cmplx)*blk);
    if(Z == NULL){
      printf("malloc rdata error H\n");
      return -1;
    }
    memset(Z, 0, sizeof(Cmplx)*blk);

  clock_t start, end, sumC =0;

  if(x_win == NULL){
    printf("malloc rdata error\n");
    return -1;
  }

  memset(x_win, 0, sizeof(double)*blk);
 
  mtime_start(1);
  start = clock();

  printf("計算開始\n");
   
  for(int n=0; n < 2*N/blk-1; n++){

    memset(x_win, 0, sizeof(double)*blk);
   
    for(int i=0; i< blk ; i++){
      x_win[i] = x[i+blk/2*n];
    }

    Hamming_w(x_win);

    fftw_ExecFwd(v1, x_win, Z);

    for(int k=0; k<blk; k++){
      X[k] += Z[k].r*Z[k].r + Z[k].i* Z[k].i;
    }
  }

  for(int k=0; k<blk; k++){
      X[k] /= 2.0*N/blk-1;
  }

  end = clock();
  sumC += end-start;
  printf("fft計算時間: %f秒\n", (float)sumC / CLOCKS_PER_SEC);
  printf("パワーペクトル計算完了\n");

  fftw_Close(v1);
  free(Z);
  free(x_win);

  return 0;   
}

////////////////////////////////////////////////////////////main関数
int main(int argc, char *argv[]){
    Opt *op =opt_open(argc, argv, argv[0]);
    SF_INFO sfo,sfo2;
    
  //オプション（モード２の)の判定 ///
    char *inFileName = opt_char(op, "-x", "NoMessage", "ｘの音源ファイル場所"); 
    char *in2FileName = opt_char(op, "-y", "NoMessage" , "観測音源のsoundfile名");
    char *outFileName = opt_char(op, "-txt", "NoMessage", "出力するtextファイル名");
    blk = opt_int(op, "-blk", 0, "ブロック数");

    if(inFileName == "NoMessage") printf("オプション-xの指定がありません。\n");
    if(in2FileName == "NoMessage") printf("オプション-yの指定がありません。\n");
    if(outFileName == "NoMessage"){
      printf("オプション-txtの指定がありません。\n");
      return 0;
    }
    
    if(blk == 0) printf("オプション　-blkの指定がありません。\n");

    SNDFILE *isf = sf_open(inFileName, SFM_READ, &sfo);
    if( isf == NULL ){
      fputs( "1つ目のファイルオープンに失敗しました。\n", stderr );
      exit( EXIT_FAILURE );
    }
    SNDFILE *isf2 = sf_open(in2FileName, SFM_READ, &sfo2);
    if( isf2 == NULL ){
        fputs( "2つ目のファイルオープンに失敗しました。\n", stderr );
        exit( EXIT_FAILURE );
    }
    if(sfo.frames == sfo2.frames &&  sfo.channels == sfo2.channels){
      printf("ｘのサンプル数：%ld\n", sfo.frames);
      printf("ｙのサンプル数：%ld\n", sfo2.frames);
      printf("ｘのチャンネル数：%d\n", sfo.channels);
      printf("ｙのチャンネル数：%d\n", sfo2.channels);
    }else{
      printf("ｘのサンプル数：%ld\n", sfo.frames);
      printf("ｙのサンプル数：%ld\n", sfo2.frames);
      printf("ｘのチャンネル数：%d\n", sfo.channels);
      printf("ｙのチャンネル数：%d\n", sfo2.channels);
      printf("サンプル数またはチャンネル数が正しくありません。\n");
      return -1;
    }

     N = sfo.frames;

    double *x, *y, *dif;

    x = (double*)malloc(sizeof(double)*N);
    y = (double*)malloc(sizeof(double)*N);
    dif = (double*)malloc(sizeof(double)*blk);
    if(x == NULL){
      printf("malloc rdata error x\n");
      return -1;
    }
    if(y == NULL){
      printf("malloc rdata error y\n");
      return -1;
    }
     if(dif == NULL){
      printf("malloc rdata error h\n");
      return -1;
    }
    memset(x, 0, sizeof(double)*N);
    memset(y, 0, sizeof(double)*N);
    memset(dif, 0, sizeof(double)*blk);

    double *X, *Y;
    X = (double*)malloc(sizeof(double)*blk);
    Y = (double*)malloc(sizeof(double)*blk);
    if(X == NULL){
        printf("malloc rdata error\n");
        return -1;
    }
    if(Y == NULL){
        printf("malloc rdata error\n");
        return -1;
    }
    memset(X, 0, sizeof(double)*blk);
    memset(Y, 0, sizeof(double)*blk);

    
    char *power1FileName = (char*)malloc(sizeof(char)*7);
     if(power1FileName == NULL){
      printf("malloc rdata error\n");
      return -1;
    }
    sprintf(power1FileName, "pwL.txt");
    FILE *pw1Textpointer = fopen(power1FileName, "w");

    char *power2FileName = (char*)malloc(sizeof(char)*7);
    if(power2FileName == NULL){
      printf("malloc rdata error\n");
      return -1;
    }
    sprintf(power2FileName, "pwR.txt");
    FILE *pw2Textpointer = fopen(power2FileName, "w");

    char *pwdifFileName = (char*)malloc(sizeof(char)*strlen(outFileName));
    if(pwdifFileName == NULL){
      printf("malloc rdata error\n");
      return -1;
    }
    sprintf(pwdifFileName, "%s", outFileName);
    FILE *pwdifTextpointer = fopen(pwdifFileName, "w");
  
    sf_read_double(isf, x, N); 
    sf_read_double(isf2, y, N);

    //2つのファイルの差を求める
    pwsp(x, X);
    pwsp(y, Y);

    for(int k=0; k<blk; k++){
      dif[k] = X[k]-Y[k];
    }

    //テキストに数値の書き込み
    for(int k=0; k<blk; k++){
      fprintf(pw1Textpointer, "%d %f\n", 44100/blk*k, 10*log10(X[k]));
      fprintf(pw2Textpointer, "%d %f\n", 44100/blk*k, 10*log10(Y[k]));
      fprintf(pwdifTextpointer, "%d %f \n",  44100/blk*k, 10*log10(dif[k]));
      /*
      printf("%d %f\n", 44100/blk*k, 10*log10(X[k]));
      printf("%d %f\n", 44100/blk*k, 10*log10(Y[k]));
      printf( "%d %f \n",  44100/blk*k, 10*log10(dif[k]));
      */
    }

    //gnuplotの出力
    FILE *pipe_pointer = popen("gnuplot -persist", "w");
    fprintf(pipe_pointer, "set xlabel '周波数(Hz)'\n");
    fprintf(pipe_pointer, "set ylabel '音圧レベル(dB)'\n");
    fprintf(pipe_pointer, "plot [-1000:]\"%s\" u 1:2 with l , \"%s\" u 1:2 with l\n", power1FileName, power2FileName);
    fprintf(pipe_pointer, "q\n");
    
    FILE *InXpipe_pointer = popen("gnuplot -persist", "w");
    fprintf(InXpipe_pointer, "set xlabel '周波数(Hz)'\n");
    fprintf(InXpipe_pointer, "set ylabel '音圧レベル(dB)'\n");
    fprintf(InXpipe_pointer, "plot [-1000:] \"%s\" u 1:2 with l linetype 3\n ",pwdifFileName);
    fprintf(InXpipe_pointer, "q\n");
    
    free(x);
    free(y);
    free(dif);
    pclose(pipe_pointer);
    pclose(InXpipe_pointer);
    free(X);
    free(Y);
    pclose(pw1Textpointer);
    free(power1FileName);
    pclose(pw2Textpointer);
    free(power2FileName);
    pclose(pwdifTextpointer);
    free(pwdifFileName);
    sf_close(isf);
    sf_close(isf2);
    return 0;
}