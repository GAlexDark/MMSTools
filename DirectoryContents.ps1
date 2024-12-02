
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
    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    [string] $EnvFileName,

    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    [string] $OutputDir,

    [Parameter(Mandatory=$false)]
    [switch] $Advanced = $false
)

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
[string] $remotePath = $REMOTE_PATH

[string] $userName = $($env:sshUserName)
[string] $userPassword = $($env:sshPassword)
[string] $SshHostKeyFingerprint = $($env:sshHostKeyFingerprint)

[string] $archiveStorage = $ARCHIVE_STORAGE

[string] $dateTimeFormat = $DATETIME_FORMAT
#=========================================================================

try {
    Remove-Item -Path (Join-Path -Path $OutputDir -ChildPath "*.json") -ErrorAction Stop
    [string] $assemblyPath = if ($env:WINSCP_PATH) { $env:WINSCP_PATH } else { $PSScriptRoot }
    $assemblyPath = Join-Path -Path $assemblyPath -ChildPath "WinSCPnet.dll"
    $retVal = Test-Path -Path $assemblyPath
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
        SshHostKeyFingerprint = $SshHostKeyFingerprint
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
        # Connect
        Write-Host "Connect to the $($sessionOptions.HostName)"
        $session.Open($sessionOptions)
        Write-Host "Connection to the $($sessionOptions.HostName) succeeded"

        Write-Host "Starting to get a list of the files and the directories in the $remotePath" -ForegroundColor Yellow
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
        if ($registry) {
            #$registry | Format-Table -AutoSize
            [string] $registryFileName = "registry" + (Get-Date).ToString('yyyyMMddHHmmss') + ".json"
            $registryFileName = Join-Path -Path $OutputDir -ChildPath $registryFileName
            $registry | ConvertTo-Json -Compress | Out-File $registryFileName
            Write-Host "List of files and directories received`nTotal $($count) files."
            Copy-Item -Path $registryFileName -Destination $archiveStorage -ErrorAction Stop
        } else {
            Write-Host "No files found"
        }
    } finally {
        # Disconnect, clean up
        $session.Dispose()
    }
} catch {
    Write-Error $PSItem
    exit 1
}

exit 0
