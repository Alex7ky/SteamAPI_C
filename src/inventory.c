#include "../inc/inventory.h"
#include "../inc/steam.h"

static char *get_inventory (char *, char *, char *);

/*===========================================================================*
 * Function name    : get_inventory                                          *
 *                                                                           *
 * Description      : This function get inventory                            *
 *                                                                           *
 * Input values(s)  : inventory_id                                           *
 *                    count_items                                            *
 *                    last_asset_id                                          *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : Request data                                           *
 *===========================================================================*/
static char *get_inventory (char *inventory_id, char *count_items,
                            char *last_asset_id)
{
	char steam_url[URL_SIZE] = {0};
	char steam_url_referer[URL_SIZE] = {0};

	print_debug_information ("Entering the function to "
	                         "get_inventory ()", __LINE__);

	snprintf (steam_url, sizeof (steam_url), URL_STEAM_COMMUNITY
	          "inventory/%s/753/6?l=russian&count=%s&start_assetid=%s",
	          inventory_id, count_items, last_asset_id);

	snprintf (steam_url_referer, sizeof (steam_url_referer),
	          URL_STEAM_COMMUNITY "profiles/%s/inventory/", g_steam_id);

	print_debug_information ("Exiting the function to "
	                         "get_inventory ()", __LINE__);

	return curl_general_request (steam_url, steam_url_referer, NULL, 0);
}

