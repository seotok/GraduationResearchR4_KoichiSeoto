# 令和4年 岩手大学 永田研究室 卒業研究
 
研究名：頭部伝達関数の機械学習に基づく前後方向の音像識別に関する研究

作成者：瀬音 光一 (Koichi Seoto)
  
# 主要なフォルダとファイル

* フォルダ
  * GraduationTesis : 卒業論文、予稿のソースコードが入っている
  * dataset : 使用した教師データが入っている。
  * lerningDataText : 教師データ（全データの50%）が入る
  * learningLabelText :  テストデータ（全データの25%）が入る
  * learnigValText : 評価データ（全データの25%)が入る
  * originalSound : 音源信号（HRTFの計算時に使用する）
  * sund : 録音信号（ノイズを追加する前）
  * fusion : ノイズを追加した録音信号
  * text : 計算したHRTFまたはILDのデータ（グラフ表示に使用する）

* ファイル
  * main2.c : HRTFまたはILDの計算を行う
  * LearningMain.c : 機械学習により識別率を求める
  * RecognitionRato.txt : 識別率を記録しておくテキスト（LearningMain.c実行時に出力）
  * addnoise.c : 録音信号に任意のSN比でノイズを追加したwavファイルを出力する
  * go1 .ch : コンパイルを行うだけのシェルスクリプト


# コンパイルの方法
 
実行例）main2.cをコンパイルする場合
 
```bash
 chmod +x go1.sh
 export PATH="${PATH}:."
 go1.sh main2
```
1行目で実行権にあたる’x’を付与する。
2行目でパスを通す。
3行目でシェルを実行する。（注意：拡張子は付けない）

# 実行の方法

#### ◎ HRTFまたはILDを計算する（実行ファイル：main2.c)
  * **mode 1：ILDを計算する**
    オプションの説明：
    -No [解析したいファイル番号の範囲] -blk [FFT点数] -subMode [1:前後, 2:上下 , 3: 前後上下]  
   実行例）
   録音信号1~220の前後のILDを計算し、機械学習用のデータを出力場合
   ```
   ./main2.out -mode 1 -No 1-220 -blk 1024 -subMode 1
   ```
   
   * **mode 2：２つのwavファイルのクロススペクトルを求める**
    オプションの説明：
    -blk [FFT点数] -x [分母のファイル] -y [分子のファイル] -txt [結果を出力するファイル名]
    実行例）
    wn.wavとwn_mae_r.wavのクロススペクトルを求めwa_mae_r.txtに出力する場合
    ```
    ./main2.out -mode 2 -blk 1024 -x wn.wav -y sinwnoise1220/wn_mae_r.wav -txt wn_mae_r.txt
    ```

  * **mode 3：前後各々の左右のテキストファイルを全てグラフに表示する**
   下記コマンド実行後、前の左、前の右、後ろの左、後ろの右から１つ選択する。
   ```
   ./main2.out -mode3 
   ```

  * **mode 4：HRTFを計算する**
  オプション説明：
   -mode 4 -No[解析したいファイル番号の範囲] -blk[FFT点数] -subMode[1:前後, 2:上下 , 3: 前後上下]
  実行例）
  録音信号1~220の前後のHRTFを計算し、機械学習用のデータを出力する場合
   ```
   ./main2.out -mode 4 -No 1-220 -blk 1024 -subMode 1
   ```

#### ◎  識別率を求める(実行ファイル：LearningMain.c)
  * **(mode 1)パラメータを変化させながら識別率を求める**
  オプション説明：
  -teach[教師データ] -test[評価データorテストデータ] -No[パラメータの変化範囲] -skip [パラメータの変化量] -outputs[識別率を何回平均で求めるか]
  実行例）
  教師データLF1-220_mu_l.txt -test、評価データLV1-220_mu_l.txtでパラメータを10から20で1回刻みで10回平均の識別率を求めていく。
  ```
  ./LearningMain.out  -teach dataset/HRTF4~12/LF1-220_mu_l.txt -test dataset/HRTF4~12/LV1-220_mu_l.txt -mode 2 -No 10-20 -skip 1 -outputs 10
  ```

  * **(mode 2)パラメータを指定して任意の回数分だけ識別率を求める**
  オプション説明：
  -mlcount [学習回数] -hcount[中間層のノード数] -outputs [識別率を何回出力するか]
  例）
  教師データLF1-220_mu_l.txt -test、評価データLV1-220_mu_l.txt、学習回数6回、中間層のノード数6で識別率を10回施行する。

  ```
  ./LearningMain.out  -teach dataset/HRTF4~12/LF1-220_mu_l.txt -test dataset/HRTF4~12/LV1-220_mu_l.txt -mlcount 6 -hcount 6 -outputs 10 -mode 2 	
  ```

#### ◎ SN比を用いた雑音の追加
  * **別途ダミーヘッドで録音したノイズ信号「n_l.wav」,「n_r.wav」を任意のSN比で追加する**
  オプション説明：
  -No [ノイズ信号を追加する録音信号のファイル番号の範囲] -SN [SN比]
  例）
  「sound」フォルダのNo1-220の前後上下それぞれの左右の録音信号に、ノイズ信号を各々SN比10dBで追加し、「fusion」フォルダに保存する。

  ```
  ./addnoise.out -No 1-220 -SN 10
  ```