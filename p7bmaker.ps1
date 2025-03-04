
<#PSScriptInfo

.VERSION 1.0

.GUID f8c0a75c-e65c-4aee-9adf-59b2f80f97a9

.AUTHOR Oleksii Gaienko <support@galexsoftware.info>

.COPYRIGHT 2024 Oleksii Gaienko. All rights reserved.

.TAGS p7bmaker

.LICENSEURI https://www.gnu.org/licenses/#GPL

.PROJECTURI https://github.com/GAlexDark/MMSTools

#>

<#

.SYNOPSIS
 Downloads the p7b file fron the IIT site and adds certificates to it.

.DESCRIPTION
 The p7bmaker.ps1 script downloads the p7b file fron the IIT site and adds certificates to it.
 The p7bmaker.ps1 script must be located in the same folder where p7bmaker.exe is located.

.PARAMETER Workdir
 Specifies the path to to the folder, contains *.cer and/or *.crt files and were will be saved p7b files.

.PARAMETER Url
 Specifies the download URL. Attention! If the URL value contains ?d=, it must have the ?d=* format (include *)!

.INPUTS
 None. You can't pipe objects to p7bmaker.ps1.

.OUTPUTS
 None. p7bmaker.ps1 doesn't generate any output.

.EXAMPLE
 PS> .\p7bmaker.ps1 -Workdir B:\certdata
 p7b file maker PoSH Script Version 1.0
 Copyright (C) 2024 Oleksii Gaienko, support@galexsoftware.info
 This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.

 Attension! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!!

 OK - The domain name iit.com.ua was resolved to the IP address: 104.26.0.134,104.26.1.134,172.67.69.107 [hidden as minor]
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

 Creating archive: CACertificates20240917.zip
 Delete the B:\certdata\20240917 folder if it exists
 Creating the B:\certdata\20240917 folder and copy files to it
 DONE

.EXAMPLE
 PS> .\p7bmaker.ps1 -Workdir B:\certdata -Url "https://iit.com.ua/download/productfiles/CACertificates.p7b"
 p7b file maker PoSH Script Version 1.0
 Copyright (C) 2024 Oleksii Gaienko, support@galexsoftware.info
 This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.

 Attention! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!!

 The value of the 'download_url' was changed with the value from the Url argument...

 OK - The domain name iit.com.ua was resolved to the IP address: 172.67.69.107,104.26.0.134,104.26.1.134  [hidden as minor]
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

 Creating archive: CACertificates20240917.zip
 Delete the B:\certdata\20240917 folder if it exists
 Creating the B:\certdata\20240917 folder and copy files to it
 DONE

#>

#requires -version 5

[CmdletBinding()]
Param (
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

$Workdir = $Workdir.Trim()
if ((($Workdir.Length -eq 1) -and ($Workdir -eq ".")) -or
    (($Workdir.Length -eq 2) -and ($Workdir -eq ".\"))) {
    $Workdir = $PSScriptRoot
}

try {
    [bool] $retVal = Test-Path -Path $Workdir -ErrorAction Stop
    if (!$retVal) {
        Write-Warning "The work dir $Workdir not found."
        exit 1
    }
} catch {
    Write-Host $PSItem.ErrorRecord -ForegroundColor Red
    exit 1
}

Write-Host "p7b file maker PoSH Script Version 1.0`nCopyright (C) 2024 Oleksii Gaienko, support@galexsoftware.info`nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.`n" -ForegroundColor green
Write-Host "Attention! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!! Or use the 'Url' script parameter.`n" -ForegroundColor Cyan
#**************************************************************************************
#
# Задаем Url, откуда будет загружаться файл CACertificates.p7b:
# [string] $download_url = "https://iit.com.ua/download/productfiles/CACertificates.p7b?d="
 [string] $download_url = "https://iit.com.ua/download/productfiles/CACertificates.p7b"
#
#**************************************************************************************
[string] $userAgent = "p7bmaker PoSH downloader module/1.0"
# Nondefault Web Proxy:
[string] $proxyName = ""

function Remove-File {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $Path,

        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string[]] $Extensions
    )

    try {
        [bool] $retVal = Test-Path -Path $Path -ErrorAction Stop
        if ($retVal) {
            Remove-Item -Path (Join-Path -Path $Path -ChildPath '*' -ErrorAction Stop) -Include $Extensions -Force -ErrorAction Stop
        } else {
            Write-Host "The path '$Path' is not found" -ForegroundColor Red
        }
    } catch {
        $retVal = $false
        Write-Host $PSItem.ErrorRecord -ForegroundColor Red
    }
    return $retVal
}

function Get-DomainFromUrl {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $baseUrl
    )

    $url = [System.Uri]$baseUrl
    return $url.Host
}

