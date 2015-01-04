#ifndef X_ERROR_H
#define X_ERROR_H

struct BasicString {
  char *data;
  int len;
};

struct ErrorInfo {
  struct BasicString msg, file_name, func_name, code;
  int line;
  struct ErrorInfo *next;
};

/**
 * Will attempt to link a new error node in the current structure. If that's
 * not possible, errors will be printed to the console.
 *
 * Returns 0 on succes and negative values on failure.
 *
 * This function isn't supposed to be called directly, although you can do it.
 * It's meant to be used through the error macros below.
 */
int
XERR_LinkError(const char *msg,
               int line,
               const char *file_name,
               const char *func_name,
               const char *code);

/**
 * Copies and returns the current internal error structure. You're responsible
 * to call clean_error on it later.
 */
struct ErrorInfo*
XERR_CopyError(void);

/**
 * Passing null as the argument will clean the internally kept error
 * structure.
 */
void
XERR_FreeError(struct ErrorInfo *err);

/*
 * Control flow macros for error handling that also sets error information
 * through XERR_LinkError calls.
 *
 * You can always pass 0 (or any null-ish eqv) where MSG is required. This
 * will cause a 'messageless' error to be set up (file name and line number
 * are still recorded).
 *
 * The 'PE' (or 'jPE') prefix stands for "pass error along". It's equivalent
 * to the 'E' (or 'jE') prefix macros, but with 0 as the MSG parameter.
 */

/**
 * If (EXPR) < 0, then set error information and goto GOTO.
 */
#define jErrLt0(EXPR, GOTO, MSG) \
  do { \
    if ((EXPR) < 0) { \
      XERR_LinkError((MSG), __LINE__, __FILE__, __func__, # EXPR); \
      goto GOTO; \
    } \
  } while (0)

#define jPErrLt0(EXPR, GOTO) jErrLt0(EXPR, GOTO, 0)

/**
 * If !(COND), then set error information and goto LABEL.
 */
#define jErrIf0(COND, LABEL, MSG) \
  do { \
    if (!(COND)) { \
      XERR_LinkError((MSG), __LINE__, __FILE__, __func__, # COND); \
      goto LABEL; \
    } \
  } while (0)

#define jPErrIf0(COND, LABEL) jErrIf0(COND, LABEL, 0)

/**
 * If (COND) is true-ish, then set error information and goto GOTO.
 */
#define jErrIf(COND, LABEL, MSG) \
  do { \
    if ((COND)) { \
      XERR_LinkError((MSG), __LINE__, __FILE__, __func__, # COND); \
      goto LABEL; \
    } \
  } while (0)

#define jPErrIf(COND, LABEL) jErrIf(COND, LABEL, 0)

/**
 * If e = (EXPR) and e < 0, set error information and return e.
 */
#define ErrLt0(EXPR, MSG) \
  do { \
    int X_ERROR_test = (EXPR); \
    if (X_ERROR_test < 0) { \
      XERR_LinkError((MSG), __LINE__, __FILE__, __func__, # EXPR); \
      return X_ERROR_test; \
    } \
  } while (0)

#define PErrLt0(EXPR) ErrLt0(EXPR, 0)

/**
 * If !(COND), set error information and return 0.
 */
#define ErrIf0(COND, VAL, MSG) do { \
    if (!(COND)) { \
      XERR_LinkError((MSG), __LINE__, __FILE__, __func__, # COND); \
      return (VAL); \
    } \
  } while (0)

#define PErrIf0(EXPR) ErrIf0(COND, VAL, 0)

/**
 * If (COND), set error information and return VAL.
 */
#define ErrIf(COND, VAL, MSG) \
  do { \
    if (COND) { \
      XERR_LinkError((MSG), __LINE__, __FILE__, __func__, # COND); \
      return (VAL); \
    } \
  } while (0)

#define PErrIf(COND, VAL) ErrIf(COND, VAL, 0)

/**
 * Special macro to syntactically indicate that you're ignoring an error. This
 * doesn't affect the code at all. ErrIgnore(X) is just the same as just
 * writing X.
 */
#define ErrIgnore(x) x

#endif
