#include <windows.h>
#include <shlobj.h>
#include <stdio.h>
#include "ReadFontName.h"
#include "libs\GetArgv.h"
#include "libs\SimpleString.h"
#include "version.h"

// Stuff to exclude from directory scans:
#define FILE_ATTRIBUTE_EXCLUDE ( FILE_ATTRIBUTE_DIRECTORY | \
                                 FILE_ATTRIBUTE_OFFLINE   | \
                                 FILE_ATTRIBUTE_HIDDEN    | \
                                 FILE_ATTRIBUTE_SYSTEM )

#define countof(x) (sizeof(x)/sizeof(x[0]))




typedef enum {
    OP_NONE,
    OP_COPY,
    OP_MOVE
} FILEOP, * PFILEOP;

typedef enum {
    MODE_USER,
    MODE_ADMIN
} MODE, * PMODE;


struct APP_OPTIONS {
    FILEOP op;
    MODE mode;
    TCHAR szFontsRoot[MAX_PATH];
};

struct APP_OPTIONS options;

// Main operations; function that are called only once should be inlined
__forceinline
UINT WINAPI ParseCommandline();
__forceinline
UINT WINAPI InstallFontFiles(HKEY hKeyFonts, PTSTR pszPath, PTSTR pszPathAppend);
__forceinline
VOID WINAPI RemoveOrphanRegs(HKEY hKeyFonts, PTSTR pszPath, PTSTR pszPathAppend);
__forceinline
VOID WINAPI RegisterAllFonts(HKEY hKeyFonts, PTSTR pszPath, PTSTR pszPathAppend);

// Helper functions
__forceinline
BOOL WINAPI CopyMoveFile(PCTSTR pszSource, PCTSTR pszDest, FILEOP op);
__forceinline
BOOL WINAPI IsInvalidType(FI_FONTTYPE fonttype, BOOL fIgnoreFON);
BOOL WINAPI RegisterFont(HKEY hKeyFonts, PCTSTR pszFileName, PCTSTR pszFontName);


