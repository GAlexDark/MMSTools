
<#PSScriptInfo

.VERSION 1.0

.GUID 36b029b8-6b14-4a23-b2a7-959f737c6a5d

.AUTHOR Oleksii Gaienko <support@galexsoftware.info>

.COPYRIGHT 2024 Oleksii Gaienko. All rights reserved.

.TAGS DirectoryContents

.LICENSEURI https://www.gnu.org/licenses/gpl-3.0.html

.PROJECTURI https://github.com/GAlexDark/MMSTools

.RELEASENOTES This script comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.

#>

<#

.SYNOPSIS
 Get a list of the files on the remote MMS resource.

.DESCRIPTION
 The PoSH script to get a list of files on the remote MMS resource.

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
    HelpMessage = "Select to add additional data to the data files.")]
    [switch] $Advanced = $false,

    [Parameter(Mandatory=$false,
    HelpMessage = "Select to change to the Slave mode.")]
    [switch] $Slave = $false
)

#=========================================================================
try {
    [int] $exitCode = 0
    if ($Slave) {
        $Advanced = $false
    }

    $EnvFileName = $EnvFileName.Trim()
    if ([string]::IsNullOrEmpty($EnvFileName)) {
        throw "The 'EnvFileName' value is not set."
    }
    $EnvFileName = Join-Path -Path $PSScriptRoot -ChildPath $EnvFileName -ErrorAction Stop
    [bool] $retVal = Test-Path -Path $EnvFileName -ErrorAction Stop
    if (-not $retVal) {
        throw "`nThe '$EnvFileName' file is not found.`n"
    }

    $retVal = Test-Path -Path $OutputDir -ErrorAction Stop
    if (-not $retVal) {
        throw "`nThe OutputDir '$OutputDir' is not exists.`n"
    }

    #Mapping env variables
    . $EnvFileName
    [string] $proxyName = $PROXY_NAME
    [string] $proxyPort = $PROXY_PORT

    [string] $remoteHost = $REMOTE_HOST
    [int] $remotePort = $REMOTE_PORT
    [string] $remotePath = $REMOTE_PATH

    [string] $userName = $($env:sshUserName)
    [string] $userPassword = $($env:sshPassword)

    [string] $archiveStorage = $ARCHIVE_STORAGE
    [string] $dateTimeFormat = $DATETIME_FORMAT

    if ([string]::IsNullOrEmpty($remoteHost)) {
        throw "The 'remoteHost' value is not set."
    }
    if ($remotePort -eq 0) {
        Write-Host `n
        Write-Warning "The 'remotePort' value is not set. Using the default value.`n"
    } elseif (($remotePort -lt 0) -and ($remotePort -le 65535)) {
        throw "The 'remotePort' value is wrong: $remotePort."
    }
    if ([string]::IsNullOrEmpty($remotePath)) {
        throw "The 'remotePath' value is not set."
    }
    if ([string]::IsNullOrEmpty($userName)) {
        throw "The 'userName' value is not set."
    }
    if ([string]::IsNullOrEmpty($userPassword)) {
        throw "The 'userPassword' value is not set."
    }
    if ([string]::IsNullOrEmpty($archiveStorage)) {
        throw "The 'archiveStorage' value is not set."
    } else {
        $retVal = Test-Path -Path $archiveStorage -ErrorAction Stop
        if (-not $retVal) {
            throw "`nThe '$archiveStorage' directory is not exists.`n"
        }
    }
    if ([string]::IsNullOrEmpty($dateTimeFormat)) {
        throw "The 'dateTimeFormat' value is not set."
    }

    #=========================================================================

    [string] $assemblyPath = if ($env:WINSCP_PATH) { $env:WINSCP_PATH } else { $PSScriptRoot }
    $assemblyPath = Join-Path -Path $assemblyPath -ChildPath "WinSCPnet.dll" -ErrorAction Stop
    $retVal = Test-Path -Path $assemblyPath -ErrorAction Stop
    if (-not $retVal) {
        throw "The WinSCPnet.dll library is not found."
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
        Write-Host "`nThe Proxy is not using (The settings is not present).`n" -ForegroundColor Yellow
    }

    $session = New-Object WinSCP.Session
    try {
        $sessionOptions.SshHostKeyFingerprint = $session.ScanFingerprint($sessionOptions, "SHA-256")
        # Connect
        Write-Host "Connect to the $remoteHost."
        $session.Open($sessionOptions)
        Write-Host "Connection to the $remoteHost succeeded."

        [string] $buf = if ($Slave) { "remote host" } else { $remotePath }
        Write-Host "`nStarting to get a list of the files and the directories in the $buf." -ForegroundColor Yellow
        $filesInfo = $session.EnumerateRemoteFiles($remotePath, "*.*", [WinSCP.EnumerationOptions]::AllDirectories)

        # Print results
        [int] $count = 0
        [array] $registry = @()
        foreach ($fileInfo in $filesInfo) { # $fileInfo - RemoteFileInfo
            if ($Advanced) {
                $registryItem = [PSCustomObject]@{
                    'File Name' = ([WinSCP.RemotePath]::EscapeFileMask($fileInfo.FullName)).Substring($remotePath.Length)
                    'Last Write Time' = $fileInfo.LastWriteTime.ToString($dateTimeFormat)
                    'Owner' = $fileInfo.Owner + ":" + $fileInfo.Group
                    'Permissions' = $fileInfo.FilePermissions.Text
                }
            } else {
                $registryItem = [PSCustomObject]@{
                    'File Name' = ([WinSCP.RemotePath]::EscapeFileMask($fileInfo.FullName)).Substring($remotePath.Length)
                    'Last Write Time' = $fileInfo.LastWriteTime.ToString($dateTimeFormat)
                }
            }

            $count++
            $registry += $registryItem
        }

        Remove-Item -Path (Join-Path -Path $OutputDir -ChildPath "*.json") -ErrorAction Stop
        Remove-Item -Path (Join-Path -Path $OutputDir -ChildPath "*.csv") -ErrorAction Stop

        if ($registry) {
            if (-not $Slave) {
                Write-Host "`nList of files and directories received.`n"
                $console = $Host.UI.RawUI
                $buffer = $console.BufferSize
                $buffer.Width = '400'
                $console.BufferSize = $buffer
                $registry | Format-Table -AutoSize

                Write-Host "Total $($count) files."
            }

            [string] $registryFileName = if ($Slave) { "registry.json" } else { "registry" + (Get-Date).ToString('yyyyMMddHHmmss') + ".json" }
            $registryFileName = Join-Path -Path $OutputDir -ChildPath $registryFileName -ErrorAction Stop
            $registry | ConvertTo-Json -Compress | Out-File $registryFileName

            if (-not $Slave) {
                Copy-Item -Path $registryFileName -Destination $archiveStorage -ErrorAction Stop
                $registry | ConvertTo-Csv -Delimiter ';' | Out-File -encoding UTF8 -filepath (Join-Path -Path $OutputDir -ChildPath "fileslist.csv" -ErrorAction Stop)
            }
        } else {
            Write-Host "No files found."
        }
    } finally {
        # Disconnect, clean up
        $session.Dispose()
    }
} catch {
    $exitCode = 1
    Write-Host $PSItem -ForegroundColor Red
}

exit $exitCode
