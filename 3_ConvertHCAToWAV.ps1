$folderIn = ".\VOICES_IN"
$folderOut = ".\VOICES_IN_WAV"

foreach ($f in gci $folderIn) {
	.\hca.exe $f.FullName
	$newFileName = $f.FullName.Replace(".hca", ".wav")
	Move-Item -LiteralPath $newFileName -Destination $folderOut
}
