#ifndef _PROJECT__H_
#define _PROJECT__H_

struct calendar_entry;

typedef struct calendar_entry {
  char* description;
  int month, day, hour;
  struct calendar_entry* next;
} CalEntry;

#endif //! _PROJECT__H_