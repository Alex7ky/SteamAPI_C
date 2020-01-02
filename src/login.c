#include <openssl/rsa.h>
#include <openssl/opensslv.h>
#include <openssl/err.h>

#include "../inc/login.h"
#include "../inc/steam.h"
#include "../inc/steamdef.h"

static char *get_rsa_key (char *);
static char *encode_steam_password (char *, char *, LoginResponse *);

/*===========================================================================*
 * Function name    : free_login_response                                    *
 *                                                                           *
 * Description      : This function free memory for login response           *
 *                                                                           *
 * Input values(s)  : loginResponse                                          *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
static void free_login_response (LoginResponse *loginResponse)
{
	print_debug_information ("Entering the function to "
	                         "free_login_response ()", __LINE__);

	if (loginResponse->public_key_mod != NULL)
		free (loginResponse->public_key_mod);
	
	if (loginResponse->public_key_exp != NULL)
		free (loginResponse->public_key_exp);

	if (loginResponse->timestamp != NULL)
		free (loginResponse->timestamp);

	if (loginResponse->token_gid != NULL)
		free (loginResponse->token_gid);

	print_debug_information ("Entering the function to "
	                         "free_login_response ()", __LINE__);
}


/*===========================================================================*
 * Function name    : get_rsa_key                                            *
 *                                                                           *
 * Description      : This function get rsa key                              *
 *                                                                           *
 * Input values(s)  : login - user login                                     *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : Responce rsa key data                                  *
 *===========================================================================*/
static char *get_rsa_key (char *login)
{
	char steam_url[URL_SIZE] = {0};
	char url_referer[URL_SIZE] = {0};

	print_debug_information ("Entering the function to "
	                         "get_rsa_key ()", __LINE__);

	snprintf (steam_url, sizeof (steam_url), URL_STEAM_GET_RSA_KEY "%s", login);
	snprintf (url_referer, sizeof (url_referer), URL_STEAM_REFERER_LOGIN);

	print_debug_information ("Exiting the function to "
	                         "get_rsa_key ()", __LINE__);

	return curl_general_request (steam_url, url_referer, NULL, 0);
}

/*===========================================================================*
 * Function name    : encode_steam_password                                  *
 *                                                                           *
 * Description      : This function get rsa key                              *
 *                                                                           *
 * Input values(s)  : password                                               *
 *                    ptr_rsa_key_data                                       *
 *                    loginResponse                                          *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : Encoded password                                       *
 *===========================================================================*/
static char *encode_steam_password (char *password, char *ptr_rsa_key_data,
                                    LoginResponse *loginResponse)
{
	struct json_object *parsed_json = NULL;
	struct json_object *json_success = NULL;
	BIGNUM *modul = BN_new();
	BIGNUM *expon = BN_new();
	RSA *pubkey = RSA_new();
	int16_t rsa_length = 0;
	unsigned char encript_password[PASSWORD_SIZE] = {0};
	char base64_password[PASSWORD_SIZE] = {0};
	size_t password_length;
	char *ptr_encode_password = NULL;
	char error_message[ERROR_MESSAGE_SIZE] = {0};

	print_debug_information ("Entering the function to "
	                         "encode_steam_password ()", __LINE__);

	parsed_json = json_tokener_parse (ptr_rsa_key_data);

	free (ptr_rsa_key_data);

	if (parsed_json == NULL)
	{
		snprintf (error_message, ERROR_MESSAGE_SIZE,
		          "[ERROR %u] json_tokener_parse ()", __LINE__);
		printf ("%s\n", error_message);

		return NULL;
	}

	print_debug_information ("json_tokener_parse ()", __LINE__);

	json_object_object_get_ex (parsed_json, "success", &json_success);

	if (json_success == NULL)
	{
		snprintf (error_message, ERROR_MESSAGE_SIZE,
		          "[ERROR %u] json_object_object_get_ex ()", __LINE__);
		printf ("%s\n", error_message);

		return NULL;
	}

	if (strcmp (json_object_get_string (json_success), "true") == STRINGS_EQUAL)
	{
		loginResponse->success = SUCCESS;
	}
	else
	{
		loginResponse->success = FAILURE;
	}

	print_debug_information ("strcmp () for json_success", __LINE__);

	get_json_object_as_string (&loginResponse->public_key_mod, parsed_json, "publickey_mod");
	get_json_object_as_string (&loginResponse->public_key_exp, parsed_json, "publickey_exp");
	get_json_object_as_string (&loginResponse->timestamp, parsed_json, "timestamp");
	get_json_object_as_string (&loginResponse->token_gid, parsed_json, "token_gid");

	json_object_put (parsed_json);

	print_debug_information ("calloc () for loginResponce", __LINE__);

	if (loginResponse->public_key_mod == NULL ||
	    loginResponse->public_key_exp == NULL ||
	    loginResponse->timestamp == NULL ||
	    loginResponse->token_gid == NULL)
	{
		snprintf (error_message, ERROR_MESSAGE_SIZE, "[ERROR %u] ", __LINE__);

		return NULL;
	}

	print_debug_information ("snprintf () for loginResponce", __LINE__);

	print_debug_information ("free () json objects", __LINE__);

	if (BN_hex2bn (&modul, (const char *) loginResponse->public_key_mod) == 0)
	{
		snprintf (error_message, ERROR_MESSAGE_SIZE, "[ERROR %u] BN_hex2bn ()", __LINE__);
		printf ("%s\n", error_message);

		RSA_free (pubkey);

		return NULL;
	}

	if (BN_hex2bn (&expon, (const char *) loginResponse->public_key_exp) == 0)
	{
		snprintf (error_message, ERROR_MESSAGE_SIZE, "[ERROR %u] BN_hex2bn ()", __LINE__);
		printf ("%s\n", error_message);

		RSA_free (pubkey);

		return NULL;
	}

	if (RSA_set0_key (pubkey, modul, expon, NULL) != 1)
	{
		snprintf (error_message, ERROR_MESSAGE_SIZE, "[ERROR %u] RSA_set0_key ()", __LINE__);
		printf ("%s\n", error_message);

		RSA_free (pubkey);

		return NULL;
	}

	rsa_length = RSA_public_encrypt (strlen((const char *) password), 
	                                 (const unsigned char *) password,
	                                 (unsigned char *) encript_password,
	                                 pubkey, RSA_PKCS1_PADDING);

	print_debug_information ("RSA_public_encrypt ()", __LINE__);

	if (rsa_length <= 0)
	{
		snprintf (error_message, ERROR_MESSAGE_SIZE, "[ERROR %u] Public key %s",
		          __LINE__, ERR_error_string (ERR_get_error(), NULL ));
		printf ("%s\n", error_message);

		RSA_free (pubkey);

		return NULL;
	}
	else
	{
		base64_encode (encript_password, rsa_length, base64_password, &password_length);

		print_debug_information ("base64_encode ()", __LINE__);

		ptr_encode_password = calloc (ENCODE_PASSWORD_SIZE, sizeof (char));

		url_encode (base64_password, ptr_encode_password, g_rfc3986);

		print_debug_information ("url_encode ()", __LINE__);
	}

	RSA_free (pubkey);

	print_debug_information ("Exiting the function to "
	                         "encode_steam_password ()", __LINE__);

	return ptr_encode_password;
}


