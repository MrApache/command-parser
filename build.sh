#!/bin/bash

#OUTPUT=$1;
#BUILD_TYPE=$2;
#FLAGS=$3;

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
GENERATOR="Ninja"

if [ ! -d "$BUILD_DIR" ]; then
  mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR" || exit 1

# Конфигурируем сборку в Release-режиме
cmake "$SCRIPT_DIR" -G "$GENERATOR" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-DMEMPRNT" || { echo "Ошибка конфигурации"; exit 1; }

echo "Сборка проекта..."
cmake --build . || { echo "Ошибка сборки"; exit 1; }

echo "Сборка завершена успешно."

