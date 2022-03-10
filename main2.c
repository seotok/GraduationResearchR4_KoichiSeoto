/*----------------------------------------------------
  |元音源と観測音源の伝達関数を出力する
  |
  |mode 1 元音源とその観測音源4種（前後での左右）を解析する
  |実行例 -No [解析したいファイルの識別番号の範囲] 　-blk[FFT点数] -subMode[1:前後のみ、2：上下のみ、3：前後と上下]
  ./a.out -No 1-220 -blk 1024 -mode 1 -subMode 1

  |
  |mode 2 １つの元音源と１つの観測音源に対して解析を行う
  |実行例 -x [元音源のファイル] -y[観測音源のファイル]　-blk[FFT点数]　-txt[出力ファイル名]
  ./a.out  -x soundTest1220/wn_r.wav -blk 1024 -mode 2 -y soundTest1220/wn_r.wav -txt test.txt
  |
  |mode 3　
  |実行例 ディレクトリ内の出力したファイルを検索し、グラフ表示する。
  |　実行後、mae_r, mae_l, usiro_r, usiro_lの中から１つ選択する。
  ./a.out -mode 3
  |
  |
  |コンパイル
  gcc  -pg main_2.c -g pa-misc.c opt.c optck.c setopt.c hpck.c mtime.c fftw.c anacx.c fftw-cx.c winfunc.c SavitzkyGolaySample.c -lm -lportaudio -lsndfile -pg -lfftw3 -o main2.out
  |
  +-------------------------------------------------------*/

//コンパイル
// gcc  -pg main_2.c -g pa-misc.c opt.c optck.c setopt.c hpck.c mtime.c fftw.c anacx.c fftw-cx.c winfunc.c SavitzkyGolaySample.c -lm -lportaudio -lsndfile -pg -lfftw3 -o main2.out


//delay用の実行
//./a.out -T 3 -x sound/1104/k3_mae2_l_1106_0.0sdelay.wav -y sound/1104/k3_mae2_l_1106_0.01sdelay.wav -txt 1104_0.01sdelay.txt


// gnuplot
//plot "test.txt" u 1:2 with l
//↑のコマンドで出力する

/*delayの比較
  plot "1107_0.002sdelay.txt" u 1:2 w l,"1107_0.005sdelay.txt" u 1:2 w l,"1107_0.002sdelay.txt" u 1:2 w l, "1107_0.001sdelay.txt" u 1:2 with l
  gnuplot> plot "1107_0.03sdelay.txt" u 1:2 w l,"1107_0.005sdelay.txt" u 1:2 w l,"1107_0.002sdelay.txt" u 1:2 w l, "1107_0.001sdelay.txt" u 1:2 with l
*/

/*sinの比較
  set xlabel '(Hz)' 
  set ylabel '(dB)'
  plot "./text/sinseep_usiro.txt" u 1:2 with l,"./text/sinseep_mae.txt" u 1:2 with l,"./text/sinseep_ue.txt" u 1:2 with l,"./text/sinseep_sita.txt" u 1:2 with l,
*/

/*
  set xlabel 'Frequency [Hz]' 
  set nokey
  set lmargin 30
  set bmargin 6
  set xlabel offset -25,-1
  set ylabel offset -10,-1  
 
  //case ILD
  set ylabel 'ILD [dB]'　
  set tics font "Arial,25"
  set xlabel font "Arial,25"
  set ylabel font "Arial,25"

 //case HRTF
  set ylabel 'HRTF [dB]'
  set tics font "Arial,35"
  set xlabel font "Arial,30"
  set ylabel font "Arial,30"
  

  plot "o1_mae_l.txt" u 1:2 with l, "o2_mae_l.txt" u 1:2 with l,  "o3_mae_l.txt" u 1:2 with l
  plot "o1_mae_r.txt" u 1:2 with l, "o2_mae_r.txt" u 1:2 with l,  "o3_mae_r.txt" u 1:2 with l
  plot "o1_usiro_l.txt" u 1:2 with l, "o2_usiro_l.txt" u 1:2 with l , "o3_usiro_l.txt" u 1:2 with l
  plot "o1_usiro_r.txt" u 1:2 with l, "o2_usiro_r.txt" u 1:2 with l , "o3_usiro_r.txt" u 1:2 with l

  plot "wn_mae_l.txt" u 1:2 with l
  plot "wn_mae_r.txt" u 1:2 with l
  plot "wn_usiro_l.txt" u 1:2 with l
  plot "wn_usiro_r.txt" u 1:2 with l


  wnoiseの比較

  plot "./text/wn_usiro.txt" u 1:2 with l,"./text/wn_mae.txt" u 1:2 with l,"./text/wn_ue.txt" u 1:2 with l,"./text/wn_sita.txt" u 1:2 with l,
  plot "./text/wnoise_usiro.txt" u 1:2 with l,"./text/wnoise_mae.txt" u 1:2 with l,"./text/wnoise_ue.txt" u 1:2 with l,"./text/wnoise_sita.txt" u 1:2 with l,

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
#include "SavitzkyGolaySample.h"

#define soundFolderPath_IN "fusion"//読み込むサウンドファイルのフォルダへのパス
//#define soundFolderPath_IN "sound"//読み込むサウンドファイルのフォルダへのパス
#define textFolderPath "text" //伝達関数のテキストファイルと出力するフォルダ
#define IFFTtextFolderPath "IFFTtext" //逆フーリエのテキストファイルを出力するフォルダ
#define learningDataTextFolderPath "learningDataText" //学習用の実データのテキストを出力するフォルダ
#define learningLabelTextFolderPath "learningLabelText" // 学習用のラベルデータのテキストを出力するフォルダ
#define learningValTextFolderPath "learningValText"//学習用のバリデーションデータのテキストを出力するフォルダ

int blk = 0;
int N = 0;

char *sndFolPath_IN = soundFolderPath_IN;


/////////////////////////////////////////////////////////////////////　Hamming_w関数
double Hamming_w(double* out){

  //Hamming窓の処理を加える
  for(int i=0;i < blk; i++){
    out[i] = out[i]*(0.54-0.46*cos(2*M_PI*i/(blk-1)));
    //out[i] = in[i+blk/2*n];
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////　Analysis関数
int Analysis(double* x, double* y, Cmplx* H, double* h){

  FFTW *v1 = fftw_Open(blk);
  FFTW *v2 = fftw_Open(blk);
  FFTW *v3 = fftw_Open(blk);
  double *x_win, *y_win;

  Cmplx *X, *Y;
  
  clock_t start, end, sumC =0;

  x_win = (double*)malloc(sizeof(double)*blk);
  y_win = (double*)malloc(sizeof(double)*blk);
 
  X = (Cmplx*)malloc(sizeof(Cmplx)*blk);
  Y = (Cmplx*)malloc(sizeof(Cmplx)*blk);

  if(x_win == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
  if(y_win == NULL){
    printf("malloc rdata error\n");
    return -1;
  }

  if(X == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
  if(Y == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
 
  memset(x_win, 0, sizeof(double)*blk);
  memset(y_win, 0, sizeof(double)*blk);
  memset(h, 0, sizeof(double)*blk);
  memset(X, 0, sizeof(Cmplx)*blk);
  memset(Y, 0, sizeof(Cmplx)*blk);
 
  mtime_start(1);
  start = clock();

  printf("計算開始\n");

  Cmplx *X_sum, *Y_sum;

  X_sum = (Cmplx*)malloc(sizeof(Cmplx)*blk);
  Y_sum = (Cmplx*)malloc(sizeof(Cmplx)*blk);
 
  if(X_sum == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
  if(Y_sum == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
  memset(X_sum, 0, sizeof(Cmplx)*blk);
  memset(Y_sum, 0, sizeof(Cmplx)*blk);
   
  int countSum = 0;
  for(int n = 0; n < 2*N/blk-1; n++){

    memset(x_win, 0, sizeof(double)*blk);
    memset(y_win, 0, sizeof(double)*blk);
    memset(X, 0, sizeof(Cmplx)*blk);
    memset(Y, 0, sizeof(Cmplx)*blk);
   
    for(int i=0; i< blk ; i++){
      x_win[i] = x[i+blk/2*n];
      y_win[i] = y[i+blk/2*n];
    }

    //窓掛け
    Hamming_w(x_win);
    Hamming_w(y_win);
    
    //FFT
    fftw_ExecFwd(v1, x_win, X);
    fftw_ExecFwd(v2, y_win, Y);

    for(int k=0; k<blk; k++){
      X_sum[k].r += X[k].r;
      X_sum[k].i += X[k].i;

      Y_sum[k].r += Y[k].r;
      Y_sum[k].i += Y[k].i;
    }
    countSum ++;
  }
  
    for(int k=0; k<blk; k++){
      X_sum[k].r /= countSum ;
      X_sum[k].i /= countSum ;

      Y_sum[k].r /= countSum ;
      Y_sum[k].i /= countSum ;

      H[k].r = Y_sum[k].r;
      H[k].i = Y_sum[k].i;
    }

  /*
  //クロススペクトルの計算
  for(int k=0; k<blk; k++){
    H[k].r = (Y_sum[k].r*X_sum[k].r + Y_sum[k].i*X_sum[k].i) / (X_sum[k].r*X_sum[k].r + X_sum[k].i*X_sum[k].i);
    H[k].i = (X_sum[k].i*Y_sum[k].r - Y_sum[k].i*X_sum[k].r ) / (X_sum[k].r*X_sum[k].r + X_sum[k].i*X_sum[k].i);
  }
  */
    
  printf("%d回\n", countSum);
  end = clock();
  sumC += end-start;
  printf("fft計算時間: %f秒\n", (float)sumC / CLOCKS_PER_SEC);


  printf("クロススペクトル計算完了\n");

  fftw_ExecInv(v3, h, H); //IFFT

  printf("伝達関数　計算完了\n");

  fftw_Close(v1);
  fftw_Close(v2);
  fftw_Close(v3);
  free(X);
  free(Y);
  free(X_sum);
  free(Y_sum);
  free(x_win);
  free(y_win);

  return 0;
    
}

