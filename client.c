#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "client.h"

char* user_name;
char* user_password;
char* cookie;
char* token;

int main(int argc, char *argv[])
{
    char command[MAX_LEN];

    user_name = (char*)malloc(MAX_LEN * sizeof(char));
    user_password = (char*)malloc(MAX_LEN * sizeof(char));
    cookie = (char*)malloc(MAX_LEN * sizeof(char));
    token = (char*)malloc(MAX_LEN * sizeof(char));

    cookie[0] = '\0';
    token[0] = '\0';

    while (1) {
        fgets(command, MAX_LEN, stdin);
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "register") == 0) {
            register_command();
        } else if (strcmp(command, "login") == 0) {
            login();
        } else if (strcmp(command, "enter_library") == 0) {
            enter_library();
        } else if (strcmp(command, "get_books") == 0) {
            get_books();
        } else if (strcmp(command, "get_book") == 0) {
            get_book();
        } else if (strcmp(command, "add_book") == 0) {
            add_book();
        } else if (strcmp(command, "delete_book") == 0) {
            delete_book();
        } else if (strcmp(command, "logout") == 0) {
            logout();
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "export_books") == 0) {
            export_books();
        } else if (strcmp(command, "export_book") == 0) {
            export_book();
        } else if (strcmp(command, "import_book") == 0) {
            import_book();
        } else {
            printf("Invalid command\n");
        }  
    }

    free(user_name);
    free(user_password);
    free(cookie);
    free(token);

    return 0;
}

void register_command()
{
    char *message;
    char *response;

    printf("username=");
    fgets(user_name, MAX_LEN, stdin);
    user_name[strcspn(user_name, "\n")] = 0;

    printf("password=");
    fgets(user_password, MAX_LEN, stdin);
    user_password[strcspn(user_password, "\n")] = 0;

    if (strchr(user_name, ' ')) {
        printf("ERROR, Username contains spaces\n");
        return;
    }

    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "username", user_name);
    json_object_set_string(root_object, "password", user_password);

    message = compute_http_request(POST, "/api/v1/tema/auth/register", 
                            NULL, "application/json", root_value, NULL, NULL);

    response = send_http_request(message);

    if (get_response_code(response) == 201) {
        printf("SUCCESS, User created\n");
    } else {
        printf("ERROR, User not created\n");
        printf("Response: %s\n", get_body(response));
    }

    free(message);
    free(response);
    json_value_free(root_value);
}

void login() 
{
    if (cookie[0] != '\0') {
        printf("ERROR, You are already logged in\n");
        return;
    }

    char *message;
    char *response;

    printf("username=");
    fgets(user_name, MAX_LEN, stdin);
    user_name[strcspn(user_name, "\n")] = 0;
    printf("password=");
    fgets(user_password, MAX_LEN, stdin);
    user_password[strcspn(user_password, "\n")] = 0;

     if (strchr(user_name, ' ')) {
        printf("ERROR, Username contains spaces\n");
        return;
    }

    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "username", user_name);
    json_object_set_string(root_object, "password", user_password);

    message = compute_http_request(POST, "/api/v1/tema/auth/login", 
                            NULL, "application/json", root_value, NULL, NULL);

    response = send_http_request(message);

    if (get_response_code(response) == 200) {
        printf("SUCCESS, User logged in\n");
        set_cookie(response);
    } else {
        printf("ERROR, User not logged in\n");
        printf("Response: %s\n", get_body(response));
    }

    free(message);
    free(response);
    json_value_free(root_value);
}

void enter_library() 
{
    if (cookie[0] == '\0') {
        printf("ERROR, You are not logged in\n");
        return;
    }

    char *message;
    char *response;

    message = compute_http_request(GET, "/api/v1/tema/library/access", 
                            NULL, NULL, NULL, cookie, NULL);

    response = send_http_request(message);

    if (get_response_code(response) == 200) {
        printf("SUCCESS, Library accessed\n");
        set_token(response);
    } else {
        printf("ERROR, Library not accessed\n");
        printf("Response: %s\n", get_body(response));
    }

    free(message);
    free(response);
}

void get_books() 
{
    if (cookie[0] == '\0') {
        printf("ERROR, You are not logged in\n");
        return;
    }

    if (token[0] == '\0') {
        printf("ERROR, You are not in the library\n");
        return;
    }

    char *message;
    char *response;

    message = compute_http_request(GET, "/api/v1/tema/library/books", 
                            NULL, NULL, NULL, NULL, token);

    response = send_http_request(message);

    if (get_response_code(response) == 200) {
        printf("SUCCESS, Books retrieved\n");
        char* books = print_books(get_body(response));
        printf("%s\n", books);
        free(books);
    } else {
        printf("ERROR, Books not retrieved\n");
        printf("Response: %s\n", get_body(response));
    }

    free(message);
    free(response);
}

