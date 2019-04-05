/**
 * @file tls_server.c
 * @brief Handshake message processing (TLS server)
 *
 * @section License
 *
 * Copyright (C) 2010-2018 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneSSL Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @section Description
 *
 * The TLS protocol provides communications security over the Internet. The
 * protocol allows client/server applications to communicate in a way that
 * is designed to prevent eavesdropping, tampering, or message forgery
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.8.6
 **/

//Switch to the appropriate trace level
#define TRACE_LEVEL TLS_TRACE_LEVEL

//Dependencies
#include <string.h>
#include "tls.h"
#include "tls_cipher_suites.h"
#include "tls_handshake_hash.h"
#include "tls_handshake_misc.h"
#include "tls_server.h"
#include "tls_server_misc.h"
#include "tls_common.h"
#include "tls_record.h"
#include "tls_signature.h"
#include "tls_key_material.h"
#include "tls_cache.h"
#include "tls_ffdhe.h"
#include "tls_misc.h"
#include "dtls_record.h"
#include "dtls_misc.h"
#include "certificate/pem_import.h"
#include "certificate/x509_cert_parse.h"
#include "date_time.h"
#include "debug.h"

//Check SSL library configuration
#if (TLS_SUPPORT == ENABLED && TLS_SERVER_SUPPORT == ENABLED)


/**
 * @brief Send ServerHello message
 *
 * The server will send this message in response to a ClientHello
 * message when it was able to find an acceptable set of algorithms.
 * If it cannot find such a match, it will respond with a handshake
 * failure alert
 *
 * @param[in] context Pointer to the TLS context
 * @return Error code
 **/

error_t tlsSendServerHello(TlsContext *context)
{
   error_t error;
   size_t length;
   TlsServerHello *message;

   //Point to the buffer where to format the message
   message = (TlsServerHello *) (context->txBuffer + context->txBufferLen);

   //Generate the server random value using a cryptographically-safe
   //pseudorandom number generator
   error = tlsGenerateRandomValue(context, &context->serverRandom);

   //Check status code
   if(!error)
   {
      //Format ServerHello message
      error = tlsFormatServerHello(context, message, &length);
   }

   //Check status code
   if(!error)
   {
      //Debug message
      TRACE_INFO("Sending ServerHello message (%" PRIuSIZE " bytes)...\r\n", length);
      TRACE_DEBUG_ARRAY("  ", message, length);

      //Send handshake message
      error = tlsSendHandshakeMessage(context, message, length,
         TLS_TYPE_SERVER_HELLO);
   }

   //Check status code
   if(error == NO_ERROR || error == ERROR_WOULD_BLOCK || error == ERROR_TIMEOUT)
   {
#if (TLS_SESSION_RESUME_SUPPORT == ENABLED)
      //Use abbreviated handshake?
      if(context->resume)
      {
         //Derive session keys from the master secret
         error = tlsGenerateSessionKeys(context);

         //Key material successfully generated?
         if(!error)
         {
            //At this point, both client and server must send ChangeCipherSpec
            //messages and proceed directly to Finished messages
            context->state = TLS_STATE_SERVER_CHANGE_CIPHER_SPEC;
         }
      }
      else
#endif
      {
         //Perform a full handshake
         context->state = TLS_STATE_SERVER_CERTIFICATE;
      }
   }

   //Return status code
   return error;
}


/**
 * @brief Send ServerKeyExchange message
 *
 * The ServerKeyExchange message is sent by the server only when the
 * server Certificate message does not contain enough data to allow
 * the client to exchange a premaster secret
 *
 * @param[in] context Pointer to the TLS context
 * @return Error code
 **/

error_t tlsSendServerKeyExchange(TlsContext *context)
{
   error_t error;
   size_t length;
   TlsServerKeyExchange *message;

   //Initialize status code
   error = NO_ERROR;

   //Point to the buffer where to format the message
   message = (TlsServerKeyExchange *) (context->txBuffer + context->txBufferLen);
   //Initialize length
   length = 0;

   //The ServerKeyExchange message is sent by the server only when the server
   //Certificate message (if sent) does not contain enough data to allow the
   //client to exchange a premaster secret
   if(context->keyExchMethod == TLS_KEY_EXCH_DH_ANON ||
      context->keyExchMethod == TLS_KEY_EXCH_DHE_RSA ||
      context->keyExchMethod == TLS_KEY_EXCH_DHE_DSS ||
      context->keyExchMethod == TLS_KEY_EXCH_DHE_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDH_ANON ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDHE_RSA ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDHE_ECDSA ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDHE_PSK)
   {
      //Format ServerKeyExchange message
      error = tlsFormatServerKeyExchange(context, message, &length);
   }
   else if(context->keyExchMethod == TLS_KEY_EXCH_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_RSA_PSK)
   {
#if (TLS_PSK_SUPPORT == ENABLED || TLS_RSA_PSK_SUPPORT == ENABLED || \
   TLS_DHE_PSK_SUPPORT == ENABLED || TLS_ECDHE_PSK_SUPPORT == ENABLED)
      //If no PSK identity hint is provided by the server, the
      //ServerKeyExchange message is omitted...
      if(context->pskIdentityHint != NULL)
      {
         //Format ServerKeyExchange message
         error = tlsFormatServerKeyExchange(context, message, &length);
      }
#endif
   }

   //Check status code
   if(!error)
   {
      //Any message to send?
      if(length > 0)
      {
         //Debug message
         TRACE_INFO("Sending ServerKeyExchange message (%" PRIuSIZE " bytes)...\r\n", length);
         TRACE_DEBUG_ARRAY("  ", message, length);

         //Send handshake message
         error = tlsSendHandshakeMessage(context, message, length,
            TLS_TYPE_SERVER_KEY_EXCHANGE);
      }
   }

   //Check status code
   if(error == NO_ERROR || error == ERROR_WOULD_BLOCK || error == ERROR_TIMEOUT)
   {
      //Prepare to send a CertificateRequest message...
      context->state = TLS_STATE_CERTIFICATE_REQUEST;
   }

   //Return status code
   return error;
}


/**
 * @brief Send CertificateRequest message
 *
 * A server can optionally request a certificate from the client, if
 * appropriate for the selected cipher suite. This message will
 * immediately follow the ServerKeyExchange message
 *
 * @param[in] context Pointer to the TLS context
 * @return Error code
 **/

