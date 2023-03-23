#include "../inc/market.h"
#include "../inc/steam.h"

/*===========================================================================*
 * Function name    : sell_item                                              *
 *                                                                           *
 * Description      : This function sell item                                *
 *                                                                           *
 * Input values(s)  : inventory_item                                         *
 *                    price_item                                             *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : SUCCESS/FAILURE                                        *
 *===========================================================================*/
int8_t sell_item (InventoryItem inventory_item, char *price_item)
{
	char steam_sell_url[URL_SIZE] = {0};
	char steam_url_referer[URL_SIZE] = {0};
	char post_data[POST_DATA_SIZE] = {0};
	char *ptr_data = NULL;

	print_debug_information ("Entering the function to "
	                         "sell_item ()", __LINE__);

	snprintf (steam_sell_url, sizeof (steam_sell_url), URL_STEAM_COMMUNITY
	          "market/sellitem/");

	snprintf (steam_url_referer, sizeof (steam_url_referer),
	          URL_STEAM_COMMUNITY "profiles/%s/inventory/", g_steam_id);

	snprintf (post_data, sizeof (post_data),
		      "sessionid=" "%s" "&"
		      "appid=" "%s" "&"
		      "contextid=" "%s" "&"
		      "assetid=" "%s" "&"
		      "amount=" "1" "&"
		      "price=" "%s",
		      g_session_id, inventory_item.app_id, inventory_item.context_id,
		      inventory_item.asset_id, price_item);

	ptr_data = curl_general_request (steam_sell_url, steam_url_referer, post_data, 0);

	printf ("Sell item\n");
	printf ("Response: %s\n", ptr_data);
	
	free (ptr_data);

	print_debug_information ("Exiting the function to "
	                         "sell_item ()", __LINE__);

	return SUCCESS;
}

/*===========================================================================*
 * Function name    : create_buy_order                                       *
 *                                                                           *
 * Description      : This function create buy order                         *
 *                                                                           *
 * Input values(s)  : market_hash_name                                       *
 *                    price_item                                             *
 *                    quantity                                               *
 *                    appid                                                  *
 *                    currency                                               *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : SUCCESS/FAILURE                                        *
 *===========================================================================*/
int8_t create_buy_order (char *market_hash_name, double price_item,
                         uint32_t quantity, char *appid, char *currency)
{
	char steam_buy_url[URL_SIZE] = {0};
	char steam_url_referer[URL_SIZE] = {0};
	char post_data[POST_DATA_SIZE] = {0};
	char *ptr_data = NULL;
	uint32_t price_total = 0.0;
	char str_price_total[20] = {0};
	char str_quantity[16] = {0};
	char *ptr_hash_name_encode = NULL;

	print_debug_information ("Entering the function to "
	                         "create_buy_order ()", __LINE__);

	price_total = price_item * quantity * 100;

	snprintf (str_price_total, sizeof (str_price_total), "%u", price_total);
	snprintf (str_quantity, sizeof (str_quantity), "%u", quantity);

	ptr_hash_name_encode = calloc (strlen (market_hash_name) * 3 + 1,
	                               sizeof (char));

	if (ptr_hash_name_encode == NULL)
	{
		return FAILURE;
	}

	snprintf (steam_buy_url, sizeof (steam_buy_url), URL_STEAM_COMMUNITY
	          "market/createbuyorder/");

	url_encode (market_hash_name, ptr_hash_name_encode, g_rfc3986);

	snprintf (steam_url_referer, sizeof (steam_url_referer),
	          URL_STEAM_REFERER_BUY_ITEM "%s/%s/", appid,
	          ptr_hash_name_encode);

	memset (ptr_hash_name_encode, 0, strlen (ptr_hash_name_encode));

	url_encode (market_hash_name, ptr_hash_name_encode, g_html5);

	snprintf (post_data, sizeof (post_data),
		      "sessionid=" "%s" "&"
		      "currency=" "%s" "&"
		      "appid=" "%s" "&"
		      "market_hash_name=" "%s" "&"
		      "price_total=" "%s" "&"
		      "quantity=" "%s",
		      g_session_id, currency, appid, ptr_hash_name_encode,
		      str_price_total, str_quantity);

	ptr_data = curl_general_request (steam_buy_url, steam_url_referer,
	                                 post_data, 0);

	printf ("Buy item\n");
	printf ("Response: %s\n", ptr_data);
	
	free (ptr_data);
	free (ptr_hash_name_encode);

	print_debug_information ("Exiting the function to "
	                         "create_buy_order ()", __LINE__);

	return SUCCESS;
}