void get_book() 
{
    if (cookie[0] == '\0') {
        printf("ERROR, You are not logged in\n");
        return;
    }

    if (token[0] == '\0') {
        printf("ERROR, You are not in the library\n");
        return;
    }

    char *message;
    char *response;
    char id[MAX_LEN];

    printf("id=");
    fgets(id, MAX_LEN, stdin);
    id[strcspn(id, "\n")] = 0;

    char url[MAX_LEN] = "/api/v1/tema/library/books/";
    strcat(url, id);

    message = compute_http_request(GET, url, 
                            NULL, NULL, NULL, NULL, token);

    response = send_http_request(message);

    if (get_response_code(response) == 200) {
        printf("SUCCESS, Book retrieved\n");
        char* book = print_books(get_body(response));
        printf("%s\n", book);
        free(book);
    } else {
        printf("ERROR, Book not retrieved\n");
        printf("Response: %s\n", get_body(response));
    }

    free(message);
    free(response);
}

void add_book() 
{
    if (cookie[0] == '\0') {
        printf("ERROR, You are not logged in\n");
        return;
    }

    if (token[0] == '\0') {
        printf("ERROR, You are not in the library\n");
        return;
    }

    char *message;
    char *response;

    char title[MAX_LEN];
    char author[MAX_LEN];
    char genre[MAX_LEN];
    char publisher[MAX_LEN];
    char page_count[MAX_LEN];

    printf("title=");
    fgets(title, MAX_LEN, stdin);
    title[strcspn(title, "\n")] = 0;

    printf("author=");
    fgets(author, MAX_LEN, stdin);
    author[strcspn(author, "\n")] = 0;

    printf("genre=");
    fgets(genre, MAX_LEN, stdin);
    genre[strcspn(genre, "\n")] = 0;

    printf("publisher=");
    fgets(publisher, MAX_LEN, stdin);
    publisher[strcspn(publisher, "\n")] = 0;

    printf("page_count=");
    fgets(page_count, MAX_LEN, stdin);
    page_count[strcspn(page_count, "\n")] = 0;

    for (int i = 0; i < strlen(page_count); i++) {
        if (page_count[i] < '0' || page_count[i] > '9') {
            printf("ERROR, page_count is not a number\n");
            return;
        }
    }

    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_string(root_object, "publisher", publisher);
    json_object_set_string(root_object, "page_count", page_count);

    message = compute_http_request(POST, "/api/v1/tema/library/books", 
                            NULL, "application/json", root_value, NULL, token);

    response = send_http_request(message);

    if (get_response_code(response) == 200) {
        printf("SUCCESS, Book added\n");
    } else {
        printf("ERROR, Book not added\n");
        printf("Response: %s\n", get_body(response));
    }

    free(message);
    free(response);
    json_value_free(root_value);
}

void delete_book() 
{
    if (cookie[0] == '\0') {
        printf("ERROR, You are not logged in\n");
        return;
    }

    if (token[0] == '\0') {
        printf("ERROR, You are not in the library\n");
        return;
    }

    char *message;
    char *response;
    char id[MAX_LEN];

    printf("id=");
    fgets(id, MAX_LEN, stdin);
    id[strcspn(id, "\n")] = 0;

    char url[MAX_LEN] = "/api/v1/tema/library/books/";
    strcat(url, id);

    message = compute_http_request(DELETE, url, 
                            NULL, NULL, NULL, NULL, token);

    response = send_http_request(message);

    if (get_response_code(response) == 200) {
        printf("SUCCESS, Book deleted\n");
    } else {
        printf("ERROR, Book not deleted\n");
        printf("Response: %s\n", get_body(response));
    }

    free(message);
    free(response);
}

void logout() 
{
    char *message;
    char *response;

    message = compute_http_request(GET, "/api/v1/tema/auth/logout", 
                            NULL, NULL, NULL, cookie, token);

    response = send_http_request(message);

    if (get_response_code(response) == 200) {
        printf("SUCCESS, User logged out\n");
        cookie[0] = '\0';
        token[0] = '\0';
    } else {
        printf("ERROR, User not logged out\n");
        printf("Response: %s\n", get_body(response));
    }

    free(message);
    free(response);
}

