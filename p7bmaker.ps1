
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
 Downloads the p7b file from the IIT site, adds certificates to it and upload BASE64 data to the HashiCorp Vault.

.DESCRIPTION
 The p7bmaker.ps1 script downloads the p7b file fron the IIT site, adds certificates to it and upload BASE64 data to the HashiCorp Vault.
 The p7bmaker.ps1 script must be located in the same folder where p7bmaker.exe is located.

.PARAMETER EnvFileName
 Specifies the path and name of the configuration file.

.PARAMETER Workdir
 Specifies the path to to the folder, contains *.cer and/or *.crt files and were will be saved p7b files.

.PARAMETER UseProxy
 Specifies the flag when the script must download the p7b file over the Corporate Firewall.

.PARAMETER Url
 Specifies the download URL. Attention! If the URL value contains ?d=, it must have the ?d=* format (include *)!

.PARAMETER Upload
 Specifies the flag when the script must upload the p7b data to the Vault.

.INPUTS
 None. You can't pipe objects to p7bmaker.ps1.

.OUTPUTS
 None. p7bmaker.ps1 doesn't generate any output.

.EXAMPLE
 PS> .\p7bmaker.ps1 -Workdir B:\certdata
 p7b file maker PoSH Script Version 2.0
 Copyright (C) 2025 Oleksii Gaienko, support@galexsoftware.info
 This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.

 Attension! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!!

 OK - The domain name 'demosite.local' was resolved to the IP address: [hidden as minor]
 OK - The file was successfully saved in B:\certdata

 Starting B:\MMSTools\p7bmaker.exe...

 Start reading p7b file 'B:/certdata/CACertificates.p7b'.
 The p7b file 'B:/certdata/CACertificates.p7b' was read successful.

 The number of certificates in the container before adding: 628.

 The 2 certificates found.
 Start adding certificates.
 The certificates were added successful.

 The number of certificates in the container after adding: 631.

 File 'B:/certdata/CACertificates20240917.p7b' saved successfully.

 Calculating result p7b file hash:
 SHA-1 hash: 3d70fbefe0c4a30fa75926301349cf0ecbd362db.

 Creating hash File.
 The hash file 'B:/certdata/CACertificates20240917.sha' saved successfully.

 DONE
 OK - The  URL exists.
 Checking the Vault status...
 The Vault is available.

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
 Delete the B:\certdata\20240917 folder if it exists
 Creating the B:\certdata\20240917 folder and copy files to it
 DONE

.EXAMPLE
 PS> .\p7bmaker.ps1 -Workdir B:\certdata -Url "https://demosite.local/download_new/CACertificates.p7b"
 p7b file maker PoSH Script Version 2.0
 Copyright (C) 2025 Oleksii Gaienko, support@galexsoftware.info
 This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.

 Attention! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!!

 The value of the 'download_url' was changed with the value from the Url argument...

 OK - The domain name 'demosite.local' was resolved to the IP address: [hidden as minor]
 OK - The file was successfully saved in B:\certdata\

 Starting B:\MMSTools\p7bmaker.exe...

 Start reading p7b file 'B:/certdata/CACertificates.p7b'.
 The p7b file 'B:/certdata/CACertificates.p7b' was read successful.

 The number of certificates in the container before adding: 628.

 The 2 certificates found.
 Start adding certificates.
 The certificates were added successful.

 The number of certificates in the container after adding: 631.

 File 'B:/certdata/CACertificates20240917.p7b' saved successfully.

 Calculating result p7b file hash:
 SHA-1 hash: 3d70fbefe0c4a30fa75926301349cf0ecbd362db.

 Creating hash File.
 The hash file 'B:/certdata/CACertificates20240917.sha' saved successfully.

 DONE
 OK - The  URL exists.
 Checking the Vault status...
 The Vault is available.

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
 Delete the B:\certdata\20240917 folder if it exists
 Creating the B:\certdata\20240917 folder and copy files to it
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

