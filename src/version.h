// Full name; this will appear in the version info    
#if defined(_M_IX86)    
#define WINFONTINSTALLER_NAME_STR "Windows Font Installation Utility (x32)"    
#elif defined(_M_AMD64) || defined(_M_X64)    
#define WINFONTINSTALLER_NAME_STR "Windows Font Installation Utility (x64)"    
#elif defined(_M_IA64)    
#define WINFONTINSTALLER_NAME_STR "Windows Font Installation Utility (IA-64)"    
#else    
#define WINFONTINSTALLER_NAME_STR "Windows Font Installation Utility"   
#endif   

// Full version: MUST be in the form of major,minor,revision,build  
#define WINFONTINSTALLER_VERSION_FULL 1,0,1,0  
// String version: May be any suitable string  
#define WINFONTINSTALLER_VERSION_STR "1.0.1.0"  
// PE version: MUST be in the form of major.minor  
#pragma comment(linker, "/version:1.0")  
