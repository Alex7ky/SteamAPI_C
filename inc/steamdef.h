#ifndef __STEAMDEF_H__
#define __STEAMDEF_H__

#include <stdint.h>
#include <stddef.h>

#define LOGIN_LENGTH 64
#define PASSWORD_LENGTH 64
#define TWO_FACTOR_CODE_LENGTH 8

#define TIMER_BUFFER_SIZE 26
#define ERROR_MESSAGE_SIZE 64
#define ENCODE_TABLE_SIZE 64
#define POST_DATA_SIZE 1024
#define MEMORY_CHUNK_SIZE 1024
#define TIME_SIZE 32
#define URL_SIZE 512
#define PASSWORD_SIZE 512
#define ENCODE_PASSWORD_SIZE PASSWORD_SIZE * 3 + 1
#define MAX_COUNT_LOAD_ITEMS  "5000"

#define SUCCESS  1
#define FAILURE  0

#define LOGIN_TRUE   1
#define LOGIN_FALSE  0

#define LOGIN_SUCCESS  SUCCESS
#define LOGIN_FAILURE  FAILURE

#define URL_STEAM_COMMUNITY         "https://steamcommunity.com/"
#define URL_STEAM_MARKET            URL_STEAM_COMMUNITY "market/"
#define URL_STEAM_GET_RSA_KEY       URL_STEAM_COMMUNITY "login/getrsakey?username="
#define URL_STEAM_LOGIN             URL_STEAM_COMMUNITY "login/dologin/?"
#define URL_STEAM_REFERER_LOGIN     URL_STEAM_COMMUNITY "login/home/?goto="
#define URL_STEAM_REFERER_BUY_ITEM  URL_STEAM_MARKET "listings/"

#define DEBUG_ENABLE   1
#define DEBUG_DISABLE  0

#define STRINGS_EQUAL  0

#define MARKETABLE_TRUE   1
#define MARKETABLE_FALSE  0

#define STR_FOUND       1
#define STR_NOT_FOUND   0

#define GET_COOKIE      1

typedef struct tMemory {
	char   *memory;
	size_t  size;
} Memory;

typedef struct tLoginResponse {
	uint8_t   success;
	char     *public_key_mod;
	char     *public_key_exp;
	char     *timestamp;
	char     *token_gid;
} LoginResponse;

typedef struct tInventoryItem {
	char   *app_id;
	char   *context_id;
	char   *asset_id;
	char   *class_id;
	char   *instance_id;
	char   *market_hash_name;
	int8_t  marketable;
} InventoryItem;

typedef struct tSteamInventory {
	char                    *app_id;
	uint16_t                 count_items;
	InventoryItem           *inventory_items;
	struct tSteamInventory  *next_steam_inventory;
} SteamInventory;

#endif
