
<#PSScriptInfo

.VERSION 2.0

.GUID f8c0a75c-e65c-4aee-9adf-59b2f80f97a9

.AUTHOR Oleksii Gaienko <support@galexsoftware.info>

.COPYRIGHT 2025 Oleksii Gaienko. All rights reserved.

.TAGS p7bmaker

.LICENSEURI https://www.gnu.org/licenses/#GPL

.PROJECTURI https://github.com/GAlexDark/MMSTools

#>

<#

.SYNOPSIS
 Downloads the p7b file from the IIT site, adds certificates to it and upload BASE64 data to the Vault.

.DESCRIPTION
 This script downloads a p7b file, processes certificates, and optionally uploads BASE64 data to the Vault.
 This script must be located in the same folder where p7bmaker.exe is located.

.PARAMETER EnvFileName
 Specifies the configuration file.

.PARAMETER Workdir
 Specifies the working directory for certificate and p7b file operations.

.PARAMETER UseProxy
 Flag to use a corporate network proxy.

.PARAMETER Url
 Optional URL for downloading the p7b file. Attention! The URL value must not contains the '?d=' parameter!

.PARAMETER Upload
 Flag to upload p7b BASE data to the Vault.

.PARAMETER DateStamp
 Optional datestamp from the IIT letter. If the '01011970' value was set, this parameter was ignored.

.INPUTS
 None. You can't pipe objects to p7bmaker.ps1.

.OUTPUTS
 None. p7bmaker.ps1 doesn't generate any output.

.EXAMPLE
 PS> .\p7bmaker.ps1 -EnvFileName test.ps1 -Workdir B:\certdata -Upload
 The p7b file maker PoSH Script Version 2.0
 Copyright (C) 2025 Oleksii Gaienko, support@galexsoftware.info
 This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under certain conditions. For more details, see the GNU General Public License.

 Attention! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!!
  Or use the 'Url' script parameter.

 Files with specified extensions were successfully removed from 'B:\certdata'.
 OK - The domain name 'demosite.local' was resolved to the IP address: [hidden as minor]
 OK - The 'https://demosite.local/download/CACertificates.p7b' URL available.

 Downloading from https 'https://demosite.local/download/CACertificates.p7b'.

 OK - The file was successfully saved in the B:\certdata folder.

 Starting B:\MMSTools\p7bmaker.exe...

 Start reading p7b file 'B:/certdata/CACertificates.p7b'.
 The p7b file 'B:/certdata/CACertificates.p7b' was read successful.

 The number of certificates in the container before adding: 628.

 The 2 certificates found.
 Start adding certificates.
 The certificates were added successful.

 The number of certificates in the container after adding: 631.

 Output format: PEM
 File 'B:/certdata/CACertificates20240917.p7b' saved successfully.

 Calculating result p7b file hash:
 SHA-1 hash: 3d70fbefe0c4a30fa75926301349cf0ecbd362db.

 Creating hash File.
 The hash file 'B:/certdata/CACertificates20240917.sha' saved successfully.

 DONE
 OK - The hash file 'B:\certdata\CACertificates20240917.base64.sha' was successfully created.
 OK - The 'http://vault.local:8200' URL available.

 Checking the Vault status...
 OK - The Vault is available.

 Connect to the Vault...
 Connected.

 Upload data to the Vault...
 The secret version 2 was created successfully:

 request_id     : 00951943-0fed-e552-9f80-f4c45c8e1abe
 lease_id       :
 renewable      : False
 lease_duration : 0
 data           : @{created_time=2025-03-04T17:01:02.275398322Z; custom_metadata=; deletion_time=; destroyed=False; version=2}
 wrap_info      :
 warnings       :
 auth           :
 mount_type     : kv

 The metadata was uploaded successfully.
 The data was uploaded successfully.

 Creating archive: CACertificates20240917.zip
 Creating the B:\certdata\20240917 folder and copy files to it
 Files with specified extensions were successfully removed from 'B:\certdata'.

 DONE

