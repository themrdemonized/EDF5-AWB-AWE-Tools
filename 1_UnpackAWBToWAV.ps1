echo "AWE Parser will ask for AWE, AWB files and output directory"
echo "Please drag VOICES_IN directory as output"
pause

$folderOut = ".\VOICES_IN"
invoke-expression 'cmd /c .\awe_parser.exe'
 
$ids = @{}
$new_files = @()
$max_id = 0
$id_length = 0

$folder = ".\VOICES_IN"

foreach ($f in gci $folder) {
	$arr = $f.Name.Split("_")
	$id, $name = $arr
	$id_length = $id.Length
	$id = [int]$id
	$max_id = $id
	$name = $name -join "_"
	$ids[$id] = @($name, $f.Name)
}

for ($i = 0; $i -le $max_id; $i +=1) {
	if ($ids.ContainsKey($i)) {

	} else {
		$new_id = [string]$i
		$new_id = $new_id.PadLeft($id_length, '0')
		$arr = ,@($i, $ids[0][0], ($new_id + "_" + "UNUSED"))
		$new_files += $arr
	}
}

foreach ($f in $new_files) {
	$p = $folder + "\" + $ids[0][1]
	$d = $folder + "\" + $f[2]
	echo ("$p -> $d")
	Copy-Item -Path $p -Destination $d
}

$folderIn = ".\VOICES_IN"

foreach ($f in gci $folderIn) {
	.\hca.exe $f.FullName
	Remove-Item -LiteralPath $f.FullName
}
