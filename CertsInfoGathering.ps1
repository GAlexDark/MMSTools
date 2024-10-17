
<#PSScriptInfo

.VERSION 1.0

.GUID dbc9d91a-f19c-49d8-ac78-7daa51d41488

.AUTHOR Oleksii Gaienko <support@galexsoftware.info>

.COPYRIGHT 2024 Oleksii Gaienko. All rights reserved.

.TAGS CertsInfoGathering

.LICENSEURI https://www.gnu.org/licenses/#GPL

.PROJECTURI https://github.com/GAlexDark/MMSTools

#>

<#

.SYNOPSIS
 Gathering certificates info.

.DESCRIPTION
 The Certificates Information Gathering PoSH Script

.PARAMETER Path
 Specifies the path to to the folder, contains *.cer, *.crt, *.pem, and *.p7b files.

.PARAMETER Gui
 Specifies output certificates information to the GridView.

.PARAMETER PKCS7Only
 Select only PKCS7 files (default - select all supported files).

.PARAMETER CertsOnly
 Select only certificate files (default - select all supported files).

.INPUTS
 None. You can't pipe objects to CertsInfoGathering.ps1.

.OUTPUTS
 CertsInfoGathering.ps1 generate certificates info in the PSCustomObject if the 'Gui' flag is not enabled

.EXAMPLE
 PS> .\CertsInfoGathering.ps1 -Path B:\certdata -Gui
 Output certificates information to the GridView and saves information about the selected certificates to the .\CertificatesInfo.csv file (Delimiter - '`t', UTF8).

.EXAMPLE
 PS> .\CertsInfoGathering.ps1 -Path B:\certdata | Where-Object { $_.Status -like "Ok" } | Select-Object -Property 'Common Name', 'Serial Number', 'Valid To', 'Key Usage (OID 2.5.29.15)' | Format-Table -AutoSize
 Output selected certificates information to the Console

.EXAMPLE
 PS> .\CertsInfoGathering.ps1 -Path B:\certdata | Where-Object { $_.Status -like "Ok" } | Select-Object -Property 'CN', 'SN', 'Valid To', 'KU' | Format-Table -AutoSize
 Output certificates information to the Console (same as previous example)

       Property Name                        Short Name
    ------------------                    -------------
     Common Name                            CN
     Serial Number                          SN
     Valid To                               ValidTo
     Key Usage (OID 2.5.29.15)              KU
     Extended Key Usage (OID 2.5.29.37)     EKU
     Path To The File                       Path

.EXAMPLE
 PS> .\CertsInfoGathering.ps1 -Path B:\certdata | Where-Object { $_.Status -like "Ok" } | Select-Object -Property * | Format-Table -AutoSize
 Output certificates information to the Console

#>

#requires -version 5

[CmdletBinding()]
Param (
        [Parameter(Mandatory=$true,
        HelpMessage="Enter the path to the folder, contains *.cer, *.crt, *.pem, and *.p7b files.")]
        [ValidateNotNullOrEmpty()]
        [string] $Path,
        [Parameter(Mandatory=$false,
        HelpMessage="Select GUI or Console Output (default).")]
        [switch] $Gui = $false,
        [Parameter(Mandatory=$false,
        HelpMessage="Select only PKCS7 files (default - select all supported files).")]
        [switch] $PKCS7Only = $false,
        [Parameter(Mandatory=$false,
        HelpMessage="Select only certificate files (default - select all supported files).")]
        [switch] $CertsOnly = $false
)

[Console]::OutputEncoding = [System.Text.Encoding]::GetEncoding(0)


$Path = $Path.Trim()
if ((($Path.Length -eq 1) -and ($Path -eq ".")) -or
    (($Path.Length -eq 2) -and ($Path -eq ".\"))) {
    $Path = $PSScriptRoot
}

try {
    [bool] $retVal = Test-Path -Path $Path -ErrorAction Stop -ErrorVariable err
    if (!$retVal) {
        Write-Warning "The work dir $Path not found."
        exit 1
    }
} catch {
    Write-Host $err.ErrorRecord -ForegroundColor Red
    exit 1
}

Write-Host "The Certificates Information Gathering PoSH Script Version 1.0`nCopyright (C) 2024 Oleksii Gaienko, support@galexsoftware.info`nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.`n" -ForegroundColor green

function Replace-ArrayValue {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [array] $Array,
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $What,
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $To
    )
    for ($i=0; $i -lt $Array.count; $i++) {
        if ([string]::Compare($Array[$i], $What, $true) -eq 0) {
            $Array[$i] = $To
        }
    }
    return $Array
}

function Get-CertificateInfo {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        $data
    )

    try {
        $certItem = New-Object System.Security.Cryptography.X509Certificates.X509Certificate2 $data
        [string] $certCN = $certItem.Subject -split ', ' | Where-Object { $_ -like "CN=*" }
        #[string] $certOrganisation = $certItem.Subject -split ', ' | Where-Object { $_ -like "O=*" }
        [string] $certSN = $certItem.SerialNumber
        [datetime] $certValidTo = $certItem.NotAfter
        [datetime] $currentDate = Get-Date
        [string] $status = "Ok"
        if ($currentDate -gt $certValidTo) {
            $status = "Expired"
        } elseif ((New-TimeSpan -Start $currentDate -End $certValidTo).TotalDays -le 10) {
            $status = "Warning!"
        }

        [string] $keyUsage = ($certItem.Extensions.KeyUsages) -join ', '
        [array] $enhancedKeyUsage = $certItem.EnhancedKeyUsageList
        if ($enhancedKeyUsage) {
            $enhancedKeyUsage = Replace-ArrayValue -Array $enhancedKeyUsage -What "1.2.804.2.1.1.1.3.9" -To "Digital Seal (1.2.804.2.1.1.1.3.9)"
            $enhancedKeyUsage = Replace-ArrayValue -Array $enhancedKeyUsage -What "1.3.6.1.4.1.19398.1.1.8.1" -To "CMP Message Processing (1.3.6.1.4.1.19398.1.1.8.1)"
            $enhancedKeyUsage = Replace-ArrayValue -Array $enhancedKeyUsage -What "1.3.6.1.4.1.19398.1.1.8.31" -To "IIT Gryada-301 Token Cryptomodule (1.3.6.1.4.1.19398.1.1.8.31)"
            $enhancedKeyUsage = Replace-ArrayValue -Array $enhancedKeyUsage -What "1.3.6.1.4.1.19398.1.1.8.23" -To "CryptoCard-337 or SecureToken-337 (1.3.6.1.4.1.19398.1.1.8.23)"
            $enhancedKeyUsage = Replace-ArrayValue -Array $enhancedKeyUsage -What "1.3.6.1.4.1.19398.1.1.8.22" -To "IIT Almaz-1K Token (1.3.6.1.4.1.19398.1.1.8.22)"
            $enhancedKeyUsage = Replace-ArrayValue -Array $enhancedKeyUsage -What "1.3.6.1.4.1.19398.1.1.8.35" -To "Other Tokens (1.3.6.1.4.1.19398.1.1.8.35)"
        }

        $retVal = [PSCustomObject]@{
            'Common Name' = $certCN.Substring(3).Replace('""', '"')
            #'Organisation' = $certOrganisation
            'Serial Number' = $certSN
            'Valid To' = $certValidTo
            'Status' = $status
            'Key Usage (OID 2.5.29.15)' = $keyUsage
            'Extended Key Usage (OID 2.5.29.37)' = ($enhancedKeyUsage -join ', ').ToString()
            'Path To The File' = $null
        }
    } catch {
        Write-Error $PSItem
    } finally {
        if ($certItem) {
            $certItem.Dispose()
        }
    }

    return $retVal
}

