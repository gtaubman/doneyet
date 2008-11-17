#ifndef __FILE_VERSIONS__
#define __FILE_VERSIONS__

#include "basic-types.h"

static const uint64 INITIAL_VERSION = 0;

// Tasks can now have notes associated with them.
static const uint64 NOTES_VERSION = 1;

// Keep track of all status changes to a task.
static const uint64 TASK_STATUS_VERSION = 2;

#endif
