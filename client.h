#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "helpers.h"
#include "parson.h"
#include "requests.h"

#define MAX_LEN 1024

void register_command();
void login();
void enter_library();
void get_books();
void get_book();
void add_book();
void delete_book();
void logout();
void export_books();
void export_book();
void import_book();

int get_response_code(char* response);
void set_token(char* response);
char* print_books(const char *response);
char* get_body(char* response);
void set_cookie(char* response);

#endif // _CLIENT_H_