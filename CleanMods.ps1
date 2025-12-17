$wasm = Get-ChildItem -Path "Mods\*\Scripts\*.wasm" -Recurse
$total = 0

foreach ($mod in $wasm) {
    & del $mod.FullName
    $total++
}

Write-Host "Cleaned $total mods" -ForegroundColor Green