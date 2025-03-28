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
        [string] $BaseUrl
    )

    $url = [System.Uri]$BaseUrl
    [bool] $retVal = if ($url.Query.Length -eq 0) { $false } else { $true }
    return $retVal
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

    [bool] $retVal = $true
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
            Write-Host $pattern -ForegroundColor Red
            $retVal = $false
        }
    } catch {
        Write-Error $PSItem
        $retVal = $false
    } finally {
        Get-GarbageCollector
    }

    return $retVal
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

    [bool] $retVal = $true
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
        Write-Host "OK - The file was successfully saved in the $SavePath folder." -ForegroundColor Green
    } catch {
        Write-Error $PSItem
        $retVal = $false
    } finally {
        Get-GarbageCollector
    }

    return $retVal
}

function Get-ShaHashFile {
    param (
        [Parameter(Mandatory = $true)]
        [ValidateNotNullOrEmpty()]
        [string] $FileName
    )

    [bool] $retVal = $true
    try {
        [string] $hash = (Get-FileHash -Path $FileName -Algorithm SHA1 -ErrorAction Stop).Hash
        [string] $hashFile = $FileName + ".sha"
        [string] $name = (Split-Path -Path $FileName -Leaf -ErrorAction Stop)
        $hash + " *" + $name | Out-File -FilePath $hashFile -Encoding utf8 -ErrorAction Stop
        Write-Host "OK - The hash file '$hashFile' was successfully created." -ForegroundColor Green
    } catch {
        Write-Host $PSItem -ForegroundColor Red
        $retVal = $false
    }
    return $retVal
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
