# Kermite 汎用ファームウェア(一体型)

## 概要

一体型の汎用キーボードファームウェアです。

KermiteのWebアプリに統合されている標準ファームウェアをArduinoのライブラリとFirmixによる構成に移植したものです。

オリジナルのファームウェアの機能をカバーすることを目標としています。

## 設定値の詳細

### キーボード名 (オプション)

OSによって識別されるUSBデバイスの名前を、16文字までの文字列で指定します。  
省略すると既定値`mykeeb`になります

### ボードLED (オプション)

ボード上のLEDでプログラムの死活状態とキーが押されている状態を表示します。  
ボードと設定値の対応は以下のとおりです

|値|ボード|
|--|--|
|0|なし|
|1|RPiPico|
|2|KB2040|
|3|Xiao RP2040|
|4|RP2040-Zero|
|5|Tiny2040|
|6|ProMicro RP2040|


### キーマトリクス (オプション)

キーマトリクスを構成するピンを指定します。  
マトリクスの交点で, 列のピン-->ダイオードとスイッチ-->行のピン となるようにします。ダイオードの向きに注意してください。

行/列それぞれ最大16個までのピンを指定できます。  

### 直接配線キー (オプション)

ピンをキースイッチ単体に直接配線して使用します。  

最大16個までのピンを指定できます。  

### ロータリーエンコーダ (オプション)

3個までのロータリーエンコーダに対応しています。  
ピンの数は2,4,6個のいずれかで指定してください。  
奇数個のピンを指定したときには末尾のピンが無視されます。  

### ライティング (オプション)

複数のNeoPixel LEDを連ねた回路構成に対応しています。

ピン: NeoPixel LEDを接続するピンを指定します。  
LED数: 光らせるLEDの数を指定します。


### OLED I2C ピン (オプション)

128x32のOLEDに対応しています。  
I2CのピンをSDA, SCLの順で指定します。

SDAは偶数番号のピン,SCLはその次の奇数番号のピンを指定してください。  
(例) (SDA, SCL): (gp0, gp1), (gp2, gp3), (gp4, gp5), ..., (gp28, gp29)
 