/*===========================================================================*
 * Function name    : free_steam_inventory                                   *
 *                                                                           *
 * Description      : This function free memory for steam inventory          *
 *                                                                           *
 * Input values(s)  : steam_inventory                                        *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
void free_steam_inventory (SteamInventory *steam_inventory)
{
	for (SteamInventory *ptr_steam_inventory = steam_inventory;
	     ptr_steam_inventory != NULL;
	     ptr_steam_inventory = ptr_steam_inventory->next_steam_inventory)
	{
		for (uint16_t index = 0; index < steam_inventory->count_items; index++)
		{
			free (ptr_steam_inventory->inventory_items[index].market_hash_name);
			free (ptr_steam_inventory->inventory_items[index].class_id);
			free (ptr_steam_inventory->inventory_items[index].app_id);
			free (ptr_steam_inventory->inventory_items[index].context_id);
			free (ptr_steam_inventory->inventory_items[index].asset_id);
			free (ptr_steam_inventory->inventory_items[index].instance_id);
		}

		free (ptr_steam_inventory->inventory_items);
	}

	free (steam_inventory);
}

/*===========================================================================*
 * Function name    : print_steam_inventory                                  *
 *                                                                           *
 * Description      : This function print all item steam inventory           *
 *                                                                           *
 * Input values(s)  : steam_inventory                                        *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
void print_steam_inventory (SteamInventory *steam_inventory)
{
	for (SteamInventory *ptr_steam_inventory = steam_inventory;
	     ptr_steam_inventory != NULL;
	     ptr_steam_inventory = ptr_steam_inventory->next_steam_inventory)
	{
		for (uint16_t index = 0; index < steam_inventory->count_items; index++)
		{
			printf ("\nmarket_hash_name: %s\n",
			        ptr_steam_inventory->inventory_items[index].market_hash_name);
			printf ("class_id: %s\n",
			        ptr_steam_inventory->inventory_items[index].class_id);
			printf ("app_id: %s\n",
			        ptr_steam_inventory->inventory_items[index].app_id);
			printf ("context_id: %s\n",
			        ptr_steam_inventory->inventory_items[index].context_id);
			printf ("asset_id: %s\n",
			        ptr_steam_inventory->inventory_items[index].asset_id);
			printf ("instance_id: %s\n",
			        ptr_steam_inventory->inventory_items[index].instance_id);
			printf ("marketable: %d\n",
			         ptr_steam_inventory->inventory_items[index].marketable);
		}

		printf ("\ncount_items: %d\n", ptr_steam_inventory->count_items);
	}
}

/*===========================================================================*
 * Function name    : get_inventory_items                                    *
 *                                                                           *
 * Description      : This function get inventory items                      *
 *                                                                           *
 * Input values(s)  : inventory_id                                           *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
SteamInventory *get_inventory_items (char *inventory_id)
{
	struct json_object *parsed_json = NULL;
	struct json_object *json_assets = NULL;
	struct json_object *json_descriptions = NULL;
	struct json_object *json_marketable = NULL;
	struct json_object *json_asset_entry = NULL;
	struct json_object *json_descriptions_entry = NULL;
	struct json_object *json_class_id = NULL;
	struct json_object *json_instance_id = NULL;
	char *ptr_class_id = NULL;
	char *ptr_instance_id = NULL;
	char *ptr_marketable = NULL;
	char *ptr_data = NULL;
	char *ptr_last_asset_id = NULL;
	char *ptr_total_inventory_count = NULL;
	char  last_asset_id[32] = {0};
	int8_t return_value = FAILURE;
	uint32_t size_assets_table = 0;
	uint32_t start_assets_index = 0;
	uint32_t count_all_items = 0;
	uint32_t size_descriptions_table = 0;
	uint32_t tmp_index = 0;
	uint32_t total_inventory_count = 0;
	InventoryItem *inventory_items = NULL;
	SteamInventory *steam_inventory = NULL;

	print_debug_information ("Entering the function to "
	                         "get_inventory_items ()", __LINE__);

	snprintf (last_asset_id, sizeof (last_asset_id), "0");

	ptr_data = get_inventory (inventory_id, MAX_COUNT_LOAD_ITEMS, last_asset_id);

	parsed_json = json_tokener_parse (ptr_data);

	free (ptr_data);

	get_json_object_as_string (&ptr_total_inventory_count,
	                           parsed_json,
	                           "total_inventory_count");

	total_inventory_count = atoi (ptr_total_inventory_count);

	free (ptr_total_inventory_count);

	inventory_items = calloc (total_inventory_count, sizeof (InventoryItem));
	steam_inventory = calloc (1, sizeof (SteamInventory));

	do
	{
		if (start_assets_index > 0)
		{
			ptr_data = get_inventory (inventory_id, MAX_COUNT_LOAD_ITEMS, last_asset_id);
			parsed_json = json_tokener_parse (ptr_data);
			free (ptr_data);
		}

		json_object_object_get_ex (parsed_json, "assets", &json_assets);
		json_object_object_get_ex (parsed_json, "descriptions", &json_descriptions);

		return_value = get_json_object_as_string (&ptr_last_asset_id,
		                                          parsed_json,
		                                          "last_assetid");

		if (ptr_last_asset_id != NULL && return_value == SUCCESS)
		{
			memset (last_asset_id, 0, 32);
			strncpy (last_asset_id, ptr_last_asset_id, 32);
			free (ptr_last_asset_id);
		}

		size_assets_table = json_object_array_length (json_assets);

		count_all_items += size_assets_table;

		size_descriptions_table = json_object_array_length (json_descriptions);

		if (size_assets_table <= 0 || size_descriptions_table <= 0)
		{
			json_object_put (parsed_json);

			return NULL;
		}

		for (uint16_t index_item = start_assets_index; index_item < count_all_items; index_item++)
		{
			tmp_index = index_item - start_assets_index;

			json_asset_entry = json_object_array_get_idx (json_assets, tmp_index);

			get_json_object_as_string (&inventory_items[index_item].app_id,
			                           json_asset_entry, "appid");
			get_json_object_as_string (&inventory_items[index_item].context_id,
			                           json_asset_entry, "contextid");
			get_json_object_as_string (&inventory_items[index_item].asset_id,
			                           json_asset_entry, "assetid");
			get_json_object_as_string (&inventory_items[index_item].class_id,
			                           json_asset_entry, "classid");
			get_json_object_as_string (&inventory_items[index_item].instance_id,
			                           json_asset_entry, "instanceid");
		}

		for (uint16_t index_item = 0; index_item < size_descriptions_table; index_item++)
		{
			json_descriptions_entry = json_object_array_get_idx (json_descriptions, index_item);

			json_object_object_get_ex (json_descriptions_entry, "classid", &json_class_id);
			json_object_object_get_ex (json_descriptions_entry, "instanceid", &json_instance_id);
			json_object_object_get_ex (json_descriptions_entry, "marketable", &json_marketable);
		
			ptr_class_id = (char *)json_object_get_string (json_class_id);

			ptr_instance_id = (char *)json_object_get_string (json_instance_id);

			ptr_marketable = NULL;

			for (uint16_t index_asset = start_assets_index; index_asset < count_all_items; index_asset++)
			{
				if ((strcmp (ptr_class_id, inventory_items[index_asset].class_id) == STRINGS_EQUAL) &&
					(strcmp (ptr_instance_id, inventory_items[index_asset].instance_id) == STRINGS_EQUAL))
				{
					ptr_marketable = (char *)json_object_get_string (json_marketable);

					get_json_object_as_string (&inventory_items[index_asset].market_hash_name,
					                           json_descriptions_entry, "market_hash_name");

					if (strcmp (ptr_marketable, "1") == STRINGS_EQUAL)
					{
						inventory_items[index_asset].marketable = MARKETABLE_TRUE;
					}
					else
					{
						inventory_items[index_asset].marketable = MARKETABLE_FALSE;
					}
				}
			}
		}

		start_assets_index = count_all_items;

		json_object_put (parsed_json);

	} while (return_value == SUCCESS);

	steam_inventory->count_items = count_all_items;
	steam_inventory->inventory_items = inventory_items;

	print_debug_information ("Exiting the function to "
	                         "get_inventory_items ()", __LINE__);

	return steam_inventory;
}
