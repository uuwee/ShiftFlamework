emcmake cmake -B out\web
cmake --build out\web
cd .\out\web
npx http-server