# This script will remove the Music, Documents, 3D Objects, etc...
# folders under "This PC" in the Windows Explorer sidebar.

# based on Keith A. Miller's reply here:
# https://answers.microsoft.com/en-us/windows/forum/windows_10-files/how-do-i-deleteremove-the-default-libraries-in/a75220b6-83ab-4b54-bcc8-7165af6761cf

# This will edit the registry!
# It is a good idea to back up the registry using 
# regedit's "Export" function before running this script.

# Restart your computer after running.
# Last tested on Windows 10 Pro version 1903

$key = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\MyComputer\NameSpace"
$key | gci -exclude "Delegate*" | ri -force
