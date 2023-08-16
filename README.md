# ShiftFlamework
## ビルド
### ツールのインストール
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
```
git pull --force
git submodule init
git submodule update
```

