#ifndef __STEAM_H__
#define __STEAM_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <json-c/json.h>

extern char g_session_id[128];
extern char g_rfc3986[256];
extern char g_html5[256];
extern uint8_t g_debug_state;

void print_debug_information (char *, int32_t );
void init_encode_method (void);
void url_encode (const char *, char *, char *);
int8_t get_json_object_as_string (char **, struct json_object *, char *);
char *curl_general_request (char *, char *, char *, int8_t);
void base64_encode (const void *, size_t , char *, size_t *);

#endif