.EXAMPLE
 PS> .\p7bmaker.ps1 -EnvFileName test.ps1 -Workdir B:\certdata -Url "https://demosite.local/download_new/CACertificates.p7b" -Upload
 The p7b file maker PoSH Script Version 2.0
 Copyright (C) 2025 Oleksii Gaienko, support@galexsoftware.info
 This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under certain conditions. For more details, see the GNU General Public License.

 Attention! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!!
  Or use the 'Url' script parameter.

 The value of the 'download_url' was changed with the value from the Url argument...

 Files with specified extensions were successfully removed from 'B:\certdata'.
 OK - The domain name 'demosite.local' was resolved to the IP address: [hidden as minor]
 OK - The 'https://demosite.local/download_new/CACertificates.p7b' URL available.

 Downloading from 'https://demosite.local/download_new/CACertificates.p7b'.

 OK - The file was successfully saved in the B:\certdata\ folder.

 Starting B:\MMSTools\p7bmaker.exe...

 Start reading p7b file 'B:/certdata/CACertificates.p7b'.
 The p7b file 'B:/certdata/CACertificates.p7b' was read successful.

 The number of certificates in the container before adding: 628.

 The 2 certificates found.
 Start adding certificates.
 The certificates were added successful.

 The number of certificates in the container after adding: 631.

 Output format: PEM
 File 'B:/certdata/CACertificates20240917.p7b' saved successfully.

 Calculating result p7b file hash:
 SHA-1 hash: 3d70fbefe0c4a30fa75926301349cf0ecbd362db.

 Creating hash File.
 The hash file 'B:/certdata/CACertificates20240917.sha' saved successfully.

 DONE
 OK - The hash file 'B:\certdata\CACertificates20240917.base64.sha' was successfully created.
 OK - The 'http://vault.local:8200' URL available.

 Checking the Vault status...
 OK - The Vault available.

 Connect to the Vault...
 Connected.

 Upload data to the Vault...
 The secret version 2 was created successfully:

 request_id     : 00951943-0fed-e552-9f80-f4c45c8e1abe
 lease_id       :
 renewable      : False
 lease_duration : 0
 data           : @{created_time=2025-03-04T17:01:02.275398322Z; custom_metadata=; deletion_time=; destroyed=False; version=2}
 wrap_info      :
 warnings       :
 auth           :
 mount_type     : kv

 The metadata was uploaded successfully.
 The data was uploaded successfully.

 Creating archive: CACertificates20240917.zip
 Creating the B:\certdata\20240917 folder and copy files to it
 Files with specified extensions were successfully removed from 'B:\certdata'.

 DONE

#>

#requires -version 7

[CmdletBinding()]
Param (
    [Parameter(Mandatory=$true,
    HelpMessage="Enter the configuration file name.")]
    [ValidateNotNullOrEmpty()]
    [string] $EnvFileName,

    [Parameter (Mandatory=$true,
    HelpMessage="Enter the path to the folder, contains *.cer and/or *.crt files and were will be saved p7b files.")]
    [ValidateNotNullOrEmpty()]
    [string] $Workdir,

    [Parameter (Mandatory=$false,
    HelpMessage="Enable flag when the script run in the Corporate Network.")]
    [switch] $UseProxy = $false,

    [Parameter (Mandatory=$false,
    HelpMessage="Enter the actual Download Url.")]
    [string] $Url,

    [Parameter (Mandatory=$false,
    HelpMessage="Enable flag when the script must upload p7b to the Vault.")]
    [switch] $Upload = $false,

    [Parameter (Mandatory=$false,
    HelpMessage="Enter the datestamp.")]
    [ValidatePattern("^[0-9]{8}$")]
    [string] $DateStamp
)

[int] $exitCode = 0
function Log-Message {
    param(
        [string] $Message,
        [string] $Color = "White"
    )
    Write-Host $Message -ForegroundColor $Color
}

function Validate-Path {
    param(
        [string] $Path,
        [string] $ErrorMessage
    )
    if (-not (Test-Path -Path $Path -ErrorAction Stop)) {
        throw $ErrorMessage
    }
}

