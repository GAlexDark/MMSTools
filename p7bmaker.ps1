
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
 OK - Site is UP
 OK - The file was successfully saved in B:\certdata

 Starting B:\MMSTools\p7bmaker.exe...

 Start reading p7b file 'B:/certdata/CACertificates.p7b'.
 The p7b file 'B:/certdata/CACertificates.p7b' was read successful.

 The number of certificates in the container before adding: 628.

 The 3 certificates found.
 Start adding certificates.
 The certificates were added successful.

 The number of certificates in the container after adding: 631.

 File 'B:/certdata/CACertificates20240917.p7b' saved successfully.

 Calculating result p7b file hash:
 SHA-1 hash: 3d70fbefe0c4a30fa75926301349cf0ecbd362db.

 Creating hash File.
 The hash file 'B:/certdata/CACertificates20240917.sha' saved successfully.

 DONE
 DONE

.EXAMPLE
 PS> .\p7bmaker.ps1 -Workdir B:\certdata -Url "https://iit.com.ua/download/productfiles/CACertificates.p7b"
 p7b file maker PoSH Script Version 1.0
 Copyright (C) 2024 Oleksii Gaienko, support@galexsoftware.info
 This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.

 Attention! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!!

 The value of the 'download_url' was changed with the value from the Url argument...

 OK - The domain name iit.com.ua was resolved to the IP address: 172.67.69.107,104.26.0.134,104.26.1.134  [hidden as minor]
 OK - Site is available
 OK - The file was successfully saved in B:\certdata\

 Starting B:\MMSTools\p7bmaker.exe...

 Start reading p7b file 'B:/certdata/CACertificates.p7b'.
 The p7b file 'B:/certdata/CACertificates.p7b' was read successful.

 The number of certificates in the container before adding: 628.

 The 3 certificates found.
 Start adding certificates.
 The certificates were added successful.

 The number of certificates in the container after adding: 631.

 File 'B:/certdata/CACertificates20240917.p7b' saved successfully.

 Calculating result p7b file hash:
 SHA-1 hash: 3d70fbefe0c4a30fa75926301349cf0ecbd362db.

 Creating hash File.
 The hash file 'B:/certdata/CACertificates20240917.sha' saved successfully.

 DONE
 DONE

#>

#requires -version 5

[CmdletBinding()]
Param (
    [Parameter (Mandatory=$true, Position=0,
    HelpMessage="Enter the path to the folder, contains *.cer and/or *.crt files and were will be saved p7b files.")]
    [string] $Workdir,

    [Parameter (Position=1,
    HelpMessage="Enter the actual Download Url.")]
    [string] $Url
)

Write-Host "p7b file maker PoSH Script Version 1.0`nCopyright (C) 2024 Oleksii Gaienko, support@galexsoftware.info`nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.`n" -ForegroundColor green
Write-Host "Attention! Check the CACertificates.p7b download URL periodically and save the new URL in the 'download_url' value!!`n" -ForegroundColor Cyan
#**************************************************************************************
#
# Задаем Url, откуда будет загружаться файл CACertificates.p7b:
$download_url = "https://iit.com.ua/download/productfiles/CACertificates.p7b"
#
#**************************************************************************************
#$download_url = "https://gatekeeper.galexsoftware.info/CACertificates.p7b"
[string] $userAgent = "p7bmaker PoSH downloader module/1.0"

function Remove-File {
    param (
        [Parameter(Mandatory = $true)]
        [string] $FileName
    )

    $retVal = Test-Path $FileName
    if ($retVal) {
        try {
            Remove-Item -Path $FileName -Force -ErrorAction Stop -ErrorVariable err
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
        [string] $baseUrl
    )

    $baseUrl = $baseUrl.Substring(0, $baseUrl.LastIndexOf("/") + 1)
    $baseSlash = $baseUrl.IndexOf("/", $baseUrl.IndexOf("://") + 3)

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
        [string] $Domain
    )

    $retVal = $true;
    $startIndex = $Domain.IndexOf("://")
    $name = $Domain.Substring($startIndex + 3, $Domain.LastIndexOf("/") - $startIndex - 3)
    try {
        $dnsRecord = Resolve-DnsName -Name $name -DnsOnly -ErrorAction Stop
        Write-Host "OK - The domain name $name was resolved to the IP address: $($dnsRecord.IPAddress -join ',')" -ForegroundColor green
    } catch {
        $errMessage = $PSItem.Exception.Message
        Write-Host "Error resolve host $errMessage" -ForegroundColor Red
        $retVal = $false
    }

    return $retVal
}

