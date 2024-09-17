
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

 This script used the function Start-ProcessWithOutput() by Tomas Madajevas: https://medium.com/@tomas.madajevas/retrieving-executables-output-in-powershell-68e91bdee721

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

Write-Host "MMS XLSX to JSON Conversion PoSH Script Version 1.0 `
Copyright (C) 2024 Oleksii Gaienko, support@galexsoftware.info `
This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3. `
" -ForegroundColor green

Write-Host "This script used the function Start-ProcessWithOutput() by Tomas Madajevas: https://medium.com/@tomas.madajevas/retrieving-executables-output-in-powershell-68e91bdee721 `
" -ForegroundColor Yellow

function Start-ProcessWithOutput {
    [CmdletBinding(SupportsShouldProcess = $true)]
    param (
        [Parameter(Mandatory = $true)]
        [string] $FilePath,
        [Parameter()]
        [string[]] $ArgumentsList
     )

    begin {
        $tmp = [System.IO.Path]::GetTempFileName()
        try {
            $readJob = Start-Job -ScriptBlock { param( $Path ) Get-Content -Path $Path -Wait -Encoding UTF8 } -ArgumentList $tmp  -ErrorAction Stop -ErrorVariable err
            $process = Start-Process -FilePath $FilePath -ArgumentList $ArgumentsList -RedirectStandardOutput $tmp -Wait -NoNewWindow -PassThru -ErrorAction Stop -ErrorVariable err
        } catch {
            Write-Error $err
            exit 1
        }
    }

    process {
        do {
            $readJob | Receive-Job | Write-Output
        } while (-not $process.HasExited)
    }

    end {
        $readJob | Remove-Job -Force
        Remove-Item -Path $tmp -Force
     }
}

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
    Start-ProcessWithOutput -FilePath $pathToExecute -ArgumentsList "-i $fileName -m compact --silent"
}

Write-Host "DONE" -ForegroundColor green
exit 0
