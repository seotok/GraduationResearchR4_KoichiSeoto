/*
//コンパイル
//gcc  -pg LearningMain.c -g pa-misc.c opt.c optck.c setopt.c hpck.c mtime.c -lm -lportaudio -lsndfile -pg -lfftw3 -o LearningMain.out

//実行例
// ./LearningMain.out -teach dataset/test0110/LF1-40_mu.txt  -test ./dataset/test0110/LF40-59_mu.txt -mlcount 6 -hcount 6 -outputs 10
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


double *learningmain (char *FileName, char*FileName_Test, int mlcount, int hcount){

  double *returnpoint = (double*)malloc(sizeof(double)*2); //戻り値：教師データの識別率、テストデータの識別率
  memset(returnpoint, 0, sizeof(double)*2);

  FILE *text_pointer; //訓練データのポインタ
  text_pointer = fopen(FileName, "r");
  if(text_pointer == NULL) {
    printf("%s file not open!", FileName);
    fputs( "\n", stderr );
    exit( EXIT_FAILURE );
  }

  FILE *text_pointer_Test = fopen(FileName_Test, "r"); //テストデータのポインタ
    if(text_pointer_Test == NULL) {
      printf("%s file not open!\n", FileName_Test);
      fputs( "\n", stderr );
      exit( EXIT_FAILURE );
    }

  int scanp_dummy = 0;
  int n_dummy = 0;
  char c = '\0';
  int s =0;

  while((c = fgetc(text_pointer)) != EOF){
   if(c == '\n'){
     scanp_dummy++;
   }else if(c == ' '){
       n_dummy++;
   }
 }

 int m = scanp_dummy;
 int n = n_dummy/scanp_dummy;
 
 printf("入力データ数　%d\n", m); //入力のデータ数
 printf("入力データの次元数　%d\n", n); //入力の次元数


  int *label;
  double **x;//入力
  double **y;//出力
  double *z;//伝播して戻ってくる値
  int **table;

  double *hi;//中間層の入力(活性化関数の入力)
  double *ho;//中間層の出力(活性化関数の出力)

  double *hbias;//中間層のバイアス
  double *obias;//出力のバイアス
  double **w1;//一層目の重み
  double **w2;//二層目の重み
  //int n = 14; //入力の次元数
  
  int count = 0;
  
  float ave = 0;
  float dis = 1;

  label = (int*)malloc(sizeof(int)*m);
  memset(label, 0, sizeof(int)*m);
	    
  x = (double**)malloc(sizeof(double*)*m);
  for(int i=0; i<m; i++){
    x[i] = (double*)malloc(sizeof(double)*n);
  }
  for(int i=0; i<m; i++){
    memset(x[i], 0, sizeof(double)*n);
  }

   //ファイル読み込み
  //FILE *text_pointer;
  text_pointer = fopen(FileName, "r");
  if(text_pointer == NULL) {
    printf("%s file not open!\n", FileName);
    fputs( "\n", stderr );
    exit( EXIT_FAILURE );
  }

 int scanp = m; //データの個数
  
 //int scanp_dummy = 0;
  //int n_dummy = 0;
  char scanChar_x = '\0';
  int nowd = 0;
  int nowp = 0;
  int decimalFlag = 0; //小数点以下：１､　小数点以上：０
  int minusFlag = 0; //負の数：１、正の数：０
  double pointSum = 0.0f;

  
  while((scanChar_x = fgetc(text_pointer)) != EOF){
   if(scanChar_x == '\n'){
      if(label[nowd] > s) s = label[nowd];
       nowp =0;
       nowd++;
     //scanp_dummy++;
   }else if(scanChar_x == ' '){
       //n_dummy++;
      if(minusFlag == 1){
         pointSum *= -1;
         minusFlag = 0;
       }

      x[nowd][nowp] = pointSum;

      nowp++;
      pointSum = 0.0f;
      decimalFlag = 0;

    }else {
      if(nowp < n){
        if(scanChar_x == '-'){
          minusFlag = 1;
        }
        if(scanChar_x == '.'){
          decimalFlag = 1;
        }
        if(scanChar_x >= '0' && scanChar_x <= '9' && decimalFlag == 0){
          pointSum *= 10;
          pointSum += scanChar_x - '0';
        }
        if(scanChar_x >= '0' && scanChar_x <= '9' && decimalFlag >= 1){
          double buff = 0.0f;
          buff = scanChar_x - '0';
          for(int i=0; i < decimalFlag; i++){ 
            buff /= 10;
          }
          decimalFlag ++;
          pointSum += buff;
        }
      }else if(nowp == n && scanChar_x >= '0' && scanChar_x <= '9'){
        label[nowd] *= 10;
        label[nowd] += scanChar_x - '0';
      }
    }

    if(nowd > m || nowp > n){
      printf("ERROR");
      putchar(scanChar_x);
      printf("\n");
      break;
    }

    if(scanChar_x < '0' && scanChar_x > '9'){
      if(scanChar_x != '\n' && scanChar_x != ' ' && scanChar_x != '-' && scanChar_x != '.'){
      printf("ERROR");
      putchar(scanChar_x);
      printf("\n");
      }
    }
  }

 printf("ラベルの最高値 %d\n", s);

 s++;
 

  y = (double**)malloc(sizeof(double*)*m);
  for(int i=0; i<m; i++){
    y[i] = (double*)malloc(sizeof(double)*s);
  }

  z = (double*)malloc(sizeof(double)*s);

  hi = (double*)malloc(sizeof(double)*hcount);
  ho = (double*)malloc(sizeof(double)*hcount);

  w1 = (double**)malloc(sizeof(double*)*n);
  for(int i=0; i<n; i++){
    w1[i] = (double*)malloc(sizeof(double)*hcount);
  }

  w2 = (double**)malloc(sizeof(double*)*hcount);
  for(int i=0; i<hcount; i++){
    w2[i] = (double*)malloc(sizeof(double)*s);
  }
  
  srand((unsigned int)time(NULL));
  
  /*
  for(int i=0; i<n; i++){
    for(int j=0; j<hcount; j++){
      w1[i][j] =  (double)rand()/RAND_MAX;
    }
  }
  for(int j=0; j<hcount; j++){
    for(int k=0; k<s; k++){
      w2[j][k] = (double)rand()/RAND_MAX;
    }
  }
  */

 // 重みの初期化。　標準偏差0.010のガウス分布
 for(int i=0; i<n; i++){
    for(int j=0; j<hcount; j++){
       double buff = (double)rand()/RAND_MAX;
      w1[i][j] = 0.010*( sqrt( -2.0*log( rand()/(double)RAND_MAX ) ) * cos( 2.0*M_PI* (rand()/(double)RAND_MAX) ) );
    }
  }
  for(int j=0; j<hcount; j++){
    for(int k=0; k<s; k++){
      double buff = (double)rand()/RAND_MAX;
      w2[j][k] = 0.010*( sqrt( -2.0*log( rand()/(double)RAND_MAX ) ) * cos( 2.0*M_PI* (rand()/(double)RAND_MAX) ) );
    }
  }


  hbias = (double*)malloc(sizeof(double)*hcount);
  if (NULL == hbias) {
    printf("can not malloc\n");
  }
  obias = (double*)malloc(sizeof(double)*s);
   if (NULL == obias) {
    printf("can not malloc\n");
  }

  memset(hbias, 0, sizeof(double)*hcount);
  memset(obias, 0, sizeof(double)*s);

  int chc = 0;
  double out;
  double sum =0.0;


  //テーブル準備
  int labelmax =0;
  for(int i=0; i<scanp; i++){
    if(label[i]>labelmax) labelmax = label[i];
  }
  
  table = (int**)malloc(sizeof(int*)*scanp);
  for(int i=0; i<scanp; i++){
    table[i] = (int*)malloc(sizeof(int)*s);
  }

  for(int j=0; j<scanp; j++){
    for(int i=0; i<s; i++){
      if(label[j] == i){
	    table[j][i] = 1;
      }else{
	    table[j][i] = 0;
      }
      //printf("%d",table[j][i]);
    }
    //printf("\n");
  }



  //m=scanp;
  clock_t start, end, sumC =0;

  ////////////////////// 訓練データの解析
  mtime_start(1);
  start = clock();
   
  for(int mlc=0; mlc<mlcount; mlc++){
    
    for(int i=0; i<m; i++){
      memset(hi, 0, sizeof(double)*hcount);
      memset(ho, 0, sizeof(double)*hcount);

      for(int tmp=0; tmp<scanp; tmp++){
    	  memset(y[tmp], 0, sizeof(double)*s);
      }

      //１層の計算
      for(int j=0; j<hcount; j++){
        for(int k=0; k<n; k++){
          hi[j] += x[i][k]*w1[k][j];
        }
	     hi[j] += hbias[j];
	  //printf("%f\t", hi[j]);
      }
    
     //中間層のの計算
     //活性化関数は、ReLU関数。
      for(int j=0; j<hcount; j++){
      //ho[j] =1/(1+exp(-hi[j]));
        if(hi[j] > 0.0f){
          ho[j] = hi[j];
        }else if(hi[j] <= 0.0f){
          ho[j] = 0.0f;
        }
	    //printf("%f\t", ho[j]);
      }

      //第２層の計算
      for(int k=0; k<s; k++){
        for(int j=0; j<hcount; j++){
          y[i][k] += ho[j]*w2[j][k];
        }
    	  y[i][k] += obias[k];
	  //printf("%f\t", y[i][k]);
      }
      
      int j=0;
      double ymax = y[i][j];
    
      for(j=1; j<s; j++){
        if(y[i][j] > ymax){
          ymax = y[i][j];
      	}
      }

      //出力層の計算
      //ソフトマックス関数を使用している。
      double ysum =0;
      for(int k=0; k<s; k++){
      	ysum += exp(y[i][k]-ymax);
      }

      for(int k=0; k<s; k++){
	      z[k] =  exp(y[i][k]-ymax)/ysum;
    
	  //printf("%f\t", z[k]);
      }
      //printf("%d\t%d\n", mlc+1, i);
      //printf("\n");

      double hsum[hcount]; 
    
      memset(hsum, 0, sizeof(double)*hcount);
      
      for(int j=0; j<hcount; j++){
        for(int k=0; k<s; k++){
          hsum[j] += (z[k]-table[i][k])*w2[j][k];
        }
      }
    
      //２層の重みw2を更新 
      for(int j=0; j<hcount; j++){
        for(int k=0; k<s; k++){
          w2[j][k] -= 0.010f*(z[k]-table[i][k])*ho[j];
          //printf("%f\n", z[k]-table[i][k]);
        }
      }
      //１層の重みw1を更新
      for(int j=0; j<hcount; j++){
        for(int k=0; k<n; k++){
          //w1[k][j] -= 0.010*hsum[j]*ho[j]*(1-ho[j])*x[i][k];
          if(hi[j] > 0.0f){
            w1[k][j] -= 0.010f*hsum[j]*1.0f*x[i][k];
          }else if(hi[j] <= 0.0f){
            w1[k][j] -= 0.0f;
          }
	      }
      }
  
      //biasの更新
      for(int k=0; k<s; k++){
	      obias[k] -= 0.010f*(z[k]-table[i][k]);
	    //printf("%f\n", z[k]-table[i][k]);	
      }
      for(int j=0; j<hcount; j++){
        //printf("Obias%d %f\n", j, obias[j]);
        //hbias[j] -= hsum[j]*ho[j]*(1-ho[j]);
        if(hi[j] > 0.0f){
          hbias[j] -= 0.010f*hsum[j]*1.0f;
        }else if(hi[j] <= 0.0f){
          hbias[j] -= 0.0f;
        }
      }
    }
  }

  end = clock();
  sumC += end-start;

  /*
   //確認用
  for(int j=0; j<hcount; j++){
    for(int k=0; k<n; k++){
      printf("w1[%d %d]%f\n", k, j, w1[k][j]);
    } 
  }
  for(int j=0; j<hcount; j++){
    for(int k=0; k<s; k++){
      printf("w2[%d %d]%f\n", j, k, w2[j][k]);
    }
  }
  
  for(int k=0; k<s; k++){
    printf("Obias%d %f\n", k, obias[k]);
  }
  for(int j=0; j<hcount; j++){
    printf("Hbias%d %f\n", j, hbias[j]);
  }
  printf("\n");
  */


 //訓練データの認識率

  for(int i=0; i<m; i++){

    memset(hi, 0, sizeof(double)*hcount);
    memset(ho, 0, sizeof(double)*hcount);
    
    for(int tmp=0; tmp<scanp; tmp++){
      memset(y[tmp], 0, sizeof(double)*s);
    }
    
  
    for(int j=0; j<hcount; j++){
      for(int k=0; k<n; k++){
      	hi[j] += x[i][k]*w1[k][j];
      }
      hi[j] += hbias[j];
      //printf("%f\t", hi[j]);
      }
    
    for(int j=0; j<hcount; j++){
      //ho[j] =1/(1+exp(-hi[j]));
      if(hi[j] > 0.0f){
      	ho[j] = hi[j];
      }else if(hi[j] <= 0.0f){
	      ho[j] =0.0f;
      }
      //printf("%f\t", ho[j]);

    }
    for(int k=0; k<s; k++){
      for(int j=0; j<hcount; j++){
	      y[i][k] += ho[j]*w2[j][k];
      }
      y[i][k] += obias[k];

    }
    //printf("%f\t", y[i][k]);
    int j=0;
    double ymax = y[i][j];
    
    for(j=1; j<s; j++){
      if(y[i][j] > ymax){
      	ymax = y[i][j];
      }
    }
    double ysum =0;
    for(int k=0; k<s; k++){
      ysum += exp(y[i][k]-ymax);
    }

    for(int k=0; k<s; k++){
      z[k] =  exp(y[i][k]-ymax)/ysum;

    }
    // printf("\n");

    //ラベルの判定
    j=0;
    double max = z[j];
    int labelflag = j;
    
    for(j=1; j<s; j++){
      if(z[j] > max){
	      max = z[j];
	      labelflag = j;
      }
    }
    //printf("%d", labelflag);
    //printf("%d", label[i]);
    if(labelflag == label[i]) count++;

  }
  
  printf("訓練データの解析 終了\n\n");
  //printf("%d\n", count);


  scanp_dummy = 0;
  n_dummy = 0;
  c = '\0';
  s =0;

  while((c = fgetc(text_pointer_Test)) != EOF){
   if(c == '\n'){
     scanp_dummy++;
   }else if(c == ' '){
       n_dummy++;
   }
  }
 
 int m_sum =  m;
  m = scanp_dummy;
  m_sum += m;

  int n_Test = n_dummy/scanp_dummy;

 printf("入力データ数　%d\n", m); //入力のデータ数

  if(n_Test == n){
   printf("テストデータの入力データの次元数　%d\n", n_Test); //入力の次元数
   n = n_Test; 
  }else{
    printf("テストデータと訓練データの入力次元数がちがいます。　テストデータの入力次元数　%d", n_Test);
    fputs( "\n",  stderr );
    exit( EXIT_FAILURE );
  }

  int *label_Test;
  double **x_Test;//入力
  double **y_Test;//出力
  double *z_Test;//伝播して戻ってくる値
  int **table_Test;

  int count_Test = 0;

  label_Test = (int*)malloc(sizeof(int)*m);
  memset(label_Test, 0, sizeof(int)*m);
	    
  x_Test = (double**)malloc(sizeof(double*)*m);
  for(int i=0; i<m; i++){
    x_Test[i] = (double*)malloc(sizeof(double)*n);
  }
  for(int i=0; i<m; i++){
    memset(x_Test[i], 0, sizeof(double)*n);
  }

   //ファイル読み込み
  //FILE *text_pointer;
  text_pointer_Test = fopen(FileName_Test, "r");
    if(text_pointer == NULL) {
      printf("%s file not open!\n", FileName_Test);
      fputs( "\n", stderr );
      exit( EXIT_FAILURE );
    }
  scanp = m; //データの個数


  scanChar_x = '\0';
  nowd = 0;
  nowp = 0;
  decimalFlag = 0; //小数点以下：１､　小数点以上：０
  minusFlag = 0; //負の数：１、正の数：０
  pointSum = 0.0f;

  
  while((scanChar_x = fgetc(text_pointer_Test)) != EOF){
   if(scanChar_x == '\n'){
      if(label_Test[nowd] > s) s = label_Test[nowd];
       nowp =0;
       nowd++;
     //scanp_dummy++;
   }else if(scanChar_x == ' '){
       //n_dummy++;
      if(minusFlag == 1){
         pointSum *= -1;
         minusFlag = 0;
       }

      x_Test[nowd][nowp] = pointSum;

      nowp++;
      pointSum = 0.0f;
      decimalFlag = 0;

    }else {
      if(nowp < n){
        if(scanChar_x == '-'){
          minusFlag = 1;
        }
        if(scanChar_x == '.'){
          decimalFlag = 1;
        }
        if(scanChar_x >= '0' && scanChar_x <= '9' && decimalFlag == 0){
          pointSum *= 10;
          pointSum += scanChar_x - '0';
        }
        if(scanChar_x >= '0' && scanChar_x <= '9' && decimalFlag >= 1){
          double buff = 0.0f;
          buff = scanChar_x - '0';
          for(int i=0; i < decimalFlag; i++){ 
            buff /= 10;
          }
          decimalFlag ++;
          pointSum += buff;
        }
      }else if(nowp == n && scanChar_x >= '0' && scanChar_x <= '9'){
        label_Test[nowd] *= 10;
        label_Test[nowd] += scanChar_x - '0';
      }
    }

    if(nowd > m || nowp > n){
      printf("ERROR データ数または入力の次元数が違います。");
      putchar(scanChar_x);
      printf("\n");
      break;
    }

    if(scanChar_x < '0' && scanChar_x > '9'){
      if(scanChar_x != '\n' && scanChar_x != ' ' && scanChar_x != '-' && scanChar_x != '.'){
      printf("ERROR 入力ファイルの内容に異常があります。");
      putchar(scanChar_x);
      printf("\n");
      }
    }
  }

  //ラベルの種類が同じか確認する
  if(s == labelmax){ 
    printf("ラベルの最高値 %d\n", s);
    s++;
  }else{
    printf("テストデータと訓練データのラベルがちがいます。 テストデータのラベルの最高値 %d\n", s);
    fputs("\n", stderr );
    exit( EXIT_FAILURE );
  }


 //////////////////////////////////////////////////////

 y_Test = (double**)malloc(sizeof(double*)*m);
  for(int i=0; i<m; i++){
    y_Test[i] = (double*)malloc(sizeof(double)*s);
  }

  z_Test = (double*)malloc(sizeof(double)*s);

  labelmax =0;
  for(int i=0; i<scanp; i++){
    if(label_Test[i]>labelmax) labelmax = label_Test[i];
  }
  
  table_Test = (int**)malloc(sizeof(int*)*scanp);
  for(int i=0; i<scanp; i++){
    table_Test[i] = (int*)malloc(sizeof(int)*s);
  }

  for(int j=0; j<scanp; j++){
    for(int i=0; i<s; i++){
      if(label_Test[j] == i){
	    table_Test[j][i] = 1;
      }else{
	    table_Test[j][i] = 0;
      }
      //printf("%d",table[j][i]);
    }
    //printf("\n");
  }
  

  //////////////////// //////////テストデータの解析
  for(int i=0; i<m; i++){

    memset(hi, 0, sizeof(double)*hcount);
    memset(ho, 0, sizeof(double)*hcount);
    
    for(int tmp=0; tmp<scanp; tmp++){
      memset(y_Test[tmp], 0, sizeof(double)*s);
    }
    
    //１層の計算
    for(int j=0; j<hcount; j++){
      for(int k=0; k<n; k++){
	      hi[j] += x_Test[i][k]*w1[k][j];
      }
      hi[j] += hbias[j];
      //printf("%f\n", hi[j]);
    }
    
    //中間層の計算
    //活性化関数は、ReLU関数。
    for(int j=0; j<hcount; j++){
      //ho[j] =1/(1+exp(-hi[j]));
      if(hi[j] > 0.0f){
      	ho[j] = hi[j];
      }else if(hi[j] <= 0.0f){
      	ho[j] =0.0f;
      }
      //printf("%f\n", ho[j]);
    }
    
    //２層の計算
    for(int k=0; k<s; k++){
      for(int j=0; j<hcount; j++){
      	y_Test[i][k] += ho[j]*w2[j][k];
      }
      y_Test[i][k] += obias[k];
    }
    
    
    //printf("%f\t", y[i][k]);
    int j=0;
    double ymax = y_Test[i][j];
    
    for(j=1; j<s; j++){
      if(y_Test[i][j] > ymax){
      	ymax = y_Test[i][j];
      }
    }

    //出力層の計算
    //ソフトマックス関数を使用してる。
    double ysum =0;
    for(int k=0; k<s; k++){
      ysum += exp(y_Test[i][k]-ymax);
    }

    for(int k=0; k<s; k++){
      z_Test[k] =  exp(y_Test[i][k]-ymax)/ysum;

    }
    // printf("\n");
    
    //ラベルの判定
    j=0;
    double max = z_Test[j];
    int labelflag = j;
    
    for(j=1; j<s; j++){
      if(z_Test[j] > max){
        max = z_Test[j];
        labelflag = j;
      }
    }
    //printf("%d", labelflag);
    //printf("%d", label[i]);
    if(labelflag == label_Test[i]) count_Test++;
    //printf("%d \n", labelflag);

  }
  
  printf("テストデータの解析 終了\n\n");

  //printf("%d\n", count);
  printf("中間層のノード数: %d , 学習回数: %d\n", hcount, mlcount);
  printf("訓練データの認識率 %f ％\n", 100.0f*count/(m_sum-m));
  printf("テストデータの認識率 %f ％\n", 100.0f*count_Test/m);
  printf("\n");

  returnpoint[0] = 100.0f*count/(m_sum-m);
  returnpoint[1] = 100.0f*count_Test/m;
  
  //printf("ラベルの最高値 : %d\n", labelmax);
  printf("総データ数 : %d\n", m_sum);
  printf("訓練の所要時間: %f秒\n", (float)sumC / CLOCKS_PER_SEC);

  free(label_Test);
  free(z_Test);
  for(int i=0; i<m; i++){
    free(x_Test[i]);
  }
  free(x_Test);
  
  for(int i=0; i<m; i++){
   free(y_Test[i]);
  }
  free(y_Test);

  for(int i=0; i<m; i++){
   free(table_Test[i]);
  }
  free(table_Test);

  free(label);
  free(z);
  free(hi);
  free(ho);
  free(hbias);
  free(obias);  
  for(int i=0; i<n; i++){
   free(w1[i]);
  }
  free(w1);

  for(int i=0; i<hcount; i++){
   free(w2[i]);
  }
  free(w2);

  for(int i=0; i<m_sum - m; i++){
   free(x[i]); 
  }
  free(x);

  for(int i=0; i<m_sum - m; i++){
   free(y[i]); 
  }
  free(y);

  for(int i=0; i<m_sum - m; i++){
   free(table[i]);
  }
  free(table);
  
  fclose(text_pointer);
  fclose(text_pointer_Test);

 return returnpoint;
}

