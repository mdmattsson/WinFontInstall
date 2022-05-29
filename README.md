# WinFontInstall
Windows commandline utility that will install fonts.  

If you...

...run WinFontInstall.exe without any command-line switches:

  * WinFontInstall will remove any stale font registrations in the registry.
  * WinFontInstall will repair any missing font registrations for fonts located in
    the C:\Windows\Fonts directory (this step will be skipped for .fon fonts if
    WinFontInstall cannot determine which fonts should have "hidden" registrations).

...run WinFontInstall.exe with the /copy or /move switch:

  * WinFontInstall will install all files with a .fon, .ttf, .ttc, or .otf file
    extension located in the CURRENT DIRECTORY (which might not necessarily be
    the directory in which WinFontInstall is located).  Installation will entail
    copying/moving the files to C:\Windows\Fonts and then registering the fonts.
  * WinFontInstall will remove any stale font registrations in the registry.
  * WinFontInstall will repair any missing font registrations for fonts located in
    the C:\Windows\Fonts directory (this step will be skipped for .fon fonts if
    WinFontInstall cannot determine which fonts should have "hidden" registrations).

WinFontInstall comes in 2 flavors
  * WinFontInstall.exe which requires admin privileges and installed the font(s)
    for all users on the system
  * WinFontInstallUser.exe does NOT require admin privileges and installed the font(s)
    will be for the current user only.  This is handy when running the application
	from a login script (I use it in my Windows git bashrc file)



WinFontInstall.exe is intended as a replacement for Microsoft's outdated fontinst.exe,
and like fontinst.exe, WinFontInstall.exe is fully silent--it will not print messages,
pop up dialogs, etc.; the process exit code will be 0 if there was no error.


