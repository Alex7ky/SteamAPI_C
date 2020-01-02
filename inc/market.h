#ifndef __MARKET_H__
#define __MARKET_H__

#include "steamdef.h"

extern char *g_steam_id;
extern char g_session_id[128];
extern char g_rfc3986[256];
extern char g_html5[256];

int8_t sell_item (InventoryItem, char *);
int8_t create_buy_order (char *, double, uint32_t, char *, char *);
int8_t cancel_buy_order (uint64_t buy_order_id);
int8_t load_buy_order (void);

#endif
