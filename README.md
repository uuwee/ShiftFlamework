# ShiftFlamework
## クローン
```
git clone https://github.com/UweSilver/ShiftFlamework.git
```
最近のgitのGUIクライアント(GitHub Desktop, GitKraken など)は`clone`するときに `--recurse` オプションを勝手につけるようです。これが利用しているdawnのクローン時に問題を起こすため、`--recurse`でない手段でクローンしてください。

次に
```
git pull --force
git submodule init
git submodule update
```

## ビルド
#### ネイティブ版(Windows, Mac)
google depot toolのインストール

https://www.chromium.org/developers/how-tos/depottools/#installing

に従ってdepot toolをインストールし、PATHを通しておく

lib\dawn内で、
```
cp scripts/standalone.gclient .gclient
gclient sync
```	

#### Web版

emscriptenのインストール

https://emscripten.org/docs/getting_started/downloads.html

に従ってインストールする


