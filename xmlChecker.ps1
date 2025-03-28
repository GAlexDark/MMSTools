﻿
<#PSScriptInfo

.VERSION 1.0

.GUID c3b01d81-abb7-401d-9a9a-190f95516b9c

.AUTHOR Oleksii Gaienko <support@galexsoftware.info>

.COPYRIGHT 2024 Oleksii Gaienko. All rights reserved.

.TAGS xmlChecker

.LICENSEURI https://www.gnu.org/licenses/#GPL

.PROJECTURI https://github.com/GAlexDark/MMSTools

#>

<#

.SYNOPSIS
 Simple check xml files.

.DESCRIPTION
 The xlschecker.ps1 script checks xml files

.PARAMETER FilePath
 Specifies the path to the xml file to check.

.INPUTS
 None. You can't pipe objects to p7bmaker.ps1.

.OUTPUTS
 None. p7bmaker.ps1 doesn't generate any output.

#>

#requires -version 5

[CmdletBinding()]
Param (
    [Parameter (Mandatory=$true, Position=0,
    HelpMessage="Enter the path to the xml file to check.")]
    [ValidateNotNullOrEmpty()]
    [string] $FilePath
)

$FilePath = $FilePath.Trim()
if ((($FilePath.Length -eq 1) -and ($FilePath -eq ".")) -or
    (($FilePath.Length -eq 2) -and ($FilePath -eq ".\"))) {
    $FilePath = $PSScriptRoot
}
try {
    [bool] $retVal = Test-Path -Path $FilePath -ErrorAction Stop -ErrorVariable err
	if (!$retVal) {
		Write-Warning "The $FilePath file not found."
		exit 1
	}
} catch {
    Write-Host $err.ErrorRecord -ForegroundColor Red
    exit 1
}

Write-Host "The simple PoSH XML check script Version 1.0`nCopyright (C) 2024 Oleksii Gaienko, support@galexsoftware.info`nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it according to the terms of the GPL version 3.`n" -ForegroundColor green

$xmlContent = Get-Content -Path $FilePath
$xmlDocument = [xml]$xmlContent

$xmlDeclaration = $xmlDocument.xml
Write-Host "XML declaration: $xmlDeclaration`n"

$docElem = $xmlDocument.DocumentElement
$docElemName = $docElem.Name
$xmlnsValue = $docElem.xmlns
$eclValue =  $docElem.ecl
$xsiValue = $docElem.xsi
$schemaLocation = $docElem.schemaLocation

Write-Host "Root info:`n`tName: $docElemName`n`txmlns: $xmlnsValue`n`tecl: $eclValue`n`txsi: $xsiValue`n`tschemaLocation: $schemaLocation`n" -ForegroundColor Yellow

$mRIDValue = $docElem.mRID
Write-Host "mRID: $mRIDValue"

$revisionNumberValue = $docElem.revisionNumber
Write-Host "Revision Number: $revisionNumberValue"

$typeValue = $docElem.type
Write-Host "Type: $typeValue"

$docStatusValue = $docElem.value
Write-Host "Doc Status: $docStatusValue"

$processProcessTypeValue = $docElem.'process.processType'
Write-Host "Process Type: $processProcessTypeValue"

$processClassificationTypeValue = $docElem.'process.classificationType'
Write-Host "Classification type: $processClassificationTypeValue`n"

$sender_MarketParticipantmRIDcodingSchemeValue = $docElem.'sender_MarketParticipant.mRID'.codingScheme
$sender_MarketParticipantmRIDValue = $docElem.'sender_MarketParticipant.mRID'.InnerText
$sender_MarketParticipantMarketRoleTypeValue = $docElem.'sender_MarketParticipant.marketRole.type'
Write-Host "Sender Market Participant:`n`tmRID: $sender_MarketParticipantmRIDValue [Scheme: $sender_MarketParticipantmRIDcodingSchemeValue]`n`tRole type: $sender_MarketParticipantMarketRoleTypeValue`n"

$receiver_MarketParticipantmRIDcodingSchemeValue = $docElem.'receiver_MarketParticipant.mRID'.codingScheme
$receiver_MarketParticipantmRIDValue = $docElem.'receiver_MarketParticipant.mRID'.InnerText
$receiver_MarketParticipantMarketRoleTypeValue = $docElem.'receiver_MarketParticipant.marketRole.type'
Write-Host "Receiver Market Participant:`n`tmRID: $receiver_MarketParticipantmRIDValue [Scheme: $receiver_MarketParticipantmRIDcodingSchemeValue]`n`tRole type: $receiver_MarketParticipantMarketRoleTypeValue`n"

$createdDateTimeValue = $docElem.createdDateTime
Write-Host "Creation DateTime: $createdDateTimeValue"

$periodTimeIntervalStart = $docElem.'period.timeInterval'.start
$periodTimeIntervalEnd = $docElem.'period.timeInterval'.end
Write-Host "Time interval: $periodTimeIntervalStart - $periodTimeIntervalEnd`n"

$domainMRIDcodingSchemeValue = $docElem.'domain.mRID'.codingScheme
$domainMRIDValue = $docElem.'domain.mRID'.InnerText
Write-Host "Domain mRID: $domainMRIDValue [Scheme: $domainMRIDcodingSchemeValue]`n"

#############################################################################################################

$timeSeries = $docElem.GetElementsByTagName("TimeSeries")
$timeSeriasCount = $timeSeries.Count
Write-Host "Total Time Series found: $timeSeriasCount`n"

$i = 1
$timeSeries | ForEach-Object {
    $count = $_.Period.Point.Count
    $periodResolution = $_.Period.resolution
    $status = "OK"
    switch ( $periodResolution ) {
        "PT1M" { $hoursPerDay = 1440 }
        "PT15M" { $hoursPerDay = 96 }
        "PT30M" { $hoursPerDay = 48 }
        "PT60M" { $hoursPerDay = 24 }
    }
    if ($count -ne $hoursPerDay) {
        $status = "WARNING"
    }
    [PSCustomObject]@{
        'Item#' = $i
        'area_Domain_mRID' = $_.'area_Domain.mRID'.InnerText
        'marketParticipant_mRID' = $_.'marketParticipant.mRID'.InnerText
        'marketEvaluationPoint_mRID' = $_.'marketEvaluationPoint.mRID'.InnerText
        'Resolution' = $periodResolution
        'Points_Count' = $count
        'Status' = $status
    }
    $i++

} | Format-Table -AutoSize

exit 0
