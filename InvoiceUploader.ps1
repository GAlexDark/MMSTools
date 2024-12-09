
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
    [Parameter(Mandatory=$true,
    HelpMessage="Enter the configuration file name.")]
    [ValidateNotNullOrEmpty()]
    [string] $EnvFileName,

    [Parameter(Mandatory=$true,
    HelpMessage = "Enter the path to the folder were will be saved data files.")]
    [ValidateNotNullOrEmpty()]
    [string] $OutputDir,

    [Parameter(Mandatory=$false,
    HelpMessage = "Enter the path to the data file")]
    [string] $DataFile
)

#=========================================================================

function Calc-ShaHash {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $LocalStorage
    )

    [bool] $res = $true
    try {
        [array] $retVal = @()
        if ($LocalStorage.EndsWith('*')) {
            $LocalStorage = $LocalStorage.Replace('\*','')
        }
        $files = Get-ChildItem -Path $LocalStorage -Include $fileTypes -Recurse -ErrorAction Stop
        if ($files) {
            $files | ForEach-Object {
                [string] $pathToFile = $_.FullName.Replace('\', '/') # the replase for the pretty json format output
                $fileHash = [PSCustomObject]@{
                    'File Name' = "." + $pathToFile.Substring($LocalStorage.Length)
                    'Last Write Time' = (Get-ItemProperty -Path $pathToFile).LastWriteTime.ToString($dateTimeFormat)
                    'Hash' = (Get-FileHash -Algorithm SHA256 -Path $pathToFile -ErrorAction Stop).hash
                    }
            $retVal += $fileHash
            }
        } else {
            $res = $false
            Write-Host "No any *.xlsx and/or *.p7s files found." -ForegroundColor Red
        }
    } catch {
        $res = $false
        Write-Error $PSItem
    }

    return @{Status = $res; FileHashInfo = $retVal}
}

