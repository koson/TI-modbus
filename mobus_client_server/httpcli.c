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
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef __ICCARM__
#include <strings.h>
#endif

#include "httpcli.h"

#include <errno.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* Configurable lengths */
#define SEND_BUFLEN 256
#define DOMAIN_BUFLEN 64
#define MAX_FIELD_NAME_LEN 24

/* Minimum size required for HTTP status */
#define STATUS_BUFLEN 16

#define HTTP_PREFIX "http://"
#define HTTP_PORT   80

/* HTTP client instance state flags */
#define READ_FLAG       (0x01)
#define CHUNKED_FLAG    (0x04)
#define INPROGRESS_FLAG (0x08)

#define SOCKET_TIMEOUT  (-11)

#define HTTP_VER                     "HTTP/1.1"
#define FIELD_NAME_CONTENT_LENGTH    "Content-Length"
#define FIELD_NAME_TRANSFER_ENCODING "Transfer-Encoding"

/* Proxy address */
static struct sockaddr proxyAddr = {0};

static ssize_t sockRecv(int fd, void * buf, size_t len, int flags);
static ssize_t sockSend(int fd, const void * buf, size_t len,
        int flags);
static ssize_t bufferedRecv(HTTPCli_Handle cli, int sockFd,
        void *buf, size_t len, int flags);
static int checkContentField(HTTPCli_Handle cli, char *fname, char *fvalue,
        bool moreFlag);
static int getChunkedData(HTTPCli_Handle cli, char *body, int len,
        bool *moreFlag);
static int getStatus(HTTPCli_Handle cli);
static int lookUpResponseFields(HTTPCli_Handle cli, char *field);
static int readLine(HTTPCli_Handle cli, char *line, int len, bool *moreFlag);
static int skipLine(HTTPCli_Handle cli);
static int sprsend(HTTPCli_Handle cli, const char *fmt, ...);
static int readRawResponseBody(HTTPCli_Handle cli, char *body, int len);

/*
 *  ======== getCliState ========
 *  Get the state of HTTP client instance for the input flag
 */
static inline bool getCliState(HTTPCli_Handle cli, int flag)
{
    return ((cli->state) & flag);
}

/*
 *  ======== setCliState ========
 *  Set or clear the flag in the HTTP client instance
 */
static inline void setCliState(HTTPCli_Handle cli, int flag, bool value)
{
    if (value) {
        cli->state |= flag;
    }
    else {
        cli->state &= ~flag;
    }
}

/*
 *  ======== sockRecv ========
 */
static ssize_t sockRecv(int sockFd, void * buf, size_t len, int flags)
{
    ssize_t numRead;

    numRead = recv(sockFd, buf, len, flags);

    if ((numRead == -1) && (errno == EAGAIN)) {
        numRead = SOCKET_TIMEOUT;
    }

    return (numRead);
}

/*
 *  ======== sockSend ========
 */
static ssize_t sockSend(int sockFd, const void * buf, size_t len,
        int flags)
{
    ssize_t nbytes;
    ssize_t status = -1;

    while (len > 0) {
        nbytes = send(sockFd, (void *)buf, len, flags);

        if (nbytes >= 0) {
            len -= nbytes;
            buf = (uint8_t *)buf + nbytes;
            status = (status == -1) ? nbytes : (status + nbytes);
        }
        else {
            status = nbytes;
            break;
        }
    }

    return (status);
}

/*
 *  ======== bufferedRecv ========
 */
static ssize_t bufferedRecv(HTTPCli_Handle cli, int sockFd,
        void *buf, size_t len, int flags)
{
    ssize_t numRead = 0;

    if (cli->buflen) {
        if (len > cli->buflen) {
            memcpy(buf, cli->bufptr, cli->buflen);

            numRead = sockRecv(sockFd, ((uint8_t *)buf + cli->buflen),
                    (len - cli->buflen), flags);
            if (numRead >= 0) {
                numRead += cli->buflen;
                cli->buflen = 0;
            }
        }
        else {
            memcpy(buf, cli->bufptr, len);
            cli->buflen -= len;
            cli->bufptr += len;
            numRead = len;
        }
    }
    else {
        if (len > HTTPCli_BUF_LEN) {
            numRead = sockRecv(sockFd, buf, len, flags);
        }
        else {
            numRead = sockRecv(sockFd, cli->buf, HTTPCli_BUF_LEN,
                    MSG_DONTWAIT);
            if (numRead > 0) {
                cli->bufptr = cli->buf;
                cli->buflen = numRead;
            }

            if (numRead == SOCKET_TIMEOUT || (numRead >= 0 && numRead < len)) {
                if (numRead == SOCKET_TIMEOUT) {
                    numRead = 0;
                }

                numRead = sockRecv(sockFd, (cli->buf + numRead),
                        (len - numRead), flags);
                if (numRead > 0) {
                    cli->buflen += numRead;
                }
            }

            if (numRead >= 0) {
                if (len > cli->buflen) {
                    len = cli->buflen;
                }

                memcpy(buf, cli->buf, len);
                cli->buflen -= len;
                cli->bufptr += len;
                numRead = len;
            }
        }
    }

    if (numRead < 0) {
        return (HTTPCli_ERECVFAIL);
    }

    return (numRead);
}

