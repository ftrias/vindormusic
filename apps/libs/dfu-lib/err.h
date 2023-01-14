extern char dfu_error_buffer[];
extern int dfu_error_length;
int throw_exception(int num);
# define warnx(...) do {\
    dfu_error_length += sprintf(dfu_error_buffer+dfu_error_length, __VA_ARGS__);\
    dfu_error_length += sprintf(dfu_error_buffer+dfu_error_length, "\n"); } while (0)
# define errx(eval, ...) do {\
    warnx(__VA_ARGS__);\
    throw_exception(eval); } while (0)
# define errxx(eval, ...) do {\
    warnx(__VA_ARGS__);\
    throw_exception(eval); } while (0)
# define warn(...) do {\
    dfu_error_length += sprintf(dfu_error_buffer+dfu_error_length, "%s: ", strerror(errno));\
    warnx(__VA_ARGS__); } while (0)
# define err(eval, ...) do {\
    warn(__VA_ARGS__);\
    throw_exception(eval); } while (0)
# define errxv(eval, ...) do {\
    warnx(__VA_ARGS__);\
    throw_exception(eval); } while (0)
//#define printf(...) do { dfu_error_length += sprintf(dfu_error_buffer+dfu_error_length, __VA_ARGS__); } while(0)
