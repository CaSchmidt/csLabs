Option Explicit

Dim i
For i = 0 to WScript.Arguments.Count - 1
  WScript.Echo WScript.Arguments(i)
Next

Dim wshShell
Set wshShell = CreateObject("WScript.Shell")

Dim envVar
envVar = wshShell.ExpandEnvironmentStrings("%CSMENU_ENV_TEST%")
If envVar = "%CSMENU_ENV_TEST%" Then
  WScript.Echo "CSMENU_ENV_TEST not set!"
Else
  WScript.Echo envVar
End If

Set wshShell = Nothing
