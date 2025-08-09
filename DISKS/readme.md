# CPU側のコンフィグレーション

MEZ86.CFGファイルは、ファームウェア起動時に読み込まれ、
CPUのCLK値や、INTRの有無、割込みベクターの値、プログラムを
ロードする情報などの情報を得ます。
<br>
＜MEZ86.CFGの内容＞<br>
<br>
セミコロン(;)から始まる行はコメント<br>
<br>
![](../photos/clk_intr_info.png)<br>
<br>
CPU側のプログラムを変更する必要があるため、
プログラムのロード情報は変更してはならない。
<br>
![](../photos/load_info.png)<br>
<br>
デバッグモードの設定<br>
![](../photos/debug_info.png)<br>
<br>
