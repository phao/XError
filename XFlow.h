#ifndef X_FLOW_H
#define X_FLOW_H

#define DoIf(cond, effect) if (cond) { effect; } else (void) 0
#define DoLt0(expr, effect) DoIf((expr) < 0, effect)
#define DoIf0(expr, effect) DoIf(!(expr), effect)

#define ReturnIf(cond, val) DoIf(cond, return (val))
#define ReturnLt0(expr, val) DoLt0(expr, return (val))
#define ReturnIf0(expr, val) DoIf0(expr, return (val))

/**
 * Macro expands to:
 *
 * if (1) {
 *  int X_FLOW_test = (expr);
 *  if (X_FLOW_test < 0) return X_FLOW_test;
 * } else (void) 0
 *
 * If X_FLOW_H__expr conflicts with an identifier you have in your code, I'm
 * sorry (really) for you.
 */
#define ReturnMeLt0(expr) \
  if (1) { \
    int X_FLOW_test = (expr); \
    if (X_FLOW_test < 0) return X_FLOW_test; \
  } else (void) 0

#define GotoIf(cond, label) DoIf(cond, goto label)
#define GotoLt0(expr, label) DoLt0(cond, goto label)
#define GotoIf0(expr, label) DoIf0(cond, goto label)

#define BreakIf(cond) DoIf(cond, break)
#define BreakLt0(expr) DoLt0(expr, break)
#define BreakIf0(expr) DoIf0(expr, break)

#define ContinueIf(cond) DoIf(cond, continue)
#define ContinueLt0(expr) DoLt0(expr, continue)
#define ContinueIf0(expr) DoIf0(expr, continue)

#endif
