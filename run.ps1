$env:PATH = "D:\Software\QT\Tools\mingw1310_64\bin;D:\Software\QT\Tools\CMake_64\bin;D:\Software\QT\Tools\Ninja;" + $env:PATH

$projectDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $projectDir

if (-not (Test-Path "build\AccountApp.exe")) {
    Write-Host "Application not built yet. Run build.ps1 first." -ForegroundColor Red
    exit 1
}

Write-Host "Running AccountApp..." -ForegroundColor Cyan
& ".\build\AccountApp.exe"