void export_books() {
    if (cookie[0] == '\0') {
        printf("ERROR, You are not logged in\n");
        return;
    }

    if (token[0] == '\0') {
        printf("ERROR, You are not in the library\n");
        return;
    }

    char *message;
    char *response;

    message = compute_http_request(GET, "/api/v1/tema/library/books", 
                            NULL, NULL, NULL, NULL, token);

    response = send_http_request(message);

    if (get_response_code(response) == 200) {
        printf("SUCCESS, Books retrieved\n");
        char* books = print_books(get_body(response));
        FILE *f = fopen("books.json", "w");
        fprintf(f, "%s", books);
        fclose(f);
        free(books);
    } else {
        printf("ERROR, Books not retrieved\n");
        printf("Response: %s\n", get_body(response));
    }

    free(message);
    free(response);
}

void export_book() {
    if (cookie[0] == '\0') {
        printf("ERROR, You are not logged in\n");
        return;
    }

    if (token[0] == '\0') {
        printf("ERROR, You are not in the library\n");
        return;
    }

    char *message;
    char *response;
    char id[MAX_LEN];

    printf("id=");
    fgets(id, MAX_LEN, stdin);
    id[strcspn(id, "\n")] = 0;

    char url[MAX_LEN] = "/api/v1/tema/library/books/";
    strcat(url, id);

    message = compute_http_request(GET, url, 
                            NULL, NULL, NULL, NULL, token);

    response = send_http_request(message);

    if (get_response_code(response) == 200) {
        printf("SUCCESS, Book retrieved\n");
        char* book = print_books(get_body(response));
        JSON_Value *root_value = json_parse_string(book);
        JSON_Object *root_object = json_value_get_object(root_value);
        const char* title = json_object_get_string(root_object, "title");
        char file_name[MAX_LEN];
        strcpy(file_name, title);
        strcat(file_name, ".json");
        FILE *f = fopen(file_name, "w");
        fprintf(f, "%s", book);
        fclose(f);
        free(book);
        json_value_free(root_value);
    } else {
        printf("ERROR, Book not retrieved\n");
        printf("Response: %s\n", get_body(response));
    }

    free(message);
    free(response);
}

void import_book() {
    if (cookie[0] == '\0') {
        printf("ERROR, You are not logged in\n");
        return;
    }

    if (token[0] == '\0') {
        printf("ERROR, You are not in the library\n");
        return;
    }

    char *message;
    char *response;

    char file_name[MAX_LEN];
    printf("file_name=");
    fgets(file_name, MAX_LEN, stdin);
    file_name[strcspn(file_name, "\n")] = 0;

    printf("file_name=%s\n", file_name);

    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        printf("ERROR, File not found\n");
        return;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *file_content = malloc(fsize + 1);
    fread(file_content, 1, fsize, f);
    fclose(f);

    file_content[fsize] = 0;

    JSON_Value *root_value = json_parse_string(file_content);

    message = compute_http_request(POST, "/api/v1/tema/library/books", 
                            NULL, "application/json", root_value, NULL, token);

    response = send_http_request(message);

    if (get_response_code(response) == 200) {
        printf("SUCCESS, Book added\n");
    } else {
        printf("ERROR, Book not added\n");
        printf("Response: %s\n", get_body(response));
    }

    free(message);
    free(response);
    free(file_content);
}

// ------------------ funtii in plus ------------------ //

int get_response_code(char* response) {
    char* p = strstr(response, "HTTP/1.1");
    if (p) {
        p += strlen("HTTP/1.1");
        while (*p == ' ') {
            p++;
        }
        return atoi(p);
    }
    return -1;
}


char* print_books(const char *response) {
    JSON_Value *root_value;

    root_value = json_parse_string(response);
    if (json_value_get_type(root_value) != JSONArray 
        && json_value_get_type(root_value) != JSONObject) {
        printf("Invalid JSON format\n");
        json_value_free(root_value);
        return NULL;
    }

    return json_serialize_to_string_pretty(root_value);
}

char* get_body(char* response) {
    char* body = strstr(response, "\r\n\r\n");
    if (body) {
        body += 4;
    }

    return body;
}

void set_cookie(char* response) {
    char* p = strstr(response, "Set-Cookie: ");
    if (p) {
        p += strlen("Set-Cookie: ");
        char* q = strstr(p, ";");
        if (q) {
            strncpy(cookie, p, q - p);
            cookie[q - p] = '\0';
        }
    }
}

void set_token(char* response) {
    char* p = strstr(response, "token");
    if (p) {
        p += strlen("token") + 3;
        char* q = strstr(p, "\"");
        if (q) {
            strncpy(token, p, q - p);
            token[q - p] = '\0';
        }
    }
}
