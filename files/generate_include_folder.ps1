# generate_include_folder.ps1
# place in a subfolder of the repo-dir, and it will copy .h or .inc files
# from '(repo-dir)\$SourceDir' to '(repo-parent-dir)\include\$LibraryName'
# while maintaining the directory structure
#
# Example parameters: 
# $LibraryName = "abseil-cpp"
# $SourceDir = "lib\abseil-cpp"
#
# Example call: 
# .\generate_include_folder.ps1 "abseil-cpp" "lib\abseil-cpp"

param (
	[String]$LibraryName = $(throw "LibraryName parameter is required"),
	[String]$SourceDir = $(throw "SourceDir parameter is required")
)

$RepoDir = Join-Path (Split-Path (Get-Variable MyInvocation).Value.MyCommand.Path) ".." -Resolve
$SourcePath = Join-Path $RepoDir $SourceDir -Resolve

# Create the destination directory
if (!(Test-Path "$RepoDir\..\include")) {
	New-Item -Path "$RepoDir\.." -Name "include" -ItemType "directory" -Confirm
}
if (!(Test-Path "$RepoDir\..\include\$LibraryName")) {
	New-Item -Path "$RepoDir\..\include" -Name "$LibraryName" -ItemType "directory" -Confirm
}
if (!(Test-Path "$RepoDir\..\include\$LibraryName")) {
  Write-Info "Without a destination directory, nothing can be copied. Exiting."
	return 0;
}

$DestinationPath = Join-Path $RepoDir "..\include\$LibraryName" -Resolve

# Copy the entire source code
Copy-Item "$SourcePath\*" -Destination $DestinationPath -Recurse

# Remove anything that isn't a header (or directory)
Get-ChildItem $DestinationPath -Recurse -Attributes !Directory | ForEach-Object {
  if (!($_.Extension | Select-String '^.*.[hi][np]?[cp]?$')) {
    Remove-Item $_.FullName
  }
}
