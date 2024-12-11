#!/bin/bash

# テストの実行とカバレッジ測定用スクリプト

# ビルドディレクトリの作成と移動
mkdir -p build
cd build

# CMakeの設定
cmake .. -DBUILD_TESTING=ON -DCODE_COVERAGE=ON

# ビルド
cmake --build . --config Release

# テストの実行
ctest -C Release --output-on-failure

# カバレッジレポートの生成（gcovr使用）
if command -v gcovr &> /dev/null; then
    echo "Generating coverage report..."
    gcovr -r .. --html --html-details -o coverage_report.html
    
    # レポートを開く
    if command -v xdg-open &> /dev/null; then
        xdg-open coverage_report.html
    elif command -v open &> /dev/null; then
        open coverage_report.html
    fi
else
    echo "gcovr not found. Please install it using: pip install gcovr"
fi

# 元のディレクトリに戻る
cd ..
