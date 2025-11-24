/* platform_console.c
 * Cross-platform implementation for console encoding helpers.
 */
#include "platform_console.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <locale.h>
#endif

void setConsoleOutputCPToUTF8(void) {
#ifdef _WIN32
    /* Set output code page to UTF-8 on Windows */
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else
    /* Use the user's locale settings on POSIX; UTF-8 is expected from environment */
    setlocale(LC_ALL, "");
#endif
}

void setConsoleEncodingFromEnv(void) {
    const char *enc = getenv("CONSOLE_ENCODING");
    if (!enc) enc = "UTF-8";
    /* normalize to upper for checks */
    char buf[32] = {0};
    size_t i = 0;
    for (; i < sizeof(buf)-1 && enc[i]; ++i) buf[i] = (char)toupper((unsigned char)enc[i]);
    buf[i] = '\0';

#ifdef _WIN32
    UINT cp = CP_UTF8;
    if (strcmp(buf, "GBK") == 0 || strcmp(buf, "GB2312") == 0 || strcmp(buf, "CP936") == 0) {
        cp = 936; /* GBK code page */
    }
    SetConsoleOutputCP(cp);
    SetConsoleCP(cp);
#else
    if (strcmp(buf, "GBK") == 0 || strcmp(buf, "GB2312") == 0 || strcmp(buf, "CP936") == 0) {
        /* try common GBK locale variants */
        if (!setlocale(LC_ALL, "zh_CN.GB18030") && !setlocale(LC_ALL, "zh_CN.GBK") && !setlocale(LC_ALL, "zh_CN.GB2312")) {
            /* fall back to C locale if none available */
            setlocale(LC_ALL, "");
        }
    } else {
        /* default: use environment / UTF-8 */
        setlocale(LC_ALL, "");
    }
#endif
}
