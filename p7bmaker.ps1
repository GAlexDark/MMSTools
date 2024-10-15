
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
 Specifies the download URL.

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
    [Parameter (Mandatory=$true, Position=0,
    HelpMessage="Enter the path to the folder, contains *.cer and/or *.crt files and were will be saved p7b files.")]
    [ValidateNotNullOrEmpty()]
    [string] $Workdir,

    [Parameter (Mandatory=$false, Position=1,
    HelpMessage="Enable flag when the script run in the Corporate Network.")]
    [switch] $UseProxy = $false,

    [Parameter (Mandatory=$false, Position=2,
    HelpMessage="Enter the actual Download Url.")]
    [string] $Url
)

$Workdir = $Workdir.Trim()
try {
    [bool] $retVal = Test-Path -Path $Workdir -ErrorAction Stop -ErrorVariable err
    if (!$retVal) {
        Write-Warning "The work dir $Workdir not found."
        exit 1
    }
} catch {
    Write-Host $err.ErrorRecord -ForegroundColor Red
    exit 1
}

Write-Host "p7b file maker PoSH Script Version 1.0`nCopyright (C) 2024 Oleksii Gaienko, support@galexsoftware.info`nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.`n" -ForegroundColor green
Write-Host "This script used the function Start-ProcessWithOutput() by Tomas Madajevas: https://medium.com/@tomas.madajevas/retrieving-executables-output-in-powershell-68e91bdee721`n" -ForegroundColor Yellow
Write-Host "Attention! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!! Or use the 'Url' script parameter.`n" -ForegroundColor Cyan
#**************************************************************************************
#
# Задаем Url, откуда будет загружаться файл CACertificates.p7b:
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
        [Alias('FullName')]
        [string] $FileName
    )

    [bool] $retVal = Test-Path -Path $FileName
    if ($retVal) {
        try {
            Remove-Item -Path $FileName -Force -Recurse -ErrorAction Stop -ErrorVariable err
        } catch {
            Write-Host $err.ErrorRecord -ForegroundColor Red
            $retVal = $false
        }
    } else {
        $retVal = $true
    }

    return $retVal
}

function Get-DomainFromUrl {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $baseUrl
    )

    [string] $baseUrl = $baseUrl.Substring(0, $baseUrl.LastIndexOf("/") + 1)
    [string] $baseSlash = $baseUrl.IndexOf("/", $baseUrl.IndexOf("://") + 3)

    [string] $retVal = ""
    if($baseSlash -ge 0) {
        $retVal = $baseUrl.Substring(0, $baseSlash) + '/'
    } else {
        $retVal = $baseUrl
    }
    return $retVal
}

