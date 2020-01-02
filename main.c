#include "inc/steam.h"
#include "inc/steamdef.h"
#include "inc/login.h"
#include "inc/inventory.h"
#include "inc/market.h"

int8_t steam_input_user_data (void);

/*===========================================================================*
 * Function name    : steam_input_user_data                                  *
 *                                                                           *
 * Description      : This function allows to enter data to logging          *
 *                    in to steam account                                    *
 *                                                                           *
 * Input values(s)  : None.                                                  *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
int8_t steam_input_user_data (void)
{
	char login[LOGIN_LENGTH] = {0};
	char password[PASSWORD_LENGTH] = {0};
	char two_factor_code[TWO_FACTOR_CODE_LENGTH] = {0};
	char error_message[ERROR_MESSAGE_SIZE] = {0};

	print_debug_information ("Entering the function to "
	                         "steam_input_user_data ()", __LINE__);

	printf ("Enter login: ");
	if (scanf ("%s", login) <= 0)
	{
		return FAILURE;
	}

	printf ("Enter password: ");
	if (scanf ("%s", password) <= 0)
	{
		return FAILURE;
	}

	printf ("Enter two_factor_code: ");
	if (scanf ("%s", two_factor_code) <= 0)
	{
		return FAILURE;
	}

	if (steam_login (login, password, two_factor_code) != LOGIN_SUCCESS)
	{
		snprintf (error_message, ERROR_MESSAGE_SIZE,
		          "[ERROR %u] steam_login ()", __LINE__);
		printf ("%s\n", error_message);

		print_debug_information ("Exiting the function to "
		                         "steam_input_user_data ()", __LINE__);

		return FAILURE;
	}

	print_debug_information ("Exiting the function to "
	                         "steam_input_user_data ()", __LINE__);

	return SUCCESS;
}

int main (void)
{
	SteamInventory *steam_inventory;
	g_debug_state = DEBUG_DISABLE;

	init_encode_method ();

	if (steam_input_user_data () != LOGIN_SUCCESS)
	{
		return 0;
	}

	/* Examples */
	steam_inventory = get_inventory_items (g_steam_id);

	if (steam_inventory != NULL)
	{
		print_steam_inventory (steam_inventory);
		//sell_item (steam_inventory->inventory_items[1], "300");
		// We must free these steam_inventory when we're done
		free_steam_inventory (steam_inventory);
	}

	/*
	create_buy_order ("203770-The Khan", 0.25, 10, "753", "5");

	cancel_buy_order (2786883663);
	*/

	return 0;
}