/*
 *  ======== checkContentField ========
 *  Check field for information on content length/transfer encoding
 */
static int checkContentField(HTTPCli_Handle cli, char *fname, char *fvalue,
        bool moreFlag)
{
    const char chunk[] = "chunked";

    if ((strcasecmp(fname, FIELD_NAME_TRANSFER_ENCODING) == 0)
            && (strcasecmp(chunk, fvalue) == 0)) {
        setCliState(cli, CHUNKED_FLAG, true);
    }
    else if (strcasecmp(fname, FIELD_NAME_CONTENT_LENGTH) == 0) {
        if (moreFlag) {
            return (HTTPCli_ECONTENTLENLARGE);
        }
        cli->clen = strtoul(fvalue, NULL, 10);
    }

    return (0);
}

/*
 *  ======== getChunkedData ========
 *  Get chunked transfer encoded data
 */
static int getChunkedData(HTTPCli_Handle cli, char *body, int len,
        bool *moreFlag)
{
    bool lastFlag = true;
    bool mFlag = false;
    char crlf;
    int ret = 0;
    unsigned long chunkLen;

    if (!(getCliState(cli, CHUNKED_FLAG))) {
        return (HTTPCli_ENOTCHUNKDATA);
    }

    *moreFlag = true;

    /* Check if in the middle of reading respone body? */
    chunkLen = cli->clen;
    if (chunkLen == 0) {
        ret = readLine(cli, body, len, &mFlag);
        if (ret < 0) {
            return (ret);
        }

        chunkLen = strtoul(body, NULL, 16);
        /* don't need the remaining buffer */
        if (mFlag) {
            ret = readLine(cli, body, len, &mFlag);
            if (ret < 0) {
                return (ret);
            }
        }

        if (chunkLen == 0) {
            /* skip the rest (trailer headers) */
            do {
                ret = readLine(cli, body, len, &mFlag);
                if (ret < 0) {
                    return (ret);
                }

                /* Avoids fake do-while check */
                if (lastFlag) {
                    body[0] = 'a';
                    lastFlag = false;
                }

                if (mFlag) {
                    lastFlag = true;
                }

            }
            while (body[0] == '\0');

            *moreFlag = false;
            cli->clen = 0;

            return (0);
        }

        cli->clen = chunkLen;
    }

    if (chunkLen < len) {
        len = chunkLen;
    }

    ret = readRawResponseBody(cli, body, len);
    if (ret > 0) {
        cli->clen -= ret;
        /* Clean up the CRLF */
        if (cli->clen == 0) {
            chunkLen = readLine(cli, &crlf, sizeof(crlf), &mFlag);
            if (chunkLen != 0) {
                return (chunkLen);
            }
        }
    }

    return (ret);
}

/*
 *  ======== getStatus ========
 *  Processes the response line to get the status
 */
static int getStatus(HTTPCli_Handle cli)
{
    bool moreFlag = false;
    char statusBuf[STATUS_BUFLEN];
    int status;
    int vlen;
    int rflag;

    do {
        /* Set redirect repeat getStatus flag to zero */
        rflag = 0;

        vlen = strlen(HTTP_VER);
        status = bufferedRecv(cli, cli->sockFd, statusBuf, vlen, 0);
        if (status < 0) {
            return (status);
        }

        /* match HTTP/1.1 */
        if (strncmp(statusBuf, HTTP_VER, (vlen - 1))) {
            /* not a valid HTTP header - give up */
            return (HTTPCli_ERESPONSEINVALID);
        }
        /* get the numeric status code (and ignore the readable status) */
        status = readLine(cli, statusBuf, sizeof(statusBuf), &moreFlag);

        /* Skip the rest of the status */
        if ((status == 0) && moreFlag) {
            status = skipLine(cli);
        }

        if (status == 0) {
            status = strtoul(statusBuf, NULL, 10);
        }
    }
    while (rflag);

    return (status);
}

