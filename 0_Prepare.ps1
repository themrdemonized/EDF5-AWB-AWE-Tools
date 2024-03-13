$folders = @(
    "VOICES_IN",
    "VOICES_IN_WAV",
    "VOICES_OUT"
)
foreach ($f in $folders) {
    mkdir $f 2>$null
    if (Test-Path $f) {
        echo "Folder $f created"
    }
}