/*===========================================================================*
 * Function name    : steam_login                                            *
 *                                                                           *
 * Description      : This function logging to steam account                 *
 *                                                                           *
 * Input values(s)  : login                                                  *
 *                    password                                               *
 *                    two_factor_code                                        *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
int8_t steam_login (char *login, char *password, char *two_factor_code)
{
	char *ptr_rsa_key_data = NULL;
	char *ptr_encode_password = NULL;
	char steam_url[URL_SIZE] = {0};
	char url_referer[URL_SIZE] = {0};
	char post_data[POST_DATA_SIZE] = {0};
	unsigned long long curr_time = 0;
	char curr_time_str[TIME_SIZE] = {0};
	char *ptr_data = NULL;
	struct json_object *parsed_json = NULL;
	struct json_object *json_transfer_parameters= NULL;
	LoginResponse loginResponse;
	char *responce_success = NULL;
	int8_t return_value = LOGIN_SUCCESS;

	memset (&loginResponse, 0, sizeof (loginResponse));

	print_debug_information ("Entering the function to "
	                         "steam_login ()", __LINE__);

	ptr_rsa_key_data = get_rsa_key (login);

	if (ptr_rsa_key_data != NULL)
	{
		ptr_encode_password = encode_steam_password (password, ptr_rsa_key_data, &loginResponse);
	}
	else
	{
		return LOGIN_FAILURE;
	}

	if (ptr_encode_password == NULL)
	{
		free_login_response (&loginResponse);

		return LOGIN_FAILURE;
	}

	curr_time = (unsigned long long)time (NULL);
	curr_time = curr_time * 1000;

	sprintf (curr_time_str, "%lld", curr_time);

	snprintf (post_data, sizeof (post_data),
	         "username=" "%s" "&"
	         "password=" "%s" "&"
	         "twofactorcode=" "%s" "&"
	         "captcha_text=" "&"
	         "captchagid=" "-1" "&"
	         "emailauth=" "&"
	         "emailsteamid=" "&"
	         "remember_login=" "true" "&"
	         "rsatimestamp=" "%s" "&"
	         "donotcache=" "%s",
	         login, ptr_encode_password, two_factor_code,
			 loginResponse.timestamp, curr_time_str);

	free_login_response (&loginResponse);
	free (ptr_encode_password);

	snprintf (steam_url, sizeof (steam_url), URL_STEAM_LOGIN "%s", login);
	snprintf (url_referer, sizeof (url_referer), URL_STEAM_REFERER_LOGIN);

	ptr_data = curl_general_request (steam_url, url_referer, post_data, GET_COOKIE);

	parsed_json = json_tokener_parse (ptr_data);

	printf ("Steam Login\n");
	printf ("Response: %s\n", ptr_data);

	free (ptr_data);

	get_json_object_as_string (&responce_success, parsed_json, "success");

	if (strcmp (responce_success, "true") == STRINGS_EQUAL)
	{
		json_object_object_get_ex (parsed_json, "transfer_parameters", &json_transfer_parameters);

		get_json_object_as_string (&g_steam_id, json_transfer_parameters, "steamid");
	}
	else
	{
		return_value = LOGIN_FAILURE;
	}

	json_object_put (parsed_json);
	free (responce_success);

	print_debug_information ("Exiting the function to "
	                         "steam_login ()", __LINE__);

	return return_value;
}
