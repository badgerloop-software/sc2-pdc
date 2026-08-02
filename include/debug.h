#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "board_config.h"

// ------------- FUNCTIONS -------------

#if SC2_DEBUG

void debugInit();
void debugUpdate();
void debugError(const char* msg);

#else

// no-ops when SC2_DEBUG is 0 (not linked into flash)
inline void debugInit() {}
inline void debugUpdate() {}
inline void debugError(const char*) {}

#endif

#endif  // __DEBUG_H__
