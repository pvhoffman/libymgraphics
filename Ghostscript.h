#ifndef LIBYMGRAPHICS_GHOSTSCRIPT_H
#define LIBYMGRAPHICS_GHOSTSCRIPT_H (1)
//---------------------------------------------------------------------------
#include <stdio.h>
#include "ymgraphics.h"

#define GHOSTSCRIPT_REGISTRY_PATH ("SOFTWARE\\GPL Ghostscript")

//---------------------------------------------------------------------------
YM_NS_DEF(ghostscript)

//---------------------------------------------------------------------------
// A default callback if the caller doesn't specify one
int gsdll_callback(int message, char *str, unsigned long count);
//---------------------------------------------------------------------------
class Ghostscript
{
private:
  HINSTANCE _dll;
  char* _ghostscriptPath;
  char* _ghostscriptInclude;

  void init();
  void dealloc();

  void loadGhostscriptDll();
  void readGhostscriptRegistry();
  HKEY getGhostscriptRegistryKey() const;
  void loadGhostscriptExports();
public:
  // Platform independant exports
  int (__stdcall *gsdll_revision)(char **product, char **copyright, long *gs_revision, long *gs_revisiondate);
  int (__stdcall *gsdll_init)( int (*gsdll_callback)(int message, char* str, unsigned long count), HWND hwnd, int argc, char *argv[]);
  int (__stdcall *gsdll_execute_begin)(void);
  int (__stdcall *gsdll_execute_cont)(const char *str, int len);
  int (__stdcall *gsdll_execute_end)(void);
  int (__stdcall *gsdll_lock_device)(unsigned char *device, int flag);
  int (__stdcall *gsdll_exit)(void);

  // Windows specific exports
  HGLOBAL (__stdcall *gsdll_copy_dib)(unsigned char *device);
  HPALETTE (__stdcall *gsdll_copy_palette)(unsigned char *device);
  void (__stdcall *gsdll_draw)(unsigned char *device, HDC hdc, LPRECT dest, LPRECT src);
  int (__stdcall *gsdll_get_bitmap_row)(unsigned char *device, LPBITMAPINFOHEADER pbmih, LPRGBQUAD prgbquad, LPBYTE *ppbyte, unsigned int row);


  // Static callbacks in response to ghostscript messages
  static YM_NS_SCOPE(types)GhostscriptCallback_t stdinCallback;
  static YM_NS_SCOPE(types)GhostscriptCallback_t stdoutCallback;
  static YM_NS_SCOPE(types)GhostscriptCallback_t deviceCallback;
  static YM_NS_SCOPE(types)GhostscriptCallback_t syncCallback;
  static YM_NS_SCOPE(types)GhostscriptCallback_t pageCallback;
  static YM_NS_SCOPE(types)GhostscriptCallback_t sizeCallback;
  static YM_NS_SCOPE(types)GhostscriptCallback_t pollCallback;

  Ghostscript();
  ~Ghostscript();

  void resetGhostscript();

  const char* getDllPath() const;
  const char* getIncludePath() const;
};
YM_NS_END
//---------------------------------------------------------------------------
#endif /*LIBYMGRAPHICS_GHOSTSCRIPT_H*/