/*
 *  ======== lookUpResponseFields ========
 *  Is the input field in the set response field array?
 */
static int lookUpResponseFields(HTTPCli_Handle cli, char *field)
{
    char **respFields = cli->respFields;
    int index;

    if (respFields && field) {
        for (index = 0; respFields[index] != NULL; index++) {
             if (strcasecmp(field, respFields[index]) == 0) {
               return (index);
            }
        }
    }

    return (-1);
}

/*
 *  ======== readLine ========
 *  Read a header line
 */
static int readLine(HTTPCli_Handle cli, char *line, int len, bool *moreFlag)
{
    char c;
    int i = 0;
    int ret;

    *moreFlag = true;
    setCliState(cli, READ_FLAG, true);

    for (i = 0; i < len; i++) {

        ret = bufferedRecv(cli, cli->sockFd, &c, 1, 0);
        if (ret < 0) {
            *moreFlag = false;
            setCliState(cli, READ_FLAG, false);
            return (ret);
        }

        if (c == '\n') {
            line[i] = 0;
            /* Line read completed */
            *moreFlag = false;
            setCliState(cli, READ_FLAG, false);
            break;
        }
        else if ((c == '\r') || (i == 0 && c == ' ')) {
            i--;
            /* drop CR or drop leading SP*/
        }
        else {
            line[i] = c;
        }

    }

    return (0);
}

/*
 *  ======== skipLine ========
 *  Skip the rest of the header line
 */
static int skipLine(HTTPCli_Handle cli)
{
    char c = 0;
    int ret;

    while (c != '\n') {
        ret = bufferedRecv(cli, cli->sockFd, &c, 1, 0);
        if (ret < 0) {
            return (ret);
        }
    }

    return (0);
}

/*
 *  ======== sprsend ========
 *  Constructs an HTTP Request line to send
 */
static int sprsend(HTTPCli_Handle cli, const char * fmt, ...)
{
    char sendbuf[SEND_BUFLEN];
    int len = 0;
    int ret;
    int sendbuflen = sizeof(sendbuf);
    va_list ap;

    if (!getCliState(cli, INPROGRESS_FLAG)) {
        va_start(ap, fmt);
        len = vsnprintf(sendbuf, sendbuflen, fmt, ap);
        va_end(ap);
    }

    if (len > sendbuflen) {
        return (HTTPCli_ESENDBUFSMALL);
    }

    ret = sockSend(cli->sockFd, sendbuf, len, 0);
    if (ret < 0) {
        return (HTTPCli_ESENDFAIL);
    }

    return (ret);
}

/*
 *  ======== HTTPCli_Params_init ========
 */
void HTTPCli_Params_init(HTTPCli_Params *params)
{
    if (params) {
        memset(params, 0, sizeof(HTTPCli_Params));
    }
}

/*
 *  ======== HTTPCli_initSockAddr ========
 */
int HTTPCli_initSockAddr(struct sockaddr *addr, const char *uri, int flags)
{
    char luri[DOMAIN_BUFLEN];
    char *domain;
    char *delim;
    int dlen;
    int port = HTTP_PORT;
    int ret = 0;
    struct addrinfo hints;
    struct addrinfo *addrs;

    assert(addr != NULL);
    assert(uri != NULL);

    domain = (char *)uri;

    if (strncasecmp(HTTP_PREFIX, domain, (sizeof(HTTP_PREFIX) - 1))
            == 0) {
        port = HTTP_PORT;
        domain = domain + sizeof(HTTP_PREFIX) - 1;
    }

    delim = strchr(domain, ':');
    if (delim != NULL) {
        dlen = delim - domain;
        port = strtoul((delim + 1), NULL, 10);
    }
    else {
        delim = strchr(domain, '/');
        if (delim != NULL) {
            dlen = delim - domain;
        }
        else {
            dlen = strlen(domain);
        }
    }

    if (dlen >= DOMAIN_BUFLEN) {
        return (HTTPCli_EURILENLONG);
    }

    strncpy(luri, domain, dlen);
    domain = luri;
    domain[dlen] = '\0';

    memset(&hints, 0, sizeof(struct addrinfo));
    if (flags & HTTPCli_TYPE_IPV6) {
        hints.ai_family = AF_INET6;
    }
    else {
        hints.ai_family = AF_INET;
    }

    ret = getaddrinfo(domain, "0", &hints, &addrs);
    if (ret != 0) {
        return (HTTPCli_EHOSTNAME);
    }

    *addr = *(addrs->ai_addr);

    if (flags & HTTPCli_TYPE_IPV6) {
        ((struct sockaddr_in6 *)addr)->sin6_port = htons(port);
    }
    else {
        ((struct sockaddr_in *)addr)->sin_port = htons(port);
    }
    freeaddrinfo(addrs);

    return (0);
}