error_t tlsSendCertificateRequest(TlsContext *context)
{
   error_t error;
   size_t length;
   TlsCertificateRequest *message;

   //Initialize status code
   error = NO_ERROR;

#if (TLS_RSA_SIGN_SUPPORT == ENABLED || TLS_RSA_PSS_SIGN_SUPPORT == ENABLED || \
   TLS_DSA_SIGN_SUPPORT == ENABLED || TLS_ECDSA_SIGN_SUPPORT == ENABLED)
   //A server can optionally request a certificate from the client
   if(context->clientAuthMode != TLS_CLIENT_AUTH_NONE)
   {
      //Non-anonymous key exchange?
      if(context->keyExchMethod == TLS_KEY_EXCH_RSA ||
         context->keyExchMethod == TLS_KEY_EXCH_DHE_RSA ||
         context->keyExchMethod == TLS_KEY_EXCH_DHE_DSS ||
         context->keyExchMethod == TLS_KEY_EXCH_ECDHE_RSA ||
         context->keyExchMethod == TLS_KEY_EXCH_ECDHE_ECDSA ||
         context->keyExchMethod == TLS_KEY_EXCH_RSA_PSK)
      {
         //Point to the buffer where to format the message
         message = (TlsCertificateRequest *) (context->txBuffer + context->txBufferLen);

         //Format CertificateRequest message
         error = tlsFormatCertificateRequest(context, message, &length);

         //Check status code
         if(!error)
         {
            //Debug message
            TRACE_INFO("Sending CertificateRequest message (%" PRIuSIZE " bytes)...\r\n", length);
            TRACE_DEBUG_ARRAY("  ", message, length);

            //Send handshake message
            error = tlsSendHandshakeMessage(context, message, length,
               TLS_TYPE_CERTIFICATE_REQUEST);
         }
      }
   }
#endif

   //Check status code
   if(error == NO_ERROR || error == ERROR_WOULD_BLOCK || error == ERROR_TIMEOUT)
   {
      //Prepare to send a ServerHelloDone message...
      context->state = TLS_STATE_SERVER_HELLO_DONE;
   }

   //Return status code
   return error;
}


/**
 * @brief Send ServerHelloDone message
 *
 * The ServerHelloDone message is sent by the server to indicate the
 * end of the ServerHello and associated messages. After sending this
 * message, the server will wait for a client response
 *
 * @param[in] context Pointer to the TLS context
 * @return Error code
 **/

error_t tlsSendServerHelloDone(TlsContext *context)
{
   error_t error;
   size_t length;
   TlsServerHelloDone *message;

   //Point to the buffer where to format the message
   message = (TlsServerHelloDone *) (context->txBuffer + context->txBufferLen);

   //Format ServerHelloDone message
   error = tlsFormatServerHelloDone(context, message, &length);

   //Check status code
   if(!error)
   {
      //Debug message
      TRACE_INFO("Sending ServerHelloDone message (%" PRIuSIZE " bytes)...\r\n", length);
      TRACE_DEBUG_ARRAY("  ", message, length);

      //Send handshake message
      error = tlsSendHandshakeMessage(context, message, length,
         TLS_TYPE_SERVER_HELLO_DONE);
   }

   //Check status code
   if(error == NO_ERROR || error == ERROR_WOULD_BLOCK || error == ERROR_TIMEOUT)
   {
      //The client must send a Certificate message if the server requests it
      if(context->clientAuthMode != TLS_CLIENT_AUTH_NONE)
         context->state = TLS_STATE_CLIENT_CERTIFICATE;
      else
         context->state = TLS_STATE_CLIENT_KEY_EXCHANGE;
   }

   //Return status code
   return error;
}


/**
 * @brief Format ServerHello message
 * @param[in] context Pointer to the TLS context
 * @param[out] message Buffer where to format the ServerHello message
 * @param[out] length Length of the resulting ServerHello message
 * @return Error code
 **/

