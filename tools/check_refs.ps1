# Checks the names the mod refers to in its XML and Python, and the format of its source files.
# build.bat runs it after each build (the errors do not stop the deploy). By hand:
#   powershell -ExecutionPolicy Bypass -File tools\check_refs.ps1
#
# The game reads each XML file from the first of Mod, BtS, Warlords and Civ4 that has it (ModularLoading = 0), so the
# names defined are collected from those files: <Type> (and the other defining elements below), the names listed in
# GlobalTypes.xml, and the <Tag> of the text XML. Then, in the files of the mod:
#   error    a name with a known prefix (TRAIT_, UNIT_, ...) that is not defined, e.g. TRAIT_INDUSTRIUS
#   error    a TXT_KEY_ that no text XML has (the game would show the key itself)
#   error    a <Type> twice in a file
#   warning  a name of another kind than the element takes (e.g. a UNIT_ name in <TraitType>). The kinds an element
#            takes are learned from the base game files; an element they do not use takes the kind its name gives
#            (<SpecialBuildingType> takes SPECIALBUILDING_ names)
#   warning  a text key defined twice in the text XML of the mod
#   error    a text that uses an argument twice (%s1 ... %s1): the game replaces only one of them
#   error    the same for the names in quotes in the Python of the mod and in the Python expressions of the XML (Py...)
#   error    a character that is not ASCII, or a line that does not end with CRLF, in the XML, Python, C++ sources,
#            Makefile, build.bat, claude_sample.ini and tools
# A file the mod copied from the base game keeps the mistakes of the original: a reference the base game file also
# has is only counted, not shown.
# Exit code: 0 = no errors, 1 = errors.

param([string]$Bts = '')

$repo = Split-Path -Parent $PSScriptRoot
if ($Bts -eq '') {
	# The path of the game is defined once, in build.bat
	$found = Select-String -Path (Join-Path $repo 'build.bat') -Pattern '^set "BTS=(.*)"' | Select-Object -First 1
	if ($found -ne $null) {
		$Bts = $found.Matches[0].Groups[1].Value
	}
}
$civ4 = Split-Path -Parent $Bts
$roots = @((Join-Path $repo 'Assets'), (Join-Path $Bts 'Assets'), (Join-Path $civ4 'Warlords(J)\Assets'), (Join-Path $civ4 'Assets'))
if (-not (Test-Path (Join-Path $roots[1] 'XML'))) {
	Write-Output "check_refs: the game is not found at $Bts; skipped"
	exit 0
}

$latin1 = [Text.Encoding]::GetEncoding(28591)
$nameElement = New-Object Text.RegularExpressions.Regex('<(\w+)>\s*(([A-Z][A-Z0-9]*)(?:_[A-Z0-9]+)+)\s*</\1>', 'Compiled')
$pyElement = New-Object Text.RegularExpressions.Regex('<(Py\w+)>([^<]*)</\1>', 'Compiled')
$quotedName = New-Object Text.RegularExpressions.Regex('([''"])(([A-Z][A-Z0-9]*)(?:_[A-Z0-9]+)+)\1', 'Compiled')
$languageElement = New-Object Text.RegularExpressions.Regex('<(English|French|German|Italian|Spanish|Japanese)>([^<]*)</\1>', 'Compiled')
$textArgument = New-Object Text.RegularExpressions.Regex('%([A-Za-z])(\d+)', 'Compiled')
$definingElements = @{ 'Type' = 1; 'ScriptID' = 1; 'SoundID' = 1; 'DefineName' = 1; 'InfoType' = 1; 'Name' = 1 }

$defined = New-Object 'System.Collections.Generic.HashSet[string]'
$textTags = New-Object 'System.Collections.Generic.HashSet[string]'
$learned = New-Object 'System.Collections.Generic.HashSet[string]'
$errors = 0
$warnings = 0
$inherited = 0

function Read-Text([string]$path) {
	return $latin1.GetString([IO.File]::ReadAllBytes($path))
}