///////////////////////////////////////////////////////////////////////////////////mode1
int mode1(Opt *op){
  SF_INFO sfo,sfo2;

  //オプション（モード２のみ）のエラー処理
  char *FileCountRange = opt_char(op, "-No", "0", "解析するファイルの番号"); 
  int subMode = opt_int(op, "-subMode", 0, "1:前後のみ、2：上下のみ、3：前後と上下");
  blk = opt_int(op, "-blk", 0, "ブロック数");
  // if(inFileName == "NoMessage") printf("オプション-xの指定がありません。\n");
  if(FileCountRange == "0"){
    printf("オプション-Noの指定がありません。\n");
    return -1;
  }
  if(subMode == 0){
    printf("オプション-subModeの指定がありません\n");
    return -1;
  }
  if(blk == 0){
    printf("オプション　-blkの指定がありません。\n");
    return -1;
  }
    
  printf("FFT点数　%d\n\n", blk);

  int fileStartCount = 0; //解析ファイルのはじめのファイル番号
  int fileLastCount = 0; //解析ファイルの終わりのファイル番号
  int fileCountFlag = 0;
  int startFileDigit = 0;
  int lastFileDigit = 0; 
 
  for(int i=0; i< strlen(FileCountRange); i++){
    if(FileCountRange[i] == '-' && fileCountFlag==0) {
      fileCountFlag = 1;
      i++;
    }else if(FileCountRange[i]>= '0'&& FileCountRange[i]<= '9' && fileCountFlag == 0){
      fileStartCount *= 10;
      fileStartCount += FileCountRange[i] - '0';
      startFileDigit ++;
    }
    if(fileCountFlag == 1 && FileCountRange[i]>= '0'&& FileCountRange[i]<= '9'){
      fileLastCount *=10;
      fileLastCount += FileCountRange[i] - '0';
      lastFileDigit ++; 
    }
  }

  if(fileStartCount > fileLastCount){
    int i = fileStartCount;
    fileStartCount = fileLastCount;
    fileLastCount = i;
    int e = startFileDigit;
    startFileDigit = lastFileDigit;
    lastFileDigit = e;
  }


  printf("No%dから", fileStartCount);
  printf("No%dまで計算を行う\n\n", fileLastCount);
  
  FILE *learningText_pointer; //機械学習に使用するラベル付されたテキスト
  char *learningFileName=(char*) malloc(sizeof(char)*(12+startFileDigit+lastFileDigit));
  if(learningFileName == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
  FILE *learningLabel_pointer; //機械学習に使用するラベル付されたテキスト
  char *learningLabelName=(char*) malloc(sizeof(char)*(12+startFileDigit+lastFileDigit));
  if(learningLabelName == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
  FILE *learningVal_pointer; //機械学習に使用するバリデーションテキスト
  char *learningValName=(char*) malloc(sizeof(char)*(12+startFileDigit+lastFileDigit));
  if(learningValName == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
    
  switch(subMode){
  case 1:
    sprintf(learningFileName, "LF%d-%d_mu.txt", fileStartCount, fileLastCount );
    sprintf(learningLabelName, "LL%d-%d_mu.txt", fileStartCount, fileLastCount );
    sprintf(learningValName, "LV%d-%d_mu.txt", fileStartCount, fileLastCount );
    break;
  case 2:
    sprintf(learningFileName, "LF%d-%d_us.txt", fileStartCount, fileLastCount);
    sprintf(learningLabelName, "LL%d-%d_us.txt", fileStartCount, fileLastCount );
    sprintf(learningValName, "LV%d-%d_us.txt", fileStartCount, fileLastCount );
    break;
  case 3:
    sprintf(learningFileName, "LF%d-%d_muus.txt", fileStartCount, fileLastCount);
    sprintf(learningLabelName, "LL%d-%d_muus.txt", fileStartCount, fileLastCount );
    sprintf(learningValName, "LV%d-%d_muus.txt", fileStartCount, fileLastCount );
    break;
  }
     
  char *path_learningDataText = learningDataTextFolderPath; 
  char learningDataBuff[strlen(learningFileName)+strlen(path_learningDataText)+1];
  sprintf(learningDataBuff, "%s/%s", path_learningDataText, learningFileName);

  char *path_learningLabelText = learningLabelTextFolderPath; 
  char learningLabelBuff[strlen(learningLabelName)+strlen(path_learningLabelText)+1+1];
  sprintf(learningLabelBuff, "%s/%s", path_learningLabelText, learningLabelName);

  char *path_learningValText = learningValTextFolderPath;
  char learningValBuff[strlen(learningValName)+strlen(path_learningValText)+1+1];
  sprintf(learningValBuff, "%s/%s", path_learningValText, learningValName);

  learningText_pointer = fopen(learningDataBuff, "w");
  learningLabel_pointer = fopen(learningLabelBuff, "w");
  learningVal_pointer = fopen(learningValBuff, "w");


  //smoothing
  int outputleng_Start = blk*8/44; 
  int outputleng =blk*15/44;
  int plotValue = 12;
  int skipValue = (outputleng - outputleng_Start) / plotValue ; //抽出したいデータ数からスキップ数を求める

  skipValue = 0; //スキップ数を直接指定する。


  ////
  //printf("%d\n", outputleng - outputleng_Start);
  //double* savtky = CalcSavGolCoeff(outputleng - outputleng_Start, 3, 0); //第三引数は０で固定する（特徴が伝達関数化から大きく離れるため）
  //double* savtky = CalcSavGolCoeff(163, 3, 0); 
   
   
  double* savtky =(double*)malloc(sizeof(double)*(outputleng-outputleng_Start));
  for(int i=0; i<outputleng-outputleng_Start; i++){
    savtky[i] = 1.0f;
  }    
  

  /*
    for(int i=outputleng_Start; i<outputleng; i++){
    printf("%f\n",savtky[i]); 
    }
    printf("\n");
  */
    

  //ここからfileNumberのループ

  //現在解析しているファイルんのナンバー
  for(int fileCounter = fileStartCount; fileCounter<=fileLastCount; fileCounter++){

    /*
      int fileCounterDigit = 0;
      int fileCounterDigitSub = 0;
      while(fileCounterDigitSub / 10){
      fileCounterDigitSub /= 10;
      fileCounterDigit ++;
      }
      char *FileNumber = (char*)malloc(sizeof(char)*fileCounterDigit);
      if(FileNumber == NULL){
      printf("malloc rdata error\n");
      return -1;
      }
    */

    char *FileNumber = (char*)malloc(sizeof(char)*lastFileDigit+1);
    if(FileNumber == NULL){
      printf("malloc rdata error\n");
      return -1;
    }

    sprintf(FileNumber, "%d", fileCounter);

    //ファイルオープン
    //出力するデータのテキストファイル
    char *FileName=(char*) malloc(sizeof(char)*(14+strlen(FileNumber)) );
    if(FileName == NULL){
      printf("malloc rdata error\n");
      return -1;
    }
    printf("strlen %ld \n",strlen(sndFolPath_IN) );
    char *inFileName = (char*) malloc(sizeof(char)*(14+strlen(sndFolPath_IN)+strlen(FileNumber)) );
    char *in2FileName = (char*) malloc(sizeof(char)*(14+strlen(sndFolPath_IN)+strlen(FileNumber)) );
     if(inFileName == NULL){
      printf("malloc inFileName error\n");
      return -1;
    }
     if(in2FileName == NULL){
      printf("malloc in2FileName error\n");
      return -1;
    }
    char *rl_Name = (char*)malloc(sizeof(char)*7);
    if(rl_Name == NULL){
      printf("malloc rdata error\n");
      return -1;
    }
    char *IFFT_FileName=(char*) malloc(sizeof(char)*(17+strlen(FileNumber)) );
    if(IFFT_FileName == NULL){
      printf("malloc rdata error\n");
      return -1;
    }
    

    SNDFILE *isf, *isf2;
    FILE *pipe_pointer = popen("gnuplot -persist", "w");
    FILE *InXpipe_pointer = popen("gnuplot -persist", "w");
    
    if(fileStartCount == fileCounter || fileLastCount == fileCounter){
      fprintf(pipe_pointer, "set xlabel '周波数(Hz)'\n");
      fprintf(pipe_pointer, "set ylabel '音圧レベル(dB)'\n");
      fprintf(pipe_pointer, "plot [-1000:]");

      fprintf(InXpipe_pointer, "set xlabel '時間(μsec)'\n");
      fprintf(InXpipe_pointer, "set ylabel '音圧(Pa)'\n");
      fprintf(InXpipe_pointer, "plot [-1000:] ");

    }
    
    int label = 0;
    //int label = 2;

    int rl_count = 0;
    int rl_last =0;

    switch(subMode){
    case 1:
      rl_count = 1;
      rl_last = 2;
      break;
    case 2:
      rl_count = 3;
      rl_last = 4;
      break;
    case 3:
      rl_count = 1;
      rl_last = 4;
      break;
    }


    //上下前後の切り替え
    for(rl_count; rl_count <= rl_last; rl_count++){

      //前後上下のきりかえ
      switch(rl_count){
      case 1:
        rl_Name = "mae";
        break;
      case 2:
        rl_Name = "usiro";
        break;
      case 3:
        rl_Name = "ue";
        break;
      case 4:
        rl_Name = "sita";
        break;
      }
      
      //音源ファイルの読み込み
      sprintf(inFileName, "%s/k%s_%s_r.wav", sndFolPath_IN, FileNumber, rl_Name);
      printf("ｘの観測音源のファイル名　%s\n", inFileName);
      isf = sf_open(inFileName, SFM_READ, &sfo);
      if( isf == NULL ){
        fputs( "ｘのファイルオープンに失敗しました。\n", stderr );
        exit( EXIT_FAILURE );
      }
      
      sprintf(in2FileName, "%s/k%s_%s_l.wav",sndFolPath_IN, FileNumber, rl_Name);
      printf("ｙの観測音源のファイル名　%s\n", in2FileName);
      isf2 = sf_open(in2FileName, SFM_READ, &sfo2);
      if( isf2 == NULL ){
        fputs( "ｙのファイルオープンに失敗しました。\n", stderr );
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
      
      //変数の準備
      double *x, *y, *h;

      x = (double*)malloc(sizeof(double)*N);
      y = (double*)malloc(sizeof(double)*N);
      h = (double*)malloc(sizeof(double)*blk);
      if(x == NULL){
        printf("malloc rdata error x\n");
        return -1;
      }
      if(y == NULL){
        printf("malloc rdata error y\n");
        return -1;
      }
      if(h == NULL){
        printf("malloc rdata error h\n");
        return -1;
      }
      memset(x, 0, sizeof(double)*N);
      memset(y, 0, sizeof(double)*N);
      memset(h, 0, sizeof(double)*blk);

      Cmplx *H;
      H = (Cmplx*)malloc(sizeof(Cmplx)*blk);

      if(H == NULL){
        printf("malloc rdata error H\n");
        return -1;
      }
      memset(H, 0, sizeof(Cmplx)*blk);
    

      sprintf(FileName, "o%s_%s.txt", FileNumber, rl_Name);
      printf("出力ファイル名 %s\n", FileName);
      sprintf(IFFT_FileName, "IFFT%s_%s.txt", FileNumber, rl_Name);
      
      char *path_text = textFolderPath;
      char *path_IFFT = IFFTtextFolderPath;
      char buff[strlen(FileName)+5];
      char buffIFFT[strlen(IFFT_FileName)+4+5];
      sprintf(buff, "%s/%s", path_text, FileName);
      sprintf(buffIFFT, "%s/%s", path_IFFT, IFFT_FileName);

      FILE *text_pointer = fopen(buff, "w");
      FILE *InXtext_pointer = fopen(buffIFFT, "w");

      //音源の読み込み
      sf_read_double(isf, x, N);
      sf_read_double(isf2, y, N);

      //解析開始
      Analysis(x, y, H, h);
    
      printf("計算終了。ただいま出力中\n");

      for(int k=0; k<blk; k++){
        
        fprintf(InXtext_pointer,"%d %f \n", k, h[k]/blk);

      }
      
      int savCount = 0;
      for(int k=outputleng_Start; k<outputleng; k+=(skipValue+1)){

	      fprintf(text_pointer,"%d %f \n",  44100/blk*k, savtky[savCount]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));

        //学習用出力ファイル（ラベル付き）の書き込み
        //fprintf(learningText_pointer,"%f ", savtky[k]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));

        
        //教師データとテストデータを分ける
        if( fileCounter % 2 == 1){
          fprintf(learningText_pointer,"%f ", savtky[savCount]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));
        }else if( fileCounter % 4 == 0){
          fprintf(learningLabel_pointer,"%f ", savtky[savCount]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));
        }else if(fileCounter % 4 == 2){
          fprintf(learningVal_pointer,"%f ", savtky[savCount]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));
        }
        
        savCount++;
      }

      /*
      fprintf(learningText_pointer,"%d\n",label);
      //fprintf(learningText_pointer,"%d\n",2);
      fprintf(learningLabel_pointer,"%d\n",label);
      */
      
      //教師データとテストデータを分ける
      if( fileCounter % 2 == 1){
	      fprintf(learningText_pointer,"%d\n",label);
      }else if( fileCounter % 4 == 0){
	      fprintf(learningLabel_pointer,"%d\n",label);
      }else if(fileCounter % 4 == 2){
          fprintf(learningVal_pointer, "%d\n", label);
      }
      
      label ++; //次のラベルにする
     
      /*
      //はじめと最後だけグラフを出力する。確認用
      if(fileStartCount == fileCounter || fileLastCount == fileCounter){
      fprintf(pipe_pointer, "\"o%s_%s.txt\" u 1:2 with l,", FileNumber, rl_Name);
      fprintf(InXpipe_pointer, "\"IFFT%s_%s.txt\" u 1:2 with l,", FileNumber, rl_Name);
      }
      */
      //はじめと最後だけグラフを出力する。確認用
      if(fileStartCount == fileCounter || fileLastCount == fileCounter){
        fprintf(pipe_pointer, "\"%s\" u 1:2 with l,", buff);
        fprintf(InXpipe_pointer, "\"%s\" u 1:2 with l,", buffIFFT);
      }
    
      printf("plot [-1000:] \"%s\" u 1:2 with l\n\n\n\n", buff);

      free(x);
      free(y);
      free(h);
      free(H);
      sf_close(isf);
      sf_close(isf2);
      fclose(text_pointer);
      fclose(InXtext_pointer);
    }


    if(fileStartCount == fileCounter || fileLastCount == fileCounter){
      fprintf(pipe_pointer, "\n");
      fprintf(pipe_pointer, "q\n");

      fprintf(InXpipe_pointer, "\n");
      fprintf(InXpipe_pointer, "q\n");
    }
    
    /*↓これ短くできる。　書き込んでいるファイルから文字列で読み込んでそのまま出力すれば分岐いらない*/
    switch(subMode){
    case 1:
      printf("plot [-1000:] \"o%s_mae.txt\" u 1:2 with l, \"o%s_usiro.txt\" u 1:2 with l\n",FileNumber, FileNumber);
      printf("\n");
      printf("plot [-1000:] \"IFFT%s_mae.txt\" u 1:2 with l, \"IFFT%s_usiro.txt\" u 1:2 with l\n",FileNumber, FileNumber);
      break;

    case 2:
      printf("plot [-1000:] \"o%s_ue.txt\" u 1:2 with l, \"o%s_sita_.txt\" u 1:2 with l\n",FileNumber, FileNumber);
      printf("\n");
      printf("plot [-1000:] \"IFFT%s_ue.txt\" u 1:2 with l, \"IFFT%s_sita.txt\" u 1:2 with l\n",FileNumber, FileNumber);
      break;

    case 3:
      printf("plot [-1000:] \"o%s_mae.txt\" u 1:2 with l, \"o%s_usiro.txt\" u 1:2 with l, \"o%s_ue.txt\" u 1:2 with l, \"o%s_sita.txt\" u 1:2 with l\n",FileNumber, FileNumber, FileNumber, FileNumber);
      printf("\n");
      printf("plot [-1000:] \"IFFT%s_mae.txt\" u 1:2 with l, \"IFFT%s_usiro.txt\" u 1:2 with l, \"IFFT%s_ue.txt\" u 1:2 with l, \"IFFT%s_sita.txt\" u 1:2 with l\n",FileNumber, FileNumber, FileNumber, FileNumber);
      break;
    }

    //free(rl_Name);
   
    pclose(pipe_pointer);
    pclose(InXpipe_pointer);
    free(in2FileName);
    free(inFileName);
    free(FileNumber);
    free(FileName);

  
    // fclose(InYtext_pointer);

  
  }//fileNumberのループ
  printf("学習用出力ファイル名 %s\n", learningFileName);

  free(learningFileName);
  fclose(learningText_pointer);
  free(learningLabelName);
  fclose(learningLabel_pointer);
  free(learningValName);
  fclose(learningVal_pointer);
  free(savtky);

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////mode2
int mode2(Opt *op){
  SF_INFO sfo,sfo2;
  //オプション（モード２の)の判定 ///
  char *inFileName = opt_char(op, "-x", "NoMessage", "ｘの音源ファイル場所"); 
  char *in2FileName = opt_char(op, "-y", "NoMessage" , "観測音源のsoundfile名");
  char *FileName = opt_char(op, "-txt", "NoMessage", "出力するtextファイル名");
  blk = opt_int(op, "-blk", 0, "ブロック数");

  if(inFileName == "NoMessage") printf("オプション-xの指定がありません。\n");
  if(in2FileName == "NoMessage") printf("オプション-yの指定がありません。\n");
  if(FileName == "NoMessage"){
    printf("オプション-txtの指定がありません。\n");
    return 0;
  }
  if(blk == 0) printf("オプション　-blkの指定がありません。\n");
    
  //オプション（モード２の）の判定　おわり　/////
    
  //変数の準備//////
  SNDFILE *isf = sf_open(inFileName, SFM_READ, &sfo);
  if( isf == NULL ){
    fputs( "ｘのファイルオープンに失敗しました。\n", stderr );
    exit( EXIT_FAILURE );
  }
  SNDFILE *isf2 = sf_open(in2FileName, SFM_READ, &sfo2);
  if( isf2 == NULL ){
    fputs( "ｙのファイルオープンに失敗しました。\n", stderr );
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
  double *x, *y, *h;

  x = (double*)malloc(sizeof(double)*N);
  y = (double*)malloc(sizeof(double)*N);
  h = (double*)malloc(sizeof(double)*blk);
  if(x == NULL){
    printf("malloc rdata error x\n");
    return -1;
  }
  if(y == NULL){
    printf("malloc rdata error y\n");
    return -1;
  }
  if(h == NULL){
    printf("malloc rdata error h\n");
    return -1;
  }
  memset(x, 0, sizeof(double)*N);
  memset(y, 0, sizeof(double)*N);
  memset(h, 0, sizeof(double)*blk);

  Cmplx *H = (Cmplx*)malloc(sizeof(Cmplx)*blk);

  if(H == NULL){
    printf("malloc rdata error H\n");
    return -1;
  }
  memset(H, 0, sizeof(Cmplx)*blk);
    
  char *IFFT_FileName =(char*) malloc(sizeof(char)*(5+strlen(FileName)));
  if(IFFT_FileName == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
  sprintf(IFFT_FileName, "IFFT_%s", FileName);
  printf("%s\n", IFFT_FileName);

  char *path_text = textFolderPath;
  char *path_IFFT = IFFTtextFolderPath;
  char buff[strlen(FileName)];
  char buffIFFT[strlen(IFFT_FileName)];
  sprintf(buff, "%s/%s", path_text, FileName);
  sprintf(buffIFFT, "%s/%s", path_IFFT, IFFT_FileName);
    
  FILE *text_pointer = fopen(buff, "w");
  FILE *InXtext_pointer = fopen(buffIFFT, "w");
   
  FILE *pipe_pointer;
  FILE *InXpipe_pointer;

  double *savitzkey  = CalcSavGolCoeff(blk/2, 5, 0);

  sf_read_double(isf, x, N);
  sf_read_double(isf2, y, N);

  //解析開始
  Analysis(x, y, H, h);

  for(int k=0; k<blk/2; k++){
    //fprintf(text_pointer,"%d %f \n", 44100/blk*k, savitzkey[k]*10*log10( H[k].r*H[k].r + H[k].i*H[k].i));
    //printf("%d %f \n", 44100/blk*k, 10.0*log10( H[k].r*H[k].r + H[k].i*H[k].i));
    fprintf(text_pointer,"%d %f \n", 44100/blk*k, 10.0*log10( H[k].r*H[k].r + H[k].i*H[k].i) );
    //fprintf(text_pointer,"%d %f \n", k, sqrt( H[k].r*H[k].r + H[k].i*H[k].i));
    //fprintf(text_pointer,"%d %f \n", k,  H[k].r);
  }
  
  for(int k=0; k<blk/2; k++){
    fprintf(InXtext_pointer,"%d %f \n", k, h[k]/blk);
    //fprintf(InYtext_pointer,"%d %f \n", k, y[k]);
  }
  
 
  printf("計算終了。ただいま出力中\n"); 
  //fclose(InYtext_pointer);
  
 

  printf("plot [-100:] \"%s\" u 1:2 with l\n",buff);
  printf("plot [-100:] \"%s\" u 1:2 with l\n",buffIFFT);
  
  pipe_pointer = popen("gnuplot -persist", "w");
  fprintf(pipe_pointer, "set xlabel '周波数(Hz)'\n");
  fprintf(pipe_pointer, "set ylabel '音圧レベル(dB)'\n");
  fprintf(pipe_pointer, "plot [-100:]\"%s\" u 1:2 with l\n",buff);
  fprintf(pipe_pointer, "q\n");
   
  InXpipe_pointer = popen("gnuplot -persist", "w");
  fprintf(InXpipe_pointer, "set xlabel '時間(μsec)'\n");
  fprintf(InXpipe_pointer, "set ylabel '音圧(Pa)'\n");
  fprintf(InXpipe_pointer, "plot [-100:] \"%s\" u 1:2 with l linetype 3\n ", buffIFFT);
  //fprintf(InXpipe_pointer, ", \"%s\" u 1:2 with l linetype 4 \n","InY.txt");
  fprintf(InXpipe_pointer, "q\n");

  // plot "InX.txt" u 1:2 with l linetype 3, "InY.txt" u 1:2 with l linetype 4 
  pclose(pipe_pointer);
  pclose(InXpipe_pointer);
  free(x);
  free(y);
  free(h);
  free(H);
  sf_close(isf);
  sf_close(isf2);
  free(savitzkey);
  fclose(text_pointer);
  fclose(InXtext_pointer);

  return 0;
}

/////////////////////////////////////////////////////////////////　ls_gnuplot関数(mode 3)
int ls_gnuplot(){

  printf("下記の番号から１つ入力してください\n"
  /*
	 "1:mae\n"
	 "2:usiro\n"
	 "3:ue\n"
	 "4:sita\n"
	 "5:IFFT mae\n"
	 "6:IFFT usiro\n"
	 "7:IFFT ue\n"
	 "8:IFFT sita\n"
  */
    "1:mae_r\n"
    "2:mae_l\n"
    "3:usiro_r\n"
    "4:usiro_l\n"

	 );

  int swtch_Number =0;
  scanf("%d", &swtch_Number);

  /*
  char *rl_Name = (char*)malloc(sizeof(char)*9);
  if(rl_Name == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
  */
 char *rl_Name = NULL;

  //前後の左右きりかえ
  switch(swtch_Number){
    /*
  case 1:
    rl_Name = "mae.txt";
    break;
  case 2:
    rl_Name = "usiro.txt";
    break;
  case 3:
    rl_Name = "ue.txt";
    break;
  case 4:
    rl_Name = "sita.txt";
    break;
  case 5:
    rl_Name = "mae.txt"; //IFFEの前
    break;
  case 6:
    rl_Name = "usiro.txt"; //IFFEの後ろ
    break;
  case 7:
    rl_Name = "ue.txt"; //IFFEの上
    break;
  case 8:
    rl_Name = "sita.txt"; //IFFEの下
    break;
    */
   case 1:
    rl_Name = "mae_r.txt";
    break;
  case 2:
    rl_Name = "mae_l.txt"; //IFFEの前
    break;
  case 3:
    rl_Name = "usiro_r.txt"; //IFFEの後ろ
    break;
  case 4:
    rl_Name = "usiro_l.txt";
    break;

  }
 
 
  FILE *pipe_pointer= popen("gnuplot -persist", "w");
  //fprintf(pipe_pointer, "plot [-1000:]");
  //printf("plot [-1000:]");
  fprintf(pipe_pointer, "plot [-1000:] ");
  printf("plot [-1000:] ");
  char *path_text = textFolderPath;
  char *path_IFFT = IFFTtextFolderPath;
  char buff[1+5];
  char buffIFFT[1+9];
  sprintf(buff, "%s", path_text);
  sprintf(buffIFFT, "%s", path_IFFT);

  DIR* dp = NULL;
  if(swtch_Number>=1 && swtch_Number<=4){
    dp = opendir(buff);
  }else if(swtch_Number>=5 && swtch_Number<=8){
    dp = opendir(buffIFFT);
  }

  if (dp != NULL){
    struct dirent* de;
    do{
      de = readdir(dp);
      if(de != NULL)
	if(strstr(de->d_name, rl_Name )!= NULL){
	  if(de->d_name[0] != 'I' && swtch_Number <=4 && swtch_Number >= 1){
	    if(strcmp(rl_Name, strstr(de->d_name, rl_Name)) == 0){
	      printf("\"%s/%s\" u 1:2 with l,",buff, de->d_name);
	      fprintf(pipe_pointer, "\"%s/%s\" u 1:2 with l,",buff, de->d_name);
	    }
	  }else if(de->d_name[0] == 'I'&& swtch_Number >= 5 && swtch_Number <= 8){
	    if(strcmp(rl_Name, strstr(de->d_name, rl_Name)) == 0){
	      printf("\"%s/%s\" u 1:2 with l,",buffIFFT, de->d_name);
	      fprintf(pipe_pointer, "\"%s/%s\" u 1:2 with l,",buffIFFT, de->d_name);
	    }
	  }
	}
    }while(de != NULL);
    closedir(dp);
    fprintf(pipe_pointer, "\n");
    printf("\n");
    fprintf(pipe_pointer, "q\n");
  }else
    printf("error\n");
  pclose(pipe_pointer);
  
  return 0;
}


/////////////////////////////////////////////////////////////////////////////mode4
int mode4(Opt *op){
  
  SF_INFO sfo,sfo2;

  char *FileCountRange = opt_char(op, "-No", "0", "解析するファイルの番号"); 
  int subMode = opt_int(op, "-subMode", 0, "1:前後のみ、2：上下のみ、3：前後と上下");
  blk = opt_int(op, "-blk", 0, "ブロック数");
  // if(inFileName == "NoMessage") printf("オプション-xの指定がありません。\n");
  if(FileCountRange == "0"){
    printf("オプション-Noの指定がありません。\n");
    return -1;
  }
  if(subMode == 0){
    printf("オプション-subModeの指定がありません\n");
    return -1;
  }
  if(blk == 0){
    printf("オプション　-blkの指定がありません。\n");
    return -1;
  }
    
  printf("FFT点数　%d\n\n", blk);

  int fileStartCount = 0; //解析ファイルのはじめのファイル番号
  int fileLastCount = 0; //解析ファイルの終わりのファイル番号
  int fileCountFlag = 0;
  int startFileDigit = 0;
  int lastFileDigit = 0; 
 
  for(int i=0; i< strlen(FileCountRange); i++){
    if(FileCountRange[i] == '-' && fileCountFlag==0) {
      fileCountFlag = 1;
      i++;
    }else if(FileCountRange[i]>= '0'&& FileCountRange[i]<= '9' && fileCountFlag == 0){
      fileStartCount *= 10;
      fileStartCount += FileCountRange[i] - '0';
      startFileDigit ++;
    }
    if(fileCountFlag == 1 && FileCountRange[i]>= '0'&& FileCountRange[i]<= '9'){
      fileLastCount *=10;
      fileLastCount += FileCountRange[i] - '0';
      lastFileDigit ++; 
    }
  }

  if(fileStartCount > fileLastCount){
    int i = fileStartCount;
    fileStartCount = fileLastCount;
    fileLastCount = i;
    int e = startFileDigit;
    startFileDigit = lastFileDigit;
    lastFileDigit = e;
  }


  printf("No%dから", fileStartCount);
  printf("No%dまで計算を行う\n\n", fileLastCount);

  //////////////////////変更箇所　1/2
  
  //textpointer R and L 
  //filename R and L 
  FILE *learningText_L_pointer; //機械学習に使用するラベル付されたテキスト
  char *learningFileName_L=(char*) malloc(sizeof(char)*(13+startFileDigit+lastFileDigit));
  if(learningFileName_L == NULL){
    printf("malloc rdata error\n");
    return -1;
  }

  FILE *learningText_R_pointer; 
  char *learningFileName_R=(char*) malloc(sizeof(char)*(13+startFileDigit+lastFileDigit));
  if(learningFileName_R == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
  
  //textpointer R and L 
  FILE *learningLabel_L_pointer; //機械学習に使用するラベル付されたテキスト
  char *learningLabelName_L=(char*) malloc(sizeof(char)*(13+startFileDigit+lastFileDigit));
  if(learningLabelName_L == NULL){
    printf("malloc rdata error\n");
    return -1;
  }

  FILE *learningLabel_R_pointer; 
  char *learningLabelName_R=(char*) malloc(sizeof(char)*(13+startFileDigit+lastFileDigit));
  if(learningLabelName_R == NULL){
    printf("malloc rdata error\n");
    return -1;
  }

  FILE *learningVal_L_pointer; //機械学習に使用するバリデーションテキスト
  char *learningValName_L=(char*) malloc(sizeof(char)*(13+startFileDigit+lastFileDigit));
  if(learningValName_L == NULL){
    printf("malloc rdata error\n");
    return -1;
  }

  FILE *learningVal_R_pointer; 
  char *learningValName_R=(char*) malloc(sizeof(char)*(13+startFileDigit+lastFileDigit));
  if(learningValName_R == NULL){
    printf("malloc rdata error\n");
    return -1;
  }
  
  //textpointer R and L 
  switch(subMode){
  case 1:
    sprintf(learningFileName_L, "LF%d-%d_mu_l.txt", fileStartCount, fileLastCount );
    sprintf(learningFileName_R, "LF%d-%d_mu_r.txt", fileStartCount, fileLastCount );
    
    sprintf(learningLabelName_L, "LL%d-%d_mu_l.txt", fileStartCount, fileLastCount );
    sprintf(learningLabelName_R, "LL%d-%d_mu_r.txt", fileStartCount, fileLastCount );
    
    sprintf(learningValName_L, "LV%d-%d_mu_l.txt", fileStartCount, fileLastCount );
    sprintf(learningValName_R, "LV%d-%d_mu_r.txt", fileStartCount, fileLastCount );
    break;
  case 2:
    sprintf(learningFileName_L, "LF%d-%d_us_l.txt", fileStartCount, fileLastCount);
    sprintf(learningFileName_R, "LF%d-%d_us_r.txt", fileStartCount, fileLastCount);
    
    sprintf(learningLabelName_L, "LL%d-%d_us_l.txt", fileStartCount, fileLastCount );
    sprintf(learningLabelName_R, "LL%d-%d_us_r.txt", fileStartCount, fileLastCount);

    sprintf(learningValName_L, "LV%d-%d_us_l.txt", fileStartCount, fileLastCount );
    sprintf(learningValName_R, "LV%d-%d_us_r.txt", fileStartCount, fileLastCount );
    break;
    /*
  case 3:
    sprintf(learningFileName_L, "LF%d-%d_muus_l.txt", fileStartCount, fileLastCount );
    sprintf(learningFileName_R, "LF%d-%d_muus_r.txt", fileStartCount, fileLastCount );
    
    sprintf(learningLabelName_L, "LL%d-%d_muus_l.txt", fileStartCount, fileLastCount );
    sprintf(learningLabelName_R, "LL%d-%d_muus_r.txt", fileStartCount, fileLastCount );
    
    sprintf(learningValName_L, "LV%d-%d_muus_l.txt", fileStartCount, fileLastCount );
    sprintf(learningValName_R, "LV%d-%d_muus_r.txt", fileStartCount, fileLastCount );
    break;
    */
  }
    
  //textpointer R and L 
  // add path
  char *path_learningData_L_Text = learningDataTextFolderPath; 
  char learningData_L_Buff[strlen(learningFileName_L)+strlen(path_learningData_L_Text)+1+1];
  sprintf(learningData_L_Buff, "%s/%s", path_learningData_L_Text, learningFileName_L);

  char *path_learningData_R_Text = learningDataTextFolderPath; 
  char learningData_R_Buff[strlen(learningFileName_R)+strlen(path_learningData_R_Text)+1+1];
  sprintf(learningData_R_Buff, "%s/%s", path_learningData_R_Text, learningFileName_R);

  char *path_learningLabel_L_Text = learningLabelTextFolderPath; 
  char learningLabel_L_Buff[strlen(learningLabelName_L)+strlen(path_learningLabel_L_Text)+1+1];
  sprintf(learningLabel_L_Buff, "%s/%s", path_learningLabel_L_Text, learningLabelName_L);

  char *path_learningLabel_R_Text = learningLabelTextFolderPath; 
  char learningLabel_R_Buff[strlen(learningLabelName_R)+strlen(path_learningLabel_R_Text)+1+1];
  sprintf(learningLabel_R_Buff, "%s/%s", path_learningLabel_R_Text, learningLabelName_R);

  char *path_learningVal_L_Text = learningValTextFolderPath;
  char learningVal_L_Buff[strlen(learningValName_L)+strlen(path_learningVal_L_Text)+1+1];
  sprintf(learningVal_L_Buff, "%s/%s", path_learningVal_L_Text, learningValName_L);

  char *path_learningVal_R_Text = learningValTextFolderPath;
  char learningVal_R_Buff[strlen(learningValName_R)+strlen(path_learningVal_R_Text)+1+1];
  sprintf(learningVal_R_Buff, "%s/%s", path_learningVal_R_Text, learningValName_R);
  
  //textpointer R and L
  learningText_L_pointer = fopen(learningData_L_Buff, "w");
  learningText_R_pointer = fopen(learningData_R_Buff, "w");
  learningLabel_L_pointer = fopen(learningLabel_L_Buff, "w");
  learningLabel_R_pointer = fopen(learningLabel_R_Buff, "w");
  learningVal_L_pointer = fopen(learningVal_L_Buff, "w");
  learningVal_R_pointer = fopen(learningVal_R_Buff, "w");

  //smoothing
  int outputleng_Start = blk*8/44; 
  int outputleng =blk*12/44;
  int plotValue = 12;
  int skipValue = (outputleng - outputleng_Start) / plotValue ; //抽出したいデータ数からスキップ数を求める

  skipValue = 0; //スキップ数を直接指定する。


  ///平滑化	
  ////
  //printf("%d\n", outputleng - outputleng_Start);
  //double* savtky = CalcSavGolCoeff(outputleng - outputleng_Start, 3, 0); //第三引数は０で固定する（特徴が伝達関数化から大きく離れるため）
  
   
   
  double* savtky =(double*)malloc(sizeof(double)*(outputleng-outputleng_Start));
  for(int i=0; i<outputleng-outputleng_Start; i++){
    savtky[i] = 1.0f;
  }    
  

  /*
    for(int i=outputleng_Start; i<outputleng; i++){
    printf("%f\n",savtky[i]); 
    }
    printf("\n");
  */
    

  //ここからfileNumberのループ

  //現在解析しているファイルんのナンバー
  for(int fileCounter = fileStartCount; fileCounter<=fileLastCount; fileCounter++){

    /*
      int fileCounterDigit = 0;
      int fileCounterDigitSub = 0;
      while(fileCounterDigitSub / 10){
      fileCounterDigitSub /= 10;
      fileCounterDigit ++;
      }
      char *FileNumber = (char*)malloc(sizeof(char)*fileCounterDigit);
      if(FileNumber == NULL){
      printf("malloc rdata error\n");
      return -1;
      }
    */

    char *FileNumber = (char*)malloc(sizeof(char)*lastFileDigit+1);
    if(FileNumber == NULL){
      printf("malloc rdata error\n");
      return -1;
    }

    sprintf(FileNumber, "%d", fileCounter);

    //ファイルオープン
    char *FileName=(char*) malloc(sizeof(char)*14+strlen(FileNumber));
    if(FileName == NULL){
      printf("malloc rdata error\n");
      return -1;
    }
 
    char *inFileName = (char*) malloc(sizeof(char)*(20+strlen(FileNumber)) );
    char *in2FileName = (char*) malloc(sizeof(char)*(14+strlen(sndFolPath_IN)+strlen(FileNumber)) );
    char *rl_Name = (char*)malloc(sizeof(char)*7);
    if(rl_Name == NULL){
      printf("malloc rdata error\n");
      return -1;
    }
    char *IFFT_FileName=(char*) malloc(sizeof(char)*17+strlen(FileNumber));
    if(IFFT_FileName == NULL){
      printf("malloc rdata error\n");
      return -1;
    }
    

    SNDFILE *isf, *isf2;
    FILE *pipe_pointer = popen("gnuplot -persist", "w");
    FILE *InXpipe_pointer = popen("gnuplot -persist", "w");
    
    if(fileStartCount == fileCounter || fileLastCount == fileCounter){
      fprintf(pipe_pointer, "set xlabel '周波数(Hz)'\n");
      fprintf(pipe_pointer, "set ylabel '音圧レベル(dB)'\n");
      fprintf(pipe_pointer, "plot [-1000:]");

      fprintf(InXpipe_pointer, "set xlabel '時間(μsec)'\n");
      fprintf(InXpipe_pointer, "set ylabel '音圧(Pa)'\n");
      fprintf(InXpipe_pointer, "plot [-1000:] ");

    }
    
    int label = 0;
    //int label = 2;

    int rl_count = 0;
    int rl_last =0;

    switch(subMode){
    case 1:
      rl_count = 1;
      rl_last = 4;
      break;
    case 2:
      rl_count = 5;
      rl_last = 8;
      break;
      /* case 3:
      rl_count = 1;
      rl_last = 8;
      break;*/
    }


    //上下前後の切り替え
    for(rl_count; rl_count <= rl_last; rl_count++){

      ///前後の左右きりかえ
      switch(rl_count){
      case 1:
        rl_Name = "mae_r";
      	label = 0;
        break;
      case 2:
        rl_Name = "mae_l";
	      label = 0;
        break;
      case 3:
        rl_Name = "usiro_r";
      	label = 1;
        break;
      case 4:
        rl_Name = "usiro_l";
      	label = 1;
        break;
      
      case 5:
        rl_Name = "ue_r";
	      label = 0;
        break;
      case 6:
        rl_Name = "ue_l";
      	label = 0;
        break;
      case 7:
        rl_Name = "sita_r";
      	label = 1;
        break;
      case 8:
        rl_Name = "sita_l";
	      label = 1;
        break;
      }
      
      //音源ファイルの読み込み
      sprintf(inFileName, "originalSound/m%s.wav", FileNumber);
      printf("ｘの観測音源のファイル名　%s\n", inFileName);
      isf = sf_open(inFileName, SFM_READ, &sfo);
      if( isf == NULL ){
        fputs( "ｘのファイルオープンに失敗しました。\n", stderr );
        exit( EXIT_FAILURE );
      }
      
      sprintf(in2FileName, "%s/k%s_%s.wav",sndFolPath_IN, FileNumber, rl_Name);
      printf("ｙの観測音源のファイル名　%s\n", in2FileName);
      isf2 = sf_open(in2FileName, SFM_READ, &sfo2);
      if( isf2 == NULL ){
        fputs( "ｙのファイルオープンに失敗しました。\n", stderr );
        exit( EXIT_FAILURE );
      }

      
      if(sfo.frames == sfo2.frames &&  sfo.channels == sfo2.channels){
        printf("ｘのサンプル数：%ld\n", sfo.frames);
        printf("ｙのサンプル数：%ld\n", sfo2.frames);
        printf("ｘのチャンネル数：%d\n", sfo.channels);
        printf("ｙのチャンネル数：%d\n", sfo2.channels);
      	N = sfo.frames;
      }else{
        printf("ｘのサンプル数：%ld\n", sfo.frames);
        printf("ｙのサンプル数：%ld\n", sfo2.frames);
        printf("ｘのチャンネル数：%d\n", sfo.channels);
        printf("ｙのチャンネル数：%d\n", sfo2.channels);
        printf("サンプル数またはチャンネル数が正しくありません。\n");
	//return -1;
        if(sfo.frames < sfo2.frames){
	       N = sfo.frames;
        }else if(sfo2.frames < sfo.frames){
	      N = sfo2.frames;
        }
      }
      
      //変数の準備
      double *x, *y, *h;

      x = (double*)malloc(sizeof(double)*N);
      y = (double*)malloc(sizeof(double)*N);
      h = (double*)malloc(sizeof(double)*blk);
      if(x == NULL){
        printf("malloc rdata error x\n");
        return -1;
      }
      if(y == NULL){
        printf("malloc rdata error y\n");
        return -1;
      }
      if(h == NULL){
        printf("malloc rdata error h\n");
        return -1;
      }
      memset(x, 0, sizeof(double)*N);
      memset(y, 0, sizeof(double)*N);
      memset(h, 0, sizeof(double)*blk);

      Cmplx *H;
      H = (Cmplx*)malloc(sizeof(Cmplx)*blk);

      if(H == NULL){
        printf("malloc rdata error H\n");
        return -1;
      }
      memset(H, 0, sizeof(Cmplx)*blk);
    

      sprintf(FileName, "o%s_%s.txt", FileNumber, rl_Name);
      printf("出力ファイル名 %s\n", FileName);
      sprintf(IFFT_FileName, "IFFT%s_%s.txt", FileNumber, rl_Name);
      
      char *path_text = textFolderPath;
      char *path_IFFT = IFFTtextFolderPath;
      char buff[strlen(FileName)+5];
      char buffIFFT[strlen(IFFT_FileName)+4+5];
      sprintf(buff, "%s/%s", path_text, FileName);
      sprintf(buffIFFT, "%s/%s", path_IFFT, IFFT_FileName);

      FILE *text_pointer = fopen(buff, "w");
      FILE *InXtext_pointer = fopen(buffIFFT, "w");

      //音源の読み込み
      sf_read_double(isf, x, N);
      sf_read_double(isf2, y, N);

      //解析開始
      Analysis(x, y, H, h);
    
      printf("計算終了。ただいま出力中\n");

      for(int k=0; k<blk; k++){
        
        fprintf(InXtext_pointer,"%d %f \n", k, h[k]/blk);

      }
      
      int savCount = 0;
      for(int k=outputleng_Start; k<outputleng; k+=(skipValue+1)){

	      fprintf(text_pointer,"%d %f \n",  44100/blk*k, savtky[savCount]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));

        //学習用出力ファイル（ラベル付き）の書き込み
        //fprintf(learningText_pointer,"%f ", savtky[k]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));

        //////////////////////////////////////////変更箇所 2/2
        //教師データとテストデータを分ける+バリューデータ
        if(fileCounter % 2 == 1){
          if(rl_count % 2 == 1){     //rl_count % 2 =1 -> R
	          fprintf(learningText_R_pointer, "%f ", savtky[savCount]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));
          }
          if(rl_count % 2 == 0){   //rl_count % 2 =0 -> L
	          fprintf(learningText_L_pointer, "%f ", savtky[savCount]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));
          }
        }else if(fileCounter % 4 == 0){
	        if(rl_count % 2 == 1){    //rl_count % 2 =1 -> R
	          fprintf(learningLabel_R_pointer, "%f ", savtky[savCount]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));
          }
	        if(rl_count % 2 == 0){    //rl_count % 2 =0 -> L
	          fprintf(learningLabel_L_pointer, "%f ", savtky[savCount]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));
            
          }
        }else if(fileCounter % 4 == 2){
	        if(rl_count % 2 == 1){
	          fprintf(learningVal_R_pointer,"%f ", savtky[savCount]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));
          }
	        if(rl_count % 2 == 0){
	          fprintf(learningVal_L_pointer,"%f ", savtky[savCount]*10*log10(H[k].r*H[k].r + H[k].i*H[k].i));
          }
      	} 
        savCount++;
      }

      /*
	fprintf(learningText_pointer,"%d\n",label);
	//fprintf(learningText_pointer,"%d\n",2);
	fprintf(learningLabel_pointer,"%d\n",label);
      */
      
      //教師データとテストデータを分ける+バリューデータ
      if(fileCounter % 2 == 1){
        //rl_count % 2 =1 -> R
        if(rl_count % 2 == 1){ 
          fprintf(learningText_R_pointer,"%d\n",label);	
          // printf("rl_count 教師データ右\n");
        }
        if(rl_count % 2 == 0){    //rl_count % 2 =0 -> L
          fprintf(learningText_L_pointer,"%d\n",label);
          //printf("rl_count 教師データ左\n");
        }

      }if(fileCounter % 4 == 0){
        if(rl_count % 2 == 1){    //rl_count % 2 =1 -> R
          fprintf(learningLabel_R_pointer,"%d\n",label);
          //printf("rl_count テストデータ右\n");
        }
        if(rl_count % 2 == 0){    //rl_count % 2 =0 -> L
          fprintf(learningLabel_L_pointer,"%d\n",label);
          //printf("rl_count テストデータ左\n");
        }

     }else if(fileCounter % 4 == 2){
        if(rl_count % 2 == 1){    //rl_count % 2 =1 -> R
          fprintf(learningVal_R_pointer,"%d\n",label);
           //printf("rl_count バリデーションデータ右\n");
        }
        if(rl_count % 2 == 0){    //rl_count % 2 =0 -> L
          fprintf(learningVal_L_pointer, "%d\n", label);
          //printf("rl_count バリデーションデータ左\n");
        }
      }
      //label ++; //次のラベルにする
     
      /*
      //はじめと最後だけグラフを出力する。確認用
      if(fileStartCount == fileCounter || fileLastCount == fileCounter){
      fprintf(pipe_pointer, "\"o%s_%s.txt\" u 1:2 with l,", FileNumber, rl_Name);
      fprintf(InXpipe_pointer, "\"IFFT%s_%s.txt\" u 1:2 with l,", FileNumber, rl_Name);
      }
      */
      //はじめと最後だけグラフを出力する。確認用
      if(fileStartCount == fileCounter || fileLastCount == fileCounter){
        fprintf(pipe_pointer, "\"%s\" u 1:2 with l,", buff);
        fprintf(InXpipe_pointer, "\"%s\" u 1:2 with l,", buffIFFT);
      }
    
      printf("plot [-1000:] \"%s\" u 1:2 with l\n\n\n\n", buff);

      free(x);
      free(y);
      free(h);
      free(H);
      sf_close(isf);
      sf_close(isf2);
      fclose(text_pointer);
      fclose(InXtext_pointer);
    }


    if(fileStartCount == fileCounter || fileLastCount == fileCounter){
      fprintf(pipe_pointer, "\n");
      fprintf(pipe_pointer, "q\n");

      fprintf(InXpipe_pointer, "\n");
      fprintf(InXpipe_pointer, "q\n");
    }
    
    /*↓これ短くできる。　書き込んでいるファイルから文字列で読み込んでそのまま出力すれば分岐いらない*/
    switch(subMode){
    case 1:
      printf("plot \"o%s_mae_r.txt\" u 1:2 with l, \"o%s_mae_l.txt\" u 1:2 with l, \"o%s_usiro_l.txt\" u 1:2 with l, \"o%s_usiro_r.txt\" u 1:2 with l\n",FileNumber, FileNumber, FileNumber, FileNumber);
      printf("\n");
      printf("plot \"IFFT%s_mae_r.txt\" u 1:2 with l, \"IFFT%s_mae_l.txt\" u 1:2 with l, \"IFFT%s_usiro_l.txt\" u 1:2 with l, \"IFFT%s_usiro_r.txt\" u 1:2 with l\n",FileNumber, FileNumber, FileNumber, FileNumber);
      break;

    case 2:
      printf("plot \"o%s_ue_r.txt\" u 1:2 with l, \"o%s_ue_l.txt\" u 1:2 with l, \"o%s_sita_l.txt\" u 1:2 with l, \"o%s_sita_r.txt\" u 1:2 with l\n",FileNumber, FileNumber, FileNumber, FileNumber);
      printf("\n");
      printf("plot \"IFFT%s_ue_r.txt\" u 1:2 with l, \"IFFT%s_ue_l.txt\" u 1:2 with l, \"IFFT%s_sita_l.txt\" u 1:2 with l, \"IFFT%s_sita_r.txt\" u 1:2 with l\n",FileNumber, FileNumber, FileNumber, FileNumber);
      break;

    case 3:
      printf("plot \"o%s_mae_r.txt\" u 1:2 with l, \"o%s_mae_l.txt\" u 1:2 with l, \"o%s_usiro_l.txt\" u 1:2 with l, \"o%s_usiro_r.txt\" u 1:2 with l,\"o%s_ue_r.txt\" u 1:2 with l, \"o%s_ue_l.txt\" u 1:2 with l, \"o%s_sita_l.txt\" u 1:2 with l, \"o%s_sita_r.txt\" u 1:2 with l\n",FileNumber, FileNumber, FileNumber, FileNumber,FileNumber, FileNumber, FileNumber, FileNumber);
      printf("\n");
      printf("plot \"IFFT%s_mae_r.txt\" u 1:2 with l, \"IFFT%s_mae_l.txt\" u 1:2 with l, \"IFFT%s_usiro_l.txt\" u 1:2 with l, \"IFFT%s_usiro_r.txt\" u 1:2 with l, \"IFFT%s_ue_r.txt\" u 1:2 with l, \"IFFT%s_ue_l.txt\" u 1:2 with l, \"IFFT%s_sita_l.txt\" u 1:2 with l, \"IFFT%s_sita_r.txt\" u 1:2 with l\n",FileNumber, FileNumber, FileNumber, FileNumber,FileNumber, FileNumber, FileNumber, FileNumber);
    }

    //free(rl_Name);
   
    pclose(pipe_pointer);
    pclose(InXpipe_pointer);
    free(in2FileName);
    free(inFileName);
    free(FileNumber);
    free(FileName);
  
    // fclose(InYtext_pointer);

  
  }//fileNumberのループ
  printf("学習用出力ファイル名 %s, %s\n", learningFileName_L, learningFileName_R);

  free(learningFileName_L);
  free(learningFileName_R);
  fclose(learningText_L_pointer);
  fclose(learningText_R_pointer);
  free(learningLabelName_L);
  free(learningLabelName_R);
  fclose(learningLabel_L_pointer);
  fclose(learningLabel_R_pointer);
  free(learningValName_L);
  free(learningValName_R);
  fclose(learningVal_L_pointer);
  fclose(learningVal_R_pointer);
  free(savtky);

  return 0;
}


////////////////////////////////////////////////////　メイン関数
int main (int argc, char *argv[]){
  
  Opt *op =opt_open(argc, argv, argv[0]);
  int mode = opt_int(op, "-mode", 0, "1:解析, 2:単体解析, 3:検索表示, 0:オプション不足");
  if(mode == 0) printf("オプション-modeの指定がありません。\n");

  switch(mode){
  case 1:
    mode1(op);
    break;
  case 2:
    mode2(op);
    break;
  case 3:
    while (1)
      {
        ls_gnuplot();
      }
    break;
  case 4:
    mode4(op);
    break;
  }
      
  return 0;

}
