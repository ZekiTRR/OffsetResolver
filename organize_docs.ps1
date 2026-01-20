# Documentation Organization Script
# This script copies current Russian markdown files to RUS/ folder
# Then replaces root files with English versions

$projectRoot = "e:\Reverse\CSS\Cheat\Process-Module-Dumper"
$rusFolder = "$projectRoot\RUS"

# Ensure RUS folder exists
if (!(Test-Path $rusFolder)) {
    New-Item -ItemType Directory -Path $rusFolder | Out-Null
    Write-Host "Created RUS folder"
}

# Files to process (excluding README files which are already handled)
$filesToProcess = @(
    "QUICKSTART.md",
    "ARCHITECTURE.md",
    "EXAMPLES.md",
    "API.md",
    "BUILD.md",
    "SUMMARY.md",
    "VISUAL_STRUCTURE.md",
    "CHANGELOG.md",
    "INDEX.md"
)

Write-Host "Documentation files to process: $($filesToProcess.Count)"
Write-Host ""

foreach ($file in $filesToProcess) {
    $rootPath = "$projectRoot\$file"
    $rusPath = "$rusFolder\$file"
    
    if (Test-Path $rootPath) {
        # Copy current (Russian) version to RUS folder
        Copy-Item -Path $rootPath -Destination $rusPath -Force
        Write-Host "✓ Copied $file to RUS/"
    }
    else {
        Write-Host "✗ File not found: $file"
    }
}

Write-Host ""
Write-Host "Documentation organization complete!"
Write-Host "Next steps:"
Write-Host "1. Edit root markdown files to English version"
Write-Host "2. Add language selector links to all files"
Write-Host "3. Update cross-references"
