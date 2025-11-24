/* platform_console.h
 * Cross-platform console helpers
 */
#ifndef PLATFORM_CONSOLE_H
#define PLATFORM_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Ensure console output uses UTF-8 on supported platforms. */
void setConsoleOutputCPToUTF8(void);
/* Read environment variable CONSOLE_ENCODING and set console encoding accordingly.
 * Supported values (case-insensitive): "UTF-8" (default), "GBK" / "GB2312" / "CP936".
 */
void setConsoleEncodingFromEnv(void);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_CONSOLE_H */
