[Setup]
AppName=LoneShooter
AppVersion=1.0
DefaultDirName={autopf}\LoneShooter
DefaultGroupName=LoneShooter
UninstallDisplayIcon={app}\LoneShooter.exe
Compression=lzma2
SolidCompression=yes
OutputDir=Output
OutputBaseFilename=LoneShooter_Setup
SetupIconFile=logos\ls-logo.ico
ChangesEnvironment=yes

[Files]
Source: "bin\LoneShooter.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "bin\assets\*"; DestDir: "{app}\assets"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\LoneShooter"; Filename: "{app}\LoneShooter.exe"; IconFilename: "{app}\LoneShooter.exe"
Name: "{commondesktop}\LoneShooter"; Filename: "{app}\LoneShooter.exe"; IconFilename: "{app}\LoneShooter.exe"; Tasks: desktopicon
Name: "{group}\Uninstall LoneShooter"; Filename: "{uninstallexe}"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional icons:"; Flags: unchecked
Name: "addtopath"; Description: "Add LoneShooter to system PATH"; GroupDescription: "Additional tasks:"; Flags: unchecked

[Registry]
Root: HKLM; Subkey: "System\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}"; Tasks: addtopath; Check: NeedsAddPath(ExpandConstant('{app}'))

[Code]
function NeedsAddPath(Param: string): boolean;
var
  OrigPath: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'System\CurrentControlSet\Control\Session Manager\Environment', 'Path', OrigPath)
  then begin
    Result := True;
    exit;
  end;
  Result := Pos(';' + Param + ';', ';' + OrigPath + ';') = 0;
end;
