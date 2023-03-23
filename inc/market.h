#ifndef __MARKET_H__
#define __MARKET_H__

#include "steamdef.h"

extern char *g_steam_id;
extern char g_session_id[128];
extern char g_rfc3986[256];
extern char g_html5[256];

int8_t sell_item (InventoryItem, char *);
int8_t create_buy_order (char *, double, uint32_t, char *, char *);
int8_t cancel_buy_order (const char *);
int8_t remove_sell_order (const char *);
char *load_my_listings (void);
char *get_market_history (uint32_t, uint32_t);

#endif