/*
 *  ======== HTTPCli_connect ========
 */
int HTTPCli_connect(HTTPCli_Handle cli, const struct sockaddr *addr,
        int flags, const HTTPCli_Params *params)
{
    int skt = 0;
    int ret;
    int slen;
    int sopt = 0;
    struct sockaddr *sa;

    struct timeval tv;

    assert(cli != NULL);

    if (proxyAddr.sa_family != 0) {
        sa = &proxyAddr;
    }
    else {
        assert(addr != NULL);
        sa = (struct sockaddr *)addr;
    }

    if (sa->sa_family == AF_INET6) {
        slen = sizeof(struct sockaddr_in6);
    }
    else {
        slen = sizeof(struct sockaddr_in);
    }

    if (!getCliState(cli, INPROGRESS_FLAG)) {

        skt = socket(sa->sa_family, SOCK_STREAM, sopt);
        if (skt != -1) {
            cli->sockFd = skt;

            if (params != NULL && params->timeout) {
                tv.tv_sec = params->timeout;
                tv.tv_usec = 0;
                if ((ret = setsockopt(skt, SOL_SOCKET, SO_RCVTIMEO, &tv,
                        sizeof(tv))) < 0) {
                    HTTPCli_disconnect(cli);
                    return (HTTPCli_ESOCKETFAIL);
                }
            }
        }
        else {
            return (HTTPCli_ESOCKETFAIL);
        }
    }

    ret = connect(skt, sa, slen);
    if (ret < 0) {
        HTTPCli_disconnect(cli);
        return (HTTPCli_ECONNECTFAIL);
    }
    setCliState(cli, INPROGRESS_FLAG, false);

    return (0);
}

/*
 *  ======== HTTPCli_construct ========
 */
void HTTPCli_construct(HTTPCli_Struct *cli)
{
    assert(cli != NULL);

    HTTPCli_destruct(cli);
}

/*
 *  ======== HTTPCli_destruct ========
 */
void HTTPCli_destruct(HTTPCli_Struct *cli)
{
    assert(cli != NULL);

    memset(cli, 0, sizeof(HTTPCli_Struct));
    cli->bufptr = cli->buf;
}

/*
 *  ======== HTTPCli_disconnect ========
 */
void HTTPCli_disconnect(HTTPCli_Handle cli)
{
    int skt;

    assert(cli != NULL);

    skt = cli->sockFd;
    close(skt);

    HTTPCli_destruct(cli);
}

/*
 *  ======== HTTPCli_setRequestFields ========
 */
HTTPCli_Field *HTTPCli_setRequestFields(HTTPCli_Handle cli,
        const HTTPCli_Field *fields)
{
    HTTPCli_Field *prevField;

    assert(cli != NULL);

    prevField = cli->fields;
    if (fields) {
        cli->fields = (HTTPCli_Field *)fields;
    }

    return (prevField);
}

/*
 *  ======== HTTPCli_sendRequest ========
 */
int HTTPCli_sendRequest(HTTPCli_Handle cli, const char *method,
        const char *requestURI, bool moreFlag)
{
    int i = 0;
    int ret;
    HTTPCli_Field *fields = NULL;

    assert(cli != NULL);
    assert(method != NULL);
    assert(requestURI != NULL);

    ret = sprsend(cli, "%s %s %s\r\n", method, requestURI? requestURI : "/",
            HTTP_VER);
    if (ret < 0) {
        return (ret);
    }

    if (cli->fields) {
        fields = cli->fields;
        for (i = 0; fields[i].name != NULL; i++) {
            ret = HTTPCli_sendField(cli, fields[i].name, fields[i].value,
                    false);
            if (ret < 0) {
                return (ret);
            }
        }
    }

    if (!moreFlag) {
        ret = HTTPCli_sendField(cli, NULL, NULL, true);
        if (ret < 0) {
            return (ret);
        }
    }

    return (0);
}

/*
 *  ======== HTTPCli_sendField ========
 */
