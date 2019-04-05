/*
 * Copyright (c) 2017, Texas Instruments Incorporated
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
 
 /*
 *  ======== httpdefines.h ========
 */
#ifndef HTTPDefines__include
#define HTTPDefines__include

#ifdef __cplusplus
extern "C" {
#endif


/*
 *
 */
#define HTTPStd_CONTINUE                           100 /*!< Informational */
#define HTTPStd_SWITCHING_PROTOCOLS                101 /*!< Informational */

#define HTTPStd_OK                                 200 /*!< Success */
#define HTTPStd_CREATED                            201 /*!< Success */
#define HTTPStd_ACCEPTED                           202 /*!< Success */
#define HTTPStd_NON_AUTHORITATIVE_INFORMATION      203 /*!< Success */
#define HTTPStd_NO_CONTENT                         204 /*!< Success */
#define HTTPStd_RESET_CONTENT                      205 /*!< Success */
#define HTTPStd_PARTIAL_CONTENT                    206 /*!< Success */
#define HTTPStd_MULTI_STATUS                       207 /*!< Success */

#define HTTPStd_MULTIPLE_CHOICES                   300 /*!< Redirection */
#define HTTPStd_MOVED_PERMANENTLY                  301 /*!< Redirection */
#define HTTPStd_FOUND                              302 /*!< Redirection */
#define HTTPStd_SEE_OTHER                          303 /*!< Redirection */
#define HTTPStd_NOT_MODIFIED                       304 /*!< Redirection */
#define HTTPStd_USE_PROXY                          305 /*!< Redirection */
#define HTTPStd_TEMPORARY_REDIRECT                 307 /*!< Redirection */

#define HTTPStd_BAD_REQUEST                        400 /*!< Client Error */
#define HTTPStd_UNAUTHORIZED                       401 /*!< Client Error */
#define HTTPStd_PAYMENT_REQUIRED                   402 /*!< Client Error */
#define HTTPStd_FORBIDDEN                          403 /*!< Client Error */
#define HTTPStd_NOT_FOUND                          404 /*!< Client Error */
#define HTTPStd_METHOD_NOT_ALLOWED                 405 /*!< Client Error */
#define HTTPStd_NOT_ACCEPTABLE                     406 /*!< Client Error */
#define HTTPStd_PROXY_AUTHENTICATION_REQUIRED      407 /*!< Client Error */
#define HTTPStd_REQUEST_TIMEOUT                    408 /*!< Client Error */
#define HTTPStd_CONFLICT                           409 /*!< Client Error */
#define HTTPStd_GONE                               410 /*!< Client Error */
#define HTTPStd_LENGTH_REQUIRED                    411 /*!< Client Error */
#define HTTPStd_PRECONDITION_FAILED                412 /*!< Client Error */
#define HTTPStd_REQUEST_ENTITY_TOO_LARGE           413 /*!< Client Error */
#define HTTPStd_REQUEST_URI_TOO_LONG               414 /*!< Client Error */
#define HTTPStd_UNSUPPORTED_MEDIA_TYPE             415 /*!< Client Error */
#define HTTPStd_REQUESTED_RANGE_NOT_SATISFAIABLE   416 /*!< Client Error */
#define HTTPStd_EXPECTATION_FAILED                 417 /*!< Client Error */
#define HTTPStd_UNPROCESSABLE_ENTITY               422 /*!< Client Error */
#define HTTPStd_TOO_MANY_REQUESTS                  429 /*!< Client Error */

#define HTTPStd_INTERNAL_SERVER_ERROR              500 /*!< Server Error */
#define HTTPStd_NOT_IMPLEMENTED                    501 /*!< Server Error */
#define HTTPStd_BAD_GATEWAY                        502 /*!< Server Error */
#define HTTPStd_SERVICE_UNAVAILABLE                503 /*!< Server Error */
#define HTTPStd_GATEWAY_TIMEOUT                    504 /*!< Server Error */
#define HTTPStd_HTTP_VERSION_NOT_SUPPORTED         505 /*!< Server Error */

#define HTTPStd_STATUS_CODE_END                    600

/*! @endcond */
/*!
 *  @name HTTP Request Methods
 *  @{
 */
#define HTTPStd_GET     "GET"
#define HTTPStd_POST    "POST"
#define HTTPStd_HEAD     "HEAD"
#define HTTPStd_OPTIONS  "OPTIONS"
#define HTTPStd_PUT      "PUT"
#define HTTPStd_DELETE   "DELETE"
#define HTTPStd_CONNECT  "CONNECT"
#define HTTPStd_PATCH    "PATCH"
/*! @} */

/*!
 *  @name HTTP Content Types
 *  @{
 */
#define HTTPStd_CONTENT_TYPE_APPLET   "application/octet-stream "
#define HTTPStd_CONTENT_TYPE_AU       "audio/au "
#define HTTPStd_CONTENT_TYPE_CSS      "text/css "
#define HTTPStd_CONTENT_TYPE_DOC      "application/msword "
#define HTTPStd_CONTENT_TYPE_GIF      "image/gif "
#define HTTPStd_CONTENT_TYPE_HTML     "text/html "
#define HTTPStd_CONTENT_TYPE_JPG      "image/jpeg "
#define HTTPStd_CONTENT_TYPE_MPEG     "video/mpeg "
#define HTTPStd_CONTENT_TYPE_PDF      "application/pdf "
#define HTTPStd_CONTENT_TYPE_WAV      "audio/wav "
#define HTTPStd_CONTENT_TYPE_ZIP      "application/zip "
#define HTTPStd_CONTENT_TYPE_PLAIN    "text/plain "
/*! @} */

/*!
 *  @name HTTP Header Fields
 *  @{
 */
#define HTTPStd_FIELD_NAME_ACCEPT               "Accept"
#define HTTPStd_FIELD_NAME_ACCEPT_CHARSET       "Accept-Charset"
#define HTTPStd_FIELD_NAME_ACCEPT_ENCODING      "Accept-Encoding"
#define HTTPStd_FIELD_NAME_ACCEPT_LANGUAGE      "Accept-Language"
#define HTTPStd_FIELD_NAME_ACCEPT_RANGES        "Accept-Ranges"
#define HTTPStd_FIELD_NAME_AGE                  "Age"
#define HTTPStd_FIELD_NAME_ALLOW                "Allow"
#define HTTPStd_FIELD_NAME_AUTHORIZATION        "Authorization"
#define HTTPStd_FIELD_NAME_CACHE_CONTROL        "Cache-Control"
#define HTTPStd_FIELD_NAME_CONNECTION           "Connection"
#define HTTPStd_FIELD_NAME_CONTENT_ENCODING     "Content-Encoding"
#define HTTPStd_FIELD_NAME_CONTENT_LANGUAGE     "Content-Language"
#define HTTPStd_FIELD_NAME_CONTENT_LENGTH       "Content-Length"
#define HTTPStd_FIELD_NAME_CONTENT_LOCATION     "Content-Location"
#define HTTPStd_FIELD_NAME_CONTENT_MD5          "Content-MD5"
#define HTTPStd_FIELD_NAME_CONTENT_RANGE        "Content-Range"
#define HTTPStd_FIELD_NAME_CONTENT_TYPE         "Content-Type"
#define HTTPStd_FIELD_NAME_COOKIE               "Cookie"
#define HTTPStd_FIELD_NAME_DATE                 "Date"
#define HTTPStd_FIELD_NAME_ETAG                 "ETag"
#define HTTPStd_FIELD_NAME_EXPECT               "Expect"
#define HTTPStd_FIELD_NAME_EXPIRES              "Expires"
#define HTTPStd_FIELD_NAME_FROM                 "From"
#define HTTPStd_FIELD_NAME_HOST                 "Host"
#define HTTPStd_FIELD_NAME_IF_MATCH             "If-Match"
#define HTTPStd_FIELD_NAME_IF_MODIFIED_SINCE    "If-Modified-Since"
#define HTTPStd_FIELD_NAME_IF_NONE_MATCH        "If-None-Match"
#define HTTPStd_FIELD_NAME_IF_RANGE             "If-Range"
#define HTTPStd_FIELD_NAME_IF_UNMODIFIED_SINCE  "If-Unmodified-Since"
#define HTTPStd_FIELD_NAME_LAST_MODIFIED        "Last-Modified"
#define HTTPStd_FIELD_NAME_LOCATION             "Location"
#define HTTPStd_FIELD_NAME_MAX_FORWARDS         "Max-Forwards"
#define HTTPStd_FIELD_NAME_ORIGIN               "Origin"
#define HTTPStd_FIELD_NAME_PRAGMA               "Pragma"
#define HTTPStd_FIELD_NAME_PROXY_AUTHENTICATE   "Proxy-Authenticate"
#define HTTPStd_FIELD_NAME_PROXY_AUTHORIZATION  "Proxy-Authorization"
#define HTTPStd_FIELD_NAME_RANGE                "Range"
#define HTTPStd_FIELD_NAME_REFERER              "Referer"
#define HTTPStd_FIELD_NAME_RETRY_AFTER          "Retry-After"
#define HTTPStd_FIELD_NAME_SERVER               "Server"
#define HTTPStd_FIELD_NAME_TE                   "TE"
#define HTTPStd_FIELD_NAME_TRAILER              "Trailer"
#define HTTPStd_FIELD_NAME_TRANSFER_ENCODING    "Transfer-Encoding"
#define HTTPStd_FIELD_NAME_UPGRADE              "Upgrade"
#define HTTPStd_FIELD_NAME_USER_AGENT           "User-Agent"
#define HTTPStd_FIELD_NAME_VARY                 "Vary"
#define HTTPStd_FIELD_NAME_VIA                  "Via"
#define HTTPStd_FIELD_NAME_WWW_AUTHENTICATE     "WWW-Authenticate"
#define HTTPStd_FIELD_NAME_WARNING              "Warning"
#define HTTPStd_FIELD_NAME_X_FORWARDED_FOR      "X-Forwarded-For"

#ifdef __cplusplus
}
#endif

#endif
