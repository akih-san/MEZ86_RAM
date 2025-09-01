# 工事中(Under construction)
Please wait a moment. As this is before release, data is being updated as needed.
Firmware Rev1.1 is [here](https://github.com/akih-san/MEZ86_RAM/tree/Rev1.1)

# MEZ86_RAM Rev1.1 (ファームウェアRev1.3）

## ファームウェアRev1.3での変更点
  - Diskイメージの対応強化
    ファームウェアRev1.1では、ディスクイメージが1.4MBと9.84MB固定でした。
    Rev1.3では起動時にディスクイメージのFAT情報を読み取り、MS-DOSのBPB情報を
    設定します。
    したがって、ネット上にあるアーカイブ上のディスクイメージをDOSDISKSにコピー
    してファイル名をDRIVEA.DSK～DRIVED.DSKに変更すれば、MS-DOS起動時に自動的に
    認識されます。
    ただし、セクターサイズが512バイトのものに限ります。
    
  - MS-DOS Ver3.10のサポート
    MS-DOSをVer2.11からVer3.10へバージョンアップしました。
    これによって、FAT16のディスクイメージを扱うことが出来、
    大容量のディスクイメージをサポートします。
    DISKD.DSKはFAT16の40MBのディスクイメージです。
    
  - MS-DOS Ver4.0の開発環境
    公開されているMS-DOS Ver4.0のソースコードの中には、Ver4.0をビルド出来る
    強力な開発環境が提供されています。
    この開発環境は従来のMASMに加えて、メイクツールであるNMAKEが提供されており、
    さらに、Cコンパイラを内包しています。
    DRIVEDのディスクイメージには、このソースコードを収録しました。
    8086のCPUパワーではmakeにとても時間がかかり、実用的ではありません。
    [DOSVAXJ3](https://www.nanshiki.co.jp/software/dosvaxj3.html)のような
    エミュレーターを使った方が実用的です。
    MS-DOSVer4.0については、[ここ](https://qiita.com/reika727/items/32a74226aea427044b41)がとても参考になります。

## インストール等

詳細は、[MEZ86_RAM Rev1.1](https://github.com/akih-san/MEZ86_RAM/tree/Rev1.1)を参照してください。
