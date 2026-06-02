#!/bin/bash

# 기존 실행 파일 제거 (오류 방지)
rm -f simulator simulator.exe

echo "컴파일을 시작합니다..."
# 모든 .c 파일을 모아서 simulator 라는 실행 파일로 컴파일
gcc -o simulator main.c process.c scheduler.c evaluation.c

# 컴파일이 성공적으로 끝났는지 확인 (Exit code 0)
if [ $? -eq 0 ]; then
    echo "컴파일 성공! 시뮬레이터를 실행합니다..."
    echo "------------------------------------------------"
    ./simulator
else
    echo "컴파일 실패! 오류 메시지를 확인하세요."
fi