function Write-Problem([string]$level, [string]$file, [int]$line, [string]$message) {
	if ($level -eq 'error') {
		$script:errors++
	}
	else {
		$script:warnings++
	}
	Write-Output ("{0} line {1}: {2}: {3}" -f $file, $line, $level, $message)
}

# bDefine: the names the file defines count. bLearn: the kinds of names its elements take are learned (base game files)
function Add-Names([string]$text, [bool]$bGlobalTypes, [bool]$bDefine, [bool]$bLearn) {
	foreach ($m in $nameElement.Matches($text)) {
		$element = $m.Groups[1].Value
		$value = $m.Groups[2].Value
		if ($element -eq 'Tag') {
			if ($bDefine) {
				[void]$textTags.Add($value)
			}
		}
		elseif ($bGlobalTypes -or $definingElements.ContainsKey($element)) {
			if ($bDefine) {
				[void]$defined.Add($value)
			}
		}
		elseif ($bLearn) {
			# No function call here: this runs for each of about 100,000 names
			[void]$learned.Add($element + '|' + $m.Groups[3].Value)
		}
	}
}

# The line of a position in a text
function Get-Line([string]$text, [int]$index) {
	return $text.Substring(0, $index).Split("`n").Count
}

# The message for a name that does not exist, or $null. Called only for the names the quick test in the loops rejects
function Get-NameProblem([string]$value, [string]$where) {
	if ($value.StartsWith('TXT_KEY_')) {
		return "$value is not a text key of any text XML ($where)"
	}
	return "$value is not defined ($where)"
}

# The prefixes of the names an element takes, or $null when unknown
$expectedCache = @{}
function Get-ExpectedPrefixes([string]$element) {
	if ($elementPrefixes.ContainsKey($element)) {
		return $elementPrefixes[$element]
	}
	if (-not $expectedCache.ContainsKey($element)) {
		$expectedCache[$element] = $null
		if ($element -cmatch '^([A-Z][A-Za-z]*?)Type$') {
			$prefix = $matches[1].ToUpper()
			if ($prefixes.Contains($prefix)) {
				$list = New-Object 'System.Collections.Generic.List[string]'
				$list.Add($prefix)
				$expectedCache[$element] = $list
			}
		}
	}
	return $expectedCache[$element]
}

# The XML files the game reads, from the first root that has each; Base is the base game file a file of the mod replaces
$xmlFiles = @{}
for ($r = 0; $r -lt $roots.Count; $r++) {
	$dir = Join-Path $roots[$r] 'XML'
	if (-not (Test-Path $dir)) {
		continue
	}
	foreach ($f in Get-ChildItem $dir -Recurse -Filter *.xml) {
		$rel = $f.FullName.Substring($dir.Length + 1).ToLower()
		if ($rel.Contains('schema')) {
			continue
		}
		if (-not $xmlFiles.ContainsKey($rel)) {
			$xmlFiles[$rel] = @{ Path = $f.FullName; Root = $r; Base = $null }
		}
		elseif ($xmlFiles[$rel].Root -eq 0 -and $xmlFiles[$rel].Base -eq $null) {
			$xmlFiles[$rel].Base = $f.FullName
		}
	}
}

foreach ($rel in $xmlFiles.Keys) {
	$info = $xmlFiles[$rel]
	Add-Names (Read-Text $info.Path) ($rel -eq 'globaltypes.xml') $true ($info.Root -gt 0)
	if ($info.Base -ne $null) {
		Add-Names (Read-Text $info.Base) $false $false $true
	}
}

$prefixes = New-Object 'System.Collections.Generic.HashSet[string]'
foreach ($name in $defined) {
	[void]$prefixes.Add($name.Substring(0, $name.IndexOf('_')))
}

# element -> the prefixes of the names it takes in the base game
$elementPrefixes = @{}
foreach ($pair in $learned) {
	$parts = $pair.Split('|')
	if (-not $elementPrefixes.ContainsKey($parts[0])) {
		$elementPrefixes[$parts[0]] = New-Object 'System.Collections.Generic.List[string]'
	}
	$elementPrefixes[$parts[0]].Add($parts[1])
}

