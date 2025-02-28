#include "requests.h"
#include "helpers.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "parson.h"

// Send a HTTP request to the server
char* send_http_request(char* message) {
    int sock = open_connection(DESTIONATION_IP, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sock, message);
    char* response = receive_from_server(sock);
    close_connection(sock);

    return response;
}

// Compute the message for a HTTP request
// The function is generic, it can be used to create any type of request
char* compute_http_request(enum HttpMethod method, const char *url, 
                           const char *query_params, const char *content_type, 
                           JSON_Value *body_data, const char *cookie, const char *token) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = NULL;

    // Determine the request method
    switch (method) {
        case GET:
            if (query_params != NULL) {
                sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
            } else {
                sprintf(line, "GET %s HTTP/1.1", url);
            }
            break;
        case POST:
            sprintf(line, "POST %s HTTP/1.1", url);
            break;
        case DELETE:
            sprintf(line, "DELETE %s HTTP/1.1", url);
            break;
    }
    compute_message(message, line);

    // Add Host header
    sprintf(line, "Host: %s", DESTIONATION_IP);
    compute_message(message, line);

    // Add Content-Type header for POST request
    if (method == POST && content_type != NULL) {
        sprintf(line, "Content-type: %s", content_type);
        compute_message(message, line);
    }

    // Add Cookie header if present
    if (cookie != NULL) {
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }

    // Add Authorization header if present
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Prepare body data for POST request
    if (method == POST && body_data != NULL) {
        body_data_buffer = json_serialize_to_string(body_data);
        int content_len = strlen(body_data_buffer);

        sprintf(line, "Content-length: %d", content_len);
        compute_message(message, line);
    }

    // End headers section
    compute_message(message, "");

    // Append body data for POST request
    if (method == POST && body_data_buffer != NULL) {
        strcat(message, body_data_buffer);
        free(body_data_buffer);
    }

    free(line);

    return message;
}
