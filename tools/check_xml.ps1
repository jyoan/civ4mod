# Validates the XML of the mod against its schemas with MSXML 3, as the game does when it loads them.
# Run before starting the game: it finds the schema errors that would stop the game at startup.
#   powershell -ExecutionPolicy Bypass -File tools\check_xml.ps1
# A file whose schema is not in the mod (the game reads it from the base game) cannot be checked and is only listed.
# Exit code: 0 = no errors, 1 = errors.

$root = Join-Path (Split-Path -Parent $PSScriptRoot) 'Assets\XML'
$errors = 0
$skipped = 0
$count = 0

foreach ($file in Get-ChildItem $root -Recurse -Filter *.xml) {
	$doc = New-Object -ComObject Msxml2.DOMDocument.3.0
	$doc.async = $false
	$doc.validateOnParse = $true
	$doc.resolveExternals = $true
	$count++
	if (-not $doc.load($file.FullName)) {
		$name = $file.FullName.Substring($root.Length + 1)
		if ($doc.parseError.line -eq 0) {
			$skipped++
			Write-Output ("$name not checked: " + $doc.parseError.reason.Trim())
		}
		else {
			$errors++
			Write-Output ("$name line " + $doc.parseError.line + ': ' + $doc.parseError.reason.Trim())
			Write-Output ('    ' + $doc.parseError.srcText.Trim())
		}
	}
}

Write-Output ("$count files, $errors errors, $skipped not checked")
if ($errors -gt 0) {
	exit 1
}
exit 0