function Get-FileNameFromUrl {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $baseUrl
    )

    $url = [System.Uri]$baseUrl
    return $url.Segments[-1]
}

function Test-CheckDnsName {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $Domain
    )

    [bool] $retVal = $true;
    try {
        $dnsRecord = Resolve-DnsName -Name $Domain -DnsOnly -ErrorAction Stop
        Write-Host "OK - The domain name '$Domain' was resolved to the IP address: $($dnsRecord.IPAddress -join ',')" -ForegroundColor green
    } catch {
        $errMessage = $PSItem.Exception.Message
        Write-Host "Error resolve host $errMessage" -ForegroundColor Red
        $retVal = $false
    }

    return $retVal
}

function Test-CheckQuery {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $baseUrl
    )

    $url = [System.Uri]$baseUrl
    [bool] $retVal = $true
    if ($url.Query.Length -eq 0) {
        $retVal = $false
    }
    return $retVal
}

function Get-DownloadFile {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $DownloadUrl,
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [Alias('FullName')]
        [string] $SavePath
    )

    try {
        $fileName = Get-FileNameFromUrl $DownloadUrl
        $fileName = Join-Path -Path $SavePath -ChildPath $fileName -ErrorAction Stop

        Add-Type -AssemblyName System.Net.Http -ErrorAction Stop
        if ($UseProxy) {
            $handler = New-Object System.Net.Http.HttpClientHandler
            $handler.UseDefaultCredentials = $true
            if ([string]::IsNullOrEmpty($proxyName)) {
                $handler.Proxy = [System.Net.WebRequest]::DefaultWebProxy
            } else {
                $handler.Proxy = New-Object System.Net.WebProxy($proxyName)
            }
            $handler.Proxy.Credentials = [System.Net.CredentialCache]::DefaultNetworkCredentials
            $httpClient = New-Object System.Net.Http.HttpClient($handler)
        } else {
            $httpClient = New-Object System.Net.Http.HttpClient
        }

        [bool] $retVal = $false
        $httpClient.DefaultRequestHeaders.add('User-Agent', $userAgent)
        $response = $httpClient.GetAsync($DownloadUrl)
        $response.Wait()
        $result = $response.Result
        if ($result.StatusCode -eq 200) {
            try {
                $outputFileStream = [System.IO.FileStream]::new($fileName, [System.IO.FileMode]::Create, [System.IO.FileAccess]::Write)
                $downloadTask = $response.Result.Content.CopyToAsync($outputFileStream)
                $downloadTask.Wait()
                Write-Host "OK - The file was successfully saved in $SavePath" -ForegroundColor Green
                $retVal = $true
            } catch {
                Write-Host "Error save file $SavePath Reason: $($PSItem.ToString())" -ForegroundColor Red
            } finally {
                $outputFileStream.Close()
            }
        } else {
            Write-Host $('Error download file: Status {0}, Reason: {1}.' -f [int]$result.StatusCode, $result.ReasonPhrase)
        }
    } catch {
        $e = $response.Exception.InnerException
        Write-Error ('Fatal Error: {0}' -f $e)
    } finally {
        if($null -ne $result) {
            $result.Dispose()
        }
        if ($null -ne $handler) {
            $handler.Dispose()
        }
        if ($null -ne $outputFileStream) {
            $outputFileStream.Dispose()
        }
    }

    return $retVal
}

#Checking certs
try {
    $certs = Get-ChildItem -Path $Workdir -Filter *.cer -ErrorAction Stop
} catch {
    Write-Error $PSItem
    exit 1
}

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
        } catch {
            Write-Error $PSItem
            exit 1
        } finally {
            if ($certItem) {
                $certItem.Dispose()
            }
        }
    }
} else {
    Write-Host "The certificates is not found!" -ForegroundColor Red
    exit 1
}

#Removing existing p7b, sha, base64, and zip files
[string[]] $ext = ('*.p7b','*.sha','*.zip','*.base64')
[bool] $retVal = Remove-File -Path $Workdir -Extensions $ext
if (-not $retVal) {
    exit 1
}

# Check domain name
if (-not [string]::IsNullOrEmpty($Url)) {
    $download_url = $Url.Trim()
    Write-Host "The value of the 'download_url' was changed with the value from the Url argument...`n" -ForegroundColor Yellow
}

