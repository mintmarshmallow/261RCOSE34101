#!/bin/bash

rm -f simulator simulator.exe

echo "Starting compilation..."
# 모든 .c 파일을 모아서 simulator 라는 실행 파일로 컴파일
gcc -o simulator main.c process.c scheduler.c evaluation.c

# 컴파일이 성공적으로 끝났는지 확인 (Exit code 0)
if [ $? -eq 0 ]; then
    echo "Compilation successful. Starting simulator..."
    echo "------------------------------------------------"
    ./simulator
else
    echo "Compilation failed! Please check the error message."
fi
