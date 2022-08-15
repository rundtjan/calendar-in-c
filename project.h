#ifndef _PROJECT__H_
#define _PROJECT__H_

#include <stdio.h>
#include <stdlib.h>

struct calendar_entry;
/**
 * @brief A calendar entry, properties self-explanatory
 * 
 */
typedef struct calendar_entry {
  char* description;
  int month, day, hour;
  struct calendar_entry* next;
} CalEntry;

/*Functions for memory allocation*/
CalEntry* free_entry(CalEntry* entry);
void free_all(CalEntry* db);

/*Printing or reading from streams*/
void print_entry(CalEntry* entry, FILE* stream);
void print_whole_db(CalEntry* db, FILE* stream);
void write_to_file(char* command_buffer, CalEntry* db);
CalEntry* load_from_file(char* command_buffer, CalEntry* db);

/*Database CRUD or other functions*/
CalEntry* init_db();
void create_entry_in_db(CalEntry* db, char* description, int month, int day, int hour);
CalEntry* entry_found_in_db(CalEntry* db, int month, int day, int hour);
int delete_entry_in_db(CalEntry* db, int month, int day, int hour);
void try_delete_entry(char* command_buffer, CalEntry* db);
void try_add_entry(char* command_buffer, CalEntry* db);
void enter_test_entries(CalEntry* db);

/*Functions for validation or comparing*/
int earlier_date(CalEntry* new_db_entry, CalEntry* check);
int valid_date(int month, int day, int hour);
int valid_add_args(CalEntry* db, char* command_buffer, char* description, int* month, int* day, int* hour);
int valid_delete_args(char* command_buffer, int* month, int* day, int* hour);

#endif //! _PROJECT__H_