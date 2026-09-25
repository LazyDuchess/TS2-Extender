#Requires -Version 7.4
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true

Set-Location $PSScriptRoot/..
[Environment]::CurrentDirectory = (Get-Location -PSProvider FileSystem).ProviderPath

$versionLines = Get-Content -Path "include/PatchVersion.h"
$line = $versionLines[1]
$version = $line.Substring(40);
$version = $version.Replace('"', "")

Write-Host "Make sure you've built the CMake build targets from out/build/x86!"

Write-Host "Working..."

function EnsureDir($path) {
    if(!(Test-Path $path)) { New-Item -Type Directory $path > $null }
}

function Clean() {
    if(Test-Path "Build") {
        Remove-Item -Recurse "Build"
    }
}

function CreateZip($zipPath) {
    if(Test-Path $zipPath) { Remove-Item $zipPath }
    $zip = [System.IO.Compression.ZipFile]::Open($zipPath, 'Create')
    return $zip
}

function ExtractZip($zipPath){
    $targetPath = [System.IO.Path]::Combine([System.IO.Path]::GetDirectoryName($zipPath),[System.IO.Path]::GetFileNameWithoutExtension($zipPath))
    [System.IO.Compression.ZipFile]::ExtractToDirectory($zipPath, $targetPath)
}

function AddToZip($zip, $path, $pathInZip=$path) {
    if(Test-Path $path){
        [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zip, $path, $pathInZip) > $Null
    }
}

function CreateLCZip(){
    $zipPath = "Build/TS2.Extender.$version.LC.zip"
    $zip = CreateZip $zipPath

    Push-Location "files_lc"
    Get-ChildItem -Recurse -File './' | ForEach-Object {
        $path = ($_ | Resolve-Path -Relative).Replace('.\', '')
        AddToZip $zip $_.FullName $path
    }
    Pop-Location

    Push-Location "files_shared"
    Get-ChildItem -Recurse -File './' | ForEach-Object {
        $path = ($_ | Resolve-Path -Relative).Replace('.\', '')
        AddToZip $zip $_.FullName $path
    }
    Pop-Location

    AddToZip $zip "out/build/x86/bin/TS2Extender.asi" "TSBin/TS2Extender.asi"

    $zip.Dispose()

    ExtractZip $zipPath
}

function CreateUCZip(){
    $zipPath = "Build/TS2.Extender.$version.UC.zip"
    $zip = CreateZip $zipPath

    Push-Location "files_uc"
    Get-ChildItem -Recurse -File './' | ForEach-Object {
        $path = ($_ | Resolve-Path -Relative).Replace('.\', '')
        AddToZip $zip $_.FullName $path
    }
    Pop-Location

    Push-Location "files_shared"
    Get-ChildItem -Recurse -File './' | ForEach-Object {
        $path = ($_ | Resolve-Path -Relative).Replace('.\', '')
        AddToZip $zip $_.FullName $path
    }
    Pop-Location

    AddToZip $zip "out/build/x86/bin/TS2ExtenderUC.asi" "TSBin/TS2ExtenderUC.asi"

    $zip.Dispose()

    ExtractZip $zipPath
}

Clean
EnsureDir "Build"
CreateLCZip
CreateUCZip

Write-Host "Done! Output is in the Build directory."