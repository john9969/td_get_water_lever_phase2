/**
 * @file http.h
 * @brief HTTP request handling header file
 * @author Tuanndm
 * @date 2024-10-05
 */

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "esp_http_client.h"

/** @brief init  http requires,
*   @note need to Lock and Unlock mutex to protect request data.
*/
void http_init();

/** @brief post data to server
*   @param request: URl request..
*   @param post_data: data to post to server type json
*   @param size: size of data response from server
*   @note need to Lock and Unlock mutex to protect request data.
*   @return [char *] data response from server or NULL if error.
*/
char* http_request_post(const char * url, const char *post_data, int *size);

/** @brief get data from server
 *  @param Url: URl request.
 *  @param size: size of data response from server
 *  @note need to Lock and Unlock mutex to protect request data.
 *  @return [char *] data response from server or NULL if error.
 */
char* http_request_get(const char * url, int *size);

/** @brief function test post/get data to/from server
 */
void http_post_test();
void http_lock();
void http_unlock();
extern SemaphoreHandle_t http_request_mutex; // Mutex to protect request data.
#endif // HTTP_REQUEST_H