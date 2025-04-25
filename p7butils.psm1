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
        if (-not (Test-Path -Path $Path -ErrorAction Stop)) {
            Write-Host "The path '$Path' is not found" -ForegroundColor Red
            return $false
        }

        $itemsToRemove = Get-ChildItem -Path (Join-Path -Path $Path -ChildPath '*' -ErrorAction Stop) -Include $Extensions -Force -ErrorAction Stop
        if ($itemsToRemove) {
            $itemsToRemove | Remove-Item -Force -ErrorAction Stop
            Write-Host "Files with specified extensions were successfully removed from '$Path'." -ForegroundColor Green
        } else {
            Write-Host "No files with the specified extensions were found in '$Path'." -ForegroundColor Yellow
        }
        return $true
    } catch {
        Write-Host "An error occurred: $($_.Exception.Message)" -ForegroundColor Red
        return $false
    }
}

function Get-DomainFromUrl {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $BaseUrl
    )

    $url = [System.Uri]$BaseUrl
    return $url.Host
}

function Get-FileNameFromUrl {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $BaseUrl
    )

    $url = [System.Uri]$BaseUrl
    return $url.Segments[-1]
}

function Test-CheckDnsName {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $Domain
    )

    try {
        $dnsRecord = Resolve-DnsName -Name $Domain -DnsOnly -ErrorAction Stop
        Write-Host "OK - The domain name '$Domain' was resolved to the IP address: $($dnsRecord.IPAddress -join ',')" -ForegroundColor green
    } catch {
        throw "Error resolve host: $($_.Exception.Message)"
    }
}

function Test-CheckQuery {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $BaseUrl
    )

    $url = [System.Uri]$BaseUrl
    return $url.Query.Length -eq 0
}

function Test-CheckURL {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $Url,

        [Parameter(Mandatory = $false)]
        [ValidateNotNullOrEmpty()]
        [bool] $IsUsingProxy = $true
    )

    try {
        $params = @{
            Method = 'Head'
            Uri = $Url
            UserAgent = $userAgent
            UseBasicParsing = $true
            SkipCertificateCheck = $true
            Headers = @{ "Cache-Control" = "no-cache" }
        }
        if ($UseProxy -and $IsUsingProxy) {
            $proxy = New-Object System.Net.WebProxy
            if ([string]::IsNullOrEmpty($proxyName)) {
                $proxy = [System.Net.WebRequest]::DefaultWebProxy
            } else {
                $proxy.Address = [Uri] $proxyName
            }
            $proxy.Credentials = [System.Net.CredentialCache]::DefaultCredentials
            $params.ProxyUseDefaultCredentials = $true
            $params.Proxy = $proxy.Address
        }
        $result = Invoke-WebRequest @params
        if ($result.StatusCode -eq 200) {
            [string] $pattern = [string]::Format("OK - The '{0}' URL available.", $Url)
            Write-Host $pattern -ForegroundColor Green
        } else {
            [string] $pattern = [string]::Format("Error connect to the '{0}'. Error code: {1}.", $Url, $result.StatusCode)
            throw $pattern
        }
    } catch {
        [string] $pattern = [string]::Format("Error check URL '{0}': {1}", $Url, $_.Exception.Message)
        throw $pattern
    } finally {
        Get-GarbageCollector
    }
}

function Get-GarbageCollector {
    [System.GC]::Collect()
    [System.GC]::WaitForPendingFinalizers()
    [System.GC]::Collect()
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
        $fileName = Get-FileNameFromUrl -BaseUrl $DownloadUrl
        $fileName = Join-Path -Path $SavePath -ChildPath $fileName -ErrorAction Stop

        $params = @{
            Method = 'Get'
            Uri = $DownloadUrl
            OutFile = $fileName
            UserAgent = $userAgent
            UseBasicParsing = $true
            Headers = @{ "Cache-Control" = "no-cache" }
        }
        if ($UseProxy) {
            $proxy = New-Object System.Net.WebProxy
            if ([string]::IsNullOrEmpty($proxyName)) {
                $proxy = [System.Net.WebRequest]::DefaultWebProxy
            } else {
                $proxy.Address = [Uri] $proxyName
            }
            $proxy.Credentials = [System.Net.CredentialCache]::DefaultCredentials
            $params.ProxyUseDefaultCredentials = $true
            $params.Proxy = $proxy.Address
        }
        Invoke-WebRequest @params
        Write-Host "OK - The file was successfully saved in the '$SavePath' folder." -ForegroundColor Green
    } catch {
        [string] $pattern = [string]::Format("Error download file from the '{0}': {1}", $DownloadUrl, $_.Exception.Message)
        throw $pattern
    } finally {
        Get-GarbageCollector
    }
}

function Get-ShaHashFile {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $FileName
    )

    try {
        [string] $hash = (Get-FileHash -Path $FileName -Algorithm SHA1 -ErrorAction Stop).Hash
        [string] $hashFile = "$FileName.sha"
        $fileNameOnly = (Split-Path -Path $FileName -Leaf -ErrorAction Stop)
        $hashContent = "$hash *$fileNameOnly"

        $hashContent | Out-File -FilePath $hashFile -Encoding utf8 -ErrorAction Stop
        Write-Host "OK - The hash file '$hashFile' was successfully created." -ForegroundColor Green
        return $true
    } catch {
        Write-Host "An error occurred: $($_.Exception.Message)" -ForegroundColor Red
        return $false
    }
}

Export-ModuleMember -Function Remove-File
Export-ModuleMember -Function Get-DomainFromUrl
Export-ModuleMember -Function Get-FileNameFromUrl
Export-ModuleMember -Function Test-CheckDnsName
Export-ModuleMember -Function Test-CheckQuery
Export-ModuleMember -Function Test-CheckURL
Export-ModuleMember -Function Get-GarbageCollector
Export-ModuleMember -Function Get-DownloadFile
Export-ModuleMember -Function Get-ShaHashFile
