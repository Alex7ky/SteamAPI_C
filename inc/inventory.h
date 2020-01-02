#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include "steamdef.h"

extern char *g_steam_id;
extern char g_rfc3986[256];

SteamInventory *get_inventory_items (char *);
void free_steam_inventory (SteamInventory *);
void print_steam_inventory (SteamInventory *);

#endif
