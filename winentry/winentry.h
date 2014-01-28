#ifndef FZ_WINENTRY_H
#define FZ_WINENTRY_H
#ifdef CALICO_DOC_ONLY
#define _WIN32
#define GUI
#endif
#ifdef _WIN32
#ifdef __cplusplus
extern "C" {
#endif
/** @file

    Redirects the Windows entry point functions to the standard C entry point
    functions.

    The nonstandard Windows entry points, `wmain` and `wWinMain`, are
    redirected to the standard C entry point `main` and the program arguments
    in `argv` are converted to UTF-8.

    The `winentry.c` file must be compiled and linked with the program for
    this to work.

    If the program is to have a graphic user interface, define the `GUI` macro.

    Note: Some of the code here have yet to be tested.

*/

/** Declaration of the standard C entry point.

    Note that this is not a definition. The library user is expected to supply
    this function. */
int main(int argc, char **argv);

/** Displays a console for the program if it does not already exist.

    @return  `0` if it succeeds; nonzero otherwise. */
int show_console(void);

/** Type of `wmain_args`.

    Used to store the arguments of the Windows main function. */
struct wmain_args_type {

    /** Contains the arguments of the program in UTF-16.  Note: unlike the
        standard C version of `argv`, attempting to read `argv[argc]` is
        undefined behavior. */
    wchar_t **argv;

#ifdef GUI

    /** Contains the instance handle of the program (`GUI` only).

        Note that `void*` may be cast to `HINSTANCE`. */
    void *hInstance;

    /** Command-line of the program, excluding its name (`GUI` only).

        Note that `wchar_t*` is equivalent to `LPWSTR`. */
    wchar_t *lpCmdLine;

    /** Initial state of the main window (`GUI` only).

        The parameter should be passed to the first invocation of
        `ShowWindow`. */
    int nCmdShow;

#endif

};

/** Stores the original arguments provided by the Windows entry points.

    This variable is set before `main` is executed. */
extern struct wmain_args_type wmain_args;

#ifdef __cplusplus
}
#endif
#endif
#endif
