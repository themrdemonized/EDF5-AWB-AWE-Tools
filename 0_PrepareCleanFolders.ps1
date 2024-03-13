echo "Start processing AWE AWB files to WAV"
echo "This will DELETE ALL FILES in VOICES_IN and VOICES_OUT folders, press Enter to continue"
pause

$folders = @(
    "VOICES_IN",
    "VOICES_OUT"
)
foreach ($f in $folders) {
    mkdir $f 2>$null
    if (Test-Path $f) {
        echo "Folder $f created"
    }
    $folderOut = ".\" + $f
    Get-ChildItem -LiteralPath $folderOut -File -Recurse | foreach { $_.Delete()}
}