try {
    [System.Net.ServicePointManager]::ServerCertificateValidationCallback = {$true}
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls -bor [Net.SecurityProtocolType]::Tls11 -bor [Net.SecurityProtocolType]::Tls12
	$ErrorView = "NormalView"
	$PSStyle.OutputRendering = "PlainText"

	[string] $timeStamp = (Get-Date -ErrorAction Stop).ToString('yyyyMMddHHmmss')
	if (![string]::IsNullOrEmpty($timeStamp)) {
		Log-Message "`nID: $timeStamp"
	}

    [string] $copyright = "`nThe p7b file maker PoSH Script Version 2.0`n" +
                          "Copyright (C) 2025 Oleksii Gaienko, support@galexsoftware.info`n" +
                          "This program comes with ABSOLUTELY NO WARRANTY. " +
                          "This is free software, and you are welcome to redistribute it under certain conditions. " +
                          "For more details, see the GNU General Public License.`n"
    Log-Message $copyright "Green"
    $copyright = "Attention! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!!`n " +
                 "Or use the 'Url' script parameter.`n"
    Log-Message $copyright "Cyan"

    $assemblyPath = Join-Path -Path $PSScriptRoot -ChildPath "p7butils.psm1" -ErrorAction Stop
    Validate-Path -Path $assemblyPath -ErrorMessage "The p7butils.psm1 not found."
    Import-Module -Name $assemblyPath -ErrorAction Stop

    $EnvFileName = $EnvFileName.Trim()
    $EnvFileNamePath = Join-Path -Path $PSScriptRoot -ChildPath $EnvFileName -ErrorAction Stop
    Validate-Path -Path $EnvFileNamePath -ErrorMessage "The '$EnvFileName' file not found."

    $Workdir = $Workdir.Trim()
    if ($Path -eq "." -or $Path -eq ".\") {
        $Path = $PSScriptRoot
    }
    Validate-Path -Path $Workdir -ErrorMessage "The '$Workdir' folder not found."

    #Mapping env variables
    . $EnvFileNamePath
    [UInt64] $freeSpace = (Get-PSDrive -PSProvider FileSystem $LOCAL_STORAGE_MOUNT_POINT -ErrorAction Stop).Free
    if ($freeSpace -le $MIN_SPACE) {
        throw "The disk space is low."
    }

    #**************************************************************************************
    # Задаем Url, откуда будет загружаться файл CACertificates.p7b:
    [string] $download_url = "https://demosite.local/download/CACertificates.p7b"
    #
    #**************************************************************************************
    [string] $userAgent = "p7bmaker PoSH module/2.0"
    # Nondefault Web Proxy:
    [string] $proxyName = ""
    #
    #**************************************************************************************

    # Check domain name
    if (-not [string]::IsNullOrEmpty($Url)) {
        $download_url = $Url.Trim()
        Log-Message "The value of the 'download_url' was changed with the value from the Url argument...`n" "Yellow"
    }
    if ($DateStamp -and $DateStamp -ne "01011970") {
        $download_url += "?d=$DateStamp"
    }

    #Removing existing p7b, sha, base64, and zip files
    [string[]] $ext = ('*.p7b', '*.sha', '*.zip', '*.base64')
    $retVal = Remove-File -Path $Workdir -Extensions $ext
    if (-not $retVal) {
        throw "Error remove files in the $Workdir folder"
    }

    #Checking certs
    $certs = Get-ChildItem -Path $Workdir -Filter *.cer -ErrorAction Stop
    if ($certs) {
        $currentDate = Get-Date -ErrorAction Stop
        foreach ($cert in $certs) {
            try {
                $certItem = New-Object System.Security.Cryptography.X509Certificates.X509Certificate2 $cert.FullName
                [datetime] $certValidTo = $certItem.NotAfter
                if ($currentDate -gt $certValidTo) {
                    Log-Message "The certificate SN=$($certItem.SerialNumber) expired!" "Yellow"
                    Rename-Item -Path $cert.FullName -NewName "$($cert.FullName).expired" -ErrorAction Stop
                }
            } finally {
                if ($certItem) {
                    $certItem.Dispose()
                }
            }
        }
    } else {
        throw "The certificates is not found!"
    }

    [string] $domain = Get-DomainFromUrl $download_url
    Test-CheckDnsName $domain
    Test-CheckURL -Url $download_url

    #Download and save *.p7b file
    Log-Message "`nDownloading from $download_url.`n"
    Get-DownloadFile -DownloadUrl $download_url -SavePath $WorkDir

    # Let's start creating a p7b container with the necessary certificates
    $p7bmakerPath = Join-Path -Path $PSScriptRoot -ChildPath "p7bmaker.exe" -ErrorAction Stop
    Validate-Path -Path $p7bmakerPath -ErrorMessage "Executable p7bmaker.exe not found."

    Write-Output "`nStarting $p7bmakerPath...`n"
    $argsList = if ($Upload) { "-l $WorkDir --silent -o PEM" } else { "-l $WorkDir --silent" }
    $processInfo = Start-Process -FilePath $p7bmakerPath -ArgumentList $argsList -Wait -NoNewWindow -PassThru
    if ($processInfo.ExitCode -ne 0) {
        throw "The $p7bmakerPath returned an error."
    }

    #create filenames
    [string] $baseName = Get-FileNameFromUrl $download_url
    $baseName = $baseName.Substring(0, $baseName.LastIndexOf('.'))
    [string] $ts = (Get-Date -ErrorAction Stop).ToString('yyyyMMdd')

    [string] $archiveName = Join-Path -Path $Workdir -ChildPath "$baseName$ts.zip" -ErrorAction Stop
    [string] $newP7b = Join-Path -Path $Workdir -ChildPath "$baseName$ts.p7b" -ErrorAction Stop
    [string] $newSha = Join-Path -Path $Workdir -ChildPath "$baseName$ts.sha" -ErrorAction Stop

    #create .base64, if needed
    if ($Upload) {
        $content = Get-Content -Path $newP7b -Raw -ErrorAction Stop
        if ($content) {
            [string] $newP7bBase64 = Join-Path -Path $Workdir -ChildPath "$baseName$ts.base64" -ErrorAction Stop
            $pattern = "(?s)(?<=-----BEGIN PKCS7-----).*?(?=-----END PKCS7-----)"
            $matchesResult = [regex]::Matches($content, $pattern)
            if ($matchesResult.Count -gt 0) {
                $pkcs7Content = $matchesResult[0].Value.Trim() -replace '\r?\n', ''
                Out-File -FilePath $newP7bBase64 -InputObject $pkcs7Content -Encoding ascii -NoNewline -ErrorAction Stop
            } else {
                throw "Wrong PKCS7 file format."
            }

            $retVal = Get-ShaHashFile -fileName $newP7bBase64
            if (-not $retVal) {
                throw "Error create the SHA1 hash file for the $newP7bBase64 file."
            }
        } else {
            Log-Message "Error read the $newP7b file!" "Red"
        }

        try {
            #Upload base64 to the Vault
            [string] $userName = $($env:vaultUserName)
            [string] $userPassword = $($env:vaultPassword)

            if ([string]::IsNullOrEmpty($userName)) {
                throw "The 'userName' value is not set"
            }
            if ([string]::IsNullOrEmpty($userPassword)) {
                throw "The 'userPassword' value is not set"
            }
            if ([string]::IsNullOrEmpty($VAULT_ADDRESS)) {
                throw "The 'vaultAddress' value is not set"
            }
            if ([string]::IsNullOrEmpty($SECRET_NAME)) {
                throw "The 'secretName' value is not set"
            }
            if ([string]::IsNullOrEmpty($DATETIME_FORMAT)) {
                throw "The 'dateTimeFormat' value is not set"
            }

            [string] $authPath = "v1/auth/userpass/login/"
            [string] $sealedPath = "v1/sys/seal-status"
            [string] $apiPath = "v1/cryptography/data/certificate/$SECRET_NAME"
            [string] $metadataPath = "v1/cryptography/metadata/certificate/$SECRET_NAME"

            Test-CheckURL -Url $VAULT_ADDRESS -IsUsingProxy $false

            Log-Message "`nChecking the Vault Endpoint status..."
            [string] $vaultUrl = [string]::Format("{0}/{1}", $VAULT_ADDRESS, $sealedPath)
            $content = Invoke-WebRequest -Uri $vaultUrl -Method 'Get' -Headers @{ "Cache-Control" = "no-cache" } -SkipCertificateCheck -ErrorAction Stop
            [int] $statusCode = $content.StatusCode
            if ($statusCode -ne 200) {
                throw "Error connect to the Vault: $statusCode"
            }

            $sealedData = $content.Content | ConvertFrom-Json
            [bool] $sealed = $sealedData.sealed
            [bool] $initialized = $sealedData.initialized
            if (-not $sealed -and -not $initialized) {
                throw "The Vault is not initialized."
            }
            Log-Message "OK - The Vault is available.`n" "Green"

            Log-Message "Connect to the Vault..."
            $postParams = @{"password"="$userPassword"} | ConvertTo-Json -ErrorAction Stop
            $vaultUrl = [string]::Format("{0}/{1}{2}", $VAULT_ADDRESS, $authPath, $userName)
            $content = Invoke-RestMethod -Uri $vaultUrl -Method 'Post' -ContentType 'application/json' -Body $postParams -Headers @{ "Cache-Control" = "no-cache" } -SkipCertificateCheck -ErrorAction Stop
            [string] $clientToken = $content.auth.client_token
            if ([string]::IsNullOrEmpty($clientToken)) {
                throw "Wrong client token."
            }
            Log-Message "Connected.`n"

            Log-Message "Upload data to the Vault..."
            $p7bPayload = Get-Content -Path $newP7bBase64 -Encoding Ascii -ErrorAction Stop
            $postParams = @{"data"=@{"certificate"="$p7bPayload"}} | ConvertTo-Json -ErrorAction Stop
            $header = @{"X-Vault-Token"="$clientToken";"Cache-Control"="no-cache"}
            $vaultUrl = [string]::Format("{0}/{1}", $VAULT_ADDRESS, $apiPath)
            $content = Invoke-RestMethod -Uri $vaultUrl -Method 'Patch' -ContentType 'application/merge-patch+json' -Headers $header -Body $postParams -SkipCertificateCheck -ErrorAction Stop
            $newVersion = $content.data.version
            Log-Message "The secret version $newVersion was created successfully:"
            $content

            # ToDo: what is --header "X-Vault-Namespace: $VAULT_NAMESPACE"
            $payload = Get-Content -Path $newSha -ErrorAction Stop
            $newShaHash = ($payload -split ' ')[0]

            $payload = Get-Content -Path "$newP7bBase64.sha" -ErrorAction Stop
            $newP7bBase64Hash = ($payload -split ' ')[0]

            [string] $creationTime = (Get-Date -ErrorAction Stop).ToString($DATETIME_FORMAT)
            $postParams = @{"custom_metadata"=@{"Creator"="$userAgent";"Creation Time"="$creationTime";"P7B_BASE64_SHA1"="$newP7bBase64Hash";"P7B_PEM_SHA1"="$newShaHash";"P7B_BASE64_FILE"="$($baseName + $ts + ".base64")"}} | ConvertTo-Json -ErrorAction Stop
            $header = @{"X-Vault-Token"="$clientToken";"Accept"="application/json";"Cache-Control"="no-cache"}
            $vaultUrl = [string]::Format("{0}/{1}", $VAULT_ADDRESS, $metadataPath)
            $content = Invoke-RestMethod -Uri $vaultUrl -Method 'Patch' -ContentType 'application/merge-patch+json' -Headers $header -Body $postParams -SkipCertificateCheck -ErrorAction Stop
            Log-Message "The metadata was uploaded successfully."
            Log-Message "The data was uploaded successfully." "Green"
        } catch {
            $exitCode = 1
            Log-Message "Error upload data to the Vault: $($_.Exception.Message)" "Red"
            Log-Message "`nYou must upload data to the Vault manually or restart job." "Yellow"
        } finally {
            Get-GarbageCollector
            $clientToken = ""
        }
    } # $Upload

    Log-Message "`nCreating archive: $archiveName"
    [string[]] $sources = if ($Upload) { $newP7bBase64, "$newP7bBase64.sha" } else { $newP7b, $newSha }
    Compress-Archive -Path $sources -DestinationPath $archiveName -CompressionLevel Optimal -ErrorAction stop

    #create the new folder and copy files to it
    [string] $newDir = Join-Path -Path $Workdir -ChildPath $timeStamp -ErrorAction Stop

    Log-Message "Creating the $newDir folder and copy files to it"
    New-Item -ItemType Directory -Path $newDir -ErrorAction Stop | Out-Null
    $ext = ('*.p7b','*.sha','*.cer','*.crt','*.der','*.base64')
    Copy-Item -Path (Join-Path -Path $Workdir -ChildPath '*' -ErrorAction Stop) -Destination $newDir -Include $ext -Force -ErrorAction Stop
    Copy-Item -Path $archiveName -Destination $newDir -Force -ErrorAction Stop
    $ext = ('*.p7b','*.sha','*.base64')
    $retVal = Remove-File -Path $Workdir -Extensions $ext
    if (-not $retVal) {
        throw "Error remove files in the $Workdir folder"
    }
} catch {
    Log-Message $_.Exception.Message "Red"
    $exitCode = 1
} finally {
    Log-Message "`nDONE" -ForegroundColor "Green"
    if (![string]::IsNullOrEmpty($timeStamp)) {
        Log-Message "`nID: $timeStamp"
    }
    exit $exitCode
}
