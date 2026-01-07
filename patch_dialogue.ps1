$path = "src/loneshooter.cpp"
$c = Get-Content -Encoding UTF8 $path
$count = $c.Count
Write-Host "Total lines: $count"
if ($count -lt 6000) { Write-Error "File too short!"; exit 1 }

# Part 1: Lines 0 to 5833 (inclusive) -> Lines 1 to 5834
$p1 = $c[0..5833]

# Part 2: Lines 5983 to End (inclusive) -> Lines 5984 to End
# Note: array index 5983 is line 5984.
$p2 = $c[5983..($count - 1)]

# Verification (Optional logging)
Write-Host "Part 1 ends with: $(($p1[-1]).Trim())"
Write-Host "Part 2 starts with: $(($p2[0]).Trim())"

# Save
Set-Content -Path $path -Value ($p1 + $p2) -Encoding UTF8
Write-Host "File patched successfully."
