#undef ReadConsole
#undef WriteConsole

#define DEFAULT_CONSOLE_PRINT_COUNT 64000

VOID InitConsole();
BOOL ShowConsole(INT cmd);

BOOL WriteConsole(WSTRV text);
WSTR ReadConsole(size_t count);

BOOL Print(WSTRV text);
WSTR Input(WSTRV text);
