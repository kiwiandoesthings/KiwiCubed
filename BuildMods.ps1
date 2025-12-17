$cpp = Get-ChildItem -Path "Mods\*\Scripts\*.cpp" -Recurse
$total = 0

foreach ($mod in $cpp) {
    $name = $mod.BaseName
    $outputFile = Join-Path -Path $mod.DirectoryName -ChildPath "$name.wasm"
    
    Write-Host "Building: $($mod.FullName) -> $outputFile" -ForegroundColor Cyan

    & clang++ --target=wasm32 -O2 -nostdlib "-Wl,--no-entry" "-Wl,--export-all" "-Wl,--allow-undefined"	 $mod.FullName -o $outputFile -g
    $total++
}

Write-Host "Done building $total mods" -ForegroundColor Green