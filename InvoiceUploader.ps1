
<#PSScriptInfo

.VERSION 1.0

.GUID d26a33ec-0240-4ee4-b306-b48a3e4a5660

.AUTHOR Oleksii Gaienko <support@galexsoftware.info>

.COPYRIGHT 2024 Oleksii Gaienko. All rights reserved.

.TAGS InvoiceUploader

.LICENSEURI https://www.gnu.org/licenses/gpl-3.0.html

.PROJECTURI https://github.com/GAlexDark/MMSTools

.RELEASENOTES This script comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.

#>

<#

.SYNOPSIS
 Upload invoices/reports to the MMS.

.DESCRIPTION
 PoSH script for uploading the invoices/reports.

#>

#requires -version 5

[CmdletBinding()]
Param (
    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    [string] $EnvFileName,

    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    [string] $OutputDir

#    [Parameter (Mandatory=$true
#    HelpMessage="Issue Date")]
#    [string] $IssueDateFolder,

#    [Parameter (Mandatory=$false
#    HelpMessage="System Wide Clearing ID")]
#    [string] $SystemWideClearingId
)

#=========================================================================

function Check-InputParams {
    [OutputType([bool])]
    param (
        [Parameter(Mandatory = $true)]
        [string] $IssueDateFolder,

        [Parameter(Mandatory = $true)]
        [string] $SystemWideClearingId
    )

    $IssueDateFolder = $IssueDateFolder.Trim()
    $SystemWideClearingId = $SystemWideClearingId.Trim()

    [bool] $retVal = $true
    if (($IssueDateFolder.Length -eq 0) -and ($SystemWideClearingId.Length -eq 0)) {
        Write-Host "Error: The 'IssueDateFolder' and 'SystemWideClearingId' arguments are empty." -ForegroundColor Red
        $retVal = $false
    }

    if ($IssueDateFolder.Length -eq 0) {
        Write-Host "Error: The 'IssueDateFolder' argument is empty." -ForegroundColor Red
    } elseif ($IssueDateFolder -notmatch "^\d{8}$") {
        Write-Host "Error: The 'IssueDateFolder' contains nondigit chars"
        $retVal = $false
    }

    if ($SystemWideClearingId.Length -ne 0) {
        if ($SystemWideClearingId -notmatch "^\d{4}-\d{2}-\d{2}-V\d{1}$") {
            Write-Host "Error: The 'IssueDateFolder' has wrong format"
            $retVal = $false
        }
    }

    return $retVal
}

function Calc-ShaHash {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $LocalStorage
    )

    [String[]] $fileTypes = ('*.pdf','*.p7s','*.xlsx','*.xls')
    [bool] $res = $true
    try {
        [array] $retVal = @()
        if ($LocalStorage.EndsWith('*')) {
            $LocalStorage = $LocalStorage.Replace('\*','')
        }
        $files = Get-ChildItem -Path $LocalStorage -Include $fileTypes -Recurse -ErrorAction Stop
        if ($files) {
            $files | ForEach-Object {
                [string] $pathToFile = $_.FullName.Replace('\', '/') # the replase for the pretty json format
                $fileHash = [PSCustomObject]@{
                    'File Name' = "." + $pathToFile.Substring($LocalStorage.Length)
                    'Last Write Time' = (Get-ItemProperty -Path $pathToFile).LastWriteTime.ToString($dateTimeFormat)
                    'Hash' = (Get-FileHash -Algorithm SHA256 -Path $pathToFile -ErrorAction Stop).hash
                    }
            $retVal += $fileHash
            }
        } else {
            $res = $false
            Write-Host "No any *.xlsx, *.pdf and *.p7s files found." -ForegroundColor Red
        }
    } catch {
        $res = $false
        Write-Error $PSItem
    }

    return @{Status = $res; FileHashInfo = $retVal}
}

function Copy-P7sFile {
    [OutputType([bool])]
    param (
        [Parameter(Mandatory = $true, Position=0)]
        [ValidateNotNullOrEmpty()]
        [string] $LocalStorage,

        [Parameter(Mandatory = $true, Position=1)]
        [ValidateNotNullOrEmpty()]
        [string] $ArchiveStorage
    )
    [bool] $retVal = $true
    try {
        [string] $newDir = Join-Path -Path $ArchiveStorage -ChildPath (Get-Date).ToString('yyyyMMddHHmmss') -ErrorAction Stop
        New-Item -ItemType Directory -Path $newDir -ErrorAction Stop | Out-Null
        if ($LocalStorage.EndsWith('*')) {
            $LocalStorage = $LocalStorage.Replace('*','')
        }
        $p7sFiles = Get-ChildItem -Path $LocalStorage -Include "*.p7s" -Recurse -ErrorAction Stop
        if ($p7sFiles) {
            Copy-Item -Path $p7sFiles -Destination $newDir -ErrorAction Stop
        } else {
            Write-Host "No P7S files found."
        }
    } catch {
        $retVal = $false
        Write-Error $PSItem
    }
    return $retVal
}

