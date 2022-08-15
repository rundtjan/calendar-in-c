#include "project.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


// TODO:: implement your project here!
CalEntry* free_entry(CalEntry* entry){
  CalEntry* next = entry->next;
  //printf("Will now free %s\n", entry->description);
  free(entry->description);
  free(entry);
  return next;
}

void free_all(CalEntry* db){
  while(1){
    db = free_entry(db);
    if (db == NULL) break;
  }
}

void print_entry(CalEntry* entry, FILE* stream){
  fprintf(stream, "%s %02d.%02d at %02d \n", entry->description, entry->day, entry->month, entry->hour);
}

void print_whole_db(CalEntry* db, FILE* stream){
  CalEntry* to_print = db;
  while(1){
    if (to_print->description != NULL) print_entry(to_print, stream);
    if (to_print->next == NULL) break;
    else to_print = to_print->next;
  }
  printf("SUCCESS\n");
}

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

CalEntry* init_db(){
  const CalEntry first = {NULL, 1, 1, 1, NULL};
  CalEntry* db = (CalEntry*)malloc(sizeof(CalEntry));
  memcpy(db, &first, sizeof(CalEntry));
  return db;
}

int earlier_date(CalEntry* new_db_entry, CalEntry* check){
  if (new_db_entry->month < check->month) return 1;
  else if (new_db_entry->month == check->month && new_db_entry->day < check->day) return 1;
  else if (new_db_entry->month == check->month && new_db_entry->day == check->day && new_db_entry->hour < check->hour) return 1;
  //printf("Did not think that new_db_entry %d %d %d was smaller than %d %d %d", new_db_entry->month, new_db_entry->day, new_db_entry->hour, check->month, check->day, check->hour);
  return 0;
}

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

void load_from_file(char* command_buffer, CalEntry* db){
  char command[1];
  char file_name[1000];
  int read = sscanf(command_buffer, "%s %s", command, file_name);
  if (read != 2){
    printf("There was an error in your open command.\n");
  } else {
    FILE* file_ptr = fopen(file_name, "r");
    if (file_ptr == NULL){
      printf("Cannot open file %s for reading.", file_name);
      return;
    } else {
      printf("Could possibly read file\n");
      CalEntry* new_db = init_db();
      while(1){
        char description[1000];
        int month, day, hour;
        int read = fscanf(file_ptr, "%s %d.%d at %d\n", description, &day, &month, &hour);
        if (read == EOF){
          fflush(file_ptr);
          fclose(file_ptr);
          printf("SUCCESS\n");
          break;
        } else if (read != 4){
          printf("There was an error in reading the file.\n");
          free_all(new_db);
          break;
        }
        create_ordered_entry_in_db(new_db, description, month, day, hour);
        printf("Read from file %s %02d.%02d at %02d\n", description, day, month, hour);
      }
    }
  }
}

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

int valid_add_args(CalEntry* db, char* command_buffer, char* description, int* month, int* day, int* hour){
  char command[1];
  int read = sscanf(command_buffer, "%s %s %d %d %d", command, description, month, day, hour);
  if (read != 5){
    printf("Wrong number of args.\n");
    return 0;
  } else if (*month < 1 || *month > 12) {
    printf("Month cannot be less than 1 or greater than 12.\n");
    return 0;
  } else if (*day < 1 || *day > 31) {
    printf("Day cannot be less than 1 or greater than 31.\n");
    return 0;
  } else if (*hour < 0 || *hour > 23){
    printf("Hour cannot be negative or greater than 23.\n");
    return 0;
  } else if (entry_found_in_db(db, *month, *day, *hour) != NULL){
    printf("The time slot %02d.%02d at %02d is already allocated.", *day, *month, *hour);
    return 0;
  }
  return 1;
}

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

int valid_delete_args(char* command_buffer, int* month, int* day, int* hour){
  char command[1];
  int read = sscanf(command_buffer, "%s %d %d %d", command, month, day, hour);
  if (read != 4){
    printf("Wrong number of args.\n");
    return 0;
  } else if (!valid_date(*month, *day, *hour)) {
    return 0;
  } 
  return 1;
}


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

void enter_test_entries(CalEntry* db){
  create_entry_in_db(db, "First", 1, 2, 3);
  create_entry_in_db(db, "Second", 2, 3, 4);
  print_whole_db(db, stdout);
}

int main(void) {
  CalEntry* db = init_db();
  if(db == NULL) return 0;
  enter_test_entries(db);
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
        load_from_file(command_buffer, db);
        break;
      default:
        printf("Invalid command %c\n", command_buffer[0]);
        break;
    }
  }
}