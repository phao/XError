#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "XError.h"

static struct XERR_ErrorSequence err_seq;
static size_t stack_cap;

static int
SetString(struct XERR_String *string, const char *src) {
  if (src) {
    size_t src_len = strlen(src);
    free(string->data);
    string->data = malloc(src_len+1);
    if (!string->data) {
      string->len = -1;
      return -1;
    }
    strcpy(string->data, src);
    string->len = src_len;
  }
  else {
    string->data = 0;
    string->len = -1;
  }
  return 0;
}

static void
ErrorSetup(struct XERR_Error *err,
            const char *msg,
            int line,
            const char *file,
            const char *func,
            const char *code)
{
  err->line = line;
  err->msg.data = 0;
  err->file.data = 0;
  err->func.data = 0;
  err->code.data = 0;
  ErrIgnore(SetString(&err->msg, msg));
  ErrIgnore(SetString(&err->file, file));
  ErrIgnore(SetString(&err->func, func));
  ErrIgnore(SetString(&err->code, code));
}

int
XERR_PushError(const char *msg,
               int line,
               const char *file,
               const char *func,
               const char *code)
{
  if (err_seq.count == stack_cap) {
    size_t delta_cap = stack_cap/5 + 30;
    if (stack_cap > SIZE_MAX - delta_cap) {
      return -1;
    }
    size_t new_cap = stack_cap + delta_cap;
    if (new_cap > SIZE_MAX / sizeof (struct XERR_Error)) {
      return -1;
    }
    void *newp = realloc(err_seq.errors, new_cap * sizeof (struct XERR_Error));
    if (!newp) {
      return -1;
    }
    err_seq.errors = newp;
    stack_cap = new_cap;
  }
  struct XERR_Error *err = err_seq.errors + err_seq.count;
  ErrorSetup(err, msg, line, file, func, code);
  err_seq.count++;
  if ((!err->msg.data && msg)
      || (!err->file.data && file)
      || (!err->func.data && func)
      || (!err->code.data && code))
  {
    fprintf(stderr, "%s: %s: %d: %s\n\t%s\n", file, func, line, msg, code);
  }
  return 0;
}

static inline void
CopyStringWithBuffer(struct XERR_String *out,
                     const struct XERR_String *in,
                     char **buffer)
{
  // Null check is needed because it's very common for null being used as
  // a message (for pass-through errors).
  if (in->data) {
    assert(*buffer);
    out->data = *buffer;
    strcpy(out->data, in->data);
    out->len = in->len;
    *buffer += out->len+1;
  }
  else {
    out->data = 0;
    out->len = -1;
  }
}

struct XERR_ErrorSequence
XERR_CopyErrors(void) {
  struct XERR_ErrorSequence out_seq;

  // Count text bytes.
  size_t text_bytes = 0;
  for (size_t i = 0; i < err_seq.count; i++) {
    struct XERR_Error *in_err = err_seq.errors + i;
    text_bytes += in_err->msg.len + 1;
    text_bytes += in_err->file.len + 1;
    text_bytes += in_err->func.len + 1;
    text_bytes += in_err->code.len + 1;
  }

  // Allocate bytes for output.
  out_seq.errors = malloc(err_seq.count * sizeof (struct XERR_Error));
  if (!out_seq.errors) {
    // If couldn't allocate, the null `errors` member will indicate the
    // error.
    return out_seq;
  }
  char *text_buffer = malloc(text_bytes);
  if (!text_buffer) {
    free(out_seq.errors);
    out_seq.errors = 0;
    return out_seq;
  }
  out_seq.count = err_seq.count;

  // At this point on, no more errors can appear.

  assert(text_buffer);
  assert(out_seq.errors);
  assert(out_seq.count == err_seq.count);

  for (size_t i = 0; i < out_seq.count; i++) {
    const struct XERR_Error *in_err = err_seq.errors + i;
    struct XERR_Error *out_err = out_seq.errors + i;
    out_err->line = in_err->line;
    CopyStringWithBuffer(&out_err->msg, &in_err->msg, &text_buffer);
    CopyStringWithBuffer(&out_err->file, &in_err->file, &text_buffer);
    CopyStringWithBuffer(&out_err->func, &in_err->func, &text_buffer);
    CopyStringWithBuffer(&out_err->code, &in_err->code, &text_buffer);
  }

  return out_seq;
}

void
XERR_FreeErrors(struct XERR_ErrorSequence err_seq) {
  if (err_seq.errors) {
    free(err_seq.errors->msg.data);
    free(err_seq.errors);
  }
}

void
XERR_ClearInternalSequence(void) {
  for (size_t i = 0; i < err_seq.count; i++) {
    free(err_seq.errors[i].msg.data);
    free(err_seq.errors[i].file.data);
    free(err_seq.errors[i].func.data);
    free(err_seq.errors[i].code.data);
  }
  err_seq.count = 0;
}
