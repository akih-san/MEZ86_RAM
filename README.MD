# MEZ86_RAM Rev1.1 (ファームウェアRev1.3）

# ファームウェアRev1.3での変更点
  ## 1. MS-DOS Ver3.10のサポート<br>
    MS-DOSをVer2.11からVer3.10へバージョンアップしました。<br>
    これによって、FAT16のディスクイメージを扱うことが出来、<br>
    大容量のディスクイメージをサポートします。<br>
    DISKD.DSKはFAT16の30MBのディスクイメージです。<br>
    <br>
  ## 2. Diskイメージの対応強化
    ファームウェアRev1.1では、ディスクイメージが1.4MBと9.84MB固定でした。<br>
    Rev1.3では起動時にディスクイメージのFAT情報を読み取り、MS-DOSのBPB情報を設定します。<br>
    したがって、ネット上にあるアーカイブ上のディスクイメージをDOSDISKSにコピー<br>
    してファイル名をDRIVEA.DSK～DRIVED.DSKに変更すれば、MS-DOS起動時に自動的に<br>
    認識されます。<br>
    ただし、セクターサイズが512バイトのものに限ります。<br>
    <br>
  ## 3. DRIVED.DSKにMS-DOS Ver4.0の開発環境<br>
    公開されている[MS-DOS Ver4.0](https://github.com/microsoft/MS-DOS/tree/main/v4.0)のソースコードの中には、Ver4.0をビルド出来る<br>
    強力な開発環境が提供されています。<br>
    DRIVEDのディスクイメージには、この開発環境を収録しました。<br>

# インストール方法

# [MEZ86_RAM Rev1.1](https://github.com/akih-san/MEZ86_RAM/tree/Rev1.1)を参照してください。<br>
