-- Under construction --
# MEZ86_RAM Rev1.1

MEZ86_RAM Rev1.1は[EMUZ80](https://vintagechips.wordpress.com/2022/03/05/emuz80_reference/)ボード上で動く8086/V30メザニンボードです。



（MEZ86_RAM Rev1.1メザニンボード）
![](photos/DSC_0018-1.png)<br>



EMUZ80はMicrochip社のPIC18F47Q43/83/84を使用して、Z80を制御するシングルボードコンピュータ
です。また、Z80の代わりにメザニンボードを装着することで、レガシーCPU（68008や、8086、V30、他）
を動かすことが出来ます。<br>
UART, SPI, I2C等の制御をPICで行い、レガシーCPUの外部I/Oとして機能を提供します。
SDカードはSPIを通して実装されています。<br>
PICから、レガシーCPUのメモリ空間にアクセスすることで、レガシーPCのプログラムをロードする
ことが出来るため、ROMは必要なくSRAMでプログラムを動作させることが出来ます。


# 起動画面
MEZ86_RAM Rev1.1を起動すると、初期化終了後に、プログラムの選択待ちになります。

![](photos/opening.png)<br>

# 特徴
・動作CPU : Intel8086 CPU（その他互換CPU) 5MHz/8MHz/10MHz、またはNEC V30(μPD70116) 10MHz/16MHz<br>
・Microcontroller : PIC18F47Q43, PIC18F47Q84, PIC18F47Q83（注1）（注2）<br>
・1MB SRAM搭載(AS6C4008-55PCN 2個)<br>
・μSDカードI/F（SPI)<br>
・I2C I/F (RTC)リアルタイムクロック：DS1307をサポート）（注3）<br>
・UART（9600bps無手順）<br>
・動作プログラム : 後述<br>
（注1）<br>
PIC18F47QXXのCLC機能を使って8086/V30のINTRを実現しています。プログラムでの割込みベクター発生のため
CPU速度10MHzまでのサポートになります。12.8MHz, 16MHzのクロック指定をした場合、INTRが発生しないように
ファームウェアで調整されます。<br>
（注2）<br>
PIC18F47QXXから1MBメモリをアクセスするために、8086/V30のALE信号にCPLD(ATF22V10)でゲートロジックを
通しています。<br>
そのため、V30(μPD70116) 16MHz動作が安定しません。12.8MHzでの動作で安定してると思われます。<br>
クロックの設定は、12.8MHz, 16MHzが指定できますが、INTRを使用する場合には10MHz以下に設定する必要があります。<br>
（注2）<br>
I2Cの制御ドライバは、[EMU57Q-8088/V20](https://github.com/akih-san/EMU57Q-Rrv2.1_CPM_MSDOS)のファームウェアを流用しています。<br>

# プログラム
提供しているプログラムについては、他のリポジトリのプログラムをMEZ86_RAM上で動作するように移植したものです。<br>

[SBCV20_8088](https://github.com/akih-san/SBCV20_8088/tree/main)<br>
 ・Universal Monitor<br>
 ・Toyoshiki Tiny Basic<br>
 ・VTL-C<br>
 ・GAME-C Interpreter<br>
<br>
[MEZ88_RAM](https://github.com/akih-san/MEZ88_RAM)<br>
 ・CP/M-86<br>
 ・MS-DOS<br>
<br>
[8086 NASCOM BASIC](https://github.com/satoshiokue/8086_NASCOM_BASIC)<br>
 ・8086 NASCOM BASIC<br>

8086 NASCOM BASICは、[@S_Okue](https://github.com/satoshiokue)さんによって
Z80用のアセンブラから、8086用のアセンブラにコンバートされたものです。<br>
今回、MEZ86_RAM上で動くように移植しました。<br>






# SBCV20について
[SBCV20](https://vintagechips.wordpress.com/category/8088-v20/)
は[電脳伝説](https://vintagechips.wordpress.com/)さんによって作成されたSBCです。<br>
SBCV20の技術資料は[こちら](http://www.amy.hi-ho.ne.jp/officetetsu/storage/sbcv20_techdata.pdf)<br>
SBCV20の入手先は[こちら](https://store.shopping.yahoo.co.jp/orangepicoshop/pico-a-037.html)<br>

