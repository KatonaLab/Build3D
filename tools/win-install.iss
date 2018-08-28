#ifndef APP_VERSION
	#define APP_VERSION "0"
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
DefaultDirName={pf}\A3-DC
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
Source: "..\assets\icons\favicon.ico"; DestDir: "{app}"

[Icons]
Name: "{group}\A3-DC"; Filename: "{app}\a3-dc.bat"; IconFilename: "{app}\favicon.ico"; Flags: runminimized
Name: "{group}\Uninstall A3-DC"; Filename: "{uninstallexe}"

[Code]
procedure RunVCRedistInstaller;
var
  ResultCode: Integer;
begin
  if not Exec(ExpandConstant('{app}\vcredist_x64.exe'), '/passive /install', '', SW_SHOWNORMAL,
    ewWaitUntilTerminated, ResultCode)
  then
    MsgBox('vcredist_x64.exe installer failed to run!' + #13#10 +
      SysErrorMessage(ResultCode), mbError, MB_OK);
end;

procedure InitializeWizard();
var
  VersionLabel: TNewStaticText;
begin
  VersionLabel := TNewStaticText.Create(WizardForm);
  VersionLabel.Caption := Format('Version: %s', ['{#SetupSetting("AppVersion")}']);
  VersionLabel.Parent := WizardForm;
  VersionLabel.Left := ScaleX(16);
  VersionLabel.Top :=
    WizardForm.BackButton.Top +
    (WizardForm.BackButton.Height div 2) -
    (VersionLabel.Height div 2)
end;