int main(int argc, char *argv[]){

  Opt *op =opt_open(argc, argv, argv[0]);

  //double f =opt_double(op, "-f", 1000.0, "周波数");
  //double t =opt_double(op, "-s", 1.0, "出力時間");
  //int ch = opt_double(op, "-ch", 1, "チャンネル数");
  char *FileName = opt_char(op, "-teach", "teach", "訓練データのtextファイル名");
  char *FileName_Test = opt_char(op, "-test", "test", "テストデータのtextファイル名");
  //int m = opt_int(op, "-m", 1000 , "計算点数");
  //double A = opt_double(op, "-A", 1.0, "振幅");
  //int s = opt_int(op, "-s", 1 , "集合の種類");
  //double d = opt_double(op, "-d", 1.0, "しきい値");
  int mlcount = opt_int(op, "-mlcount", 1 , "学習の回数");
  int hcount = opt_int(op, "-hcount", 1 , "中間層の数");
  int outputN = opt_int(op, "-outputs", 1, "識別率を出力する回数");
  int mode = opt_int(op, "-mode", 0 , "1:パラメータを変化させながら識別率を求める。2:パラメータを固定");
  if(mode == 0) printf("オプション-modeの指定がありません。\n");
  int skip = opt_int(op, "-skip", 0, "パラメータを何回刻みで変化させるか決める");
  FILE *recognitionRate_filePointer = fopen("RecognitionRato.txt", "w");

  char *ParametarsRange = opt_char(op, "-No", NULL, "解析するファイルの番号"); //パラメータの範囲を設定する
  if(mode == 1){
    if(ParametarsRange == NULL){
      printf("パラメータ範囲指定がありません。\n");
      return 0;
    }
    if(skip == 0){
      printf("オプション-skipの指定がありません。\n");
      return 0;
    }
    
    int ParametarsStart = 0; //パラメータのはじめの値
    int ParametarsLast = 0; //パラメータの終わりの番号
    int CountFlag = 0;
    int startDigit = 0;
    int lastDigit = 0; 
  
    for(int i=0; i< strlen(ParametarsRange); i++){
      if(ParametarsRange[i] == '-' && CountFlag==0) {
        CountFlag = 1;
        i++;
      }else if(ParametarsRange[i]>= '0'&& ParametarsRange[i]<= '9' && CountFlag == 0){
        ParametarsStart *= 10;
        ParametarsStart += ParametarsRange[i] - '0';
        startDigit ++;
      }
      if(CountFlag == 1 && ParametarsRange[i]>= '0'&& ParametarsRange[i]<= '9'){
        ParametarsLast *=10;
        ParametarsLast += ParametarsRange[i] - '0';
        lastDigit ++; 
      }
    }

    if(ParametarsStart > ParametarsLast){
      int i = ParametarsStart;
      ParametarsStart = ParametarsLast;
      ParametarsLast = i;
      int e = startDigit;
      startDigit = lastDigit;
      lastDigit = e;
    }
    
    //パラメータを変化させての平均値の出力用
    double *recognitionSum = (double*)malloc(sizeof(double)*2);
    memset(recognitionSum, 0, sizeof(double)*2);
    double *recognitionRate = (double*)malloc(sizeof(double)*2); //戻り値：教師データの識別率、テストデータの識別率
    
    for(int h=ParametarsStart; h <= ParametarsLast; h+=skip){
      memset(recognitionSum, 0, sizeof(double)*2);

      for(int i=0; i < outputN; i++){
          memset(recognitionRate, 0, sizeof(double)*2);
          //recognitionRate = learningmain(FileName, FileName_Test, mlcount, hcount);
          recognitionRate = learningmain(FileName, FileName_Test, h, h); //識別率の平均を出力する用
          //fprintf(recognitionRate_filePointer, "%f,%f\n", recognitionRate[0], recognitionRate[1]); //識別率を出力する。
          for(int j =0; j < 2; j++){
            recognitionSum[j] += recognitionRate[j];
          }
      }
      //fprintf(recognitionRate_filePointer, "\n");
      //fprintf(recognitionRate_filePointer, "教の認識率の平均,テの認識率の平均\n");
      fprintf(recognitionRate_filePointer, "%f,%f\n", recognitionSum[0]/outputN, recognitionSum[1]/outputN); //識別率の平均を出力する
    }
    free(recognitionSum);
    free(recognitionRate);
    pclose(recognitionRate_filePointer); 
    printf("パラメタを%dから%dまで%d回刻みで%d回平均の計算を行った\n\n", ParametarsStart, ParametarsLast, skip, outputN);
  }

  if(mode == 2){
    //認識率の出力用
    fprintf(recognitionRate_filePointer, "教師データのファイル名,%s\n",FileName);
    fprintf(recognitionRate_filePointer, "テストデータのファイル名, %s\n",FileName_Test);
    fprintf(recognitionRate_filePointer, "学習回数,%d\n", mlcount);
    fprintf(recognitionRate_filePointer, "中間層のノード数,%d\n", hcount);
    fprintf(recognitionRate_filePointer, "教師データの認識率,テストデータの認識率\n");
    fprintf(recognitionRate_filePointer, "\n");

    double *recognitionSum = (double*)malloc(sizeof(double)*2);
    memset(recognitionSum, 0, sizeof(double)*2);
    double *recognitionRate = (double*)malloc(sizeof(double)*2); //戻り値：教師データの識別率、テストデータの識別率
    memset(recognitionSum, 0, sizeof(double)*2);

      for(int i=0; i < outputN; i++){
          memset(recognitionRate, 0, sizeof(double)*2);
          recognitionRate = learningmain(FileName, FileName_Test, mlcount, hcount);
          fprintf(recognitionRate_filePointer, "%f,%f\n", recognitionRate[0], recognitionRate[1]); //識別率を出力する。
          for(int j =0; j < 2; j++){
            recognitionSum[j] += recognitionRate[j];
          }
      }

      fprintf(recognitionRate_filePointer, "\n");
      fprintf(recognitionRate_filePointer, "教の認識率の平均,テの認識率の平均\n");
      fprintf(recognitionRate_filePointer, "%f,%f\n", recognitionSum[0]/outputN, recognitionSum[1]/outputN); //識別率の平均を出力する
    
      free(recognitionSum);
      free(recognitionRate);
      pclose(recognitionRate_filePointer); 
  }
  return 0;
}