#include "project.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


/**
 * @brief frees up dynamically allocated memory for an entry
 * 
 * @param entry the entry to be freed 
 * @return CalEntry* the next entry in the calendar
 */
CalEntry* free_entry(CalEntry* entry){
  CalEntry* next = entry->next;
  //printf("Will now free %s\n", entry->description);
  free(entry->description);
  free(entry);
  return next;
}

/**
 * @brief frees up all dynamically allocated memory for a db
 * 
 * @param db the db to be deleted / freed up
 */
void free_all(CalEntry* db){
  while(1){
    db = free_entry(db);
    if (db == NULL) break;
  }
}

/**
 * @brief prints one calendar entry
 * 
 * @param entry entry to be printed
 * @param stream stream to print to (stdout or file)
 */
void print_entry(CalEntry* entry, FILE* stream){
  fprintf(stream, "%s %02d.%02d at %02d\n", entry->description, entry->day, entry->month, entry->hour);
}

/**
 * @brief loops through whole db and prints all entries
 * 
 * @param db db (calendar) to print
 * @param stream stream to print to (stdout or file)
 */
void print_whole_db(CalEntry* db, FILE* stream){
  CalEntry* to_print = db;
  while(1){
    if (to_print->description != NULL) print_entry(to_print, stream);
    if (to_print->next == NULL) break;
    else to_print = to_print->next;
  }
  printf("SUCCESS\n");
}

/**
 * @brief writes db/calendar to file
 * 
 * @param command_buffer string containing arguments, eg. filename
 * @param db db to write to file
 */
void write_to_file(char* command_buffer, CalEntry* db){
  char command[1];
  char file_name[1000];
  int read = sscanf(command_buffer, "%s %s", command, file_name);
  if (read != 2){
    printf("There was an error in your write command.\n");
  } else {
    FILE* file_ptr = fopen(file_name, "w");
    print_whole_db(db, file_ptr);
    fflush(file_ptr);
    fclose(file_ptr);
  }
}

/**
 * @brief initializes db with one null-entry
 * 
 * @return CalEntry* pointer to first entry / the db
 */
CalEntry* init_db(){
  const CalEntry first = {NULL, 1, 1, 1, NULL};
  CalEntry* db = (CalEntry*)malloc(sizeof(CalEntry));
  memcpy(db, &first, sizeof(CalEntry));
  return db;
}

/**
 * @brief checks if the new db entry is earlier in time than entry to check against
 * 
 * @param new_db_entry the entry that might be earlier
 * @param check the entry to check against
 * @return int returns 1 if is earlier / true, 0 if false
 */
int earlier_date(CalEntry* new_db_entry, CalEntry* check){
  if (new_db_entry->month < check->month) return 1;
  else if (new_db_entry->month == check->month && new_db_entry->day < check->day) return 1;
  else if (new_db_entry->month == check->month && new_db_entry->day == check->day && new_db_entry->hour < check->hour) return 1;
  return 0;
}

/**
 * @brief Create a ordered entry in db object
 * 
 * @param db db to create entry in
 * @param description a string with description of entry/event
 * @param month 
 * @param day 
 * @param hour 
 */
void create_ordered_entry_in_db(CalEntry* db, char* description, int month, int day, int hour){
  char* desc_ptr = (char*)malloc(strlen(description)+1);
  memcpy(desc_ptr, description, strlen(description)+1);
  CalEntry entry = {desc_ptr, month, day, hour, NULL};
  CalEntry* new_db_entry = (CalEntry*)malloc(sizeof(CalEntry));
  memcpy(new_db_entry, &entry, sizeof(CalEntry));
  if (db->next == NULL){
    db->next = new_db_entry;
  } else {
    //printf("Will try and find correct place in loop\n");
    CalEntry* previous = db;
    CalEntry* check = db->next;
    while(1){
      if (earlier_date(new_db_entry, check)){
        previous->next = new_db_entry;
        new_db_entry->next = check;
        return;
      } else if (check->next == NULL){
        check->next = new_db_entry;
        return;
      } else {
        previous = check;
        check = check->next;
      }
    }
  }
}

