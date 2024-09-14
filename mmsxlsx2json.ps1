
<#PSScriptInfo

.VERSION 1.0

.GUID 407dae93-b80e-4643-b8b5-f6e14e19daa9

.AUTHOR support@galexsoftware.info

.COPYRIGHT 2024 Oleksii Gaienko. All rights reserved.

.TAGS mmsxlsx2json

.LICENSEURI http://www.gnu.org/licenses/

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
 Target file: "B:\data\Book2.xlsx"


 Total rows converted: 100.
 The JSON was saved in the file: B:/data/Book2.json

 DONE
 DONE

#>

#requires -version 5

[CmdletBinding()]
Param (
    [Parameter (Mandatory=$true, Position=0, ParameterSetName="Dir",
    HelpMessage="Enter path to the folder, contains *.xlsx files.")]
    [string] $Workdir
)

try {
    $isWorkDirExists = Test-Path -Path $Workdir -ErrorAction Stop -ErrorVariable err
} catch {
    Write-Warning $err
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
    [CmdletBinding()]
    param (
        [Parameter(Mandatory = $true)]
        [string] $FilePath,
        [Parameter()]
        [string[]] $ArgumentsList
     )

    begin {
        $tmp = [System.IO.Path]::GetTempFileName()
        $readJob = Start-Job -ScriptBlock { param( $Path ) Get-Content -Path $Path -Wait -Encoding UTF8 } -ArgumentList $tmp
        $process = Start-Process -FilePath $FilePath -ArgumentList $ArgumentsList -RedirectStandardOutput $tmp -Wait -NoNewWindow -PassThru
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
Write-Host "Starting $pathToExecute..."

try {
    $foundItems = Get-ChildItem -Path $Workdir -Filter *.xlsx -ErrorAction Stop -ErrorVariable err
} catch {
    Write-Warning $err
    exit 1
}

foreach ($item in $foundItems) {
    $fileName = '"' + $item.FullName + '"'
    Write-Host "Target file: $fileName"
    Start-ProcessWithOutput -FilePath $pathToExecute -ArgumentsList "-i $fileName -m compact --silent"
}

Write-Host "DONE" -ForegroundColor green
exit 0
