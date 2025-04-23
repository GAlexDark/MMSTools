
<#PSScriptInfo

.VERSION 1.0

.GUID 36b029b8-6b14-4a23-b2a7-959f737c6a5d

.AUTHOR Oleksii Gaienko <support@galexsoftware.info>

.COPYRIGHT 2024 Oleksii Gaienko. All rights reserved.

.TAGS Enumerator

.LICENSEURI https://www.gnu.org/licenses/gpl-3.0.html

.PROJECTURI https://github.com/GAlexDark/MMSTools

.RELEASENOTES This script comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under certain conditions. For more details, see the GNU General Public License.

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

    [Parameter(Mandatory=$false,
    HelpMessage = "Select to change to the Slave mode.")]
    [switch] $Slave = $false
)

function ConvertFrom-Filename {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $FileName
    )

    [bool] $retVal = $true
    [string] $fn = $FileName.Replace("/", "")
    [string] $baseName = $fn.Substring(0, $fn.IndexOf('.'))
    [string] $extension = $fn.Substring($fn.IndexOf('.') + 1)

    try {
        [string[]] $buffer = $baseName -split "_"
        [bool] $isREJECTED = $false
        [string] $defaultTimestamp = "1970-01-01"
        [string] $mask = "yyyy-MM-dd"
        [datetime] $dateFrom = [datetime]::ParseExact($defaultTimestamp, $mask, $null).AddHours(12).AddMinutes(0).AddSeconds(0)
        [datetime] $dateTo = [datetime]::ParseExact($defaultTimestamp, $mask, $null).AddHours(12).AddMinutes(0).AddSeconds(0)
        [string] $xCode = ""
        [datetime] $issueDate = [datetime]::ParseExact($defaultTimestamp, $mask, $null).AddHours(0).AddMinutes(0).AddSeconds(0)

        if ($buffer.Count -ge 4) {
            $dateFrom = [datetime]::ParseExact($buffer[1], $mask, $null).AddHours(12).AddMinutes(0).AddSeconds(0)
            $dateTo = [datetime]::ParseExact($buffer[2], $mask, $null).AddHours(12).AddMinutes(59).AddSeconds(59)
            $xCode = $buffer[3].Trim()

            switch ($buffer.Count) {
                7 {
                    $issueDate = [datetime]::ParseExact($buffer[5], $mask, $null).AddHours(($buffer[6] -split "-")[0]).AddMinutes(($buffer[6] -split "-")[1]).AddSeconds(0)
                    break
                }
                8 {
                    if ($buffer[5].Contains("-") -and $buffer[6].Contains("-")) {
                        $issueDate = [datetime]::ParseExact($buffer[5], $mask, $null).AddHours(($buffer[6] -split "-")[0]).AddMinutes(($buffer[6] -split "-")[1]).AddSeconds(0)
                    }
                    if ($buffer[6].Contains("-") -and $buffer[7].Contains("-")) {
                        $issueDate = [datetime]::ParseExact($buffer[6], $mask, $null).AddHours(($buffer[7] -split "-")[0]).AddMinutes(($buffer[7] -split "-")[1]).AddSeconds(0)
                    }
                    break
                }
                9 {
                    if ($buffer[6].Contains("-") -and $buffer[7].Contains("-")) {
                        $issueDate = [datetime]::ParseExact($buffer[6], $mask, $null).AddHours(($buffer[7] -split "-")[0]).AddMinutes(($buffer[7] -split "-")[1]).AddSeconds(0)
                    }
                    if ($buffer[7].Contains("-") -and $buffer[8].Contains("-")) {
                        $issueDate = [datetime]::ParseExact($buffer[7], $mask, $null).AddHours(($buffer[8] -split "-")[0]).AddMinutes(($buffer[8] -split "-")[1]).AddSeconds(0)
                    }
                    break
                }
                10 {
                    if ($buffer[7].Contains("-") -and $buffer[8].Contains("-")) {
                        $issueDate = [datetime]::ParseExact($buffer[7], $mask, $null).AddHours(($buffer[8] -split "-")[0]).AddMinutes(($buffer[8] -split "-")[1]).AddSeconds(0)
                    }
                    if ($buffer[8].Contains("-") -and $buffer[9].Contains("-")) {
                        $issueDate = [datetime]::ParseExact($buffer[8], $mask, $null).AddHours(($buffer[9] -split "-")[0]).AddMinutes(($buffer[9] -split "-")[1]).AddSeconds(0)
                    }
                    break
                }
                11 {
                    if ($buffer[7].Contains("-") -and $buffer[8].Contains("-")) {
                        $issueDate = [datetime]::ParseExact($buffer[7], $mask, $null).AddHours(($buffer[8] -split "-")[0]).AddMinutes(($buffer[8] -split "-")[1]).AddSeconds(0)
                    }
                    if ($buffer[8].Contains("-") -and $buffer[9].Contains("-")) {
                        $issueDate = [datetime]::ParseExact($buffer[8], $mask, $null).AddHours(($buffer[9] -split "-")[0]).AddMinutes(($buffer[9] -split "-")[1]).AddSeconds(0)
                    }
                    break
                }
                12 {
                    $issueDate = [datetime]::ParseExact($buffer[8], $mask, $null).AddHours(($buffer[9] -split "-")[0]).AddMinutes(($buffer[9] -split "-")[1]).AddSeconds(0)
                    break
                }
            }
            if ($FileName.Contains("REJECTED")) {
                $isREJECTED = $true
            }
        } else {
            $retVal = $false
        }
    } catch {
        $retVal = $false
    }

    return @{Status = $retVal; Basename = $baseName; Extension = $extension; DateFrom = $dateFrom; DateTo = $dateTo; XCode = $xCode; IssueDate = $issueDate; isREJECTED = $isREJECTED}
}

