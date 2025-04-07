[Setup]
; Basic setup information
AppName=LoremScriptumCompiler
AppVersion=0.1
DefaultDirName={pf64}\LoremScriptumCompiler
DefaultGroupName=LoremScriptumCompiler
OutputDir=output
OutputBaseFilename=lsc-Installer-Windows-X64

[Files]
; Add files to install
Source: ".\lsc.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\*.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs

[Icons]
; Create Start Menu icon
Name: "{group}\lsc"; Filename: "{app}\lsc.exe"

[Tasks]
Name: envPath; Description: "Add to PATH variable" 

; https://stackoverflow.com/q/3304463
[Code]
function NeedsAddPath(): boolean;
var
  OrigPath: string;
begin
  if not IsTaskSelected('envPath')
  then begin
    Result := False;
    exit;
  end;
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', OrigPath)
  then begin
    Result := True;
    exit;
  end;
  { look for the path with leading and trailing semicolon }
  { Pos() returns 0 if not found }
  Result :=
    (Pos(';' + UpperCase(ExpandConstant('{app}')) + ';', ';' + UpperCase(OrigPath) + ';') = 0) and
    (Pos(';' + UpperCase(ExpandConstant('{app}')) + '\;', ';' + UpperCase(OrigPath) + ';') = 0); 
end;

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}"; Check: NeedsAddPath()