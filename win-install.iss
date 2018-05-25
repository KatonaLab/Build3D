[Setup]
AppName=A3DC
AppVersion=0.2
DefaultDirName={localappdata}\A3DC
DefaultGroupName=A3DC
Compression=lzma2
;Compression=none
SolidCompression=yes
OutputDir=.
OutputBaseFilename=a3dc-setup-0.2
Uninstallable=yes
CreateUninstallRegKey=yes

[Files]
Source: "build\src\app\release\*"; Excludes: "*.obj,*.pdb,*.ilk,*.h,*.cpp,*.c,*.hpp,*.ipp,*.cxx,*.hxx,__pycache__,*.DS_store,Thumbs.db"; DestDir: "{app}"; Flags: recursesubdirs

[Icons]
Name: "{group}\A3DC"; Filename: "{app}\app-starter.bat"