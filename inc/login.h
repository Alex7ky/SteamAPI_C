#ifndef __LOGIN_H__
#define __LOGIN_H__

extern char *g_steam_id;
extern char g_session_id[128];

int8_t steam_login (char *, char *, char *);

#endif