function Test-CheckDnsName {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $Domain
    )

    [bool] $retVal = $true;
    [int] $startIndex = $Domain.IndexOf("://")
    [string] $name = $Domain.Substring($startIndex + 3, $Domain.LastIndexOf("/") - $startIndex - 3)
    try {
        $dnsRecord = Resolve-DnsName -Name $name -DnsOnly -ErrorAction Stop
        Write-Host "OK - The domain name '$name' was resolved to the IP address: $($dnsRecord.IPAddress -join ',')" -ForegroundColor green
    } catch {
        $errMessage = $PSItem.Exception.Message
        Write-Host "Error resolve host $errMessage" -ForegroundColor Red
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

    [string] $fileName = $DownloadUrl.Substring($DownloadUrl.LastIndexOf('/') + 1)
    if ($SavePath.EndsWith('\')) {
        $fileName = $SavePath + $fileName
    } else {
        $fileName = $SavePath + '\' + $fileName
    }

    Add-Type -AssemblyName System.Net.Http
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
    try {
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
    }

    return $retVal
}

if ($Workdir.EndsWith('\')) {
    $maskP7b = $Workdir + "*.p7b"
    $maskSha = $Workdir + "*.sha"
    $maskZip = $Workdir + "*.zip"
    $maskCer = $Workdir + "*.cer"
    $newDir = $Workdir + (Get-Date).ToString('yyyyMMdd')
} else {
    $maskP7b = $Workdir + "\*.p7b"
    $maskSha = $Workdir + "\*.sha"
    $maskZip = $Workdir + "\*.zip"
    $maskCer = $Workdir + "\*.cer"
    $newDir = $Workdir + "\" + (Get-Date).ToString('yyyyMMdd')
}

#Checking certs
$certs = Get-ChildItem -Path $Workdir -Filter *.cer
if ($certs) {
    $currentDate = Get-Date
    try {
        $certs | ForEach-Object {
            $certSN = ([System.Security.Cryptography.X509Certificates.X509Certificate2]::new($_.FullName)).SerialNumber
            $certValidTo = ([System.Security.Cryptography.X509Certificates.X509Certificate2]::new($_.FullName)).NotAfter
            if ($currentDate -gt $certValidTo) {
                Write-Warning "The certificate SN=$certSN expired!"
                $newName = $_.FullName + ".expired"
                Rename-Item -Path $_.FullName -NewName $newName -ErrorAction Stop
            }
        }
    } catch {
        Write-Error $PSItem
        exit 1
    }
} else {
    Write-Host "The certificates is not found!" -ForegroundColor Red
    exit 1
}

#Removing exists p7b and sha files
[bool] $retVal = Remove-File $maskP7b
if (-not $retVal) {
    exit 1
}
$retVal = Remove-File $maskSha
if (-not $retVal) {
    exit 1
}
#remove previsions zip archives
$retVal = Remove-File $maskZip
if (-not $retVal) {
    exit 1
}

# Check domain name
if (-not [string]::IsNullOrEmpty($Url)) {
    $download_url = $Url.Trim()
    Write-Host "The value of the 'download_url' was changed with the value from the Url argument...`n" -ForegroundColor Yellow
}
[string] $domain = Get-DomainFromUrl $download_url
$retVal = Test-CheckDnsName $domain
if (-not $retVal) {
    exit 1
}

#Download and save CACertificates.p7b file
Write-Output "`nStarting download the CACertificates.p7b file from the '$download_url' Url.`n"
$retVal = Get-DownloadFile $download_url $WorkDir
if (-not $retVal) {
    exit 1
}

# Let's start creating a p7b container with the necessary certificates
$pathToExecute = $PSScriptRoot + "\p7bmaker.exe"
$retVal = Test-Path -Path $pathToExecute
if (-not $retVal) {
    Write-Host "The executable file p7bmaker.exe not found" -ForegroundColor Red
    exit 1
}

Write-Output "`nStarting $pathToExecute...`n"

Start-Process -FilePath "B:\MMSTools\p7bmaker.exe" -ArgumentList "-l $WorkDir --silent" -Wait -NoNewWindow

#create archive
[string] $baseName = $download_url.Substring($download_url.LastIndexOf('/') + 1)
$baseName = $baseName.Substring(0, $baseName.LastIndexOf('.'))
[string] $archiveName = $baseName + (Get-Date).ToString('yyyyMMdd') + ".zip"

Write-Host "`nCreating archive: $archiveName"

[string] $newP7b = $baseName + (Get-Date).ToString('yyyyMMdd') + ".p7b"
if ($Workdir.EndsWith('\')) {
    $archiveName = $Workdir + $archiveName
    $newP7b = $Workdir + $newP7b
} else {
    $archiveName = $Workdir + "\" + $archiveName
    $newP7b = $Workdir + "\" + $newP7b
}
try {
    Compress-Archive -Path $newP7b,$maskSha -DestinationPath $archiveName -CompressionLevel Optimal -ErrorAction stop
} catch {
    Write-Error $PSItem
    exit 1
}

#create the new folder and copy files to it
Write-Host "Delete the $newDir folder if it exists"
$retVal = Remove-File $newDir
if (-not $retVal) {
    exit 1
}

Write-Host "Creating the $newDir folder and copy files to it"
try {
    New-Item -ItemType Directory -Path $newDir -ErrorAction Stop | Out-Null
    Copy-Item -Path $maskP7b -Destination $newDir -Force -ErrorAction Stop
    Copy-Item -Path $maskSha -Destination $newDir -Force -ErrorAction Stop
    Copy-Item -Path $archiveName -Destination $newDir -Force -ErrorAction Stop
    Copy-Item -Path $maskCer -Destination $newDir -Force -ErrorAction Stop
} catch {
    Write-Error $PSItem
    exit 1
}

Write-Host "DONE" -ForegroundColor Green

exit 0
