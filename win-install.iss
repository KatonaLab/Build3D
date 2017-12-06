[Setup]
AppName=A3DC
AppVersion=0.11
DefaultDirName={localappdata}\A3DC
DefaultGroupName=A3DC
UninstallDisplayIcon={app}\a3dc.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.
OutputBaseFilename=a3dc-setup-011

[Files]
Source: "install\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\A3DC"; Filename: "{app}\a3dc.exe"