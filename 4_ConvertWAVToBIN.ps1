$folderIn = ".\VOICES_IN_WAV"
$folderOut = ".\VOICES_OUT"

$folderInFileCount = [int]([System.IO.Directory]::GetFiles("$folderIn", "*.wav").Count)
$folderOutFileCount = [int]([System.IO.Directory]::GetFiles("$folderOut", "*.hca").Count)

# Clear folderOut
Get-ChildItem -LiteralPath $folderOut -File -Recurse | foreach { $_.Delete()}

# While loop cause VGAudioCli.exe batch processing is unstable, convert files until counts match
while ($folderInFileCount -ne $folderOutFileCount) {
	.\VGAudioCli.exe -b -i $folderIn -o $folderOut --out-format hca
	$folderInFileCount = [int]([System.IO.Directory]::GetFiles("$folderIn", "*.wav").Count)
	$folderOutFileCount = [int]([System.IO.Directory]::GetFiles("$folderOut", "*.hca").Count)
}

foreach ($f in gci $folderOut) {
	$newNameBIN = $f.Name.Replace(".hca", ".bin")
	Rename-Item -LiteralPath $f.FullName -NewName $newNameBIN
}