
<#PSScriptInfo

.VERSION 1.0

.GUID 407dae93-b80e-4643-b8b5-f6e14e19daa9

.AUTHOR Oleksii Gaienko <support@galexsoftware.info>

.COPYRIGHT 2024 Oleksii Gaienko. All rights reserved.

.TAGS mmsxlsx2json

.LICENSEURI https://www.gnu.org/licenses/#GPL

.PROJECTURI https://github.com/GAlexDark/MMSTools

#>

<#

.SYNOPSIS
 Performs MMS XLSX to JSON Conversion.

.DESCRIPTION
 The mmsxlsx2json.ps1 script converts an MMS XLSX file to a JSON file.
 The mmsxlsx2json.ps1 script must be located in the same folder where mmsxlsx2json.exe is located.

.PARAMETER Workdir
 Specifies the path to the XLSX input files.

.INPUTS
 None. You can't pipe objects to mmsxlsx2json.ps1.

.OUTPUTS
 None. mmsxlsx2json.ps1 doesn't generate any output.

.EXAMPLE
 PS> .\mmsxlsx2json.ps1 -Workdir C:\Data
 MMS XLSX to JSON Conversion PoSH Script Version 1.0
 Copyright (C) 2024 Oleksii Gaienko, support@galexsoftware.info
 This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.

 Starting B:\MMSTools\mmsxlsx2json.exe...
 Target file: "C:\Data\Book2.xlsx"

 Total rows converted: 100.
 The JSON was saved in the file: B:/Data/Book2.json

 DONE
 DONE

#>

#requires -version 5

[CmdletBinding()]
Param (
    [Parameter (Mandatory=$true, Position=0,
    HelpMessage="Enter path to the folder, contains *.xlsx files.")]
    [string] $Workdir
)

$Workdir = $Workdir.Trim()
if ((($Workdir.Length -eq 1) -and ($Workdir -eq ".")) -or
    (($Workdir.Length -eq 2) -and ($Workdir -eq ".\"))) {
    $Workdir = $PSScriptRoot
}
try {
    $isWorkDirExists = Test-Path -Path $Workdir -ErrorAction Stop -ErrorVariable err
} catch {
    Write-Host $err.ErrorRecord -ForegroundColor Red
    exit 1
}
if (!$isWorkDirExists) {
    Write-Warning "The work dir $Workdir not found."
    exit 1
}

Write-Host "MMS XLSX to JSON Conversion PoSH Script Version 1.0`nCopyright (C) 2024 Oleksii Gaienko, support@galexsoftware.info`nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.`n" -ForegroundColor green

$pathToExecute = $PSScriptRoot + "\mmsxlsx2json.exe"
$retVal = Test-Path $pathToExecute
if (-not $retVal) {
    Write-Host "The executable file mmsxlsx2json.exe not found" -ForegroundColor Red
    exit 1
}

Write-Output "`nStarting $pathToExecute...`n"

try {
    $foundItems = Get-ChildItem -Path $Workdir -Filter *.xlsx -ErrorAction Stop -ErrorVariable err
} catch {
    Write-Host $err.ErrorRecord -ForegroundColor Red
    exit 1
}

foreach ($item in $foundItems) {
    $fileName = '"' + $item.FullName + '"'
    Write-Output "Target file: $fileName"
    $processInfo = Start-Process -FilePath $pathToExecute -ArgumentList "-i $fileName -m compact --silent" -Wait -NoNewWindow -PassThru
    if ($processInfo.ExitCode -ne 0) {
        Write-Host "The $pathToExecute returned an error." -ForegroundColor red
    }
}

Write-Host "DONE" -ForegroundColor green
exit 0
