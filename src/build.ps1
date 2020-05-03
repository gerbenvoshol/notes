param (
  [String]$FileToOpen = ""
)

Write-Output "========================="
Write-Output "Gathering notes..."

$notes = New-Object -TypeName System.Collections.Generic.List[string]

Get-ChildItem "../notes/" -Recurse -Include "*.md" | 
  Sort-Object -Property LastWriteTime -Descending | ForEach-Object {
  $note = "../notes/$(Split-Path -LeafBase $_.Name).md"

  $notes.Add($note)
}

Write-Output "Notes: $($notes.Count)"

$notes = $notes.ToArray()

Write-Output "Compiling tool..."

clang notes.c md_renderer.c md4c/md4c.c helpers.c -o notes.exe -Wno-deprecated-declarations

Write-Output "Building notes..."

Remove-Item "../remote/*.html"
Remove-Item "../remote/tags/*.html"
Remove-Item "../local/*.html"
Remove-Item "../local/tags/*.html"

.\notes.exe $notes

Write-Output "Done"
Write-Output "========================="

if (!($FileToOpen -eq "")) {
  Invoke-Item $FileToOpen
}