try {
    [System.Net.ServicePointManager]::ServerCertificateValidationCallback = {$true}
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls -bor [Net.SecurityProtocolType]::Tls11 -bor [Net.SecurityProtocolType]::Tls12
	$ErrorView = "NormalView"
	$PSStyle.OutputRendering = "PlainText"

    $assemblyPath = Join-Path -Path $PSScriptRoot -ChildPath "p7butils.psm1" -ErrorAction Stop
    [bool] $retVal = Test-Path -Path $assemblyPath -ErrorAction Stop
    if (-not $retVal) {
        throw "The p7butils.psm1 is not found."
    }
    Import-Module -Name $assemblyPath -ErrorAction Stop

    $EnvFileName = $EnvFileName.Trim()
    $EnvFileNamePath = Join-Path -Path $PSScriptRoot -ChildPath $EnvFileName -ErrorAction Stop
    $retVal = Test-Path -Path $EnvFileNamePath -ErrorAction Stop
    if (-not $retVal) {
        throw "The '$EnvFileName' file is not found."
    }

    $Workdir = $Workdir.Trim()
    if ((($Workdir.Length -eq 1) -and ($Workdir -eq ".")) -or
        (($Workdir.Length -eq 2) -and ($Workdir -eq ".\"))) {
        $Workdir = $PSScriptRoot
    }
    $retVal = Test-Path -Path $Workdir -ErrorAction Stop
    if (!$retVal) {
        throw "The work dir $Workdir not found."
    }

    #Mapping env variables
    . $EnvFileNamePath
    [UInt64] $freeSpace = (Get-PSDrive -PSProvider FileSystem $LOCAL_STORAGE_MOUNT_POINT -ErrorAction Stop).Free
    if ($freeSpace -le $MIN_SPACE) {
        throw "The disk space is low."
    }

    Write-Host "The p7b file maker PoSH Script Version 2.0`nCopyright (C) 2025 Oleksii Gaienko, support@galexsoftware.info`nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.`n" -ForegroundColor green
    Write-Host "Attention! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!! Or use the 'Url' script parameter.`n" -ForegroundColor Cyan

    #**************************************************************************************
    # Задаем Url, откуда будет загружаться файл CACertificates.p7b:
    #[string] $download_url = "https://demosite.local/download/CACertificates.p7b?d="
    [string] $download_url = "https://demosite.local/download/CACertificates.p7b"
    #
    #**************************************************************************************
    [string] $userAgent = "p7bmaker PoSH module/2.0"
    # Nondefault Web Proxy:
    [string] $proxyName = ""

    #Removing existing p7b, sha, base64, and zip files
    [string[]] $ext = ('*.p7b','*.sha','*.zip','*.base64')
    $retVal = Remove-File -Path $Workdir -Extensions $ext
    if (-not $retVal) {
        throw "Error remove files in the $Workdir folder"
    }

    #Checking certs
    $certs = Get-ChildItem -Path $Workdir -Filter *.cer -ErrorAction Stop
    if ($certs) {
        $currentDate = Get-Date
        $certs | ForEach-Object {
            try {
                $certItem = New-Object System.Security.Cryptography.X509Certificates.X509Certificate2 $_.FullName
                [string] $certSN = $certItem.SerialNumber
                [datetime] $certValidTo = $certItem.NotAfter
                if ($currentDate -gt $certValidTo) {
                    Write-Warning "The certificate SN=$certSN expired!"
                    $newName = $_.FullName + ".expired"
                    Rename-Item -Path $_.FullName -NewName $newName -ErrorAction Stop
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

    # Check domain name
    if (-not [string]::IsNullOrEmpty($Url)) {
        $download_url = $Url.Trim()
        Write-Host "The value of the 'download_url' was changed with the value from the Url argument...`n" -ForegroundColor Yellow
    }

#    $retVal = Test-CheckQuery $download_url
#    if ($retVal) {
#        if ($download_url.EndsWith("?d=")) {
#            if ($DateStamp.Length -eq 0) {
#                $download_url = $download_url + (Get-Date -ErrorAction Stop).ToString('ddMMyyyy')
#            } else {
#                $download_url = $download_url + $DateStamp
#            }
#        }
#    } else {
#        if ($DateStamp.Length -eq 0) {
#            $download_url = $download_url + "?d=" +(Get-Date -ErrorAction Stop).ToString('ddMMyyyy')
#        } else {
#            $download_url = $download_url + "?d=" + $DateStamp
#        }
#    }

    [string] $domain = Get-DomainFromUrl $download_url
    $retVal = Test-CheckDnsName $domain
    if (-not $retVal) {
        throw "Error resolve host $domain"
    }

    $retVal = Test-CheckURL -CheckedUrl $download_url
    if (-not $retVal) {
        throw "Error connect to the $download_url"
    }

    #Download and save *.p7b file
    Write-Output "`nStarting download the P7B file from the '$download_url' Url.`n"
    $retVal = Get-DownloadFile -DownloadUrl $download_url -SavePath $WorkDir
    if (-not $retVal) {
        throw "Error download the file from the $download_url"
    }

    # Let's start creating a p7b container with the necessary certificates
    $pathToExecute = Join-Path -Path $PSScriptRoot -ChildPath "p7bmaker.exe" -ErrorAction Stop
    $retVal = Test-Path -Path $pathToExecute -ErrorAction Stop
    if (-not $retVal) {
        throw "The executable file p7bmaker.exe not found"
    }

    Write-Output "`nStarting $pathToExecute...`n"
    $argsList = if ($Upload) { "-l $WorkDir --silent -o PEM" } else { "-l $WorkDir --silent" }
    $processInfo = Start-Process -FilePath $pathToExecute -ArgumentList $argsList -Wait -NoNewWindow -PassThru
    if ($processInfo.ExitCode -ne 0) {
        throw "The $pathToExecute returned an error."
    }

    #create filenames
    [string] $baseName = Get-FileNameFromUrl $download_url
    $baseName = $baseName.Substring(0, $baseName.LastIndexOf('.'))
    [string] $ts = (Get-Date -ErrorAction Stop).ToString('yyyyMMdd')

    [string] $archiveName = $baseName + $ts + ".zip"
    $archiveName = Join-Path -Path $Workdir -ChildPath $archiveName -ErrorAction Stop

    [string] $newP7b = $baseName + $ts + ".p7b"
    $newP7b = Join-Path -Path $Workdir -ChildPath $newP7b -ErrorAction Stop

    [string] $newSha = $baseName + $ts + ".sha"
    $newSha = Join-Path -Path $Workdir -ChildPath $newSha -ErrorAction Stop

    #create .base64, if needed
    if ($Upload) {
        $content = Get-Content -Path $newP7b -Raw -ErrorAction Stop
        if ($content) {
            [string] $newP7bBase64 = $baseName + $ts + ".base64"
            $newP7bBase64 = Join-Path -Path $Workdir -ChildPath $newP7bBase64 -ErrorAction Stop
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
            Write-Host "Error read the $newP7b file!" -ForegroundColor Red
        }

        try {
            #Upload base64 to the Vault
            [string] $userName = $($env:vaultUserName)
            [string] $userPassword = $($env:vaultPassword)
            [string] $vaultAddress = $VAULT_ADDRESS
            [string] $secretName = $SECRET_NAME
            [string] $dateTimeFormat = $DATETIME_FORMAT

            if ([string]::IsNullOrEmpty($userName)) {
                throw "The 'userName' value is not set"
            }

            if ([string]::IsNullOrEmpty($userPassword)) {
                throw "The 'userPassword' value is not set"
            }
            if ([string]::IsNullOrEmpty($vaultAddress)) {
                throw "The 'vaultAddress' value is not set"
            }
            if ([string]::IsNullOrEmpty($secretName)) {
                throw "The 'secretName' value is not set"
            }
            if ([string]::IsNullOrEmpty($dateTimeFormat)) {
                throw "The 'dateTimeFormat' value is not set"
            }

            [string] $authPath = "v1/auth/userpass/login/"
            [string] $sealedPath = "v1/sys/seal-status"
            [string] $apiPath = "v1/cryptography/data/certificate/$secretName"
            [string] $metadataPath = "v1/cryptography/metadata/certificate/$secretName"

            $retVal = Test-CheckURL -CheckedUrl $vaultAddress -IsUsingProxy $false
            if (-not $retVal) {
                throw "The Vault $vaultAddress checking error"
            }

            Write-Host "`nChecking the Vault Endpoint status..."
            [string] $vaultUrl = [string]::Format("{0}/{1}", $vaultAddress, $sealedPath)
            $content = Invoke-WebRequest -Uri $vaultUrl -Method 'Get' -Headers @{ "Cache-Control" = "no-cache" } -SkipCertificateCheck -ErrorAction Stop
            [int] $statusCode = $content.StatusCode
            if ($statusCode -ne 200) {
                throw "Error connect to the Vault: $statusCode"
            }

            $sealedData = $content.Content | ConvertFrom-Json
            [bool] $sealed = $sealedData.sealed
            [bool] $initialized = $sealedData.initialized
            if (-not $sealed -and -not $initialized) {
                throw "Vault is not initialized."
            }
            Write-Host "Ok - The Vault is available.`n" -ForegroundColor Green

            Write-Host "Connect to the Vault..."
            $postParams = @{"password"="$userPassword"} | ConvertTo-Json -ErrorAction Stop
            $vaultUrl = [string]::Format("{0}/{1}{2}", $vaultAddress, $authPath, $userName)
            $content = Invoke-RestMethod -Uri $vaultUrl -Method 'Post' -ContentType 'application/json' -Body $postParams -Headers @{ "Cache-Control" = "no-cache" } -SkipCertificateCheck -ErrorAction Stop
            [string] $clientToken = $content.auth.client_token
            if ([string]::IsNullOrEmpty($clientToken)) {
                throw "Wrong client token."
            }
            write-host "Connected.`n"

            Write-Host "Upload data to the Vault..."
            $p7bPayload = Get-Content -Path $newP7bBase64 -Encoding Ascii -ErrorAction Stop
            $postParams = @{"data"=@{"certificate"="$p7bPayload"}} | ConvertTo-Json -ErrorAction Stop
            $header = @{"X-Vault-Token"="$clientToken";"Cache-Control"="no-cache"}
            $vaultUrl = [string]::Format("{0}/{1}", $vaultAddress, $apiPath)
            $content = Invoke-RestMethod -Uri $vaultUrl -Method 'Patch' -ContentType 'application/merge-patch+json' -Headers $header -Body $postParams -SkipCertificateCheck -ErrorAction Stop
            $newVersion = $content.data.version
            Write-Host "The secret version $newVersion was created successfully:"
            $content

            # ToDo: what is --header "X-Vault-Namespace: $VAULT_NAMESPACE"
            $payload = Get-Content -Path $newSha -ErrorAction Stop
            $newShaHash = ($payload -split ' ')[0]

            $payload = Get-Content -Path ($newP7bBase64 + ".sha") -ErrorAction Stop
            $newP7bBase64Hash = ($payload -split ' ')[0]

            [string] $creationTime = (Get-Date -ErrorAction Stop).ToString($dateTimeFormat)
            $postParams = @{"custom_metadata"=@{"Creator"="$userAgent";"Creation Time"="$creationTime";"P7B_BASE64_SHA1"="$newP7bBase64Hash";"P7B_PEM_SHA1"="$newShaHash";"P7B_BASE64_FILE"="$($baseName + $ts + ".base64")"}} | ConvertTo-Json -ErrorAction Stop
            $header = @{"X-Vault-Token"="$clientToken";"Accept"="application/json";"Cache-Control"="no-cache"}
            $vaultUrl = [string]::Format("{0}/{1}", $vaultAddress, $metadataPath)
            $content = Invoke-RestMethod -Uri $vaultUrl -Method 'Patch' -ContentType 'application/merge-patch+json' -Headers $header -Body $postParams -SkipCertificateCheck -ErrorAction Stop
            Write-Host "The metadata was uploaded successfully."
            Write-Host "The data was uploaded successfully." -ForegroundColor Green
        } catch {
            $exitCode = 1
            Write-Host "Error upload data to the Vault: $PSItem" -ForegroundColor Red
            Write-Host "`nYou must upload data to the Vault manually or restart job." -ForegroundColor Yellow
        } finally {
            Get-GarbageCollector
            $clientToken = ""
        }
    } # $Upload

    Write-Host "`nCreating archive: $archiveName"
    [string[]] $sources = if ($Upload) { $newP7bBase64, $($newP7bBase64 + ".sha") } else { $newP7b, $newSha }
    Compress-Archive -Path $sources -DestinationPath $archiveName -CompressionLevel Optimal -ErrorAction stop

    #create the new folder and copy files to it
    [string] $newDir = Join-Path -Path $Workdir -ChildPath $ts -ErrorAction Stop
    $retVal = Test-Path -Path $newDir -ErrorAction Stop
    if ($retVal) {
        $newDir = $newDir + '_' + (Get-Date -ErrorAction Stop).ToString("HHmmss")
    }

    Write-Host "Creating the $newDir folder and copy files to it"
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
    Write-Error $PSItem
    $exitCode = 1
}

Write-Host "`nDONE" -ForegroundColor Green

exit $exitCode