function Upload-File {
    [OutputType([bool])]
    param (
        [Parameter(Mandatory = $true, Position=0)]
        [ValidateNotNullOrEmpty()]
        [string] $LocalStorage,

        [Parameter(Mandatory = $true, Position=1)]
        [ValidateNotNullOrEmpty()]
        [string] $remotePath
    )

    try {
        [string] $assemblyPath = if ($env:WINSCP_PATH) { $env:WINSCP_PATH } else { $PSScriptRoot }
        $assemblyPath = Join-Path -Path $assemblyPath -ChildPath "WinSCPnet.dll"
        [bool] $retVal = Test-Path -Path $assemblyPath
        if (-not $retVal) {
            Write-Host "The WinSCPnet.dll library is not found" -ForegroundColor Red
            exit 1
        }
        # Load WinSCP .NET assembly
        Add-Type -Path $assemblyPath

        # Setup session options
        $sessionOptions = New-Object WinSCP.SessionOptions -Property @{
            Protocol = [WinSCP.Protocol]::Sftp
            HostName = $remoteHost
            PortNumber = $remotePort
            UserName = $userName
            Password = $userPassword
        }
        if (![string]::IsNullOrEmpty($proxyName) -and ![string]::IsNullOrEmpty($proxyPort) ) {
            $sessionOptions.AddRawSettings("ProxyMethod", "3")
            $sessionOptions.AddRawSettings("ProxyHost", $proxyName)
            $sessionOptions.AddRawSettings("ProxyPort", $proxyPort)
        } else {
            Write-Host "The Proxy is not using (The settings is not present)." -ForegroundColor Yellow
        }

        $session = New-Object WinSCP.Session
        try {
            $sessionOptions.SshHostKeyFingerprint = $session.ScanFingerprint($sessionOptions, "SHA-256")
            # Connect
            Write-Host "Connect to the $($sessionOptions.HostName)"
            $session.Open($sessionOptions)
            Write-Host "Connection to the $($sessionOptions.HostName) succeeded"

            Write-Host "Start upload files"
            $transferResult = $session.PutFiles($LocalStorage, $remotePath, $true)
            # Throw on any error
            $transferResult.Check()

            # Print results
            [int] $count = 0
            [int] $pathLenght = 0
            if ($LocalStorage.EndsWith('*')) {
                $pathLenght = $LocalStorage.Length - 1
            }
            foreach ($transfer in $transferResult.Transfers) {
                Write-Host "Upload of the $($transfer.FileName.Substring($pathLenght).Replace('\', '/')) file succeeded." -ForegroundColor Green
                # the .Replace('\', '/') for the pretty unification with the SHA hashes output
                $count++
            }
            Write-Host "Upload completed`nTotal $($count) files saved."
        } finally {
            # Disconnect, clean up
            $session.Dispose()
        }
    } catch {
        Write-Error $PSItem
        $retVal = $false
    }
    return $retVal
}

#=========================================================================
# Stages:
# -1. Clear localStorage
# 1/0. Download from SP (internal/external tools)
# 2. copy p7s files to the archive
# 3. Calc hashes
# 4. Upload to the /mms/invoices
#=========================================================================
#Mapping env variables
$EnvFileName = Join-Path -Path $PSScriptRoot -ChildPath $EnvFileName
[bool] $retVal = Test-Path -Path $EnvFileName
if (-not $retVal) {
Write-Host "The $EnvFileName is not found" -ForegroundColor Red
    exit 1
}

. $EnvFileName
[string] $proxyName = $PROXY_NAME
[string] $proxyPort = $PROXY_PORT

[string] $remoteHost = $REMOTE_HOST
[int] $remotePort = $REMOTE_PORT
[string] $remotePath = $REMOTE_PATH + '*'

[string] $userName = $($env:sshUserName)
[string] $userPassword = $($env:sshPassword)
[string] $SshHostKeyFingerprint = $($env:sshHostKeyFingerprint)

[string] $localStorage = $LOCAL_STORAGE
[string] $archiveStorage = $ARCHIVE_STORAGE

[string] $dateTimeFormat = $DATETIME_FORMAT

#=========================================================================

[int] $exitCode = 0
try {
    #[bool] $retVal = Check-InputParams -IssueDateFolder $IssueDateFolder -SystemWideClearingId $SystemWideClearingId
    #if (-not $retVal) {
    #    throw "The arguments is incorrect."
    #}

    Remove-Item -Path (Join-Path -Path $OutputDir -ChildPath "*.json") -ErrorAction Stop

    [bool] $retVal = Copy-P7sFile -LocalStorage $localStorage -ArchiveStorage $archiveStorage
    if (-not $retVal) {
        throw "Error copy P7B files"
    }

    [string] $registryFileName = ""
    $res = Calc-ShaHash -LocalStorage $localStorage
    if ($res.Status) {
        Write-Host "SHA256 hashes:" -ForegroundColor Green
        $res.FileHashInfo | Format-Table -AutoSize
        Write-Host "Total $($res.FileHashInfo.count) files`n"

        $registryFileName = "Hashes" + (Get-Date).ToString('yyyyMMddHHmmss') + ".json"
        $registryFileName = Join-Path -Path $archiveStorage -ChildPath $registryFileName
        $res.FileHashInfo | ConvertTo-Json -Compress | Out-File $registryFileName
    } else {
        throw "No Hashes"
    }

    # If script must build upload path
    #$remotePath = $remotePath.Replace("*", $IssueDateFolder + "/*")
    #if ($SystemWideClearingId.Length -ne 0) {
    #    $remotePath = $remotePath.Remove("*", $SystemWideClearingId + "/*")
    #}
    $retVal = Upload-File -LocalStorage $LocalStorage -remotePath $remotePath
    if (-not $retVal) {
        throw "Upload files canceled."
    }
    if($registryFileName) {
        Copy-Item -Path $registryFileName -Destination $OutputDir -ErrorAction Stop
    }
} catch {
    $exitCode = 1
    Write-Host $PSItem -ForegroundColor Red
}

#=========================================================================

exit $exitCode
