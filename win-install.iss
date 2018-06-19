[Setup]
AppName=A3DC
AppVersion=0.21
DefaultDirName={localappdata}\A3DC
DefaultGroupName=A3DC
Compression=lzma2
;Compression=none
SolidCompression=yes
OutputDir=.
OutputBaseFilename=a3dc-setup-0.21
Uninstallable=yes
CreateUninstallRegKey=yes

[Files]
Source: "build\src\app\release\vcredist.msi"; DestDir: "{app}"; AfterInstall: RunVCRedistInstaller
Source: "build\src\app\release\*"; Excludes: "*.obj,*.pdb,*.ilk,*.h,*.cpp,*.c,*.hpp,*.ipp,*.cxx,*.hxx,__pycache__,*.DS_store,Thumbs.db"; DestDir: "{app}"; Flags: recursesubdirs

[Icons]
Name: "{group}\A3DC"; Filename: "{app}\app-starter.bat"

[Code]
procedure RunVCRedistInstaller;
var
  ResultCode: Integer;
begin
  if not Exec(ExpandConstant('{app}\vcredist.msi'), '', '', SW_SHOWNORMAL,
    ewWaitUntilTerminated, ResultCode)
  then
    MsgBox('vcredist.msi installer failed to run!' + #13#10 +
      SysErrorMessage(ResultCode), mbError, MB_OK);
end;