int HTTPCli_sendField(HTTPCli_Handle cli, const char *name,
        const char *value, bool lastFlag)
{
    int ret;

    assert(cli != NULL);

    if (name != NULL) {
        ret = sprsend(cli, "%s: %s\r\n", name, value);
        if (ret < 0) {
            return (ret);
        }
    }

    if (lastFlag) {
        ret = sprsend(cli, "\r\n");
        if (ret < 0) {
            return (ret);
        }
    }

    return (0);
}

/*
 *  ======== HTTPCli_sendRequestBody ========
 */
int HTTPCli_sendRequestBody(HTTPCli_Handle cli, const char *body, int len)
{
    int ret;

    assert(cli != NULL);

    ret = sockSend(cli->sockFd, body, len, 0);
    if (ret < 0) {
        return (ret);
    }

    return (0);
}

/*
 *  ======== HTTPCli_getResponseStatus ========
 */
int HTTPCli_getResponseStatus(HTTPCli_Handle cli)
{
    int ret;

    assert(cli != NULL);

    ret = getStatus(cli);

    return (ret);
}

/*
 *  ======== HTTPCli_getResponseField ========
 */
int HTTPCli_getResponseField(HTTPCli_Handle cli, char *body, int len,
        bool *moreFlag)
{
    char c;
    char name[MAX_FIELD_NAME_LEN] = {0};
    int respFieldIndex = HTTPCli_FIELD_ID_DUMMY;
    int i;
    int ret;

    assert(cli != NULL);
    assert(body != NULL);
    assert(moreFlag != NULL);

    /* Minimum size required to hold content length value */
    if (len < HTTPCli_RECV_BUFLEN) {
        return (HTTPCli_ERECVBUFSMALL);
    }

    if (!(getCliState(cli, READ_FLAG))) {
        while (1) {
            for (i = 0; i < MAX_FIELD_NAME_LEN; i++) {

                ret = bufferedRecv(cli, cli->sockFd, &c, 1, 0);
                if (ret < 0) {
                    return (ret);
                }

                if (c == ':') {
                    name[i] = 0;
                    break;
                }
                else if (c == ' ' || c == '\r') {
                    i--;
                    /* drop SP */
                }
                else if (c == '\n') {
                    return (HTTPCli_FIELD_ID_END);
                }
                else {
                    name[i] = c;
                }
            }

            /* We cannot recognise this header, its too big. Skip it */
            if ((i == MAX_FIELD_NAME_LEN) && (name[i - 1] != 0)) {
                ret = skipLine(cli);
                if (ret < 0) {
                    return (ret);
                }
                continue;
            }

            respFieldIndex = lookUpResponseFields(cli, name);
            ret = readLine(cli, body, len, moreFlag);
            if (ret < 0) {
                return (ret);
            }

            ret = checkContentField(cli, name, body, *moreFlag);
            if (ret < 0) {
                return (ret);
            }

            if (respFieldIndex >= 0) {
                break;
            }

            if (*moreFlag) {
                ret = skipLine(cli);
                if (ret < 0) {
                    return (ret);
                }
            }
        }
    }
    else {
        /* Read field value */
        ret = readLine(cli, body, len, moreFlag);
        if (ret < 0) {
            return (ret);
        }
    }

    return (respFieldIndex);
}

/*
 *  ======== HTTPCli_readResponseBody ========
 */
int HTTPCli_readResponseBody(HTTPCli_Handle cli, char *body, int len,
        bool *moreFlag)
{
    int ret = 0;

    assert(cli != NULL);

    *moreFlag = false;
    if (getCliState(cli, CHUNKED_FLAG)) {
        ret = getChunkedData(cli, body, len, moreFlag);
    }
    else {
        if (cli->clen) {
            if (cli->clen < len) {
                len = cli->clen;
            }
            ret = readRawResponseBody(cli, body, len);
            if (ret > 0) {
                cli->clen -= ret;
                *moreFlag = cli->clen ? true : false;
            }
        }
    }

    return (ret);
}

/*
 *  ======== readRawResponseBody ========
 */
static int readRawResponseBody(HTTPCli_Handle cli, char *body, int len)
{
    int ret = 0;

    assert(cli != NULL);

    ret = bufferedRecv(cli, cli->sockFd, body, len, 0);

    return (ret);
}

/*
 *  ======== HTTPCli_setProxy ========
 */
void HTTPCli_setProxy(const struct sockaddr *addr)
{
    proxyAddr = *addr;
}
