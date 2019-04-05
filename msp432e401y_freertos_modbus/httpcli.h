/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef HTTPCli__include
#define HTTPCli__include

/*
 *  HTTP/1.1 Client API
 *
 *  This module provides an HTTP client implementation of IETF standard for
 *  HTTP/1.1 - RFC 2616 to interact with HTTP/1.1 servers.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <sys/socket.h>

/*
 *  Modify the HTTP client object's internal buffer length and rebuild the
 *  library if needed.
 */
#define HTTPCli_BUF_LEN 128

/* Minimum size required to hold content length string */
#define HTTPCli_RECV_BUFLEN 32

/*
 *  HTTP Client Error Codes
 */
/*
 *  Socket create failed
 */
#define HTTPCli_ESOCKETFAIL        (-101)

/*
 *  Cannot connect to the remote host
 */
#define HTTPCli_ECONNECTFAIL       (-102)

/*
 *  Cannot send to the remote host
 */
#define HTTPCli_ESENDFAIL          (-103)

/*
 *  Cannot recieve data from the remote host
 */
#define HTTPCli_ERECVFAIL          (-104)

/*
 *  Failed to resolve host name
 */
#define HTTPCli_EHOSTNAME          (-105)

/*
 *  Internal send buffer is not big enough
 *
 *  Modify the SEND_BUFLEN macro in the httpcli.c and rebuild the library if
 *  needed.
 */
#define HTTPCli_ESENDBUFSMALL      (-106)

/*
 *  Receive buffer is not big enough
 *
 *  The length of buffer parameter in HTTPCli_getResponseField() should
 *  be at least HTTPCli_RECV_BUFLEN.
 */
#define HTTPCli_ERECVBUFSMALL      (-107)

/*
 *  Response received from the server is not a valid HTTP/1.1 response
 */
#define HTTPCli_ERESPONSEINVALID   (-108)

/*
 *  Content length returned is too large
 *
 *  The length of buffer parameter in HTTPCli_getResponseField() is not
 *  sufficient
 */
#define HTTPCli_ECONTENTLENLARGE   (-109)

/*
 *  Data is not of chunked type
 */
#define HTTPCli_ENOTCHUNKDATA      (-110)

/*
 *  Input domain name length is too long to be read into buffer.
 *
 *  Modify the DOMAIN_BUFLEN macro in the httpcli.c and rebuild the library if
 *  needed.
 */
#define HTTPCli_EURILENLONG        (-111)

/*
 * HTTP Client Configuration Types
 */
#define HTTPCli_TYPE_IPV6  (0x04)

/*
 * HTTP Client Field ID
 * Special return codes for HTTPCli_getResponseField().
 */
#define HTTPCli_FIELD_ID_DUMMY   (-11)
#define HTTPCli_FIELD_ID_END     (-12)

/*
 *  HTTPCli request header field
 */
typedef struct HTTPCli_Field {
    const char *name;      /* Field name,  ex: "Accept" */
    const char *value;     /* Field value, ex: "text/plain" */
} HTTPCli_Field;

/*
 *  HTTPCli instance type
 */
typedef struct HTTPCli_Struct {
    char **respFields;
    unsigned int state;
    unsigned long clen;
    int sockFd;
    HTTPCli_Field *fields;

    char buf[HTTPCli_BUF_LEN];
    unsigned int buflen;
    char *bufptr;
} HTTPCli_Struct;

/*
 *  HTTPCli instance paramaters
 */
typedef struct HTTPCli_Params {
    int timeout;
    /* Timeout value (in seconds) for socket. Set 0 for default value */
} HTTPCli_Params;

typedef HTTPCli_Struct* HTTPCli_Handle;

/*
 *  Initialize the HTTPCli Params structure to default values
 *
 *  param[in]  params A pointer to the HTTPCli_Params struct
 */
extern void HTTPCli_Params_init(HTTPCli_Params *params);

/*
 *  Initialize the socket address structure for the given URI
 *
 *  The supported URI format is:
 *     - [http[s]://]host_name[:port_number][/request_uri]
 *
 *  For cases where port is not provided, the default port number is set.
 *
 *  param[out] addr   Handle to the sockaddr structure
 *
 *  param[in]  uri    A null terminated URI string
 *
 *  param[in]  flags  Set HTTPCli_TYPE_IPV6 for IPv6 addresses.
 *
 *  return 0 on success or error code on failure.
 */
extern int HTTPCli_initSockAddr(struct sockaddr *addr, const char *uri,
        int flags);

/*
 *  Create a new instance object in the provided structure
 *
 *  param[out] cli    Instance of an HTTP client
 */
extern void HTTPCli_construct(HTTPCli_Struct *cli);

/*
 *  Open a connection to an HTTP server
 *
 *  param[in]  cli    Instance of an HTTP client
 *
 *  param[in]  addr   IP address of the server. Can be NULL when proxy is set.
 *
 *  param[in]  flags  Reserved for future use
 *
 *  param[in]  params Per-instance config params, or NULL for default values
 *
 *  return 0 on success or error code on failure.
 */
extern int HTTPCli_connect(HTTPCli_Handle cli, const struct sockaddr *addr,
        int flags, const HTTPCli_Params *params);

