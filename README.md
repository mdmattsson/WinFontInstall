# WinFontInstaller    
Windows commandline utility that will install fonts.  
                                                              
 Michael Mattsson (mats@ymail.com)                             
 https://github.com/mdmattsson/WinFontInstall                 

 Find the latest release here: https://github.com/mdmattsson/WinFontInstall/releases/latest
 
 
 **Usage:**                                                        
   WinFontInstaller [options] [-f] <fontpath>                 
                                                              
 **Params:**                                                      
 -c,--copy   copies files to font folder.                     
 -m,--move   moves files to font folder.                      
 -u,--user   installs font to current users font folder.      
 -a,--all    installs font to windows font folder.            
 -f,--folder source folder of fonts to be installed reside. 
**************************************************************  
	
	
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

WinFontInstall comes in 2 install modes
  * **WinFontInstall /all**  requires admin privileges and installs the font(s)
	to the main Windows Font folder for all users (C:\Windows\Fonts)
  * **WinFontInstall /user** does NOT require admin privileges and installed the font(s)
	will be for the current user only (C:\Users\<username>\AppData\Local\Microsoft\Windows\Fonts).  
	This is handy when running the application from a login script (I use it in my Windows git bashrc file)



WinFontInstall.exe is intended as a replacement for Microsoft's outdated fontinst.exe,
and like fontinst.exe, WinFontInstall.exe is fully silent--it will not print messages,
pop up dialogs, etc.; the process exit code will be 0 if there was no error.