BOOL IsElevated() {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

BOOL dirExists(PTSTR dirName_in)
{
    DWORD ftyp = GetFileAttributes(dirName_in);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return FALSE;  //something is wrong with your path!

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return TRUE;   // this is a directory!

    return FALSE;    // this is not a directory!
}



BOOL WINAPI GetUserFontFolder(PTSTR szFontsRoot)
{
    if (SHGetSpecialFolderPath(NULL, szFontsRoot, CSIDL_LOCAL_APPDATA, FALSE))
    {
        SSCat(szFontsRoot, TEXT("\\Microsoft\\Windows\\Fonts"));
        return TRUE;
    }
    else {
        return FALSE;
    }
}


#pragma comment(linker, "/entry:WinFontInstall")
VOID WINAPI WinFontInstall()
{
    UINT uExitCode = 1;
    HKEY hKeyFonts = NULL;
    HKEY hKey;

    uExitCode = ParseCommandline();
    if (uExitCode > 0)
    {
        ExitProcess(uExitCode);
    }

    if (options.mode == MODE_USER)
    {
        hKey = HKEY_CURRENT_USER;
    }
    else {
        hKey = HKEY_LOCAL_MACHINE;
    }

    if (RegOpenKeyEx(hKey,
        TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"),
        0,
        KEY_READ | KEY_WRITE,
        &hKeyFonts) == ERROR_SUCCESS)
    {
        TCHAR szFontsRoot[MAX_PATH];

        if (((options.mode == MODE_USER) && (GetUserFontFolder(szFontsRoot)))
            || ((options.mode == MODE_ADMIN) && (SHGetSpecialFolderPath(NULL, szFontsRoot, CSIDL_FONTS, FALSE))))
        {
            TCHAR pszPathAppend[MAX_PATH];
            SSCpy(pszPathAppend, szFontsRoot);
            SSCat(pszPathAppend, TEXT("\\"));

            uExitCode = InstallFontFiles(hKeyFonts, szFontsRoot, pszPathAppend);
            if (uExitCode == 0)
            {
                RemoveOrphanRegs(hKeyFonts, szFontsRoot, pszPathAppend);
                RegisterAllFonts(hKeyFonts, szFontsRoot, pszPathAppend);
            }
        }

        RegCloseKey(hKeyFonts);
    }

    ExitProcess(uExitCode);
}

VOID WINAPI PrintHelp()
{
    printf("**************************************************************\n");
    printf("* WinFontInstaller v1.0.1                                    *\n");
    printf("*                                                            *\n");
    printf("* Michael Mattsson (mats@ymail.com)                          *\n");
    printf("* https://github.com/mdmattsson/WinFontInstall               *\n");
    printf("*                                                            *\n");
    printf("* usage                                                      *\n");
    printf("*   WinFontInstaller [options] [-f] <fontpath>               *\n");
    printf("*                                                            *\n");
    printf("* params:                                                    *\n");
    printf("* -c,--copy   copies files to font folder.                   *\n");
    printf("* -m,--move   moves files to font folder.                    *\n");
    printf("* -u,--user   installs font to current users font folder.    *\n");
    printf("* -a,--all    installs font to windows font folder.          *\n");
    printf("* -f,--folder source folder of fonts to be installed reside. *\n");
    printf("**************************************************************\n");
    printf("\n");
}
/**
 * Main operations
 **/


UINT WINAPI ParseCommandline()
{
    UINT argc = 0;
    PTSTR* argv = GetArgv(&argc);
    TCHAR szSourceFontsRoot[MAX_PATH] = { 0 };
    options.op = OP_NONE;
    options.mode = MODE_ADMIN;

    for (UINT i = 1; i < argc; i++)
    {
        if ((lstrcmpi(argv[i], TEXT("/copy")) == 0) || (lstrcmpi(argv[i], TEXT("--copy")) == 0) || (lstrcmpi(argv[i], TEXT("/c")) == 0) || (lstrcmpi(argv[i], TEXT("-c")) == 0))
        {
            options.op = OP_COPY;
        }
        else if ((lstrcmpi(argv[i], TEXT("/move")) == 0) || (lstrcmpi(argv[i], TEXT("--move")) == 0) || (lstrcmpi(argv[i], TEXT("/m")) == 0) || (lstrcmpi(argv[i], TEXT("-m")) == 0))
        {
            options.op = OP_MOVE;
        }
        else if ((lstrcmpi(argv[i], TEXT("/user")) == 0) || (lstrcmpi(argv[i], TEXT("--user")) == 0) || (lstrcmpi(argv[i], TEXT("/u")) == 0) || (lstrcmpi(argv[i], TEXT("-u")) == 0))
        {
            options.mode = MODE_USER;
        }
        else if ((lstrcmpi(argv[i], TEXT("/all")) == 0) || (lstrcmpi(argv[i], TEXT("--all")) == 0) || (lstrcmpi(argv[i], TEXT("/a")) == 0) || (lstrcmpi(argv[i], TEXT("-a")) == 0))
        {
            options.mode = MODE_ADMIN;
        }
        else if ((lstrcmpi(argv[i], TEXT("/folder")) == 0) || (lstrcmpi(argv[i], TEXT("--folder")) == 0) || (lstrcmpi(argv[i], TEXT("/f")) == 0) || (lstrcmpi(argv[i], TEXT("-f")) == 0))
        {
            if (i + 1 <= argc) {
                SSCpy(szSourceFontsRoot, argv[++i]);
            }
            else {
                printf("Running in default Admin Mode without eleveated permissions.   Run as Admin, or use parameter --user to install fonts for user only.\n");
                PrintHelp();
                return 0;
            }
        }
        else if ((lstrcmpi(argv[i], TEXT("/help")) == 0) || (lstrcmpi(argv[i], TEXT("--help")) == 0) || (lstrcmpi(argv[i], TEXT("/h")) == 0) || (lstrcmpi(argv[i], TEXT("-h")) == 0))
        {
            PrintHelp();
            return 0;
        }
        else
        {
            if (dirExists(argv[i]))
            {
                SSCpy(szSourceFontsRoot, argv[i]);
            }
        }
    }

    LocalFree(argv);


    if (SSLen(szSourceFontsRoot) == 0) {
        SSCpy(szSourceFontsRoot, TEXT("."));
    }

    DWORD result = ExpandEnvironmentStrings(szSourceFontsRoot, options.szFontsRoot, sizeof(options.szFontsRoot) / sizeof(*options.szFontsRoot));
    if (!result)
    {
        int lastError = GetLastError();
        printf("Failed to expand environment strings in directory name. GetLastError=%d", 1, lastError);
        return 1;
    }
    
    
    
    //SSCpy(options.szFontsRoot, szSourceFontsRoot);
    SSCat(options.szFontsRoot, TEXT("\\"));

    if (options.mode == MODE_ADMIN && !IsElevated())
    {
        printf("Running in default Admin Mode without eleveated permissions.   Run as Admin, or use parameter --user to install fonts for user only.\n");
        PrintHelp();
        return 1;
    }
    return 0;
}



UINT WINAPI InstallFontFiles(HKEY hKeyFonts, PTSTR pszPath, PTSTR pszPathAppend)
{
    if (options.op != OP_NONE)
    {
        HANDLE hFind;
        WIN32_FIND_DATA finddata;
        TCHAR szSourceFontsWildcard[MAX_PATH] = { 0 };

        SSCpy(szSourceFontsWildcard, options.szFontsRoot);
        SSCat(szSourceFontsWildcard, TEXT("*"));

        if ((hFind = FindFirstFile(szSourceFontsWildcard, &finddata)) != INVALID_HANDLE_VALUE)
        {
            TCHAR szFontName[MAX_FONTNAME];
            BOOL fChanged = FALSE;

            do
            {
                SSChainCpyCat(pszPathAppend, options.szFontsRoot, finddata.cFileName);


                TCHAR destFontPath[MAX_PATH];
                SSCpy(destFontPath, pszPath);
                SSCat(destFontPath, TEXT("\\"));
                SSCat(destFontPath, finddata.cFileName);

                if (!(finddata.dwFileAttributes & FILE_ATTRIBUTE_EXCLUDE ||
                    ReadFontName(pszPathAppend, szFontName) == TYPE_NULL ||
                    CopyMoveFile(pszPathAppend, destFontPath, options.op) == FALSE ||
                    AddFontResource(destFontPath) == 0))
                {
                    if (RegisterFont(hKeyFonts, finddata.cFileName, szFontName))
                        fChanged = TRUE;
                }

            } while (FindNextFile(hFind, &finddata));

            FindClose(hFind);

            if (fChanged)
                PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
        }
    }
    return 0;
}

VOID WINAPI RemoveOrphanRegs(HKEY hKeyFonts, PTSTR pszPath, PTSTR pszPathAppend)
{
    TCHAR szName[0x400];
    TCHAR szFile[0x400];
    DWORD dwIndex = 0;
    DWORD dwType;
    DWORD cchName = countof(szName);
    DWORD cbFile = sizeof(szFile);

    while (RegEnumValue(hKeyFonts, dwIndex++, szName, &cchName, NULL, &dwType, (PBYTE)szFile, &cbFile) == ERROR_SUCCESS)
    {
        if (dwType == REG_SZ && cbFile > sizeof(TCHAR))
        {
            memcpy(pszPathAppend, szFile, cbFile);

            // In some cases, such as Equation Editor's "MT Extra" font, an
            // absolute path is given instead of a relative path, so we
            // need to check for that case as well.
            if (GetFileAttributes(pszPath) == INVALID_FILE_ATTRIBUTES &&
                GetFileAttributes(szFile) == INVALID_FILE_ATTRIBUTES)
            {
                // Font file does not exist, so delete it and reset the
                // index (modifying the registry can affect the indexing).
                if (RegDeleteValue(hKeyFonts, szName) == ERROR_SUCCESS)
                    dwIndex = 0;
            }
        }

        cchName = countof(szName);
        cbFile = sizeof(szFile);
    }
}

VOID WINAPI RegisterAllFonts(HKEY hKeyFonts, PTSTR pszPath, PTSTR pszPathAppend)
{
    HANDLE hFind;
    WIN32_FIND_DATA finddata;

    // If this is run at an early stage of Windows setup (prior to the
    // cmdlines.txt stage), the hidden attributes have not yet been set on
    // system fonts, which means that we must play it safe and ignore FON files
    // if this is the case.  Our litmus test: is marlett.ttf hidden?
    BOOL fIgnoreFON;
    SSStaticCpy(pszPathAppend, TEXT("marlett.ttf"));
    fIgnoreFON = !(GetFileAttributes(pszPath) & FILE_ATTRIBUTE_HIDDEN);

    SSCpy2Ch(pszPathAppend, TEXT('*'), 0);

    if ((hFind = FindFirstFile(pszPath, &finddata)) != INVALID_HANDLE_VALUE)
    {
        TCHAR szFontName[MAX_FONTNAME];
        BOOL fChanged = FALSE;

        do
        {

            if (options.mode == MODE_USER
                || (options.mode == MODE_ADMIN && (!(finddata.dwFileAttributes & FILE_ATTRIBUTE_EXCLUDE || lstrcmpi(finddata.cFileName, TEXT("marlett.ttf")) == 0))))

            {
                SSChainCpy(pszPathAppend, finddata.cFileName);

                if (!(IsInvalidType(ReadFontName(pszPath, szFontName), fIgnoreFON) ||
                    AddFontResource(pszPath) == 0))
                {
                    if (RegisterFont(hKeyFonts, finddata.cFileName, szFontName))
                        fChanged = TRUE;
                }
            }

        } while (FindNextFile(hFind, &finddata));

        FindClose(hFind);

        if (fChanged)
            PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    }
}

/**
 * Helper functions
 **/

BOOL WINAPI CopyMoveFile(PCTSTR pszSource, PCTSTR pszDest, FILEOP op)
{
    if (op == OP_COPY)
        return(CopyFile(pszSource, pszDest, FALSE));
    else
        return(MoveFileEx(pszSource, pszDest, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING));
}

BOOL WINAPI IsInvalidType(FI_FONTTYPE fonttype, BOOL fIgnoreFON)
{
    return(fonttype == TYPE_NULL || (fIgnoreFON && fonttype == TYPE_FON));
}

BOOL WINAPI RegisterFont(HKEY hKeyFonts, PCTSTR pszFileName, PCTSTR pszFontName)
{
    BOOL fFoundDupe = FALSE;
    TCHAR szName[0x400];
    TCHAR szData[0x400];
    DWORD dwIndex = 0;
    DWORD dwType;
    DWORD cchName = countof(szName);
    DWORD cbData = sizeof(szData);

    while (!fFoundDupe && RegEnumValue(hKeyFonts, dwIndex++, szName, &cchName, NULL, &dwType, (PBYTE)szData, &cbData) == ERROR_SUCCESS)
    {
        if (dwType == REG_SZ && cbData > sizeof(TCHAR))
        {
            if (lstrcmpi(szName, pszFontName) == 0 ||
                lstrcmpi(szData, pszFileName) == 0)
            {
                fFoundDupe = TRUE;
            }
        }

        cchName = countof(szName);
        cbData = sizeof(szData);
    }

    if (!fFoundDupe)
    {
        RegSetValueEx(
            hKeyFonts,
            pszFontName,
            0,
            REG_SZ,
            (PBYTE)pszFileName,
            ((DWORD)SSLen(pszFileName) + 1) * sizeof(TCHAR)
        );

        return(TRUE);
    }

    return(FALSE);
}
