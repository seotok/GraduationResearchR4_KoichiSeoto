/*
●コンパイル
    gcc  -pg addnoise.c -g pa-misc.c opt.c optck.c setopt.c hpck.c mtime.c fftw.c anacx.c fftw-cx.c winfunc.c SavitzkyGolaySample.c -lm -lportaudio -lsndfile -pg -lfftw3 -o addnoise.out
●実行
    ./addnoise.out
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

#define soundFolderPath_IN "sound"//読み込むサウンドファイルのフォルダへのパス
#define soundFolderPath_OUT "fusion"//出力するサンドフォルダのフォルダへのパス

//Constant SN(Sound to Noise)ratio 10db 
const double SNraito = 10.0;


//Function to find Vs,Vs_noise(Average sound pressure) of soundfile
//INPUT(maxPoint, soundData) Is this a Filepointer ?
//OUTPUT(double Vs)
//振幅値の二乗平均平方根(Root Mean Square)をもとめる関数
double RMS(double *soundData, int maxPoint){
    double buff = 0.0;

    for(int i=0; i<maxPoint; i++){
        buff += soundData[i]*soundData[i];
    }
    
    buff /= maxPoint;

    buff = sqrt(buff);

    return buff;
}


int main (int argc, char *argv[]){

    Opt *op =opt_open(argc, argv, argv[0]);
    char *FileCountRange = opt_char(op, "-No", "0", "解析するファイルの番号"); 
    double SNraito = opt_double(op,"-SN",  0.0, "SN比");

    if(FileCountRange == "0"){
      fputs( "オプション -Noの指定がありません。\n", stderr );
      exit( EXIT_FAILURE );
    }

    if(SNraito == 0.0){
      fputs( "オプション -SNの指定がありません。\n", stderr );
      exit( EXIT_FAILURE );
    }

  
    SF_INFO sfo_noise_R, sfo_noise_L, sfo_X, sfo_out;
    

    //Noise_RorL of soundfle read in ./sound/fusion
    double *Noise_R = NULL;
    double *Noise_L = NULL;

    //Find Vs_noise_RorL
    double Vs_noise_R = 0.0;
    double Vs_noise_L = 0.0;

    char *fileNumber = NULL;
    char *direction = NULL ;

    char* sndfol_IN = soundFolderPath_IN;
    char* sndfol_OUT = soundFolderPath_OUT;

    //Read Number 
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

    fileNumber = (char*)malloc(sizeof(char)*(lastFileDigit+1));
    if(fileNumber == NULL){
      fputs( "malloc fileNumber error\n", stderr );
      exit( EXIT_FAILURE );
    }

    
    //Switching Number
    for(int fileCounter = fileStartCount; fileCounter<=fileLastCount; fileCounter++){
        sprintf(fileNumber, "%d", fileCounter);
        
        //Switching muus
        for(int direction_count = 1; direction_count <= 4; direction_count++){
            switch(direction_count){
            case 1:
                direction = "mae";
                break;
            case 2:
                direction = "usiro";
                break;
            case 3:
                direction = "ue";
                break;
            case 4:
                direction = "sita";
                break;
            default:
                fputs( "入力ファイル名に異常があります。\n", stderr );
                exit( EXIT_FAILURE );
                break;
            }
            //Switching between R and L 
            for(int rl_count = 0; rl_count <=1; rl_count++){

                char *fileName_X =(char*)malloc( sizeof(char)*(1+strlen(sndfol_IN)+1+strlen(fileNumber)+1+strlen(direction)+7) );
                if( fileName_X == NULL ){
                    fputs( "fileName_X の領域確保に失敗しました。\n", stderr );
                    exit( EXIT_FAILURE );
                }

                char *fileName_out =(char*)malloc( sizeof(char)*(1+strlen(sndfol_OUT)+1+strlen(fileNumber)+1+strlen(direction)+7) );
                if( fileName_out == NULL ){
                    fputs( "fileName_out の領域確保に失敗しました。\n", stderr );
                    exit( EXIT_FAILURE );
                }

                //Switching Noise R and L  
                switch (rl_count){
                case 0:
                    sprintf(fileName_X, "%s/k%s_%s_r.wav", sndfol_IN, fileNumber, direction);
                    sprintf(fileName_out, "%s/k%s_%s_r.wav", sndfol_OUT, fileNumber, direction);
                    break;
                case 1:
                    sprintf(fileName_X, "%s/k%s_%s_l.wav", sndfol_IN, fileNumber, direction);
                    sprintf(fileName_out, "%s/k%s_%s_l.wav", sndfol_OUT, fileNumber, direction);
                    break;
                default:
                    fputs( "入力ファイル名に異常があります。\n", stderr );
                    exit( EXIT_FAILURE );
                    break;
                }
                
                SNDFILE *isf_noise_R = sf_open("fusion/n_l.wav", SFM_READ, &sfo_noise_R);
                SNDFILE *isf_noise_L = sf_open("fusion/n_l.wav", SFM_READ, &sfo_noise_L);
                
                if(isf_noise_R == NULL){
                    fputs( "malloc isf_noise_R error\n", stderr );
                    exit( EXIT_FAILURE );
                }
                if(isf_noise_L == NULL){
                    fputs( "malloc isf_noise_L error\n", stderr );
                    exit( EXIT_FAILURE );
                }
                SNDFILE *isf_X = sf_open(fileName_X, SFM_READ, &sfo_X);
                if(isf_X == NULL){
                    fputs( "malloc isf_X error\n", stderr );
                    exit( EXIT_FAILURE );
                }

                sfo_out.samplerate = sfo_X.samplerate;
                sfo_out.channels = sfo_X.channels;
                sfo_out.format = sfo_X.format;
                sfo_out.frames =sfo_X.frames;
                SNDFILE *isf_out = sf_open(fileName_out, SFM_WRITE, &sfo_out);
                if(isf_out == NULL){
                    fputs( "malloc isf_out error\n", stderr );
                    exit( EXIT_FAILURE );
                }

                double *X = (double*)malloc(sizeof(double)*sfo_X.frames);
                if(X == NULL){
                    fputs( "malloc X error\n", stderr );
                    exit( EXIT_FAILURE );
                }
                memset(X, 0.0, sizeof(double)*sfo_X.frames);

                double *Y = (double*)malloc(sizeof(double)*sfo_X.frames);
                if(Y == NULL){
                    fputs( "malloc X error\n", stderr );
                    exit( EXIT_FAILURE );
                }
                memset(Y, 0.0, sizeof(double)*sfo_X.frames);
                
                sf_read_double(isf_X, X, sfo_X.frames);

                switch (rl_count){
                case 0:
                   sf_read_double(isf_noise_R, Y, sfo_X.frames);
                    break;
                case 1:
                    sf_read_double(isf_noise_R, Y, sfo_X.frames);
                    break;
                default:
                    fputs( "入力ファイル名に異常があります。\n", stderr );
                    exit( EXIT_FAILURE );
                    break;
                }

                //printf("point max = %ld\n", sfo_X.frames);
                
                //Find Vs
                double Asignal =RMS(X, sfo_X.frames);
                //printf("Asignal %f dB\n", 10*log10(Asignal) ); 
                //printf("Asignal %f Pa\n", Asignal );

                //Calculate Vn by SN ratio  //double SN raito 10 db
                double Anoise = Asignal / pow(10.0, SNraito/20.0);
                //printf("Anoise %f dB\n", 10*log10(Anoise) );
                //printf("Anoise %f Pa\n", Anoise );

                double Asignal_noise =RMS(Y, sfo_X.frames);
                //printf("Asignal_noise %f dB\n", 10*log10(Asignal_noise) ); 
                //printf("Asignal_noise %f Pa\n", Asignal_noise );

                double noiseRatio = Anoise / Asignal_noise;//Calculate  noiseRatio (Vn / Vs_noise) 
                //printf("noiseRatio %f dB\n", 10*log10(noiseRatio) ); 
                //printf("noiseRatio %f times\n", noiseRatio );

                //X = X[t] + Noise[t] //roop t
                 //and, find new Noise (Noise[t] = Noise[t]*noiseRaito // roop t)
                  for(int i=0; i<sfo_X.frames; i++){
                    X[i] += noiseRatio*Y[i]; 
                }

                sf_write_double(isf_out, X, sfo_X.frames); //Output new X wavfile to sound/fusion
                
                sf_close(isf_X);
                free(fileName_X);
                free(X);
                sf_close(isf_out);
                free(fileName_out);
                sf_close(isf_noise_R);
                sf_close(isf_noise_L);
                free(Y);
            }
            //Switching between R and L
            
        //Swiching muus
        }
    //Switching Number
    }

    //sf_close(isf_noise_R);
    //sf_close(isf_noise_L);
    free(Noise_R);
    free(Noise_L);
    free(fileNumber);
    printf("ノイズの追加完了\n");
    

    return 0;
}