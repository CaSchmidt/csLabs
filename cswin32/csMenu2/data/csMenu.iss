#define AppName     "CS::Menu"
#define AppVersion  "1.2"

[Setup]
AppName={#AppName}
AppVersion={#AppVersion}
OutputBaseFilename=csMenu-{#AppVersion}-x64-Setup
DefaultDirName={pf}\csLabs\csMenu
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

[Components]
Name: "shellext"; Description: "CS::Menu shell extension"; Types: full; Flags: fixed
Name: "examples"; Description: "Script examples"; Types: full

[Registry]
Root: HKCU; Subkey: "Software\csLabs\csMenu"; ValueType: dword; ValueName: "Flags"; ValueData: 0; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\csLabs\csMenu"; ValueType: string; ValueName: "Scripts"; ValueData: "{localappdata}\csLabs\csMenu\Scripts"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "Software\csLabs\csMenu"; ValueType: string; ValueName: "Parallel"; ValueData: "{app}\csParallel-x64.exe"; Flags: uninsdeletevalue
Root: HKCU; Subkey: "Software\csLabs\csMenu"; ValueType: dword; ValueName: "ParallelCount"; ValueData: 2; Flags: createvalueifdoesntexist

[Files]
Source: "..\csMenu-x64.dll"; DestDir: "{app}"; Components: shellext; Flags: regserver uninsrestartdelete
Source: "..\csCore2-x64.dll"; DestDir: "{app}"; Components: shellext; Flags: uninsrestartdelete
Source: "..\csParallel-x64.exe"; DestDir: "{app}"; Components: shellext
Source: "..\MSVC*.DLL"; DestDir: "{app}"; Components: shellext; Flags: uninsrestartdelete
Source: "..\Scripts\example.*"; DestDir: {localappdata}\csLabs\csMenu\Scripts; Components: examples
