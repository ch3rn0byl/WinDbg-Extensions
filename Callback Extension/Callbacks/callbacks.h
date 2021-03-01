// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the DLL11_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// DLL11_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef CALLBACK_EXPORTS
#define CALLBACK_API __declspec(dllexport)
#else
#define CALLBACK_API __declspec(dllimport)
#endif

typedef IDebugClient* PIDebugClient;
typedef IDebugControl* PIDebugControl;
typedef IDebugSymbols* PIDebugSymbols;

HRESULT init();
HRESULT enumerateLoadImageRoutines();
HRESULT enumerateCreateThreadRoutines();
HRESULT enumerateCreateProcessRoutines();
void cleanUpAndExit();

EXTERN_C CALLBACK_API HRESULT CALLBACK callbacks(
	_In_ PIDebugClient,
	_In_ PCSTR args
);


/// EOF