error_t tlsFormatServerHello(TlsContext *context,
   TlsServerHello *message, size_t *length)
{
   error_t error;
   uint16_t version;
   size_t n;
   uint8_t *p;
   TlsExtensionList *extensionList;

   //Retrieve the TLS version that has been negotiated
   version = context->version;

#if (DTLS_SUPPORT == ENABLED)
   //DTLS protocol?
   if(context->transportProtocol == TLS_TRANSPORT_PROTOCOL_DATAGRAM)
   {
      //Get the corresponding DTLS version
      version = dtlsTranslateVersion(version);
   }
#endif

   //This field contains the lower of the version suggested by the client
   //in the ClientHello and the highest supported by the server
   message->serverVersion = htons(version);

   //Server random value
   message->random = context->serverRandom;

   //Point to the session ID
   p = message->sessionId;
   //Length of the handshake message
   *length = sizeof(TlsServerHello);

#if (TLS_SESSION_RESUME_SUPPORT == ENABLED)
   //The SessionID uniquely identifies the current session
   message->sessionIdLen = (uint8_t)context->sessionIdLen;
   memcpy(message->sessionId, context->sessionId, context->sessionIdLen);
#else
   //The server may return an empty session ID to indicate that the session
   //will not be cached and therefore cannot be resumed
   message->sessionIdLen = 0;
#endif

   //Debug message
   TRACE_INFO("Session ID (%" PRIu8 " bytes):\r\n", message->sessionIdLen);
   TRACE_INFO_ARRAY("  ", message->sessionId, message->sessionIdLen);

   //Advance data pointer
   p += message->sessionIdLen;
   //Adjust the length of the message
   *length += message->sessionIdLen;

   //The single cipher suite selected by the server
   STORE16BE(context->cipherSuite.identifier, p);
   //Advance data pointer
   p += sizeof(TlsCipherSuite);
   //Adjust the length of the message
   *length += sizeof(TlsCipherSuite);

   //The single compression algorithm selected by the server
   *p = context->compressMethod;
   //Advance data pointer
   p += sizeof(TlsCompressMethod);
   //Adjust the length of the message
   *length += sizeof(TlsCompressMethod);

   //Only extensions offered by the client can appear in the server's list
   extensionList = (TlsExtensionList *) p;
   //Total length of the extension list
   extensionList->length = 0;

   //Point to the first extension of the list
   p += sizeof(TlsExtensionList);

#if (TLS_SNI_SUPPORT == ENABLED)
   //The server may include a SNI extension in the ServerHello
   error = tlsFormatServerSniExtension(context, p, &n);
   //Any error to report?
   if(error)
      return error;

   //Fix the length of the extension list
   extensionList->length += (uint16_t) n;
   //Point to the next field
   p += n;
   //Adjust the length of the message
   *length += n;
#endif

#if (TLS_MAX_FRAG_LEN_SUPPORT == ENABLED)
   //Servers that receive an ClientHello containing a MaxFragmentLength
   //extension may accept the requested maximum fragment length by including
   //an extension of type MaxFragmentLength in the ServerHello
   error = tlsFormatServerMaxFragLenExtension(context, p, &n);
   //Any error to report?
   if(error)
      return error;

   //Fix the length of the extension list
   extensionList->length += (uint16_t) n;
   //Point to the next field
   p += n;
   //Adjust the length of the message
   *length += n;
#endif

#if (TLS_RECORD_SIZE_LIMIT_SUPPORT == ENABLED)
   //The value of RecordSizeLimit is the maximum size of record in octets
   //that the endpoint is willing to receive
   error = tlsFormatServerRecordSizeLimitExtension(context, p, &n);
   //Any error to report?
   if(error)
      return error;

   //Fix the length of the extension list
   extensionList->length += (uint16_t) n;
   //Point to the next field
   p += n;
   //Adjust the length of the message
   *length += n;
#endif

   //A server that selects an ECC cipher suite in response to a ClientHello
   //message including an EcPointFormats extension appends this extension
   //to its ServerHello message
   error = tlsFormatServerEcPointFormatsExtension(context, p, &n);
   //Any error to report?
   if(error)
      return error;

   //Fix the length of the extension list
   extensionList->length += (uint16_t) n;
   //Point to the next field
   p += n;
   //Adjust the length of the message
   *length += n;

#if (TLS_ALPN_SUPPORT == ENABLED)
   //The ALPN extension contains the name of the selected protocol
   error = tlsFormatServerAlpnExtension(context, p, &n);
   //Any error to report?
   if(error)
      return error;

   //Fix the length of the extension list
   extensionList->length += (uint16_t) n;
   //Point to the next field
   p += n;
   //Adjust the length of the message
   *length += n;
#endif

#if (TLS_RAW_PUBLIC_KEY_SUPPORT == ENABLED)
   //The ClientCertType extension in the ServerHello indicates the type
   //of certificates the client is requested to provide in a subsequent
   //certificate payload
   error = tlsFormatClientCertTypeExtension(context, p, &n);
   //Any error to report?
   if(error)
      return error;

   //Fix the length of the extension list
   extensionList->length += (uint16_t) n;
   //Point to the next field
   p += n;
   //Adjust the length of the message
   *length += n;

   //With the ServerCertType extension in the ServerHello, the TLS server
   //indicates the certificate type carried in the certificate payload
   error = tlsFormatServerCertTypeExtension(context, p, &n);
   //Any error to report?
   if(error)
      return error;

   //Fix the length of the extension list
   extensionList->length += (uint16_t) n;
   //Point to the next field
   p += n;
   //Adjust the length of the message
   *length += n;
#endif

#if (TLS_EXT_MASTER_SECRET_SUPPORT == ENABLED)
   //If a server implementing RFC 7627 receives the ExtendedMasterSecret
   //extension, it must include the extension in its ServerHello message
   error = tlsFormatServerEmsExtension(context, p, &n);
   //Any error to report?
   if(error)
      return error;

   //Fix the length of the extension list
   extensionList->length += (uint16_t) n;
   //Point to the next field
   p += n;
   //Adjust the length of the message
   *length += n;
#endif

#if (TLS_SECURE_RENEGOTIATION_SUPPORT == ENABLED)
   //During secure renegotiation, the server must include a renegotiation_info
   //extension containing the saved client_verify_data and server_verify_data
   error = tlsFormatServerRenegoInfoExtension(context, p, &n);
   //Any error to report?
   if(error)
      return error;

   //Fix the length of the extension list
   extensionList->length += (uint16_t) n;
   //Point to the next field
   p += n;
   //Adjust the length of the message
   *length += n;
#endif

   //Any extensions included in the ServerHello message?
   if(extensionList->length > 0)
   {
      //Convert the length of the extension list to network byte order
      extensionList->length = htons(extensionList->length);
      //Total length of the message
      *length += sizeof(TlsExtensionList);
   }

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Format ServerKeyExchange message
 * @param[in] context Pointer to the TLS context
 * @param[out] message Buffer where to format the ServerKeyExchange message
 * @param[out] length Length of the resulting ServerKeyExchange message
 * @return Error code
 **/

error_t tlsFormatServerKeyExchange(TlsContext *context,
   TlsServerKeyExchange *message, size_t *length)
{
   error_t error;
   size_t n;
   size_t paramsLen;
   uint8_t *p;
   uint8_t *params;

   //Point to the beginning of the handshake message
   p = message;
   //Length of the handshake message
   *length = 0;

#if (TLS_PSK_SUPPORT == ENABLED || TLS_RSA_PSK_SUPPORT == ENABLED || \
   TLS_DHE_PSK_SUPPORT == ENABLED || TLS_ECDHE_PSK_SUPPORT == ENABLED)
   //PSK key exchange method?
   if(context->keyExchMethod == TLS_KEY_EXCH_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_RSA_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_DHE_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDHE_PSK)
   {
      //To help the client in selecting which identity to use, the server
      //can provide a PSK identity hint in the ServerKeyExchange message
      error = tlsFormatPskIdentityHint(context, p, &n);
      //Any error to report?
      if(error)
         return error;

      //Advance data pointer
      p += n;
      //Adjust the length of the message
      *length += n;
   }
#endif

   //Diffie-Hellman or ECDH key exchange method?
   if(context->keyExchMethod == TLS_KEY_EXCH_DH_ANON ||
      context->keyExchMethod == TLS_KEY_EXCH_DHE_RSA ||
      context->keyExchMethod == TLS_KEY_EXCH_DHE_DSS ||
      context->keyExchMethod == TLS_KEY_EXCH_DHE_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDH_ANON ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDHE_RSA ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDHE_ECDSA ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDHE_PSK)
   {
      //Point to the server's key exchange parameters
      params = p;

      //Format server's key exchange parameters
      error = tlsFormatServerKeyParams(context, p, &paramsLen);
      //Any error to report?
      if(error)
         return error;

      //Advance data pointer
      p += paramsLen;
      //Adjust the length of the message
      *length += paramsLen;
   }

   //For non-anonymous Diffie-Hellman and ECDH key exchanges, a signature
   //over the server's key exchange parameters shall be generated
   if(context->keyExchMethod == TLS_KEY_EXCH_DHE_RSA ||
      context->keyExchMethod == TLS_KEY_EXCH_DHE_DSS ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDHE_RSA ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDHE_ECDSA)
   {
#if (TLS_MAX_VERSION >= SSL_VERSION_3_0 && TLS_MIN_VERSION <= TLS_VERSION_1_1)
      //SSL 3.0, TLS 1.0 or TLS 1.1 currently selected?
      if(context->version <= TLS_VERSION_1_1)
      {
         //Sign server's key exchange parameters
         error = tlsGenerateServerKeySignature(context,
            (TlsDigitalSignature *) p, params, paramsLen, &n);
      }
      else
#endif
#if (TLS_MAX_VERSION >= TLS_VERSION_1_2 && TLS_MIN_VERSION <= TLS_VERSION_1_2)
      //TLS 1.2 currently selected?
      if(context->version == TLS_VERSION_1_2)
      {
         //Sign server's key exchange parameters
         error = tls12GenerateServerKeySignature(context,
            (Tls12DigitalSignature *) p, params, paramsLen, &n);
      }
      else
#endif
      {
         //Report an error
         error = ERROR_INVALID_VERSION;
      }

      //Any error to report?
      if(error)
         return error;

      //Advance data pointer
      p += n;
      //Adjust the length of the message
      *length += n;
   }

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Format CertificateRequest message
 * @param[in] context Pointer to the TLS context
 * @param[out] message Buffer where to format the CertificateRequest message
 * @param[out] length Length of the resulting CertificateRequest message
 * @return Error code
 **/

error_t tlsFormatCertificateRequest(TlsContext *context,
   TlsCertificateRequest *message, size_t *length)
{
   error_t error;
   size_t n;
   uint8_t *p;
   const char_t *pemCert;
   size_t pemCertLen;
   uint8_t *derCert;
   size_t derCertSize;
   size_t derCertLen;
   X509CertificateInfo *certInfo;
   TlsCertAuthorities *certAuthorities;

   //Initialize status code
   error = NO_ERROR;

   //Enumerate the types of certificate types that the client may offer
   n = 0;

#if (TLS_RSA_SIGN_SUPPORT == ENABLED || TLS_RSA_PSS_SIGN_SUPPORT == ENABLED)
   //Accept certificates that contain a RSA public key
   message->certificateTypes[n++] = TLS_CERT_RSA_SIGN;
#endif
#if (TLS_DSA_SIGN_SUPPORT == ENABLED)
   //Accept certificates that contain a DSA public key
   message->certificateTypes[n++] = TLS_CERT_DSS_SIGN;
#endif
#if (TLS_ECDSA_SIGN_SUPPORT == ENABLED)
   //Accept certificates that contain an ECDSA public key
   message->certificateTypes[n++] = TLS_CERT_ECDSA_SIGN;
#endif

   //Fix the length of the list
   message->certificateTypesLen = (uint8_t) n;
   //Length of the handshake message
   *length = sizeof(TlsCertificateRequest) + n;

#if (TLS_MAX_VERSION >= TLS_VERSION_1_2 && TLS_MIN_VERSION <= TLS_VERSION_1_2)
   //TLS 1.2 currently selected?
   if(context->version == TLS_VERSION_1_2)
   {
      TlsSignHashAlgos *supportedSignAlgos;

      //Point to the list of the hash/signature algorithm pairs that the server
      //is able to verify. Servers can minimize the computation cost by offering
      //a restricted set of digest algorithms
      supportedSignAlgos = PTR_OFFSET(message, *length);

      //Enumerate the hash/signature algorithm pairs in descending
      //order of preference
      n = 0;

#if (TLS_RSA_PSS_SIGN_SUPPORT == ENABLED)
#if (TLS_SHA256_SUPPORT == ENABLED)
      //RSASSA-PSS RSAE signature algorithm with SHA-256
      supportedSignAlgos->value[n].signature = TLS_SIGN_ALGO_RSA_PSS_RSAE_SHA256;
      supportedSignAlgos->value[n++].hash = TLS_HASH_ALGO_INTRINSIC;
#endif
#if (TLS_SHA384_SUPPORT == ENABLED)
      //RSASSA-PSS RSAE signature algorithm with SHA-384
      supportedSignAlgos->value[n].signature = TLS_SIGN_ALGO_RSA_PSS_RSAE_SHA384;
      supportedSignAlgos->value[n++].hash = TLS_HASH_ALGO_INTRINSIC;
#endif
#if (TLS_SHA512_SUPPORT == ENABLED)
      //RSASSA-PSS RSAE signature algorithm with SHA-512
      supportedSignAlgos->value[n].signature = TLS_SIGN_ALGO_RSA_PSS_RSAE_SHA512;
      supportedSignAlgos->value[n++].hash = TLS_HASH_ALGO_INTRINSIC;
#endif
#endif

#if (TLS_RSA_SIGN_SUPPORT == ENABLED)
#if (TLS_SHA1_SUPPORT == ENABLED)
      //RSASSA-PKCS1-v1_5 signature algorithm with SHA-1
      supportedSignAlgos->value[n].signature = TLS_SIGN_ALGO_RSA;
      supportedSignAlgos->value[n++].hash = TLS_HASH_ALGO_SHA1;
#endif
#if (TLS_SHA256_SUPPORT == ENABLED)
      //The hash algorithm used for PRF operations can also be used for signing
      if(context->cipherSuite.prfHashAlgo == SHA256_HASH_ALGO)
      {
         //RSASSA-PKCS1-v1_5 signature algorithm with SHA-256
         supportedSignAlgos->value[n].signature = TLS_SIGN_ALGO_RSA;
         supportedSignAlgos->value[n++].hash = TLS_HASH_ALGO_SHA256;
      }
#endif
#if (TLS_SHA384_SUPPORT == ENABLED)
      //The hash algorithm used for PRF operations can also be used for signing
      if(context->cipherSuite.prfHashAlgo == SHA384_HASH_ALGO)
      {
         //RSASSA-PKCS1-v1_5 signature algorithm with SHA-384
         supportedSignAlgos->value[n].signature = TLS_SIGN_ALGO_RSA;
         supportedSignAlgos->value[n++].hash = TLS_HASH_ALGO_SHA384;
      }
#endif
#endif

#if (TLS_DSA_SIGN_SUPPORT == ENABLED)
#if (TLS_SHA1_SUPPORT == ENABLED)
      //DSA signature algorithm with SHA-1
      supportedSignAlgos->value[n].signature = TLS_SIGN_ALGO_DSA;
      supportedSignAlgos->value[n++].hash = TLS_HASH_ALGO_SHA1;
#endif
#if (TLS_SHA256_SUPPORT == ENABLED)
      //The hash algorithm used for PRF operations can also be used for signing
      if(context->cipherSuite.prfHashAlgo == SHA256_HASH_ALGO)
      {
         //DSA signature algorithm with SHA-256
         supportedSignAlgos->value[n].signature = TLS_SIGN_ALGO_DSA;
         supportedSignAlgos->value[n++].hash = TLS_HASH_ALGO_SHA256;
      }
#endif
#endif

#if (TLS_ECDSA_SIGN_SUPPORT == ENABLED)
#if (TLS_SHA1_SUPPORT == ENABLED)
      //ECDSA signature algorithm with SHA-1
      supportedSignAlgos->value[n].signature = TLS_SIGN_ALGO_ECDSA;
      supportedSignAlgos->value[n++].hash = TLS_HASH_ALGO_SHA1;
#endif
#if (TLS_SHA256_SUPPORT == ENABLED)
      //The hash algorithm used for PRF operations can also be used for signing
      if(context->cipherSuite.prfHashAlgo == SHA256_HASH_ALGO)
      {
         //ECDSA signature algorithm with SHA-256
         supportedSignAlgos->value[n].signature = TLS_SIGN_ALGO_ECDSA;
         supportedSignAlgos->value[n++].hash = TLS_HASH_ALGO_SHA256;
      }
#endif
#if (TLS_SHA384_SUPPORT == ENABLED)
      //The hash algorithm used for PRF operations can also be used for signing
      if(context->cipherSuite.prfHashAlgo == SHA384_HASH_ALGO)
      {
         //ECDSA signature algorithm with SHA-384
         supportedSignAlgos->value[n].signature = TLS_SIGN_ALGO_ECDSA;
         supportedSignAlgos->value[n++].hash = TLS_HASH_ALGO_SHA384;
      }
#endif
#endif

      //Fix the length of the list
      supportedSignAlgos->length = htons(n * sizeof(TlsSignHashAlgo));
      //Adjust the length of the message
      *length += sizeof(TlsSignHashAlgos) + n * sizeof(TlsSignHashAlgo);
   }
#endif

   //Point to the list of the distinguished names of acceptable
   //certificate authorities
   certAuthorities = PTR_OFFSET(message, *length);
   //Adjust the length of the message
   *length += sizeof(TlsCertAuthorities);

   //Point to the first certificate authority
   p = certAuthorities->value;
   //Length of the list in bytes
   n = 0;

   //Point to the first trusted CA certificate
   pemCert = context->trustedCaList;
   //Get the total length, in bytes, of the trusted CA list
   pemCertLen = context->trustedCaListLen;

   //DER encoded certificate
   derCert = NULL;
   derCertSize = 0;
   derCertLen = 0;

   //Allocate a memory buffer to store X.509 certificate info
   certInfo = tlsAllocMem(sizeof(X509CertificateInfo));

   //Successful memory allocation?
   if(certInfo != NULL)
   {
      //Loop through the list of trusted CA certificates
      while(pemCertLen > 0)
      {
         //Decode PEM certificate
         error = pemImportCertificate(&pemCert, &pemCertLen,
            &derCert, &derCertSize, &derCertLen);

         //Any error to report?
         if(error)
         {
            //End of file detected
            error = NO_ERROR;
            break;
         }

         //Parse X.509 certificate
         error = x509ParseCertificate(derCert, derCertLen, certInfo);
         //Failed to parse the X.509 certificate?
         if(error)
            break;

         //Adjust the length of the message
         *length += certInfo->subject.rawDataLen + 2;

         //Prevent the buffer from overflowing
         if(*length > context->txBufferMaxLen)
         {
            //Report an error
            error = ERROR_MESSAGE_TOO_LONG;
            break;
         }

         //Each distinguished name is preceded by a 2-byte length field
         STORE16BE(certInfo->subject.rawDataLen, p);
         //The distinguished name shall be DER encoded
         memcpy(p + 2, certInfo->subject.rawData, certInfo->subject.rawDataLen);

         //Advance data pointer
         p += certInfo->subject.rawDataLen + 2;
         //Adjust the length of the list
         n += certInfo->subject.rawDataLen + 2;
      }

      //Free previously allocated memory
      tlsFreeMem(derCert);
      tlsFreeMem(certInfo);

      //Fix the length of the list
      certAuthorities->length = htons(n);
   }
   else
   {
      //Report an error
      error = ERROR_OUT_OF_MEMORY;
   }

   //Return status code
   return error;
}


/**
 * @brief Format ServerHelloDone message
 * @param[in] context Pointer to the TLS context
 * @param[out] message Buffer where to format the ServerHelloDone message
 * @param[out] length Length of the resulting ServerHelloDone message
 * @return Error code
 **/

error_t tlsFormatServerHelloDone(TlsContext *context,
   TlsServerHelloDone *message, size_t *length)
{
   //The ServerHelloDone message does not contain any data
   *length = 0;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Parse ClientHello message
 *
 * When a client first connects to a server, it is required to send
 * the ClientHello as its first message. The client can also send a
 * ClientHello in response to a HelloRequest or on its own initiative
 * in order to renegotiate the security parameters in an existing
 * connection
 *
 * @param[in] context Pointer to the TLS context
 * @param[in] message Incoming ClientHello message to parse
 * @param[in] length Message length
 * @return Error code
 **/

error_t tlsParseClientHello(TlsContext *context,
   const TlsClientHello *message, size_t length)
{
   error_t error;
   size_t i;
   size_t n;
   const uint8_t *p;
   const TlsCipherSuites *cipherSuites;
   const TlsCompressMethods *compressMethods;
   TlsHelloExtensions extensions;
#if (DTLS_SUPPORT == ENABLED)
   const DtlsCookie *cookie;
#endif

   //Debug message
   TRACE_INFO("ClientHello message received (%" PRIuSIZE " bytes)...\r\n", length);
   TRACE_DEBUG_ARRAY("  ", message, length);

   //Check current state
   if(context->state == TLS_STATE_CLIENT_HELLO)
   {
      //When a client first connects to a server, it is required to send
      //the ClientHello as its first message
   }
#if (TLS_SECURE_RENEGOTIATION_SUPPORT == ENABLED)
   else if(context->state == TLS_STATE_APPLICATION_DATA)
   {
      //Check whether secure renegotiation is enabled
      if(context->secureRenegoEnabled)
      {
         //Make sure the secure_renegociation flag is set
         if(!context->secureRenegoFlag)
         {
            //If the connection's secure_renegotiation flag is set to FALSE, it is
            //recommended that servers do not permit legacy renegotiation (refer
            //to RFC 5746, section 4.4)
            return ERROR_HANDSHAKE_FAILED;
         }
      }
      else
      {
         //Secure renegotiation is disabled
         return ERROR_HANDSHAKE_FAILED;
      }
   }
#endif
   else
   {
      //Report an error
      return ERROR_UNEXPECTED_MESSAGE;
   }

   //Check the length of the ClientHello message
   if(length < sizeof(TlsClientHello))
      return ERROR_DECODING_FAILED;

   //Get the version the client wishes to use during this session
   context->clientVersion = ntohs(message->clientVersion);

   //Point to the session ID
   p = message->sessionId;
   //Remaining bytes to process
   length -= sizeof(TlsClientHello);

   //Check the length of the session ID
   if(message->sessionIdLen > length)
      return ERROR_DECODING_FAILED;
   if(message->sessionIdLen > 32)
      return ERROR_DECODING_FAILED;

   //Debug message
   TRACE_INFO("Session ID (%" PRIu8 " bytes):\r\n", message->sessionIdLen);
   TRACE_INFO_ARRAY("  ", message->sessionId, message->sessionIdLen);

   //Point to the next field
   p += message->sessionIdLen;
   //Remaining bytes to process
   length -= message->sessionIdLen;

#if (DTLS_SUPPORT == ENABLED)
   //DTLS protocol?
   if(context->transportProtocol == TLS_TRANSPORT_PROTOCOL_DATAGRAM)
   {
      //Point to the Cookie field
      cookie = (DtlsCookie *) p;

      //Malformed ClientHello message?
      if(length < sizeof(DtlsCookie))
         return ERROR_DECODING_FAILED;
      if(length < (sizeof(DtlsCookie) + cookie->length))
         return ERROR_DECODING_FAILED;

      //Check the length of the cookie
      if(cookie->length > DTLS_MAX_COOKIE_SIZE)
         return ERROR_ILLEGAL_PARAMETER;

      //Point to the next field
      p += sizeof(DtlsCookie) + cookie->length;
      //Remaining bytes to process
      length -= sizeof(DtlsCookie) + cookie->length;
   }
#endif

   //List of cryptographic algorithms supported by the client
   cipherSuites = (TlsCipherSuites *) p;

   //Malformed ClientHello message?
   if(length < sizeof(TlsCipherSuites))
      return ERROR_DECODING_FAILED;
   if(length < (sizeof(TlsCipherSuites) + ntohs(cipherSuites->length)))
      return ERROR_DECODING_FAILED;

   //Check the length of the list
   if(ntohs(cipherSuites->length) == 0)
      return ERROR_DECODING_FAILED;
   if((ntohs(cipherSuites->length) % 2) != 0)
      return ERROR_DECODING_FAILED;

   //Point to the next field
   p += sizeof(TlsCipherSuites) + ntohs(cipherSuites->length);
   //Remaining bytes to process
   length -= sizeof(TlsCipherSuites) + ntohs(cipherSuites->length);

   //List of compression algorithms supported by the client
   compressMethods = (TlsCompressMethods *) p;

   //Malformed ClientHello message?
   if(length < sizeof(TlsCompressMethods))
      return ERROR_DECODING_FAILED;
   if(length < (sizeof(TlsCompressMethods) + compressMethods->length))
      return ERROR_DECODING_FAILED;

   //Check the length of the list
   if(compressMethods->length == 0)
      return ERROR_DECODING_FAILED;

   //Point to the next field
   p += sizeof(TlsCompressMethods) + compressMethods->length;
   //Remaining bytes to process
   length -= sizeof(TlsCompressMethods) + compressMethods->length;

   //Parse the list of extensions offered by the client
   error = tlsParseHelloExtensions(TLS_TYPE_CLIENT_HELLO, p, length,
      &extensions);
   //Any error to report?
   if(error)
      return error;

   //Check whether the ClientHello includes any SCSV cipher suites
   error = tlsCheckSignalingCipherSuiteValues(context, cipherSuites);
   //Any error to report?
   if(error)
      return error;

#if (TLS_SECURE_RENEGOTIATION_SUPPORT == ENABLED)
   //Parse RenegotiationInfo extension
   error = tlsParseClientRenegoInfoExtension(context, extensions.renegoInfo);
   //Any error to report?
   if(error)
      return error;
#endif

#if (DTLS_SUPPORT == ENABLED)
   //DTLS protocol?
   if(context->transportProtocol == TLS_TRANSPORT_PROTOCOL_DATAGRAM)
   {
      DtlsClientParameters params;

      //The server should use client parameters (version, random, session_id,
      //cipher_suites, compression_method) to generate its cookie
      params.version = ntohs(message->clientVersion);
      params.random = (const uint8_t *) &message->random;
      params.randomLen = sizeof(TlsRandom);
      params.sessionId = message->sessionId;
      params.sessionIdLen = message->sessionIdLen;
      params.cipherSuites = (const uint8_t *) cipherSuites->value;
      params.cipherSuitesLen = ntohs(cipherSuites->length);
      params.compressMethods = compressMethods->value;
      params.compressMethodsLen = compressMethods->length;

      //Verify that the cookie is valid
      error = dtlsVerifyCookie(context, cookie, &params);
      //Any error to report?
      if(error)
         return error;

      //The server may respond with a HelloVerifyRequest message containing
      //a stateless cookie
      if(context->state == TLS_STATE_HELLO_VERIFY_REQUEST)
      {
         //Exit immediately
         return NO_ERROR;
      }
   }
#endif

   //Perform version negotiation
   error = tlsNegotiateVersion(context, ntohs(message->clientVersion),
      extensions.supportedVersionList);
   //Any error to report?
   if(error)
      return error;

#if (TLS_MAX_VERSION >= TLS_VERSION_1_2 && TLS_MIN_VERSION <= TLS_VERSION_1_2)
   //The SignatureAlgorithms extension is not meaningful for TLS versions
   //prior to 1.2 (refer to RFC 5246, section 7.4.1.4.1)
   if(context->version < TLS_VERSION_1_2)
   {
      //Even if clients do offer it, the rules specified in RFC 6066 require
      //servers to ignore extensions they do not understand
      extensions.signAlgoList = NULL;
   }
#endif

   //Save client random value
   context->clientRandom = message->random;

#if (TLS_SESSION_RESUME_SUPPORT == ENABLED)
   //Check whether session caching is supported
   if(context->cache != NULL)
   {
      TlsSession *session;

      //If the session ID was non-empty, the server will look in its
      //session cache for a match
      session = tlsFindCache(context->cache, message->sessionId,
         message->sessionIdLen);

      //Matching session found?
      if(session != NULL)
      {
         //Whenever a client already knows the highest protocol version known
         //to a server (for example, when resuming a session), it should
         //initiate the connection in that native protocol
         if(session->version != context->version)
            session = NULL;
      }

      //Matching session found?
      if(session != NULL)
      {
         //Get the total number of cipher suites offered by the client
         n = ntohs(cipherSuites->length) / 2;

         //Loop through the list of cipher suite identifiers
         for(i = 0; i < n; i++)
         {
            //Matching cipher suite?
            if(ntohs(cipherSuites->value[i]) == session->cipherSuite)
               break;
         }

         //If the cipher suite is not present in the list cipher suites offered
         //by the client, the server must not perform the abbreviated handshake
         if(i >= n)
            session = NULL;
      }

#if (TLS_EXT_MASTER_SECRET_SUPPORT == ENABLED)
      //Matching session found?
      if(session != NULL)
      {
         //ExtendedMasterSecret extension found?
         if(extensions.extendedMasterSecret != NULL)
         {
            //If the original session did not use the ExtendedMasterSecret
            //extension but the new ClientHello contains the extension, then
            //the server must not perform the abbreviated handshake
            if(!session->extendedMasterSecret)
               session = NULL;
         }
      }
#endif

      //Matching session found?
      if(session != NULL)
      {
         //Restore session parameters
         tlsRestoreSession(context, session);

         //Select the relevant cipher suite
         error = tlsSelectCipherSuite(context, session->cipherSuite);
         //Any error to report?
         if(error)
            return error;

         //Perform abbreviated handshake
         context->resume = TRUE;
      }
      else
      {
         //Generate a new random ID
         error = context->prngAlgo->read(context->prngContext,
            context->sessionId, 32);
         //Any error to report?
         if(error)
            return error;

         //Session ID is limited to 32 bytes
         context->sessionIdLen = 32;
         //Perform a full handshake
         context->resume = FALSE;
      }
   }
   else
#endif
   {
      //This session cannot be resumed
      context->sessionIdLen = 0;
      //Perform a full handshake
      context->resume = FALSE;
   }

   //Full handshake?
   if(!context->resume)
   {
      //Perform cipher suite negotiation
      error = tlsNegotiateCipherSuite(context, cipherSuites, &extensions);
      //If no acceptable choices are presented, terminate the handshake
      if(error)
         return error;

      //Parse the list of compression methods supported by the client
      error = tlsParseCompressMethods(context, compressMethods);
      //Any error to report?
      if(error)
         return error;
   }

#if (TLS_SNI_SUPPORT == ENABLED)
   //In order to provide the server name, clients may include a ServerName
   //extension
   error = tlsParseClientSniExtension(context, extensions.serverNameList);
   //Any error to report?
   if(error)
      return error;
#endif

#if (TLS_MAX_FRAG_LEN_SUPPORT == ENABLED && TLS_RECORD_SIZE_LIMIT_SUPPORT == ENABLED)
   //A server that supports the RecordSizeLimit extension must ignore a
   //MaxFragmentLength that appears in a ClientHello if both extensions
   //appear (refer to RFC 8449, section 5)
   if(extensions.maxFragLen != NULL && extensions.recordSizeLimit != NULL)
      extensions.maxFragLen = NULL;
#endif

#if (TLS_MAX_FRAG_LEN_SUPPORT == ENABLED)
   //In order to negotiate smaller maximum fragment lengths, clients may
   //include a MaxFragmentLength extension
   error = tlsParseClientMaxFragLenExtension(context, extensions.maxFragLen);
   //Any error to report?
   if(error)
      return error;
#endif

#if (TLS_RECORD_SIZE_LIMIT_SUPPORT == ENABLED)
   //The value of RecordSizeLimit is the maximum size of record in octets
   //that the peer is willing to receive
   error = tlsParseClientRecordSizeLimitExtension(context,
      extensions.recordSizeLimit);
   //Any error to report?
   if(error)
      return error;
#endif

#if (TLS_ECDH_ANON_SUPPORT == ENABLED || TLS_ECDHE_RSA_SUPPORT == ENABLED || \
   TLS_ECDHE_ECDSA_SUPPORT == ENABLED || TLS_ECDHE_PSK_SUPPORT == ENABLED)
   //A client that proposes ECC cipher suites in its ClientHello message
   //may send the EcPointFormats extension
   error = tlsParseClientEcPointFormatsExtension(context,
      extensions.ecPointFormatList);
   //Any error to report?
   if(error)
      return error;
#endif

#if (TLS_ALPN_SUPPORT == ENABLED)
   //Parse ALPN extension
   error = tlsParseClientAlpnExtension(context, extensions.protocolNameList);
   //Any error to report?
   if(error)
      return error;
#endif

#if (TLS_RAW_PUBLIC_KEY_SUPPORT == ENABLED)
   //Parse ClientCertType extension
   error = tlsParseClientCertTypeListExtension(context,
      extensions.clientCertTypeList);
   //Any error to report?
   if(error)
      return error;

   //Parse ServerCertType extension
   error = tlsParseServerCertTypeListExtension(context,
      extensions.serverCertTypeList);
   //Any error to report?
   if(error)
      return error;
#endif

#if (TLS_EXT_MASTER_SECRET_SUPPORT == ENABLED)
   //Parse ExtendedMasterSecret extension
   error = tlsParseClientEmsExtension(context, extensions.extendedMasterSecret);
   //Any error to report?
   if(error)
      return error;
#endif

   //Initialize handshake message hashing
   error = tlsInitHandshakeHash(context);
   //Any error to report?
   if(error)
      return error;

   //Prepare to send ServerHello message...
   context->state = TLS_STATE_SERVER_HELLO;
   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Parse ClientKeyExchange message
 *
 * This message is always sent by the client. It must immediately
 * follow the client Certificate message, if it is sent. Otherwise,
 * it must be the first message sent by the client after it receives
 * the ServerHelloDone message
 *
 * @param[in] context Pointer to the TLS context
 * @param[in] message Incoming ClientKeyExchange message to parse
 * @param[in] length Message length
 * @return Error code
 **/

error_t tlsParseClientKeyExchange(TlsContext *context,
   const TlsClientKeyExchange *message, size_t length)
{
   error_t error;
   size_t n;
   const uint8_t *p;

   //Debug message
   TRACE_INFO("ClientKeyExchange message received (%" PRIuSIZE " bytes)...\r\n", length);
   TRACE_DEBUG_ARRAY("  ", message, length);

   //Check TLS version
   if(context->version > TLS_VERSION_1_2)
      return ERROR_UNEXPECTED_MESSAGE;

   //Check current state
   if(context->state == TLS_STATE_CLIENT_CERTIFICATE)
   {
      //A an non-anonymous server can optionally request a certificate from the client
      if(context->keyExchMethod == TLS_KEY_EXCH_RSA ||
         context->keyExchMethod == TLS_KEY_EXCH_DHE_RSA ||
         context->keyExchMethod == TLS_KEY_EXCH_DHE_DSS ||
         context->keyExchMethod == TLS_KEY_EXCH_ECDHE_RSA ||
         context->keyExchMethod == TLS_KEY_EXCH_ECDHE_ECDSA ||
         context->keyExchMethod == TLS_KEY_EXCH_RSA_PSK)
      {
         //If client authentication is required by the server for the handshake
         //to continue, it may respond with a fatal handshake failure alert
         if(context->clientAuthMode == TLS_CLIENT_AUTH_REQUIRED)
            return ERROR_HANDSHAKE_FAILED;
      }
   }
   else if(context->state != TLS_STATE_CLIENT_KEY_EXCHANGE)
   {
      //Send a fatal alert to the client
      return ERROR_UNEXPECTED_MESSAGE;
   }

   //Point to the beginning of the handshake message
   p = message;

#if (TLS_PSK_SUPPORT == ENABLED || TLS_RSA_PSK_SUPPORT == ENABLED || \
   TLS_DHE_PSK_SUPPORT == ENABLED || TLS_ECDHE_PSK_SUPPORT == ENABLED)
   //PSK key exchange method?
   if(context->keyExchMethod == TLS_KEY_EXCH_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_RSA_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_DHE_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDHE_PSK)
   {
      //The PSK identity is sent in cleartext
      error = tlsParsePskIdentity(context, p, length, &n);
      //Any error to report?
      if(error)
         return error;

      //Point to the next field
      p += n;
      //Remaining bytes to process
      length -= n;
   }
#endif

   //RSA, Diffie-Hellman or ECDH key exchange method?
   if(context->keyExchMethod != TLS_KEY_EXCH_PSK)
   {
      //Parse client's key exchange parameters
      error = tlsParseClientKeyParams(context, p, length, &n);
      //Any error to report?
      if(error)
         return error;

      //Point to the next field
      p += n;
      //Remaining bytes to process
      length -= n;
   }

   //If the amount of data in the message does not precisely match the format
   //of the ClientKeyExchange message, then send a fatal alert
   if(length != 0)
      return ERROR_DECODING_FAILED;

#if (TLS_PSK_SUPPORT == ENABLED || TLS_RSA_PSK_SUPPORT == ENABLED || \
   TLS_DHE_PSK_SUPPORT == ENABLED || TLS_ECDHE_PSK_SUPPORT == ENABLED)
   //PSK key exchange method?
   if(context->keyExchMethod == TLS_KEY_EXCH_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_RSA_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_DHE_PSK ||
      context->keyExchMethod == TLS_KEY_EXCH_ECDHE_PSK)
   {
      //Invalid pre-shared key?
      if(context->pskLen == 0)
         return ERROR_INVALID_KEY_LENGTH;

      //Generate premaster secret
      error = tlsGeneratePskPremasterSecret(context);
      //Any error to report?
      if(error)
         return error;
   }
#endif

   //Derive session keys from the premaster secret
   error = tlsGenerateSessionKeys(context);
   //Unable to generate key material?
   if(error)
      return error;

   //Update FSM state
   if(context->peerCertType != TLS_CERT_NONE)
      context->state = TLS_STATE_CLIENT_CERTIFICATE_VERIFY;
   else
      context->state = TLS_STATE_CLIENT_CHANGE_CIPHER_SPEC;

   //Successful processing
   return NO_ERROR;
}

#endif
