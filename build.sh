#!/bin/bash

# build Linux
mkdir -p build
cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j
cd ..

# build Windows
mkdir -p build-windows
cd build-windows
cmake -S .. -B . \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
  -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build . -j
cd ..

# Créer dossiers finaux
mkdir -p dist/linux/{bin,shaders,assets}
mkdir -p dist/windows/{bin,shaders,assets}

# Copier les fichiers nécessaires
cp -r shaders dist/linux/
cp -r assets dist/linux/
cp build/app dist/linux/bin/
cp -r build/_deps dist/linux/bin/  # Copier les dépendances si nécessaire

cp -r shaders dist/windows/
cp -r assets dist/windows/
cp build-windows/app.exe dist/windows/bin/
cp -r build-windows/_deps dist/windows/bin/  # Copier les dépendances si nécessaire

echo "Dossiers dist/linux et dist/windows créés avec shaders, assets et bin."

cd dist/linux/bin
./app