function Copy-File {
    [OutputType([bool])]
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $LocalStorage,

        [Parameter(Mandatory = $true)]
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
        $p7sFiles = Get-ChildItem -Path $LocalStorage -Include $fileTypes -Recurse -ErrorAction Stop
        if ($p7sFiles) {
            Copy-Item -Path $p7sFiles -Destination $newDir -ErrorAction Stop
        } else {
            Write-Host "No any *.xlsx and/or *.p7s files found."
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
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $LocalStorage,

        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $remotePath,

        [Parameter(Mandatory = $false)]
        [array] $UploadData
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
            $console = $Host.UI.RawUI
            $buffer = $console.BufferSize
            $buffer.Width = '400'
            $console.BufferSize = $buffer

            [int] $count = 0
            [int] $pathLenght = 0
            if ($LocalStorage.EndsWith('*')) {
                $pathLenght = $LocalStorage.Length - 1
            }
            if ($UploadData) {
                foreach($item in $UploadData) {
                    [string] $source = $item.Source
                    $session.PutFiles($source, $item.Destination, $true).Check()
                    Write-Host "The file $($source.Substring($pathLenght).Replace('\', '/')) was uploaded successfully." -ForegroundColor Green
                    $count++
                }
            } else {
                $transferResult = $session.PutFiles($LocalStorage, $remotePath, $true)
                # Throw on any error
                $transferResult.Check()
                # Print results
                foreach ($transfer in $transferResult.Transfers) {
                    Write-Host "The file $($transfer.FileName.Substring($pathLenght).Replace('\', '/')) was uploaded successfully." -ForegroundColor Green
                    $count++
                }
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

function Create-UploadData {
    param(
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $LocalStorage,

        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string[]] $RemoteFilePaths,

        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $RemotePath
    )

try {
    [bool] $res = $true
    [array] $retVal = @()
    $files = Get-ChildItem -Path $LocalStorage.Replace('*', '') -Include $fileTypes -Recurse -ErrorAction Stop
    [int] $length = $LocalStorage.Length - 1
    if ($files) {
        $files | ForEach-Object {
            [string] $fileName = $_.FullName.substring($length)
            [string] $pattern = "/" + $fileName + "$"
            [string[]] $buf = $RemoteFilePaths | Select-String -Pattern $pattern -ErrorAction Stop
            if ($buf.Count -eq 1) {
                $destonation = $buf[0].Replace($fileName, '*')
                $uploadDataItem = [PSCustomObject]@{
                    'Source' = $_
                    'Destination' = $RemotePath + $destonation
                }
                $retVal += $uploadDataItem
            } else {
                Write-Warning "No or Multiple matches found for the source '$fileName'`nThis source removed from the local storage."
                Remove-Item -Path $_.FullName -ErrorAction Stop
            }
        } #for
    } else {
        $res = $false
        Write-Host "No any *.xlsx and/or *.p7s files found."
    }
} catch {
    $res = $false
    Write-Error $PSItem
}
    return @{Status = $res; UploadData = $retVal}
}

try {
    [int] $exitCode = 0
    [bool] $retVal = $true

    $EnvFileName = $EnvFileName.Trim()
    if ([string]::IsNullOrEmpty($EnvFileName)) {
        throw "The 'EnvFileName' value is not set"
    }
    $EnvFileNamePath = Join-Path -Path $PSScriptRoot -ChildPath $EnvFileName -ErrorAction Stop
    $retVal = Test-Path -Path $EnvFileNamePath -ErrorAction Stop
    if (-not $retVal) {
        throw "The '$EnvFileName' file is not found."
    }

    $retVal = Test-Path -Path $OutputDir -ErrorAction Stop
    if (-not $retVal) {
        throw "The OutputDir '$OutputDir' is not exists."
    }

    #Mapping env variables
    . $EnvFileNamePath
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
    [String[]] $fileTypes = $FILE_TYPES

    if ([string]::IsNullOrEmpty($remoteHost)) {
        throw "The 'remoteHost' value is not set"
    }
    if ($remotePort -eq 0) {
        Write-Warning "`nThe 'remotePort' value is not set. Using the default value.`n"
    } elseif (($remotePort -lt 0) -and ($remotePort -le 65535)) {
        throw "The 'remotePort' value is wrong: $remotePort"
    }
    if ([string]::IsNullOrEmpty($remotePath)) {
        throw "The 'remotePath' value is not set"
    }
    if ([string]::IsNullOrEmpty($userName)) {
        throw "The 'userName' value is not set"
    }
    if ([string]::IsNullOrEmpty($userPassword)) {
        throw "The 'userPassword' value is not set"
    }
    if ([string]::IsNullOrEmpty($localStorage)) {
        throw "The 'localStorage' value is not set"
    } else {
        [string] $buf = if ($localStorage.EndsWith('*')) { $localStorage.Replace('*', '') } else { $localStorage }
        $retVal = Test-Path -Path $buf -ErrorAction Stop
        if (-not $retVal) {
            throw "The '$localStorage' directory is not exists."
        }
    }
    if ([string]::IsNullOrEmpty($archiveStorage)) {
        throw "The 'archiveStorage' value is not set"
    } else {
        $retVal = Test-Path -Path $archiveStorage -ErrorAction Stop
        if (-not $retVal) {
            throw "The '$archiveStorage' directory is not exists."
        }
    }
    if ([string]::IsNullOrEmpty($dateTimeFormat)) {
        throw "The 'dateTimeFormat' value is not set"
    }
    if ($fileTypes.Count -eq 0) {
        throw "The 'fileTypes' value is not set"
    }

#=========================================================================

    $retVal = Copy-File -LocalStorage $localStorage -ArchiveStorage $archiveStorage
    if (-not $retVal) {
        throw "Error copy files."
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

    [array] $uploadData = @()
    if (-not [string]::IsNullOrEmpty($DataFile)) {
        [string] $slaveScript = Join-Path -Path $PSScriptRoot -ChildPath "DirectoryContents.ps1" -ErrorAction Stop
        $retVal = Test-Path -Path $EnvFileName -ErrorAction Stop
        if (-not $retVal) {
            throw "The 'DirectoryContents.ps1' file is not found."
        }

        . $slaveScript -EnvFileName $EnvFileName -OutputDir $DataFile -Slave
        if ($LASTEXITCODE -ne 0) {
            throw "Error getting list of the files and directories."
        }

        $DataFile = Join-Path -Path $DataFile -ChildPath "registry.json" -ErrorAction Stop
        $retVal = Test-Path -Path $DataFile -ErrorAction Stop
        if (-not $retVal) {
            throw "The 'registry.json' file is not found."
        }
        $jsonData = Get-Content -Path $DataFile -ErrorAction Stop | ConvertFrom-Json -ErrorAction Stop
        [string[]] $remoteFilePaths = $jsonData.'File Name'

        $res1 = Create-UploadData -LocalStorage $localStorage -RemoteFilePaths $remoteFilePaths -RemotePath $remotePath
        if ($res1.Status) {
            $uploadData = $res1.UploadData
        } else {
            throw "Error get upload data."
        }
    }

    # If script must build upload path
    #$remotePath = $remotePath.Replace("*", $IssueDateFolder + "/*")
    #if ($SystemWideClearingId.Length -ne 0) {
    #    $remotePath = $remotePath.Remove("*", $SystemWideClearingId + "/*")
    #}

    $retVal = Upload-File -LocalStorage $LocalStorage -remotePath $remotePath -UploadData $uploadData
    if (-not $retVal) {
        throw "Upload files canceled."
    }
} catch {
    $exitCode = 1
    Write-Host $PSItem -ForegroundColor Red
}

#=========================================================================

exit $exitCode
