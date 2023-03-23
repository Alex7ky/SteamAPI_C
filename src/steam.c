
#include <curl/curl.h>
#include <time.h>
#include <errno.h>

#include "../inc/steam.h"
#include "../inc/steamdef.h"
#include "../inc/steamglob.h"

static size_t write_memory_callback (void *, size_t, size_t, void *);

static const char encoding_table[ENCODE_TABLE_SIZE] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

/*===========================================================================*
 * Function name    : base64_encode                                          *
 *                                                                           *
 * Description      : This function Base64 encoding algorithm                *
 *                                                                           *
 * Input values(s)  : input - Input data to encode                           *
 *                    input_length - Length of the data to encode            *
 *                                                                           *
 * Output values(s) : output - NULL-terminated string encoded with           *
 *                             Base64 algorithm                              *
 *                    output_length - Length of the encoded string           *
 *                                    (optional parameter)                   *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
void base64_encode (const void *input, size_t input_length,
                    char *output, size_t *output_length)
{
	size_t n;
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	const uint8_t *p;

	print_debug_information ("Entering the function to "
	                         "base64_encode ()", __LINE__);

	/* Point to the first byte of the input data */
	p = (const uint8_t *) input;

	/* Divide the input stream into blocks of 3 bytes */
	n = input_length / 3;

	/* A full encoding quantum is always completed at the end of a quantity */
	if (input_length == (n * 3 + 1))
	{
		/* The final quantum of encoding input is exactly 8 bits */
		if (input != NULL && output != NULL)
		{
			/* Read input data */
			a = (p[n * 3] & 0xFC) >> 2;
			b = (p[n * 3] & 0x03) << 4;

			/* The final unit of encoded output will be two characters followed
			 * by two "=" padding characters */
			output[n * 4] = encoding_table[a];
			output[n * 4 + 1] = encoding_table[b];
			output[n * 4 + 2] = '=';
			output[n * 4 + 3] = '=';
			output[n * 4 + 4] = '\0';
		}

		/* Length of the encoded string (excluding the terminating NULL) */
		if (output_length != NULL)
			*output_length = n * 4 + 4;
	}
	else if (input_length == (n * 3 + 2))
	{
		/* The final quantum of encoding input is exactly 16 bits */
		if (input != NULL && output != NULL)
		{
			/* Read input data */
			a = (p[n * 3] & 0xFC) >> 2;
			b = ((p[n * 3] & 0x03) << 4) | ((p[n * 3 + 1] & 0xF0) >> 4);
			c = (p[n * 3 + 1] & 0x0F) << 2;

			/* The final unit of encoded output will be three characters followed
			 * by one "=" padding character */
			output[n * 4] = encoding_table[a];
			output[n * 4 + 1] = encoding_table[b];
			output[n * 4 + 2] = encoding_table[c];
			output[n * 4 + 3] = '=';
			output[n * 4 + 4] = '\0';
		}

		/* Length of the encoded string (excluding the terminating NULL) */
		if (output_length != NULL)
			*output_length = n * 4 + 4;
	}
	else
	{
		/* The final quantum of encoding input is an integral multiple of 24 bits */
		if (output != NULL)
		{
			/* The final unit of encoded output will be an integral multiple of 4
			 * characters with no "=" padding */
			output[n * 4] = '\0';
		}

		/* Length of the encoded string (excluding the terminating NULL) */
		if (output_length != NULL)
			*output_length = n * 4;
	}

	/* If the output parameter is NULL, then the function calculates the
	   length of the resulting Base64 string without copying any data */
	if (input != NULL && output != NULL)
	{
		/* The input data is processed block by block */
		while (n-- > 0)
		{
			/* Read input data */
			a = (p[n * 3] & 0xFC) >> 2;
			b = ((p[n * 3] & 0x03) << 4) | ((p[n * 3 + 1] & 0xF0) >> 4);
			c = ((p[n * 3 + 1] & 0x0F) << 2) | ((p[n * 3 + 2] & 0xC0) >> 6);
			d = p[n * 3 + 2] & 0x3F;

			/* Map each 3-byte block to 4 printable characters using the Base64
			   character set */
			output[n * 4] = encoding_table[a];
			output[n * 4 + 1] = encoding_table[b];
			output[n * 4 + 2] = encoding_table[c];
			output[n * 4 + 3] = encoding_table[d];
		}
	}

	print_debug_information ("Exiting the function to "
	                         "base64_encode ()", __LINE__);

	return;
}

 /*==========================================================================*
 * Function name    : init_encode_method                                     *
 *                                                                           *
 * Description      : This function init encode method rfc3986/html5         *
 *                                                                           *
 * Input values(s)  : None.                                                  *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
void init_encode_method (void)
{
	for (int num = 0; num < 256; num++)
	{
		g_rfc3986[num] = isalnum (num) || num == '~' || num == '-' || num == '.' ||
		               num == '_' ? num : 0;
		g_html5[num] = isalnum(num) || num == '*' || num == '-' || num == '.' ||
		             num == '_' ? num : (num == ' ') ? '+' : 0;
	}
}

/*===========================================================================*
 * Function name    : print_debug_information                                *
 *                                                                           *
 * Description      : This function print debug message                      *
 *                                                                           *
 * Input values(s)  : debug_message - debug message                          *
 *                    code - code debug message                              *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
void print_debug_information (char *debug_message, int32_t code)
{
	time_t timer;
	char buffer[TIMER_BUFFER_SIZE];
	struct tm* tm_info;

	time (&timer);
	tm_info = localtime (&timer);

	strftime (buffer, 26, "%d/%m/%Y %H:%M:%S", tm_info);

	if (g_debug_state == DEBUG_ENABLE)
		printf("[%s] [DEBUG %d]: %s\n", buffer, code, debug_message);
}

/*===========================================================================*
 * Function name    : url_encode                                             *
 *                                                                           *
 * Description      : This function url encode                               *
 *                                                                           *
 * Input values(s)  : url                                                    *
 *                    url_enc - url encode                                   *
 *                    method - encode method                                 *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
void url_encode (const char *url, char *url_enc, char *method)
{
	print_debug_information ("Entering the function to "
	                         "url_encode ()", __LINE__);

	for (; *url; url++) {
		if (method[(uint8_t)*url])
			sprintf(url_enc, "%c", method[(uint8_t)*url]);
		else
			sprintf(url_enc, "%%%02X", (uint8_t)*url);
		while (*++url_enc);
	}

	print_debug_information ("Exiting the function to "
	                         "url_encode ()", __LINE__);

	return;
}

/*===========================================================================*
 * Function name    : write_memory_callback                                  *
 *                                                                           *
 * Description      : This function write memory callback                    *
 *                                                                           *
 * Input values(s)  : contents                                               *
 *                    size                                                   *
 *                    nmemb                                                  *
 *                    userp                                                  *
 *                                                                           *
 * Output values(s) : real_size - NULL-terminated string                      *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
static size_t write_memory_callback (void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t real_size = size * nmemb;
	Memory *mem = (Memory *)userp;
	char *ptr = realloc (mem->memory, mem->size + real_size + 1);
	char error_message[ERROR_MESSAGE_SIZE] = {0};

	print_debug_information ("Entering the function to "
	                         "write_memory_callback ()", __LINE__);

	if (ptr == NULL)
	{
		snprintf (error_message, ERROR_MESSAGE_SIZE, "[ERROR %u] ", __LINE__);
		perror (error_message);

		return 0;
	}

	mem->memory = ptr;
	memcpy (&(mem->memory[mem->size]), contents, real_size);
	mem->size += real_size;
	mem->memory[mem->size] = 0;

	print_debug_information ("Exiting the function to "
	                         "write_memory_callback ()", __LINE__);

	return real_size;
}

/*===========================================================================*
 * Function name    : curl_general_request                                   *
 *                                                                           *
 * Description      : This function send request                             *
 *                                                                           *
 * Input values(s)  : url - url address                                      *
 *                    url_referer - url referer                              *
 *                    post_data - post data                                  *
 *                    get_cookie_flag                                        *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : Responce data                                          *
 *===========================================================================*/
