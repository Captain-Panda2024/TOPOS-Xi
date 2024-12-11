# テストの実行とカバレッジ測定用スクリプト

# ビルドディレクトリの作成と移動
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build"
}
Set-Location build

# CMakeの設定
cmake .. -DBUILD_TESTING=ON -DCODE_COVERAGE=ON

# ビルド
cmake --build . --config Release

# テストの実行
ctest -C Release --output-on-failure

# カバレッジレポートの生成
if (Get-Command "OpenCppCoverage.exe" -ErrorAction SilentlyContinue) {
    Write-Host "Generating coverage report..."
    OpenCppCoverage.exe --sources ..\src\compiler --export_type=html:coverage_report -- tests\type_system_test.exe tests\verifier_test.exe
    
    # レポートを開く
    Start-Process "coverage_report\index.html"
} else {
    Write-Host "OpenCppCoverage not found. Please install it using: choco install opencppcoverage"
}

# 元のディレクトリに戻る
Set-Location ..
