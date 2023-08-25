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

### google depot toolのインストール

https://www.chromium.org/developers/how-tos/depottools/#installing

に従ってdepot toolをインストールし、PATHを通しておく

lib\dawn内で、
```
cp scripts/standalone.gclient .gclient
gclient sync
```	

### emscriptenのインストール

https://emscripten.org/docs/getting_started/downloads.html

に従ってインストールする

## ビルド、実行
### windows
リポジトリのディレクトリ直下で
```
.\build\win.bat
.\out\win\Debug\game.exe
```
### web
リポジトリのディレクトリ直下で
```
emcmake cmake -B out\web
cmake --build .\out\web
cd .\out\web
python -m http.server 8080
```
ブラウザで http://localhost:8080/game.html 
