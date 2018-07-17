#ifndef APP_VERSION
	#define APP_VERSION "042"
#endif

#ifndef BUILD_ID
	#define BUILD_ID "0"
#endif

#ifndef BUILD_MODE
	#define BUILD_MODE "release"
#endif

[Setup]
AppName=A3-DC
AppVersion={#APP_VERSION}
DefaultDirName={localappdata}\A3-DC
DefaultGroupName=A3-DC
Compression=lzma2
;Compression=none
SolidCompression=yes
OutputDir=.
OutputBaseFilename=a3-dc-setup-{#APP_VERSION}-{#BUILD_ID}
Uninstallable=yes
CreateUninstallRegKey=yes

[Files]
Source: "..\build\src\app\{#BUILD_MODE}\vcredist_x64.exe"; DestDir: "{app}"; AfterInstall: RunVCRedistInstaller
Source: "..\build\src\app\{#BUILD_MODE}\*"; Excludes: "*.obj,*.pdb,*.ilk,*.h,*.cpp,*.c,*.hpp,*.ipp,*.cxx,*.hxx,__pycache__,*.DS_store,Thumbs.db"; DestDir: "{app}"; Flags: recursesubdirs

[Icons]
Name: "{group}\A3-DC"; Filename: "{app}\app-starter.bat"

[Code]
procedure RunVCRedistInstaller;
var
  ResultCode: Integer;
begin
  if not Exec(ExpandConstant('{app}\vc_redist.x64.exe'), '', '', SW_SHOWNORMAL,
    ewWaitUntilTerminated, ResultCode)
  then
    MsgBox('vcredist.msi installer failed to run!' + #13#10 +
      SysErrorMessage(ResultCode), mbError, MB_OK);
end;