/**
 * @brief loads a db from a file
 * 
 * @param command_buffer string that contains arguments, eg. filename
 * @param db if loading is successful, this old db is deleted / freed
 * @return CalEntry* either old db if unsuccessful, or new db if successful
 */
CalEntry* load_from_file(char* command_buffer, CalEntry* db){
  char command[1];
  char file_name[1000];
  int read = sscanf(command_buffer, "%s %s", command, file_name);
  if (read != 2){
    printf("There was an error in your open command.\n");
  } else {
    FILE* file_ptr = fopen(file_name, "r");
    if (file_ptr == NULL){
      printf("Cannot open file %s for reading.\n", file_name);
      return db;
    } else {
      CalEntry* new_db = init_db();
      while(1){
        char description[1000];
        int month, day, hour;
        int read = fscanf(file_ptr, "%s %d.%d at %d\n", description, &day, &month, &hour);
        if (read == EOF){
          free_all(db);
          fclose(file_ptr);
          printf("SUCCESS\n");
          return new_db;
        } else if (read != 4){
          printf("There was an error in reading the file.\n");
          free_all(new_db);
          return db;
        }
        create_ordered_entry_in_db(new_db, description, month, day, hour);
      }
    }
  }
  return db;
}

/**
 * @brief checks if a date is already booked in the calendar
 * 
 * @param db 
 * @param month 
 * @param day 
 * @param hour 
 * @return CalEntry* returns pointer to the event if date is booked
 */
CalEntry* entry_found_in_db(CalEntry* db, int month, int day, int hour){
  CalEntry* check = db;
  while(1){
    if (check->description != NULL){
      if (check->month == month && check->day == day && check->hour == hour){
        return check;
      } 
    }
    if (check->next == NULL) return NULL;
    else {
      check = check->next;
    }
  }
}

/**
 * @brief checks if a date is valid
 * 
 * @param month 
 * @param day 
 * @param hour 
 * @return int 0 if invalid, 1 if valid
 */
int valid_date(int month, int day, int hour){
  if (month < 1 || month > 12) {
    printf("Month cannot be less than 1 or greater than 12.\n");
    return 0;
  } else if (day < 1 || day > 31) {
    printf("Day cannot be less than 1 or greater than 31.\n");
    return 0;
  } else if (hour < 0 || hour > 23){
    printf("Hour cannot be negative or greater than 23.\n");
    return 0;
  } 
  return 1;
}

/**
 * @brief checks if the arguments for adding event are valid
 * 
 * @param db db to write to if successful
 * @param command_buffer string containing arguments, parameters to store
 * @param description 
 * @param month 
 * @param day 
 * @param hour 
 * @return int returns 0 if unsuccessful and 1 if successful
 */
int valid_add_args(CalEntry* db, char* command_buffer, char* description, int* month, int* day, int* hour){
  char command[1];
  int read = sscanf(command_buffer, "%s %s %d %d %d", command, description, month, day, hour);
  if (read != 5){
    printf("A should be followed by exactly 4 arguments.\n");
    return 0; 
  } else if (!valid_date(*month, *day, *hour)){
    return 0;
  } else if (entry_found_in_db(db, *month, *day, *hour) != NULL){
    printf("The time slot %02d.%02d at %02d is already allocated.\n", *day, *month, *hour);
    return 0;
  }
  return 1;
}

/**
 * @brief checks if the args for a delete-command are valid
 * 
 * @param command_buffer string containing the arguments
 * @param month 
 * @param day 
 * @param hour 
 * @return int 0 if invalid and 1 if valid
 */
int valid_delete_args(char* command_buffer, int* month, int* day, int* hour){
  char command[1];
  int read = sscanf(command_buffer, "%s %d %d %d", command, month, day, hour);
  if (read != 4){
    printf("A should be followed by exactly 4 arguments.\n");
    return 0;
  } else if (!valid_date(*month, *day, *hour)) {
    return 0;
  } 
  return 1;
}