#$retVal = Test-CheckQuery $download_url
#if ($retVal) {
#    if ($download_url.EndsWith("?d=")) {
#        if ($DateStamp.Length -eq 0) {
#            $download_url = $download_url + (Get-Date).ToString('ddMMyyyy')
#        } else {
#            $download_url = $download_url + $DateStamp
#        }
#    }
#} else {
#    if ($DateStamp.Length -eq 0) {
#        $download_url = $download_url + "?d=" +(Get-Date).ToString('ddMMyyyy')
#    } else {
#        $download_url = $download_url + "?d=" + $DateStamp
#    }
#}

[string] $domain = Get-DomainFromUrl $download_url
$retVal = Test-CheckDnsName $domain
if (-not $retVal) {
    exit 1
}

#Download and save *.p7b file
Write-Output "`nStarting download the P7B file from the '$download_url' Url.`n"
$retVal = Get-DownloadFile -DownloadUrl $download_url -SavePath $WorkDir
if (-not $retVal) {
    exit 1
}

# Let's start creating a p7b container with the necessary certificates
$pathToExecute = Join-Path -Path $PSScriptRoot -ChildPath "p7bmaker.exe"
$retVal = Test-Path -Path $pathToExecute -ErrorAction Stop
if (-not $retVal) {
    Write-Host "The executable file p7bmaker.exe not found" -ForegroundColor Red
    exit 1
}

Write-Output "`nStarting $pathToExecute...`n"
$argsList = if ($Upload) { "-l $WorkDir --silent -o PEM" } else { "-l $WorkDir --silent" }
$processInfo = Start-Process -FilePath $pathToExecute -ArgumentList $argsList -Wait -NoNewWindow -PassThru
if ($processInfo.ExitCode -ne 0) {
    Write-Error "The $pathToExecute returned an error."
	exit 1
}

#create filenames
[string] $baseName = Get-FileNameFromUrl $download_url
$baseName = $baseName.Substring(0, $baseName.LastIndexOf('.'))
[string] $ts = (Get-Date).ToString('yyyyMMdd')

[string] $archiveName = $baseName + $ts + ".zip"
$archiveName = Join-Path -Path $Workdir -ChildPath $archiveName -ErrorAction Stop

[string] $newP7b = $baseName + $ts + ".p7b"
$newP7b = Join-Path -Path $Workdir -ChildPath $newP7b -ErrorAction Stop

[string] $newSha = $baseName + $ts + ".sha"
$newSha = Join-Path -Path $Workdir -ChildPath $newSha -ErrorAction Stop

[string] $newBase64 = $baseName + $ts + ".base64"
$newBase64 = Join-Path -Path $Workdir -ChildPath $newBase64 -ErrorAction Stop
[string] $newBase64Sha = $newBase64 + ".sha"

#create .base64, if needed
if ($Upload) {
    $content = Get-Content -Path $newP7b -ErrorAction Stop
    if ($content) {
        ($content | Where-Object { $_ -notlike "*BEGIN PKCS7*" -and $_ -notlike "*END PKCS7*" }) -join '' | Out-File -FilePath $newBase64 -Encoding ascii -NoNewline -ErrorAction Stop
        $base64Hash = (Get-FileHash $newBase64 -Algorithm SHA1).Hash
        $base64Hash + " *" + $newBase64.Substring($Workdir.Length + 1) | Out-File -FilePath $newBase64Sha -Encoding utf8 -ErrorAction Stop
    } else {
        Write-Host "Error read the $newP7b file!" -ForegroundColor Red
    }
}

Write-Host "`nCreating archive: $archiveName"
[string[]] $sources = if ($Upload) { $newBase64,$newBase64Sha } else { $newP7b,$newSha }
try {
    Compress-Archive -Path $sources -DestinationPath $archiveName -CompressionLevel Optimal -ErrorAction Stop
} catch {
    Write-Error $PSItem
    exit 1
}

#create the new folder and copy files to it
$newDir = Join-Path -Path $Workdir -ChildPath $ts -ErrorAction Stop
$retVal = Test-Path -Path $newDir -ErrorAction Stop
if ($retVal) {
    $newDir = $newDir + '_' + (Get-Date).ToString("HHmmss")
}

Write-Host "Creating the $newDir folder and copy files to it"
try {
    New-Item -ItemType Directory -Path $newDir -ErrorAction Stop | Out-Null
    $ext = ('*.p7b','*.sha','*.cer','*.crt','*.der','*.base64')
    Copy-Item -Path (Join-Path -Path $Workdir -ChildPath '*' -ErrorAction Stop) -Destination $newDir -Include $ext -Force -ErrorAction Stop
    Copy-Item -Path $archiveName -Destination $newDir -Force -ErrorAction Stop
} catch {
    Write-Error $PSItem
    exit 1
}

#ToDo: Upload code will be here

Write-Host "DONE" -ForegroundColor Green

exit 0