/*
 *  Destroy the HTTP client instance
 *
 *  param[in]  cli Instance of the HTTP client
 */
extern void HTTPCli_destruct(HTTPCli_Struct *cli);

/*
 *  Disconnect from the HTTP server and destroy the HTTP client instance
 *
 *  param[in]  cli Instance of the HTTP client
 */
extern void HTTPCli_disconnect(HTTPCli_Handle cli);

/*
 *  Set an array of header fields to be sent for every HTTP request
 *
 *  param[in]  cli     Instance of an HTTP client
 *
 *  param[in]  fields  An array of HTTP request header fields terminated by
 *                      an object with NULL fields, or NULL to get
 *                      previously set array.
 *
 *  note: The array should be persistant for lifetime of the HTTP instance.
 *
 *  return previously set array
 */
extern HTTPCli_Field *HTTPCli_setRequestFields(HTTPCli_Handle cli,
        const HTTPCli_Field *fields);

/*
 *  Make an HTTP 1.1 request to the HTTP server
 *
 *  Sends an HTTP 1.1 request-line and the header fields from the user set array
 *  (see HTTPCli_setRequestFields()) to the server.
 *
 *  param[in]  cli        Instance of an HTTP client
 *
 *  param[in]  method     HTTP 1.1 method (ex: HTTPStd_GET)
 *
 *  param[in]  requestURI the path on the server to open and any CGI
 *                         parameters
 *
 *  param[in]  moreFlag   Set this flag when more fields will sent to the
 *                         server
 *
 *  return 0 on success or error code on failure
 */
extern int HTTPCli_sendRequest(HTTPCli_Handle cli, const char *method,
        const char *requestURI, bool moreFlag);

/*!
 *  Send an header field to the HTTP server
 *
 *  This is a complementary function to HTTPCli_sendRequest() when more
 *  header fields are to be sent to the server.
 *
 *  param[in]  cli      Instance of an HTTP client
 *
 *  param[in]  name     HTTP 1.1 request header field
 *                       (ex: @ref HTTPStd_FIELD_NAME_HOST)
 *
 *  param[in]  value    HTTP 1.1 request header field value
 *
 *  param[in]  lastFlag Set this flag when sending the last header field
 *
 *  return 0 on success or error code on failure
 */
extern int HTTPCli_sendField(HTTPCli_Handle cli, const char *name,
        const char *value, bool lastFlag);

/*!
 *  Send the request message body to the HTTP server
 *
 *  Make a call to this function after HTTPCli_sendRequest() (always)
 *  and HTTPCli_sendField() (if applicable).
 *
 *  param[in]  cli   Instance of an HTTP client
 *
 *  param[in]  body  Request body buffer
 *
 *  param[in]  len   Length of the request body buffer
 *
 *  return 0 on success or error code on failure
 */
extern int HTTPCli_sendRequestBody(HTTPCli_Handle cli, const char *body,
        int len);

/*
 *  Process the response header from the HTTP server and return status
 *
 *  param[in]  cli    Instance of an HTTP client
 *
 *  return  On success, "status code" from the server
 *           or error code on failure.
 */
extern int HTTPCli_getResponseStatus(HTTPCli_Handle cli);

/*
 *  Process a response header from the HTTP server and return field
 *
 *  Filters the response headers based on the array of fields
 *
 *  Repeatedly call this function until HTTPCli_FIELD_ID_END is returned.
 *
 *  param[in]  cli       Instance of an HTTP client
 *
 *  param[out] value     Field value string.
 *
 *  param[in]  len       Length of field value string
 *
 *  param[out] moreFlag  Flag set if the field value could not be completely
 *                        read into `value`. A subsequent call to this function
 *                        will read the remaining field value into `value` and
 *                        will return HTTPCli_FIELD_ID_DUMMY.
 *
 *  return On Success, the index of the field set in the
 *          HTTPCli_setResponseFields() or HTTPCli_FIELD_ID_END or
 *          HTTPCli_FIELD_ID_DUMMY, or error code on failure.
 */
extern int HTTPCli_getResponseField(HTTPCli_Handle cli, char *value,
        int len, bool *moreFlag);

/*
 *  Read the parsed response body data from the HTTP server
 *
 *  This function parses the response body if the content type is chunked
 *  transfer encoding or if the content length field is returned by the HTTP
 *  server.
 *
 *  Make a call to this function only after the call to
 *  HTTPCli_getResponseStatus() and HTTPCli_getResponseField().
 *
 *  param[in]  cli       Instance of an HTTP client
 *
 *  param[out] body      Response body buffer
 *
 *  param[in]  len       Length of response body buffer
 *
 *  param[out] moreFlag  Set if more data is available
 *
 *  return The number of characters read on success or error code on failure
 */
extern int HTTPCli_readResponseBody(HTTPCli_Handle cli, char *body,
        int len, bool *moreFlag);

/*!
 *  brief Set the proxy address
 *
 *  param[in]  addr IP address of the proxy server
 */
extern void HTTPCli_setProxy(const struct sockaddr *addr);

#ifdef __cplusplus
}
#endif

#endif
