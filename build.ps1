$env:PATH = "D:\Software\QT\Tools\mingw1310_64\bin;D:\Software\QT\Tools\CMake_64\bin;D:\Software\QT\Tools\Ninja;" + $env:PATH

$projectDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $projectDir

Write-Host "===== AccountApp Build Script =====" -ForegroundColor Cyan

if (Test-Path "build") {
    Write-Host "[1/3] Cleaning old build..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "build"
}

Write-Host "[2/3] Configuring with CMake..." -ForegroundColor Yellow
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configure failed!" -ForegroundColor Red
    exit 1
}

Write-Host "[3/3] Building..." -ForegroundColor Yellow
cmake --build build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Build succeeded!" -ForegroundColor Green
Write-Host "Output: build\AccountApp.exe" -ForegroundColor Green

$run = Read-Host "Run the application? (y/n)"
if ($run -eq "y" -or $run -eq "Y") {
    & ".\build\AccountApp.exe"
}
