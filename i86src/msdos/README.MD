＜IOSYSの作成について＞

8086のアセンブラは、Macro Assembler AS V1.42を使用しています。
http://john.ccac.rwth-aachen.de:8000/as/

「PowerShell」のスクリプトを使用するため、windowsのターミナルは、
「Windows PowerShell」を使用してください。

初期状態では、スクリプトの実行が許可されていないため、
管理者権限でWindows PowerShellを起動し、以下の文を実行してください。

PowerShell Set-ExecutionPolicy RemoteSigned

上記コマンドを実行後は、管理者権限は必要ありません。
ノーマル権限のWindows PowerShellでスクリプトが使えるようになります。

後はノーマル権限でWindows PowerShellを立ち上げて、バッチコマンド
mkiosys.batを実行すると、IO.SYSが作成されます。

必要なファイル

MEZ86IO.ASM
mkiosys.bat
mkiosys.ps1
SYSINI_MES.bin

＜その他のファイル＞
MSDOS.SYS : MS-DOS Ver3.10 本体
hdd40m.zip : 40MB(FAT16)のディスクイメージの圧縮ファイル
