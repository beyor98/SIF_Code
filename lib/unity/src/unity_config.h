/**
 * List definitions and macros specific to your target here.
 */


//add color to stdout using ANSI escape codes
#define UNITY_OUTPUT_COLOR

//enable printf-like string output implementation
#define UNITY_INCLUDE_PRINT_FORMATTED

//redirect printf
#ifdef __GNUC__
#define UNITY_OUTPUT_CHAR(c)   __io_putchar(c)
#else
#define UNITY_OUTPUT_CHAR(c)   fputc(c, stdout)
#endif

//enable weak feature
#define UNITY_WEAK_ATTRIBUTE __attribute__((weak))


