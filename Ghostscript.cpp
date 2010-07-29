//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdexcept>
#include <sstream>
#pragma hdrstop

#include "Ghostscript.h"

//---------------------------------------------------------------------------
YM_NS_DEF(ghostscript)
//---------------------------------------------------------------------------
YM_NS_SCOPE(types)GhostscriptCallback_t Ghostscript::stdinCallback  = NULL;
YM_NS_SCOPE(types)GhostscriptCallback_t Ghostscript::stdoutCallback = NULL;
YM_NS_SCOPE(types)GhostscriptCallback_t Ghostscript::deviceCallback = NULL;
YM_NS_SCOPE(types)GhostscriptCallback_t Ghostscript::syncCallback   = NULL;
YM_NS_SCOPE(types)GhostscriptCallback_t Ghostscript::pageCallback   = NULL;
YM_NS_SCOPE(types)GhostscriptCallback_t Ghostscript::sizeCallback   = NULL;
YM_NS_SCOPE(types)GhostscriptCallback_t Ghostscript::pollCallback   = NULL;
//---------------------------------------------------------------------------
Ghostscript::Ghostscript()
{
        init();
}
//---------------------------------------------------------------------------
Ghostscript::~Ghostscript()
{
        dealloc();
}
//---------------------------------------------------------------------------
void Ghostscript::init()
{
        dealloc();
        loadGhostscriptDll();
}
//---------------------------------------------------------------------------
void Ghostscript::dealloc()
{
        if (_dll) ::FreeLibrary(_dll);
        _dll = 0;

        if (_ghostscriptPath) free(_ghostscriptPath);
        _ghostscriptPath = 0;

        if (_ghostscriptInclude) free(_ghostscriptInclude);
        _ghostscriptInclude = 0;

        gsdll_revision = NULL;
        gsdll_init = NULL;
        gsdll_execute_begin = NULL;
        gsdll_execute_cont = NULL;
        gsdll_execute_end = NULL;
        gsdll_lock_device = NULL;
        gsdll_exit = NULL;

        gsdll_copy_dib = 0;
        gsdll_copy_palette = 0;
        gsdll_draw = 0;
        gsdll_get_bitmap_row = 0;
}
//---------------------------------------------------------------------------
// Reset the object.  This is helpful if the dll encounters bad postscript and does not recover
void Ghostscript::resetGhostscript()
{
        dealloc();
        init();
}
//---------------------------------------------------------------------------
void Ghostscript::loadGhostscriptDll()
{
        readGhostscriptRegistry();

        _dll = ::LoadLibrary(_ghostscriptPath);
        if (!_dll)
                throw std::runtime_error(std::string("Cannot load ghostscript dll from ") + std::string(_ghostscriptPath));
        loadGhostscriptExports();
}
//---------------------------------------------------------------------------
void Ghostscript::loadGhostscriptExports()
{
        gsdll_revision       = (int (__stdcall*)(char**, char**, long*, long*))::GetProcAddress(_dll, "gsdll_revision");
        gsdll_init           = (int (__stdcall*)(int (*)(int,char *,unsigned long),void *,int,char * *))::GetProcAddress(_dll, "gsdll_init");
        gsdll_execute_begin  = (int (__stdcall*)(void))::GetProcAddress(_dll, "gsdll_execute_begin");
        gsdll_execute_cont   = (int (__stdcall*)(const char *,int))::GetProcAddress(_dll, "gsdll_execute_cont");
        gsdll_execute_end    = (int (__stdcall*)(void))::GetProcAddress(_dll, "gsdll_execute_end");
        gsdll_lock_device    = (int (__stdcall*)(unsigned char*, int))::GetProcAddress(_dll, "gsdll_lock_device");
        gsdll_exit           = (int (__stdcall*)(void))::GetProcAddress(_dll, "gsdll_exit");
        gsdll_copy_dib       = (HGLOBAL (__stdcall*)(unsigned char*))::GetProcAddress(_dll, "gsdll_copy_dib");
        gsdll_copy_palette   = (HPALETTE (__stdcall*)(unsigned char*))::GetProcAddress(_dll, "gsdll_copy_palette");
        gsdll_draw           = (void (__stdcall*)(unsigned char*,HDC,RECT*,RECT*))::GetProcAddress(_dll, "gsdll_draw");
        gsdll_get_bitmap_row = (int (__stdcall *)(unsigned char*,BITMAPINFOHEADER*,RGBQUAD*,unsigned char**,unsigned int))::GetProcAddress(_dll, "gsdll_get_bitmap_row");

        if (!gsdll_revision
                        || !gsdll_init
                        || !gsdll_execute_begin
                        || !gsdll_execute_cont
                        || !gsdll_execute_end
                        || !gsdll_lock_device
                        || !gsdll_exit
                        || !gsdll_copy_dib
                        || !gsdll_copy_palette
                        || !gsdll_draw
                        || !gsdll_get_bitmap_row) {
                throw std::runtime_error("One or more exports could not be resolved in gsdll32.dll");
        }
}
void Ghostscript::readGhostscriptRegistry()
{
        char buffer[1024];
        unsigned long l1, l2;
        HKEY key = getGhostscriptRegistryKey();

        l2 = 1024;
        l1 = ::RegQueryValueEx(key, "GS_DLL", NULL, NULL, buffer, &l2);
        if (l1 != ERROR_SUCCESS)
                throw std::runtime_error("Cannot read GS_DLL value from registry.");
        _ghostscriptPath = strdup(buffer);

        l2 = 1024;
        l1 = ::RegQueryValueEx(key, "GS_LIB", NULL, NULL, buffer, &l2);
        if (l1 != ERROR_SUCCESS)
                throw std::runtime_error("Cannot read GS_LIB value from registry.");
        _ghostscriptInclude = strdup(buffer);

        ::RegCloseKey(key);

}
//---------------------------------------------------------------------------
HKEY Ghostscript::getGhostscriptRegistryKey() const
{
        HKEY key;
        LONG l1, l2;
        char buff1[MAX_PATH], buff2[MAX_PATH];
        unsigned long dd;

        l1 = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, GHOSTSCRIPT_REGISTRY_PATH
                            , 0UL, KEY_READ, &key);
        if (l1 != ERROR_SUCCESS)
                throw std::runtime_error("Cannot open Ghostscript Registry Key.");

        for (l1=0, l2 = ERROR_SUCCESS;l2==ERROR_SUCCESS;l1++) {
                FILETIME ft;
                char version[MAX_PATH];
                dd = MAX_PATH;
                l2 = ::RegEnumKeyEx(key, l1, version, &dd, NULL, NULL, NULL, &ft);
                if (l2 == ERROR_SUCCESS)
                        strcpy(buff1, version);
        }
        ::RegCloseKey(key);

        if (!buff1[0])
                throw std::runtime_error("Cannot locate ghostscript installation in registry.");

        sprintf(buff2, "%s\\%s", GHOSTSCRIPT_REGISTRY_PATH, buff1);

        l1 = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, buff2, 0UL, KEY_READ, &key);
        if (l1 != ERROR_SUCCESS)
                throw std::runtime_error("Cannot open ghostscript registry key.");
        return key;
}
//---------------------------------------------------------------------------
const char* Ghostscript::getDllPath() const
{
        return _ghostscriptPath;
}
//---------------------------------------------------------------------------
const char* Ghostscript::getIncludePath() const
{
        return _ghostscriptInclude;
}
//---------------------------------------------------------------------------
// The default callback
int gsdll_callback(int message, char *str, unsigned long count)
{
        switch (message) {
        case YM_NS_SCOPE(types)GSDLL_STDIN:
                if (Ghostscript::stdinCallback) {
                        return (*Ghostscript::stdinCallback)(message, str, count);
                } else {
                        char *p = fgets(str, count, stdin);
                        if (p)
                                return strlen(str);
                        else
                                return 0;
                }
        case YM_NS_SCOPE(types)GSDLL_STDOUT:
                if (Ghostscript::stdoutCallback) {
                        return (*Ghostscript::stdoutCallback)(message, str, count);
                } else {
                        if (str != (char *)NULL)
                                fwrite(str, 1, count, stdout);
                        return count;
                }
        case YM_NS_SCOPE(types)GSDLL_DEVICE:
                if (Ghostscript::deviceCallback) {
                        return (*Ghostscript::deviceCallback)(message, str, count);
                }
                break;
        case YM_NS_SCOPE(types)GSDLL_SYNC:
                if (Ghostscript::syncCallback) {
                        return (*Ghostscript::syncCallback)(message, str, count);
                }
                break;
        case YM_NS_SCOPE(types)GSDLL_PAGE:
                if (Ghostscript::pageCallback) {
                        return (*Ghostscript::pageCallback)(message, str, count);
                }
                break;
        case YM_NS_SCOPE(types)GSDLL_SIZE:
                if (Ghostscript::sizeCallback) {
                        return (*Ghostscript::sizeCallback)(message, str, count);
                }
                break;
        case YM_NS_SCOPE(types)GSDLL_POLL:
                if (Ghostscript::pollCallback) {
                        return (*Ghostscript::pollCallback)(message, str, count);
                }
                break;
        }
        return (0);
}
//---------------------------------------------------------------------------
YM_NS_END