/*===========================================================================*
 * Function name    : cancel_buy_order                                       *
 *                                                                           *
 * Description      : This function cancel buy order                         *
 *                                                                           *
 * Input values(s)  : buy_order_id                                           *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : SUCCESS/FAILURE                                        *
 *===========================================================================*/
int8_t cancel_buy_order (const char *buy_order_id)
{
	char steam_cancel_buy_order_url[URL_SIZE] = {0};
	char steam_url_referer[URL_SIZE] = {0};
	char post_data[POST_DATA_SIZE] = {0};
	char *ptr_data = NULL;

	print_debug_information ("Entering the function to "
	                         "cancel_buy_order ()", __LINE__);

	snprintf (steam_cancel_buy_order_url, sizeof (steam_cancel_buy_order_url),
	          URL_STEAM_MARKET "cancelbuyorder/");

	snprintf (steam_url_referer, sizeof (steam_url_referer), URL_STEAM_MARKET);

	snprintf (post_data, sizeof (post_data),
		      "sessionid=" "%s" "&"
		      "buy_orderid=" "%s",
		      g_session_id, buy_order_id);

	ptr_data = curl_general_request (steam_cancel_buy_order_url,
	                                 steam_url_referer, post_data, 0);

	if (ptr_data != NULL)
	{
		free (ptr_data);
	}

	print_debug_information ("Exiting the function to "
	                         "cancel_buy_order ()", __LINE__);

	return SUCCESS;
}

/*===========================================================================*
 * Function name    : remove_sell_order                                      *
 *                                                                           *
 * Description      : This function cancel buy order                         *
 *                                                                           *
 * Input values(s)  : sell_order_id                                          *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : SUCCESS/FAILURE                                        *
 *===========================================================================*/
int8_t remove_sell_order (const char *sell_order_id)
{
	char steam_url[URL_SIZE] = {0};
	char steam_url_referer[URL_SIZE] = {0};
	char *ptr_data = NULL;
	char post_data[POST_DATA_SIZE] = {0};

	print_debug_information ("Entering the function to "
	                         "remove_sell_order ()", __LINE__);

	snprintf (steam_url, sizeof (steam_url), URL_STEAM_MARKET
	          "removelisting/%s", sell_order_id);

	snprintf (steam_url_referer, sizeof (steam_url_referer),
	          URL_STEAM_MARKET);

	snprintf (post_data, sizeof (post_data), "sessionid=" "%s", g_session_id);

	ptr_data = curl_general_request (steam_url, steam_url_referer, post_data, 0);

	if (ptr_data != NULL)
	{
		free (ptr_data);
	}

	print_debug_information ("Exiting the function to "
	                         "remove_sell_order ()", __LINE__);

	return SUCCESS;
}

/*===========================================================================*
 * Function name    : load_my_listings                                       *
 *                                                                           *
 * Description      : This function load buy order                           *
 *                                                                           *
 * Input values(s)  : inventory_item                                         *
 *                    price_item                                             *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : ptr_data                                               *
 *===========================================================================*/
char *load_my_listings (void)
{
	char steam_buy_url[URL_SIZE] = {0};
	char steam_url_referer[URL_SIZE] = {0};
	char *ptr_data = NULL;

	print_debug_information ("Entering the function to "
	                         "load_my_listings ()", __LINE__);

	snprintf (steam_buy_url, sizeof (steam_buy_url),
	          "https://steamcommunity.com/market/mylistings?start=0&count=100");

	snprintf (steam_url_referer, sizeof (steam_url_referer),
	          URL_STEAM_MARKET);

	ptr_data = curl_general_request (steam_buy_url, steam_url_referer, NULL, 0);

	print_debug_information ("Exiting the function to "
	                         "load_my_listings ()", __LINE__);

	return ptr_data;
}

/*===========================================================================*
 * Function name    : get_market_history                                     *
 *                                                                           *
 * Description      : This function get market history                       *
 *                                                                           *
 * Input values(s)  : count_items                                            *
 *                    start_item                                             *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
char *get_market_history (uint32_t count_items, uint32_t start_item)
{
	char steam_url[URL_SIZE] = {0};
	char steam_url_referer[URL_SIZE] = {0};

	print_debug_information ("Entering the function to "
	                         "get_inventory ()", __LINE__);

	snprintf (steam_url, sizeof (steam_url), URL_STEAM_COMMUNITY
	          "market/myhistory/render/?query=&start=%u&count=%u&l=english", start_item, count_items);

	snprintf (steam_url_referer, sizeof (steam_url_referer),
	          URL_STEAM_MARKET);

	print_debug_information ("Exiting the function to "
	                         "get_inventory ()", __LINE__);

	return curl_general_request (steam_url, steam_url_referer, NULL, 0);
}