/**
 * @brief deletes a single entry in db / calendar
 * 
 * @param db the db to delete from
 * @param month 
 * @param day 
 * @param hour 
 * @return int 0 if entry is not found, 1 if is found and deleted
 */
int delete_entry_in_db(CalEntry* db, int month, int day, int hour){
  if (db->next == NULL) return 0;
  CalEntry* previous = db;
  CalEntry* check = db->next;
  while(1){
    if (check->month == month && check->day == day && check->hour == hour){
      previous->next = check->next;
      free(check->description);
      free(check);
      return 1;
    } 
    if (check->next == NULL) return 0;
    else {
      previous = check;
      check = check->next;
    }
  }
}

/**
 * @brief checks if it's possible to delete, then passes to delete-function
 * 
 * @param command_buffer string containing arguments, eg. date to delete
 * @param db the db to delete from
 */
void try_delete_entry(char* command_buffer, CalEntry* db){
  int month, day, hour;
  if (!valid_delete_args(command_buffer, &month, &day, &hour)){
    return;
  } else {
    if (delete_entry_in_db(db, month, day, hour)){
      printf("SUCCESS\n");
    } else {
      printf("The time slot %02d.%02d at %02d is not in the calendar.\n", day, month, hour);
    };
  }
}

/**
 * @brief Create a entry in db object
 * 
 * @param db the db to create the entry in
 * @param description the description for the event
 * @param month 
 * @param day 
 * @param hour 
 */
void create_entry_in_db(CalEntry* db, char* description, int month, int day, int hour){
  char* desc_ptr = (char*)malloc(strlen(description)+1);
  memcpy(desc_ptr, description, strlen(description)+1);
  CalEntry entry = {desc_ptr, month, day, hour, NULL};
  CalEntry* new_db_entry = (CalEntry*)malloc(sizeof(CalEntry));
  memcpy(new_db_entry, &entry, sizeof(CalEntry));
  CalEntry* check = db;
  while(1){
    if (check->next != NULL) check = check->next;
    else {
      check->next = new_db_entry;
      break;
    }
  }
}

/**
 * @brief function to check if it's possible to add entry to db
 * 
 * @param command_buffer string containing arguments, eg. the data for the event
 * @param db the db to create event in
 */
void try_add_entry(char* command_buffer, CalEntry* db){
  char description[1000];
  int month, day, hour;
  if (!valid_add_args(db, command_buffer, description, &month, &day, &hour)) {
    return;
  } else {
    //create_entry_in_db(db, description, month, day, hour, db_size);
    create_ordered_entry_in_db(db, description, month, day, hour);
  }
  printf("SUCCESS\n");
}

/**
 * @brief for entering test data in db
 * 
 * @param db the db to enter test data in
 */
void enter_test_entries(CalEntry* db){
  create_entry_in_db(db, "First", 1, 2, 3);
  create_entry_in_db(db, "Second", 2, 3, 4);
  print_whole_db(db, stdout);
}

/**
 * @brief main function, test-row can be commented in if doing manual tests
 * 
 * @return int 
 */
int main(void) {
  CalEntry* db = init_db();
  if(db == NULL) return 0;
  //enter_test_entries(db);
  while(1){
    char command_buffer[1000];
    fgets(command_buffer, 1000, stdin);
    switch (command_buffer[0]){
      case 'A':
        try_add_entry(command_buffer, db);
        break;
      case 'L':
        print_whole_db(db, stdout);
        break;
      case 'Q':
        free_all(db);
        printf("SUCCESS\n");
        return 0;
      case 'D':
        try_delete_entry(command_buffer, db);
        break;
      case 'W':
        write_to_file(command_buffer, db);
        break;
      case 'O':
        db = load_from_file(command_buffer, db);
        break;
      default:
        printf("Invalid command %c\n", command_buffer[0]);
        break;
    }
  }
}