function Get-PemCertificate {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $data
    )

    [string[]] $buf = $data -split "-----END CERTIFICATE-----"
    [string[]] $retVal = $buf | Where-Object { ($_.Trim().Length -ne 0) -and ($_ -ne $null) }
    for ($i=0; $i -lt $retVal.count; $i++) {
        $retVal[$i] = $retVal[$i].Trim()
        $retVal[$i] = $retVal[$i] + "-----END CERTIFICATE-----"
    }
    return $retVal
}

# CMP - certificate management protocol
[string[]] $CertificateFileType = ('*.cer','*.crt','*.p7b', '*.pem')
if ($PKCS7Only -and $CertsOnly) {
    $PKCS7Only = $false
    $CertsOnly = $false
}
if ($PKCS7Only) {
    $CertificateFileType = ('*.p7b','*.pem')
}
if ($CertsOnly) {
    $CertificateFileType = ('*.cer','*.crt','*.pem')
}
$certs = Get-ChildItem -Path $Path -Include $CertificateFileType -Recurse

if ($certs) {
    [array] $certsInfo = @()

    $certs | ForEach-Object {
        [string] $pathToFile = $_.FullName
        [string] $prettyPath = "." + $pathToFile.Substring($Path.Length)
        if ($pathToFile.EndsWith(".p7b") -or $pathToFile.EndsWith(".pem")) {
            try {
                $certsCollection = New-Object System.Security.Cryptography.X509Certificates.X509Certificate2Collection
                [Byte[]] $bytes = @()
                if ($pathToFile.EndsWith(".pem")) {
                    [string] $pemFileContent = Get-Content -Path $pathToFile -Encoding String -ErrorAction Stop
                    [string[]] $pemCerts = Get-PemCertificate $pemFileContent
                    for ($i=0; $i -lt $pemCerts.count; $i++) {
                        $bytes.Clear()
                        $bytes = $pemCerts[$i].ToCharArray()
                        $certsCollection.Import($bytes)
                    }
                } else {
                    $bytes = [System.IO.File]::ReadAllBytes($pathToFile)
                    $certsCollection.Import($bytes)
                }
                $certsCollection | ForEach-Object {
                    $certsInfo += Get-CertificateInfo $_
                    $certsInfo[$certsInfo.count - 1].'Path To The File' = $prettyPath
                }
            } catch {
                Write-Error $PSItem
            } finally {
                if ($certsCollection) {
                    $certsCollection.Dispose()
                }
            }
        } else {
            $certsInfo += Get-CertificateInfo $pathToFile
            $certsInfo[$certsInfo.count - 1].'Path To The File' = $prettyPath
        }
    }

    if ($Gui) {
        $certsInfo |  Out-GridView -Title "Certificates Information Gathering PoSH Script" -PassThru | Export-Csv -Path .\CertificatesInfo.csv -Delimiter "`t" -Encoding UTF8
    } else {
        $certsInfo | Add-Member -MemberType AliasProperty -Name CN -Value 'Common Name'
        $certsInfo | Add-Member -MemberType AliasProperty -Name SN -Value 'Serial Number'
        $certsInfo | Add-Member -MemberType AliasProperty -Name ValidTo -Value 'Valid To'
        $certsInfo | Add-Member -MemberType AliasProperty -Name KU -Value 'Key Usage (OID 2.5.29.15)'
        $certsInfo | Add-Member -MemberType AliasProperty -Name EKU -Value 'Extended Key Usage (OID 2.5.29.37)'
        $certsInfo | Add-Member -MemberType AliasProperty -Name Path -Value 'Path To The File'
        $certsInfo
    }
} else {
    Write-Warning "No certificates found"
    exit 1
}

Write-Host "DONE" -ForegroundColor Green

exit 0
