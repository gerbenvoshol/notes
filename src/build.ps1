param (
  [int]$Compile = 1,
  [String]$FileToOpen = "",
  [int]$RemoveRemote = 0,
  [int]$RemoveLocal = 0
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

if ($Compile -ne 0) {
  Write-Output "Compiling tool..."

  clang notes.c md_renderer.c md4c/md4c.c helpers.c -o notes.exe -Wno-deprecated-declarations
}

Write-Output "Building notes..."

if ($RemoveLocal -ne 0) {
  Remove-Item "../local/*.html"
  Remove-Item "../local/tags/*.html"
}
if ($RemoveRemote -ne 0) {
  Remove-Item "../remote/*.html"
  Remove-Item "../remote/tags/*.html"
}

.\notes.exe $notes

Write-Output "Done"
Write-Output "========================="

if (!($FileToOpen -eq "")) {
  Invoke-Item $FileToOpen
}
