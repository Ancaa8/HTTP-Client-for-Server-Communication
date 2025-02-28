#ifndef _REQUESTS_
#define _REQUESTS_

#define DESTIONATION_IP "34.246.184.49"
#define PORT 8080

#include "parson.h"

enum HttpMethod { GET, POST, DELETE };

char* send_http_request(char* message);
char *compute_http_request(enum HttpMethod method, const char *url, 
						   const char *query_params, const char *content_type, 
						   JSON_Value *body_data, const char *cookie, const char *token);
#endif // REQUESTS_H