/* Link with: (kernel32.lib) user32.lib shell32.lib */
#include <errno.h>
#include <stdio.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#ifdef GUI
#include <shellapi.h>
#endif
#include "winentry.h"
#ifdef __cplusplus
extern "C" {
#endif

struct wmain_args_type wmain_args;

static LPCWSTR wmain_noargv = L"Error: unable to obtain program arguments.";
static LPCWSTR wmain_noargv_title = L"Error";

static LPCWSTR wmain_nocon = L"Error: unable to initialize console.";
static LPCWSTR wmain_nocon_title = L"Error";

/* Default heap of the process */
static HANDLE heap;

/* Argument vector (UTF-8) */
static char **argv;

/* Original codepage of the console */
static UINT prev_console_cp, prev_console_output_cp;

/* Special argument for dyn_AttachConsole to indicate that the parent process
   is to be used. */
static const DWORD dyn_ATTACH_PARENT_PROCESS = (DWORD) -1;

/* Attempts to attach to the console of the given process.  If it succeeds, a
   nonzero value is returned; otherwise, zero is returned. */
static BOOL dyn_AttachConsole(DWORD dwProcessId) {
    /* AttachConsole is not available on Win2k so it's dynamically called */
    typedef BOOL (WINAPI *func_type)(DWORD dwProcessId);
    func_type func;
    HMODULE lib = GetModuleHandleW(L"kernel32.dll");
    if (!lib)
        return 0;
    func = (func_type) GetProcAddress(lib, "AttachConsole");
    return func && func(dwProcessId);
}

/* Attaches to parent console or (if 'force' is nonzero) creates one if it
   does not exist.  Returns zero if a console was successfully attached or
   created; nonzero otherwise. */
static int console_show(int force) {
    int handle;
    FILE *file;
    if (!dyn_AttachConsole(dyn_ATTACH_PARENT_PROCESS)) {
        if (force) {
            if (!AllocConsole())
                return (int) GetLastError();
        } else {
            return -1;
        }
    }
    /* Link stdin */
    handle = _open_osfhandle((intptr_t) GetStdHandle(STD_INPUT_HANDLE),
                             _O_RDONLY);
    if (handle == -1)
        return -1;
    file = _fdopen(handle, "r");
    if (!file)
        return errno;
    *stdin = *file;
    /* Link stdout */
    handle = _open_osfhandle((intptr_t) GetStdHandle(STD_OUTPUT_HANDLE), 0);
    if (handle == -1)
        return -1;
    file = _fdopen(handle, "w");
    if (!file)
        return errno;
    *stdout = *file;
    /* Link stderr */
    handle = _open_osfhandle((intptr_t) GetStdHandle(STD_ERROR_HANDLE), 0);
    if (handle == -1)
        return -1;
    file = _fdopen(handle, "w");
    if (!file)
        return errno;
    *stderr = *file;
    return 0;
}

/* Initializes the console.  Returns zero on success. */
static void console_init() {
    /* If there is no console, try to attach to the parent console. */
    if (!GetConsoleWindow())
        console_show(0);

    /* Set console codepage to UTF-8 */
    prev_console_cp = GetConsoleCP();
    prev_console_output_cp = GetConsoleOutputCP();
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
}

/* Deinitializes the console */
static void console_deinit() {
    /* Restore the console codepage */
    SetConsoleCP(prev_console_cp);
    SetConsoleOutputCP(prev_console_output_cp);
}

/* Creates an argument vector in UTF-8 by converting from UTF-16. */
static int argv_create(int argc) {
    wchar_t **argvw = wmain_args.argv;
    int i, *lens, total_len = 0, argn = argc + 1;

    /* Store the UTF-8 lengths in an array */
    lens = (int *) HeapAlloc(heap, 0, argc * sizeof(*lens));
    if (!lens)
        return (int) GetLastError();

    /* Query the lengths required */
    for (i = 0; i != argc; ++i) {
        int len = WideCharToMultiByte(CP_UTF8, 0, argvw[i], -1, 0, 0, 0, 0);
        lens[i] = len;
        total_len += len;
    }

    /* Store argv as a single chunk: argv argv[0] ... argv[argc - 1] */
    argv = (char **) HeapAlloc(heap, 0, argn * sizeof(*argv) +
                                        total_len * sizeof(**argv));
    if (!argv) {
        HeapFree(heap, 0, lens);
        return (int) GetLastError();
    }
    argv[0] = (char *) &argv[argn];
    for (i = 0; i != argc; ++i) {

        /* Find the appropriate offset within the array */
        int len = lens[i];
        argv[i + 1] = argv[i] + len;

        /* Convert to UTF-8 */
        WideCharToMultiByte(CP_UTF8, 0, argvw[i], -1, argv[i], len, 0, 0);
    }
    argv[argc] = 0;                     /* Required by the standard */
    HeapFree(heap, 0, lens);
    return 0;
}

/* Destroys the argument vector; argv is must not be null. */
static void argv_destroy() {
    HeapFree(heap, 0, argv);
}

/* Displays a console for the program if it does not already exist.  Returns
   zero if it succeeds; nonzero otherwise. */
int display_console() {
    return console_show(1);
}

/** Defines the entry point for a Windows console program. */
int wmain(int argc, wchar_t **argvw) {
    int ret;

    /* Heap allocations are needed for argv_create */
    heap = GetProcessHeap();

    /* Store in global variables for later use */
    wmain_args.argv = argvw;

    /* Initialize argv and the console*/
    ret = argv_create(argc);
    if (ret)
        return ret;
    console_init();

    /* Call the standard C entry point with UTF-8 arguments */
    ret = main(argc, argv);

    /* Cleanup */
    console_deinit();
    argv_destroy();
    return ret;
}

#ifdef GUI

/** Defines the entry point for a Windows GUI program. */
int CALLBACK wWinMain(HINSTANCE inst, HINSTANCE prev, LPWSTR cmd, int sw) {
    int argc, ret;
    LPWSTR *argvw;

    /* Unused parameters */
    (void) prev;

    /* Store in global variables for later use */
    wmain_args.hInstance = inst;
    wmain_args.lpCmdLine = cmd;
    wmain_args.nCmdShow = sw;

    /* Obtain the arguments */
    argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argvw) {
        DWORD error = GetLastError();
        MessageBoxW(0, wmain_noargv, wmain_noargv_title, MB_ICONERROR);
        return (int) error;
    }

    /* Redirect to the Windows console entry point */
    ret = wmain(argc, argvw);

    /* Cleanup & return */
    LocalFree(argvw);
    return ret;
}

#endif
#ifdef __cplusplus
}
#endif
#endif