char *curl_general_request (char *url, char *url_referer, char *post_data,
                            int8_t get_cookie_flag)
{
	CURL *curl;
	struct curl_slist *list = NULL;
	CURLcode curl_return_code;
	char error_buffer[CURL_ERROR_SIZE] = {0};
	char error_message[ERROR_MESSAGE_SIZE] = {0};
	Memory chunk;

	print_debug_information ("Entering the function to "
	                         "curl_general_request ()", __LINE__);

	/* Will be grown as needed by the realloc above */
	chunk.memory = malloc (MEMORY_CHUNK_SIZE);
	chunk.size = 0;

	if (chunk.memory == NULL)
	{
		snprintf (error_message, ERROR_MESSAGE_SIZE, "[ERROR %u] ", __LINE__);
		perror (error_message);

		return NULL;
	}

	curl = curl_easy_init ();

	if (curl)
	{
		list = curl_slist_append (list, "X-Requested-With:"
		                          " com.valvesoftware.android.steam.community");
		list = curl_slist_append (list, "Accept:"
		                          " text/javascript, text/html, application/xml,"
		                          " text/xml, */*");

		curl_easy_setopt (curl, CURLOPT_ENCODING, "gzip, deflate, br");

		curl_easy_setopt (curl, CURLOPT_URL, url);

		curl_easy_setopt (curl, CURLOPT_HTTPHEADER, list);

		curl_easy_setopt (curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Linux; U;"
		                  " Android 4.1.1; en-us; Google Nexus 4 - 4.1.1 -"
		                  " API 16 - 768x1280 Build/JRO03S) AppleWebKit/534.30"
		                  " (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30");

		curl_easy_setopt (curl, CURLOPT_COOKIEJAR, "cookie.txt");
		curl_easy_setopt (curl, CURLOPT_COOKIEFILE, "cookie.txt");

		curl_easy_setopt (curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt (curl, CURLOPT_SSL_VERIFYHOST, 0);

		curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt (curl, CURLOPT_MAXREDIRS, 3);

		curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt (curl, CURLOPT_WRITEDATA, &chunk);

		curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, error_buffer);

		if (url_referer != NULL)
		{
			curl_easy_setopt (curl, CURLOPT_REFERER, url_referer);
		}

		if (post_data != NULL)
		{
			curl_easy_setopt (curl, CURLOPT_POSTFIELDS, post_data);
			curl_easy_setopt (curl, CURLOPT_POSTFIELDSIZE, strlen (post_data));
		}

		/* Perform the request, curl_return_code will get the return code */ 
		curl_return_code = curl_easy_perform (curl);

		curl_slist_free_all (list);

		/* Check for errors */ 
		if (curl_return_code != CURLE_OK)
		{
			free (chunk.memory);
			curl_easy_cleanup (curl);

			snprintf (error_message, ERROR_MESSAGE_SIZE,
			          "[ERROR %u] curl_easy_perform () failed: %s",
			          __LINE__, curl_easy_strerror (curl_return_code));

			printf ("%s\n", error_message);

			return NULL;
		}

		if (get_cookie_flag == GET_COOKIE)
		{
			struct curl_slist *cookies;
			char *tmp_ptr = NULL;
			uint16_t start_index = 0;

			curl_easy_getinfo (curl, CURLINFO_COOKIELIST, &cookies);

			if (cookies)
			{
				/* A linked list of cookies in cookie file format */ 
				struct curl_slist *each = cookies;
				uint16_t i = 0;

				while (each)
				{
					tmp_ptr = strstr (each->data, "sessionid");

					if (tmp_ptr != NULL)
					{
						start_index = (tmp_ptr - each->data) + strlen ("sessionid") + 1;

						for (i = start_index; each->data[i] != '\0'; i++)
						{
							g_session_id[i - start_index] = each->data[i];
						}

						g_session_id[i - start_index] = '\0';
					}

					each = each->next;
				}
				/* We must free these cookies when we're done */
				curl_slist_free_all (cookies);
			}
		}

		/* Always cleanup */ 
		curl_easy_cleanup (curl);
	}
	else
	{
		free (chunk.memory);

		snprintf (error_message, ERROR_MESSAGE_SIZE, "[ERROR %u] ", __LINE__);
		perror (error_message);

		return NULL;
	}

	print_debug_information ("Exiting the function to "
	                         "curl_general_request ()", __LINE__);

	return chunk.memory;
}

