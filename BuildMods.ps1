$cpp = Get-ChildItem -Path "Mods\*\Scripts\*.cpp" -Recurse

foreach ($mod in $cpp) {
    $name = $mod.BaseName
    $outputFile = Join-Path -Path $mod.DirectoryName -ChildPath "$name.wasm"
    
    Write-Host "Building: $($mod.FullName) -> $outputFile" -ForegroundColor Cyan

    & clang++ --target=wasm32 -O2 -nostdlib "-Wl,--no-entry" "-Wl,--export-all" "-Wl,--allow-undefined"	 $mod.FullName -o $outputFile -g
}

Write-Host "Done building mods" -ForegroundColor Green