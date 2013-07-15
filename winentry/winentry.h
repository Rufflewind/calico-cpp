#ifndef FZ_WINENTRY_H
#define FZ_WINENTRY_H
/** @file

    Defines the Windows entry point functions.

    The nonstandard Windows entry points, `wmain` and `wWinMain`, are
    redirected to the standard C entry point `main` and the program arguments
    in `argv` are converted to UTF-8.

*/
#ifdef _WIN32
#ifdef __cplusplus
extern "C" {
#endif

/** The standard C entry point. */
int main(int argc, char **argv);

/** Displays a console for the program if it does not already exist.  Returns
    zero if it succeeds; nonzero otherwise. */
int display_console();

/** Type of `wmain_args`. */
struct wmain_args_type {

    /** Contains the arguments of the program in UTF-16.  Note: unlike the
        standard C version of `argv`, attempting to read `argv[argc]` is
        undefined behavior. */
    wchar_t **argv;

#ifdef GUI

    /** Contains the instance handle of the program. */
    void *hInstance;

    /** Command-line of the program, excluding its name. */
    wchar_t *lpCmdLine;

    /** Initial state of the main window. */
    int nCmdShow;

#endif

};

/** Stores the parameters provided by the Windows entry points. */
extern struct wmain_args_type wmain_args;

#ifdef __cplusplus
}
#endif
#endif
#endif