/*===========================================================================*
 * Function name    : get_json_object_as_string                              *
 *                                                                           *
 * Description      : This function get json object as string                *
 *                                                                           *
 * Input values(s)  : ptr_data - contents of the object                      *
 *                    object - json data                                     *
 *                    get_object - object                                    *
 *                                                                           *
 * Output values(s) : None.                                                  *
 *                                                                           *
 * Return value(s)  : None.                                                  *
 *===========================================================================*/
int8_t get_json_object_as_string (char **ptr_data, struct json_object *object,
                                  char *get_object)
{
	struct json_object *tmp_object = NULL;
	char *ptr_tmp = NULL;
	uint16_t tmp_length = 0;

	print_debug_information ("Entering the function to "
	                         "get_json_object_as_string ()", __LINE__);

	json_object_object_get_ex (object, get_object, &tmp_object);

	if (tmp_object == NULL)
	{
		return FAILURE;
	}

	ptr_tmp = (char *)json_object_get_string (tmp_object);

	tmp_length = strlen (ptr_tmp) + 1;

	*ptr_data = calloc (tmp_length, sizeof (char));

	snprintf (*ptr_data, tmp_length, "%s", ptr_tmp);

	print_debug_information ("Exiting the function to "
	                         "get_json_object_as_string ()", __LINE__);

	return SUCCESS;
}