# The XML of the mod. The loops run for each name of the mod: no function call on the way of a name that is fine
$modXml = Join-Path $roots[0] 'XML'
$modTags = @{}
$numXml = 0
foreach ($rel in ($xmlFiles.Keys | Sort-Object)) {
	$info = $xmlFiles[$rel]
	if ($info.Root -ne 0) {
		continue
	}
	$numXml++
	$display = 'Assets\XML\' + $info.Path.Substring($modXml.Length + 1)
	$baseText = ''
	if ($info.Base -ne $null) {
		$baseText = Read-Text $info.Base
	}
	$text = Read-Text $info.Path
	$types = @{}
	foreach ($m in $nameElement.Matches($text)) {
		$element = $m.Groups[1].Value
		$value = $m.Groups[2].Value
		$prefix = $m.Groups[3].Value
		if ($element -eq 'Tag') {
			if ($modTags.ContainsKey($value)) {
				$first = $modTags[$value]
				Write-Problem 'warning' $display (Get-Line $text $m.Index) "$value is also defined at $($first[0]) line $(Get-Line $first[1] $first[2])"
			}
			else {
				$modTags[$value] = @($display, $text, $m.Index)
			}
			continue
		}
		if ($rel -eq 'globaltypes.xml') {
			continue
		}
		if ($definingElements.ContainsKey($element)) {
			if ($element -eq 'Type') {
				if (-not $types.ContainsKey($value)) {
					$types[$value] = $m.Index
				}
				elseif ([regex]::Matches($baseText, [regex]::Escape($m.Value)).Count -ge 2) {
					$inherited++
				}
				else {
					Write-Problem 'error' $display (Get-Line $text $m.Index) "$value is also the <Type> at line $(Get-Line $text $types[$value])"
				}
			}
			continue
		}
		if ($prefix -eq 'TXT') {
			if ($value.StartsWith('TXT_KEY_') -and -not $textTags.Contains($value)) {
				if ($baseText.Contains($m.Value)) {
					$inherited++
				}
				else {
					Write-Problem 'error' $display (Get-Line $text $m.Index) (Get-NameProblem $value "<$element>")
				}
			}
			continue
		}
		if ($prefixes.Contains($prefix) -and -not $defined.Contains($value)) {
			if ($baseText.Contains($m.Value)) {
				$inherited++
			}
			else {
				Write-Problem 'error' $display (Get-Line $text $m.Index) (Get-NameProblem $value "<$element>")
			}
			continue
		}
		$expected = $elementPrefixes[$element]
		if ($expected -eq $null) {
			$expected = Get-ExpectedPrefixes $element
		}
		if ($expected -ne $null -and -not $expected.Contains($prefix) -and -not $baseText.Contains($m.Value)) {
			Write-Problem 'warning' $display (Get-Line $text $m.Index) ("$value is a $($prefix)_ name, but <$element> takes " + (($expected | Sort-Object | ForEach-Object { $_ + '_' }) -join ' ') + ' names')
		}
	}
	# The text XML: an argument used twice in a text (%s1 ... %s1). The game replaces only one of them and shows the other
	# as it is: pass the value again as another argument (%s3)
	if ($rel.StartsWith('text\')) {
		foreach ($m in $languageElement.Matches($text)) {
			$seen = @{}
			foreach ($a in $textArgument.Matches($m.Groups[2].Value.Replace('%%', ''))) {
				$key = $a.Groups[1].Value.ToLower() + $a.Groups[2].Value
				if ($seen.ContainsKey($key)) {
					Write-Problem 'error' $display (Get-Line $text $m.Index) "<$($m.Groups[1].Value)> uses %$key twice: the game replaces only one of them (give the value again as another argument)"
					break
				}
				$seen[$key] = 1
			}
		}
	}

	# The names in quotes in the Python expressions (PyOnGameStart, PyResult, ...)
	foreach ($m in $pyElement.Matches($text)) {
		foreach ($q in $quotedName.Matches($m.Groups[2].Value)) {
			$value = $q.Groups[2].Value
			if ($value.StartsWith('TXT_KEY_')) {
				$bOk = $textTags.Contains($value)
			}
			else {
				$bOk = (-not $prefixes.Contains($q.Groups[3].Value)) -or $defined.Contains($value)
			}
			if (-not $bOk) {
				if ($baseText.Contains($q.Value)) {
					$inherited++
				}
				else {
					Write-Problem 'error' $display (Get-Line $text $m.Index) (Get-NameProblem $value "<$($m.Groups[1].Value)>")
				}
			}
		}
	}
}

# The Python of the mod
$modPython = Join-Path $roots[0] 'Python'
$numPython = 0
if (Test-Path $modPython) {
	foreach ($f in Get-ChildItem $modPython -Recurse -Filter *.py | Sort-Object FullName) {
		$numPython++
		$rel = $f.FullName.Substring($modPython.Length + 1)
		$display = 'Assets\Python\' + $rel
		$baseText = ''
		for ($r = 1; $r -lt $roots.Count; $r++) {
			$path = Join-Path $roots[$r] ('Python\' + $rel)
			if (Test-Path $path) {
				$baseText = Read-Text $path
				break
			}
		}
		$text = Read-Text $f.FullName
		foreach ($q in $quotedName.Matches($text)) {
			$value = $q.Groups[2].Value
			if ($value.StartsWith('TXT_KEY_')) {
				$bOk = $textTags.Contains($value)
			}
			else {
				$bOk = (-not $prefixes.Contains($q.Groups[3].Value)) -or $defined.Contains($value)
			}
			if ($bOk) {
				continue
			}
			# Not in the comment lines (the examples in them)
			$lineStart = $text.LastIndexOf("`n", [Math]::Max(0, $q.Index - 1)) + 1
			if ($text.Substring($lineStart, $q.Index - $lineStart).TrimStart().StartsWith('#')) {
				continue
			}
			if ($baseText.Contains($q.Value)) {
				$inherited++
			}
			else {
				Write-Problem 'error' $display (Get-Line $text $q.Index) (Get-NameProblem $value 'Python')
			}
		}
	}
}

# The format: ASCII only, CRLF
$nonAscii = New-Object Text.RegularExpressions.Regex('[^\x00-\x7F]')
$bareLf = New-Object Text.RegularExpressions.Regex('(?<!\r)\n')
$formatFiles = @()
$formatFiles += Get-ChildItem (Join-Path $repo 'Assets') -Recurse -Include *.xml, *.py
$formatFiles += Get-ChildItem (Join-Path $repo 'CvGameCoreDLL') -File | Where-Object { $_.Extension -match '^\.(cpp|h|inl|def)$' -or $_.Name -eq 'Makefile' }
$formatFiles += Get-Item (Join-Path $repo 'build.bat'), (Join-Path $repo 'claude_sample.ini')
$formatFiles += Get-ChildItem (Join-Path $repo 'tools') -File -Filter *.ps1
foreach ($f in $formatFiles) {
	$text = Read-Text $f.FullName
	$display = $f.FullName.Substring($repo.Length + 1)
	$m = $nonAscii.Match($text)
	if ($m.Success) {
		Write-Problem 'error' $display ($text.Substring(0, $m.Index).Split("`n").Count) 'a character that is not ASCII (write Japanese as &#NNNNN; in the text XML, English elsewhere)'
	}
	$m = $bareLf.Match($text)
	if ($m.Success) {
		Write-Problem 'error' $display ($text.Substring(0, $m.Index).Split("`n").Count) 'the line ends with LF, not CRLF'
	}
}

Write-Output ("check_refs: {0} XML and {1} Python files of the mod, {2} files for the format: {3} errors, {4} warnings ({5} problems copied from the base game not shown)" -f $numXml, $numPython, $formatFiles.Count, $errors, $warnings, $inherited)
if ($errors -gt 0) {
	exit 1
}
exit 0