function Test-WebStatus {
    param (
        [Parameter(Mandatory = $true)]
        [string] $Domain
    )

    $retVal = $true
    try {
        [net.httpWebRequest] $req = [net.webRequest]::create($Domain)
        $req.Method = “HEAD”
        $req.UserAgent = $userAgent
        $req.Timeout = 10000;
        [net.httpWebResponse] $res = $req.getResponse()
        if ($res.StatusCode -eq “200”) {
            Write-Host “OK - Site is available” -ForegroundColor Green
        } else {
            Write-Host “Site $url is down” -ForegroundColor red
            $retVal = $false
        }
    } catch [System.Net.WebException] {
        $errMessage = $PSItem.Exception.Message
        Write-Host "Site $Domain is not available - $errMessage." -ForegroundColor red
        $retVal = $false
    } catch {
        Write-Error $PSItem
        $retVal = $false
    } finally {
        if($null -ne $res) {
            $res.Dispose()
        }
    }

    return $retVal
}

function Get-DownloadFile {
    param (
        [Parameter(Mandatory = $true)]
        [string] $DownloadUrl,
        [Parameter(Mandatory = $true)]
        [string] $SavePath
    )

    $retVal = $true
    $fileName = $DownloadUrl.Substring($DownloadUrl.LastIndexOf('/') + 1)
    if ($SavePath.EndsWith('\')) {
        $fileName = $SavePath + $fileName
    } else {
        $fileName = $SavePath + '\' + $fileName
    }

    Add-Type -AssemblyName System.Net.Http
    $httpClient = New-Object System.Net.Http.HttpClient
    try {
        $httpClient.DefaultRequestHeaders.add('User-Agent', $userAgent)
        $response = $httpClient.GetAsync($DownloadUrl)
        $response.Wait()
        $result = $response.Result
        if (-not $result.IsSuccessStatusCode) {
            Write-Host $('Error download file: Status {0}, Reason: {1}.' -f [int]$result.StatusCode, $result.ReasonPhrase)
            $retVal = $false
        } else {
            try {
                $outputFileStream = [System.IO.FileStream]::new($fileName, [System.IO.FileMode]::Create, [System.IO.FileAccess]::Write)
                $downloadTask = $response.Result.Content.CopyToAsync($outputFileStream)
                $downloadTask.Wait()
                Write-Host "OK - The file was successfully saved in $SavePath" -ForegroundColor Green
            } catch {
                Write-Host "Error save file $SavePath Reason: $($PSItem.ToString())" -ForegroundColor Red
                $retVal = $false
            } finally {
                $outputFileStream.Close()
            }
        }
    } catch {
        Write-Error ('Error: {0}' -f $PSItem)
        $retVal = $false
    } finally {
        if($null -ne $result) {
            $result.Dispose()
        }
    }
    return $retVal
}

function Start-ProcessWithOutput {
    [CmdletBinding(SupportsShouldProcess = $true)]
    param (
        [Parameter(Mandatory = $true)]
        [string] $FilePath,
        [Parameter()]
        [string[]] $ArgumentsList
     )

    begin {
        $tmp = [System.IO.Path]::GetTempFileName()
        try {
            $readJob = Start-Job -ScriptBlock { param( $Path ) Get-Content -Path $Path -Wait -Encoding UTF8 } -ArgumentList $tmp  -ErrorAction Stop -ErrorVariable err
            $process = Start-Process -FilePath $FilePath -ArgumentList $ArgumentsList -RedirectStandardOutput $tmp -Wait -NoNewWindow -PassThru -ErrorAction Stop -ErrorVariable err
        } catch {
            Write-Error $err
            exit 1
        }
    }

    process {
        do {
            $readJob | Receive-Job | Write-Output
        } while (-not $process.HasExited)
    }

    end {
        $readJob | Remove-Job -Force
        Remove-Item -Path $tmp -Force
     }
}

#Removing exists p7b and sha files
if ($Workdir.EndsWith('\')) {
    $mask1 = $Workdir + "*.p7b"
    $mask2 = $Workdir + "*.sha"
} else {
    $mask1 = $Workdir + "\*.p7b"
    $mask2 = $Workdir + "\*.sha"
}
$retVal = Remove-File $mask1
if (-not $retVal) {
    exit 1
}
$retVal = Remove-File $mask2
if (-not $retVal) {
    exit 1
}

# Check domain name and site availability
if (-not [string]::IsNullOrEmpty($Url)) {
    $download_url = $Url.Trim()
    Write-Host "The value of the 'download_url' was changed with the value from the Url argument...`n" -ForegroundColor Yellow
}
$domain = Get-DomainFromUrl $download_url
$retVal = Test-CheckDnsName $domain
if (-not $retVal) {
    exit 1
}
$retVal = Test-WebStatus $domain
if (-not $retVal) {
    exit 1
}

#Download and save CACertificates.p7b file
$retVal = Get-DownloadFile $download_url $WorkDir
if (-not $retVal) {
    exit 1
}

# Let's start creating a p7b container with the necessary certificates
$pathToExecute = $PSScriptRoot + "\p7bmaker.exe"
$retVal = Test-Path $pathToExecute
if (-not $retVal) {
    Write-Host "The executable file p7bmaker.exe not found" -ForegroundColor Red
    exit 1
}

Write-Output "`nStarting $pathToExecute...`n"

Start-ProcessWithOutput -FilePath $pathToExecute -ArgumentsList "-l $WorkDir --silent"

Write-Host "DONE" -ForegroundColor Green

exit 0
