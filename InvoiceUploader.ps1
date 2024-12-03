
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
        $assemblyPath = Join-Path -Path $assemblyPath -ChildPath "WinSCPnet.dll" -ErrorAction Stop
        [bool] $retVal = Test-Path -Path $assemblyPath -ErrorAction Stop
        if (-not $retVal) {
            Write-Host "The WinSCPnet.dll library is not found" -ForegroundColor Red
            exit 1
        }
        # Load WinSCP .NET assembly
        Add-Type -Path $assemblyPath -ErrorAction Stop

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
            Write-Host "The Proxy is not using (The settings is not present).`n" -ForegroundColor Yellow
        }

        $session = New-Object WinSCP.Session
        try {
            $sessionOptions.SshHostKeyFingerprint = $session.ScanFingerprint($sessionOptions, "SHA-256")
            # Connect
            Write-Host "Connect to the $($sessionOptions.HostName)"
            $session.Open($sessionOptions)
            Write-Host "Connection to the $($sessionOptions.HostName) succeeded"

            Write-Host "`nStart upload files.`n"
            $transferResult = $session.PutFiles($LocalStorage, $remotePath, $true)
            # Throw on any error
            $transferResult.Check()

            # Print results
            [int] $count = 0
            [int] $pathLenght = 0
            if ($LocalStorage.EndsWith('*')) {
                $pathLenght = $LocalStorage.Length - 1
            }
            $console = $Host.UI.RawUI
            $buffer = $console.BufferSize
            $buffer.Width = '400'
            $console.BufferSize = $buffer
            foreach ($transfer in $transferResult.Transfers) {
                Write-Host "Upload of the $($transfer.FileName.Substring($pathLenght).Replace('\', '/')) file succeeded." -ForegroundColor Green
                # the .Replace('\', '/') for the pretty unification with the SHA hashes output
                $count++
            }
            Write-Host "Upload completed`nTotal $($count) files uploaded."
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

try {
    $EnvFileName = Join-Path -Path $PSScriptRoot -ChildPath $EnvFileName -ErrorAction Stop
    [bool] $retVal = Test-Path -Path $EnvFileName -ErrorAction Stop
    if (-not $retVal) {
        Write-Host "`nThe '$EnvFileName' file is not found.`n" -ForegroundColor Red
        exit 1
    }
    $retVal = Test-Path -Path $OutputDir -ErrorAction Stop
    if (-not $retVal) {
        Write-Host "`nThe OutputDir '$OutputDir' is not exists.`n" -ForegroundColor Red
        exit 1
    }

    #Mapping env variables
    . $EnvFileName
    [string] $proxyName = $PROXY_NAME
    [string] $proxyPort = $PROXY_PORT

    [string] $remoteHost = $REMOTE_HOST
    [int] $remotePort = $REMOTE_PORT
    [string] $remotePath = $REMOTE_PATH + '*'

    [string] $userName = $($env:sshUserName)
    [string] $userPassword = $($env:sshPassword)

    [string] $localStorage = $LOCAL_STORAGE
    [string] $archiveStorage = $ARCHIVE_STORAGE

    [string] $dateTimeFormat = $DATETIME_FORMAT

    if ([string]::IsNullOrEmpty($remoteHost)) {
        Write-Host "The 'remoteHost' value is not set" -ForegroundColor Red
        exit 1
    }
    if ($remotePort -eq 0) {
        Write-Host `n
        Write-Warning "The 'remotePort' value is not set. Using the default value.`n"
    } elseif (($remotePort -lt 0) -and ($remotePort -le 65535)) {
        Write-Host "The 'remotePort' value is wrong: $remotePort" -ForegroundColor Red
        exit 1
    }
    if ([string]::IsNullOrEmpty($remotePath)) {
        Write-Host "The 'remotePath' value is not set" -ForegroundColor Red
        exit 1
    }
    if ([string]::IsNullOrEmpty($userName)) {
        Write-Host "The 'userName' value is not set" -ForegroundColor Red
        exit 1
    }
    if ([string]::IsNullOrEmpty($userPassword)) {
        Write-Host "The 'userPassword' value is not set" -ForegroundColor Red
        exit 1
    }
    if ([string]::IsNullOrEmpty($localStorage)) {
        Write-Host "The 'localStorage' value is not set" -ForegroundColor Red
        exit 1
    } else {
        [string] $buf = if ($localStorage.EndsWith('*')) { $localStorage.Replace('*', '') } else { $localStorage }
        $retVal = Test-Path -Path $buf -ErrorAction Stop
        if (-not $retVal) {
            Write-Host "`nThe '$localStorage' directory is not exists.`n" -ForegroundColor Red
            exit 1
        }
    }
    if ([string]::IsNullOrEmpty($archiveStorage)) {
        Write-Host "The 'archiveStorage' value is not set" -ForegroundColor Red
        exit 1
    } else {
        $retVal = Test-Path -Path $archiveStorage -ErrorAction Stop
        if (-not $retVal) {
            Write-Host "`nThe '$archiveStorage' directory is not exists.`n" -ForegroundColor Red
            exit 1
        }
    }
    if ([string]::IsNullOrEmpty($dateTimeFormat)) {
        Write-Host "The 'dateTimeFormat' value is not set" -ForegroundColor Red
        exit 1
    }

#=========================================================================

    [int] $exitCode = 0

    #[bool] $retVal = Check-InputParams -IssueDateFolder $IssueDateFolder -SystemWideClearingId $SystemWideClearingId
    #if (-not $retVal) {
    #    throw "The arguments is incorrect."
    #}

    [bool] $retVal = Copy-P7sFile -LocalStorage $localStorage -ArchiveStorage $archiveStorage
    if (-not $retVal) {
        throw "Error copy P7B files"
    }

    Remove-Item -Path (Join-Path -Path $OutputDir -ChildPath "*.json") -ErrorAction Stop
    [string] $registryFileName = ""
    $res = Calc-ShaHash -LocalStorage $localStorage
    if ($res.Status) {
        Write-Host "`nSHA256 hashes:" -ForegroundColor Green

        $console = $Host.UI.RawUI
        $buffer = $console.BufferSize
        $buffer.Width = '400'
        $console.BufferSize = $buffer
        $res.FileHashInfo | Format-Table -AutoSize
        Write-Host "Total $($res.FileHashInfo.count) files.`n"

        $registryFileName = "Hashes" + (Get-Date).ToString('yyyyMMddHHmmss') + ".json"
        $registryFileName = Join-Path -Path $archiveStorage -ChildPath $registryFileName -ErrorAction Stop
        $res.FileHashInfo | ConvertTo-Json -Compress | Out-File $registryFileName
        Copy-Item -Path $registryFileName -Destination $OutputDir -ErrorAction Stop
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
} catch {
    $exitCode = 1
    Write-Host $PSItem -ForegroundColor Red
}

#=========================================================================

exit $exitCode