function Validate-Path {
    param (
        [string] $Path,
        [string] $ErrorMessage
    )
    if (-not (Test-Path -Path $Path -ErrorAction Stop)) {
        throw $ErrorMessage
    }
}

function Validate-Variable {
    param (
        [string] $VariableName,
        [string] $ErrorMessage
    )
    if ([string]::IsNullOrEmpty($VariableName)) {
        throw $ErrorMessage
    }
}

function Log-Message {
    param(
        [string] $Message,
        [string] $Color = "White"
    )
    Write-Host $Message -ForegroundColor $Color
}
#=========================================================================
try {
    [int] $exitCode = 0
    $EnvFileName = $EnvFileName.Trim()
    Validate-Variable -VariableName $EnvFileName -ErrorMessage "The 'EnvFileName' value is not set."
    $EnvFileName = Join-Path -Path $PSScriptRoot -ChildPath $EnvFileName -ErrorAction Stop
    Validate-Path -Path $EnvFileName -ErrorMessage "`nThe '$EnvFileName' file not found.`n"

    #Mapping env variables
    . $EnvFileName

    [string] $userName = $($env:sshUserName)
    [string] $userPassword = $($env:sshPassword)
    Validate-Variable -VariableName $userName -ErrorMessage "The 'userName' value is not set."
    Validate-Variable -VariableName $userPassword -ErrorMessage "The 'userPassword' value is not set."
    Validate-Variable -VariableName $REMOTE_HOST -ErrorMessage "The 'REMOTE_HOST' value is not set."

    if ($REMOTE_PORT -eq 0) {
        Log-Message "`nThe 'REMOTE_PORT' value is not set. Using the default value.`n" "Yellow"
        $REMOTE_PORT = 22
    } elseif (($REMOTE_PORT -lt 0) -and ($REMOTE_PORT -le 65535)) {
        throw "The 'REMOTE_PORT' value is wrong: $REMOTE_PORT."
    }
    Validate-Variable -VariableName $REMOTE_PATH -ErrorMessage "The 'REMOTE_PATH' value not set."
    Validate-Variable -VariableName $ARCHIVE_STORAGE -ErrorMessage "The 'ARCHIVE_STORAGE' value not set."
    Validate-Path -Path $ARCHIVE_STORAGE -ErrorMessage "The '$ARCHIVE_STORAGE' directory not exists."
    Validate-Variable -VariableName $DATETIME_FORMAT -ErrorMessage "The 'DATETIME_FORMAT' value not set."
    Validate-Variable -VariableName $DB_NAME -ErrorMessage "The 'DB_NAME' value is not set."
    Validate-Path -Path $DB_NAME -ErrorMessage "The '$DB_NAME' file not exists."

    [UInt64] $freeSpace = (Get-PSDrive -PSProvider FileSystem $LOCAL_STORAGE_MOUNT_POINT).Free
    if ($freeSpace -le $MIN_SPACE) {
        throw "The disk space is low"
    }
    #=========================================================================
    [string] $assemblyPath = if ($env:WINSCP_PATH) { $env:WINSCP_PATH } else { $PSScriptRoot }
    $assemblyPath = Join-Path -Path $assemblyPath -ChildPath "WinSCPnet.dll" -ErrorAction Stop
    Validate-Path -Path $assemblyPath -ErrorMessage "The 'WinSCPnet.dll' library not found."
    Add-Type -Path $assemblyPath -ErrorAction Stop

    # Setup session options
    $sessionOptions = New-Object WinSCP.SessionOptions -Property @{
        Protocol = [WinSCP.Protocol]::Sftp
        HostName = $REMOTE_HOST
        PortNumber = $REMOTE_PORT
        UserName = $userName
        Password = $userPassword
    }
    $sessionOptions.AddRawSettings("SendBuf", "0")
    $sessionOptions.AddRawSettings("SshSimple", "0")
    #$sessionOptions.AddRawSettings("SFTPListingQueue", "4")
    #$sessionOptions.AddRawSettings("SFTPDownloadQueue", "1500")

    if (![string]::IsNullOrEmpty($PROXY_NAME) -and ![string]::IsNullOrEmpty($PROXY_PORT) ) {
        $sessionOptions.AddRawSettings("ProxyMethod", "3")
        $sessionOptions.AddRawSettings("ProxyHost", $PROXY_NAME)
        $sessionOptions.AddRawSettings("ProxyPort", $PROXY_PORT)
    } else {
        Log-Message "`nThe Proxy is not using (The settings is not present).`n" "Yellow"
    }

    [bool] $retVal = $true
    $session = New-Object WinSCP.Session
    $sessionOptions.SshHostKeyFingerprint = $session.ScanFingerprint($sessionOptions, "SHA-256")
    Log-Message "Connect to the $REMOTE_HOST..."
    $session.Open($sessionOptions)
    Log-Message "Connection to the $REMOTE_HOST succeeded."

    [string] $buf = if ($Slave) { "remote host" } else { $REMOTE_PATH }
    Log-Message "`nStarting to get a list of the files and the directories in the $buf." "Yellow"
    $filesInfo = $session.EnumerateRemoteFiles($REMOTE_PATH, "*.*", [WinSCP.EnumerationOptions]::AllDirectories)

    $assemblyPath = Join-Path -Path $PSScriptRoot -ChildPath "System.Data.SQLite.dll" -ErrorAction Stop
    Validate-Path -Path $assemblyPath -ErrorMessage "The System.Data.SQLite.dll library not found"
    Add-Type -Path $assemblyPath -ErrorAction Stop

    $connectionString = [string]::Format("Data Source={0};Version=3;", $DB_NAME)
    $oSQLiteDBConnection = New-Object System.Data.SQLite.SQLiteConnection($connectionString)

    Log-Message "Open DB connection..."
    $oSQLiteDBConnection.open()
    Log-Message "DB connection opened.`n"

    $prepareCommand = $oSQLiteDBConnection.CreateCommand()
    $prepareCommand.Commandtext = "DROP TABLE IF EXISTS Fileinfo;"
    $prepareCommand.CommandType = [System.Data.CommandType]::Text
    $prepareCommand.ExecuteNonQuery() | Out-Null

    $prepareCommand.Commandtext = "VACUUM;"
    $prepareCommand.CommandType = [System.Data.CommandType]::Text
    $prepareCommand.ExecuteNonQuery() | Out-Null

    $prepareCommand.Commandtext = "CREATE TABLE Fileinfo (FileName TEXT NOT NULL, FullFileName TEXT NOT NULL, LastWriteTime TEXT NOT NULL, Owner TEXT NOT NULL, Permissions TEXT NOT NULL, DateFrom DATETIME NOT NULL, DateTo DATETIME NOT NULL, XCode TEXT, IssueDate DATETIME NOT NULL, IsREJECTED BOOLEAN NOT NULL, Extension TEXT);"
    $prepareCommand.CommandType = [System.Data.CommandType]::Text
    $prepareCommand.ExecuteNonQuery() | Out-Null

    $transaction = $oSQLiteDBConnection.BeginTransaction()

    $oSQLiteDBCommand = $oSQLiteDBConnection.CreateCommand()
    $oSQLiteDBCommand.Commandtext = "INSERT INTO Fileinfo (FileName, FullFileName, LastWriteTime, Owner, Permissions, DateFrom, DateTo, XCode, IssueDate, IsREJECTED, Extension) VALUES (@FileName, @FullFileName, @LastWriteTime, @Owner, @Permissions, @DateFrom, @DateTo, @XCode, @IssueDate, @IsREJECTED, @Extension);"

    $oSQLiteDBCommand.Parameters.Add((New-Object System.Data.SQLite.SQLiteParameter("@FileName", [System.Data.DbType]::String))) | Out-Null
    $oSQLiteDBCommand.Parameters.Add((New-Object System.Data.SQLite.SQLiteParameter("@FullFileName", [System.Data.DbType]::String))) | Out-Null
    $oSQLiteDBCommand.Parameters.Add((New-Object System.Data.SQLite.SQLiteParameter("@LastWriteTime", [System.Data.DbType]::String))) | Out-Null
    $oSQLiteDBCommand.Parameters.Add((New-Object System.Data.SQLite.SQLiteParameter("@Owner", [System.Data.DbType]::String))) | Out-Null
    $oSQLiteDBCommand.Parameters.Add((New-Object System.Data.SQLite.SQLiteParameter("@Permissions", [System.Data.DbType]::String))) | Out-Null

    $oSQLiteDBCommand.Parameters.Add((New-Object System.Data.SQLite.SQLiteParameter("@DateFrom", [System.Data.DbType]::DateTime))) | Out-Null
    $oSQLiteDBCommand.Parameters.Add((New-Object System.Data.SQLite.SQLiteParameter("@DateTo", [System.Data.DbType]::DateTime))) | Out-Null
    $oSQLiteDBCommand.Parameters.Add((New-Object System.Data.SQLite.SQLiteParameter("@XCode", [System.Data.DbType]::String))) | Out-Null
    $oSQLiteDBCommand.Parameters.Add((New-Object System.Data.SQLite.SQLiteParameter("@IssueDate", [System.Data.DbType]::DateTime))) | Out-Null
    $oSQLiteDBCommand.Parameters.Add((New-Object System.Data.SQLite.SQLiteParameter("@IsREJECTED", [System.Data.DbType]::Boolean))) | Out-Null
    $oSQLiteDBCommand.Parameters.Add((New-Object System.Data.SQLite.SQLiteParameter("@Extension", [System.Data.DbType]::String))) | Out-Null

    [int] $count = 0
    foreach ($fileInfo in $filesInfo) { # $fileInfo - RemoteFileInfo
        [string] $buf = [WinSCP.RemotePath]::EscapeFileMask($fileInfo.FullName)
        [string] $name = Split-Path -Path $buf -Leaf -ErrorAction Stop
        $oSQLiteDBCommand.Parameters["@FileName"].Value = [string]::Format("/{0}", $name)
        $oSQLiteDBCommand.Parameters["@FullFileName"].Value = $buf
        $oSQLiteDBCommand.Parameters["@LastWriteTime"].Value = $fileInfo.LastWriteTime.ToString($DATETIME_FORMAT)
        $oSQLiteDBCommand.Parameters["@Owner"].Value = "$($fileInfo.Owner):$($fileInfo.Group)"
        $oSQLiteDBCommand.Parameters["@Permissions"].Value = $fileInfo.FilePermissions.Text

        $data = ConvertFrom-Filename -FileName $name
        if (-not $data.Status) {
            Log-Message $name
        }
        $oSQLiteDBCommand.Parameters["@DateFrom"].Value = $data.DateFrom
        $oSQLiteDBCommand.Parameters["@DateTo"].Value = $data.DateTo
        $oSQLiteDBCommand.Parameters["@XCode"].Value = $data.XCode
        $oSQLiteDBCommand.Parameters["@IssueDate"].Value = $data.IssueDate
        $oSQLiteDBCommand.Parameters["@IsREJECTED"].Value = $data.IsREJECTED
        $oSQLiteDBCommand.Parameters["@Extension"].Value = $data.Extension

        $oSQLiteDBCommand.ExecuteNonQuery() | Out-Null
        $count++
    }
    Log-Message "`nTotal files: $count`n"
    $transaction.Commit()
} catch {
    Log-Message "An error occurred: $($_.Exception.Message)" "Red"
    $exitCode = 1
} finally {
    if ($oSQLiteDBConnection) {
        Log-Message "DB closed"
        $oSQLiteDBConnection.Close()
        $oSQLiteDBConnection.Dispose()
    }
    if ($session) {
        $session.Dispose()
    }
    Log-Message "`nDONE" "Green"
    exit $exitCode
}
