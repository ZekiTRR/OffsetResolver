# Build script for Process Module Manager
# Automatically finds Visual Studio and compiles the project

Write-Host "====================================================" -ForegroundColor Cyan
Write-Host "   Process Module & Offset Management Tool        " -ForegroundColor Cyan
Write-Host "              Build Script                         " -ForegroundColor Cyan
Write-Host "====================================================" -ForegroundColor Cyan
Write-Host ""

# Source files
$sources = @(
    "main.cpp",
    "ProcessManager.cpp",
    "ModuleRegistry.cpp",
    "AddressResolver.cpp",
    "OffsetStorage.cpp",
    "ConsoleUI.cpp"
)

$output = "ProcessModuleManager.exe"

# Check if all source files exist
$missingFiles = @()
foreach ($file in $sources) {
    if (-not (Test-Path $file)) {
        $missingFiles += $file
    }
}

if ($missingFiles.Count -gt 0) {
    Write-Host "[!] Missing source files:" -ForegroundColor Red
    foreach ($file in $missingFiles) {
        Write-Host "    - $file" -ForegroundColor Red
    }
    exit 1
}

Write-Host "[+] All source files found" -ForegroundColor Green

# Find Visual Studio
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

if (-not (Test-Path $vsWhere)) {
    Write-Host "[-] Visual Studio not found!" -ForegroundColor Red
    Write-Host "    Please install Visual Studio with C++ Desktop Development workload" -ForegroundColor Yellow
    exit 1
}

$vsPath = & $vsWhere -latest -property installationPath
if ([string]::IsNullOrEmpty($vsPath)) {
    Write-Host "[-] Could not locate Visual Studio installation" -ForegroundColor Red
    exit 1
}

Write-Host "[+] Found Visual Studio at: $vsPath" -ForegroundColor Green

$vcvars = "$vsPath\VC\Auxiliary\Build\vcvars64.bat"
if (-not (Test-Path $vcvars)) {
    Write-Host "[-] vcvars64.bat not found!" -ForegroundColor Red
    exit 1
}

Write-Host "[+] Configuring build environment..." -ForegroundColor Cyan

# Compile
$compileCmd = "cl /EHsc /std:c++17 /O2 /DUNICODE /D_UNICODE $($sources -join ' ') /Fe:$output"

Write-Host "[+] Compiling..." -ForegroundColor Cyan
Write-Host "    Command: $compileCmd" -ForegroundColor DarkGray
Write-Host ""

# Setup environment and compile (show all output)
cmd /c "`"$vcvars`" >nul && $compileCmd"

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "====================================================" -ForegroundColor Green
    Write-Host "           Build Successful!                       " -ForegroundColor Green
    Write-Host "====================================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "[+] Output: $output" -ForegroundColor Green
    
    # Clean up intermediate files
    Remove-Item *.obj -ErrorAction SilentlyContinue
    
    Write-Host ""
    Write-Host "To run the program:" -ForegroundColor Cyan
    Write-Host "    .\$output" -ForegroundColor White
    Write-Host ""
}
else {
    Write-Host ""
    Write-Host "====================================================" -ForegroundColor Red
    Write-Host "              Build Failed!                        " -ForegroundColor Red
    Write-Host "====================================================" -ForegroundColor Red
    Write-Host ""
    Write-Host "Check the error messages above for details." -ForegroundColor Yellow
    exit 1
}
