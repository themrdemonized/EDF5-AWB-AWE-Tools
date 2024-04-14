$folderIn = ".\VOICES_IN"
$folderOut = ".\VOICES_OUT"
$folderTemp = ".\VOICES_TEMP"

if (Test-Path -Path $folderTemp) {

} else {
	mkdir $folderTemp
}

$folderInFileCount = [int]([System.IO.Directory]::GetFiles("$folderIn", "*.wav").Count)
$folderOutFileCount = [int]([System.IO.Directory]::GetFiles("$folderOut", "*.hca").Count)

# Clear folderOut
Get-ChildItem -LiteralPath $folderOut -File -Recurse | foreach { $_.Delete()}
Get-ChildItem -LiteralPath $folderTemp -File -Recurse | foreach { $_.Delete()}

# While loop cause VGAudioCli.exe batch processing is unstable, convert files until counts match

$folderInTemp = $folderIn
$folderInTempClean = 0
while ($folderInFileCount -ne $folderOutFileCount) {

	.\VGAudioCli.exe -b -i $folderInTemp -o $folderOut --out-format hca
	$folderInFileCount = [int]([System.IO.Directory]::GetFiles("$folderIn", "*.wav").Count)
	$folderOutFileCount = [int]([System.IO.Directory]::GetFiles("$folderOut", "*.hca").Count)

	if ($folderInTempClean -eq 1) {
		Get-ChildItem -LiteralPath $folderTemp -File -Recurse | foreach { $_.Delete()}
		$folderInTempClean = 0
	}
	
	foreach ($f in gci -LiteralPath $folderIn) {
		$name = $f.BaseName + ".hca"
		if (Test-Path -Path "$folderOut\$name") {
			
		} else {
			$n = $f.Name
			$n2 = $f.FullName
			$p = "$folderTemp\$n"
			$p = [IO.Path]::GetFullPath($p)

			# echo $p
			# echo $n2
			# pause

			New-Item -ItemType HardLink -Path $p -Value $n2
			$folderInTemp = $folderTemp
			$folderInTempClean = 1
		}
	}
}

Get-ChildItem -LiteralPath $folderTemp -File -Recurse | foreach { $_.Delete()}
Remove-Item -Path $folderTemp -Recurse -Force

echo "Renaming .hca to .bin"
foreach ($f in gci $folderOut) {
	$newNameBIN = $f.Name.Replace(".hca", ".bin")
	Rename-Item -LiteralPath $f.FullName -NewName $newNameBIN
}

$folder = ".\VOICES_OUT"
.\AWB_repacker.exe $folder
