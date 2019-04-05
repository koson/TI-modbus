/**
 * @file tls_server_misc.h
 * @brief Helper functions for TLS server
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
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.8.6
 **/

#ifndef _TLS_SERVER_MISC_H
#define _TLS_SERVER_MISC_H

//Dependencies
#include "tls.h"

//C++ guard
#ifdef __cplusplus
   extern "C" {
#endif

//TLS server specific functions
error_t tlsFormatServerSniExtension(TlsContext *context,
   uint8_t *p, size_t *written);

error_t tlsFormatServerMaxFragLenExtension(TlsContext *context,
   uint8_t *p, size_t *written);

error_t tlsFormatServerRecordSizeLimitExtension(TlsContext *context,
   uint8_t *p, size_t *written);

error_t tlsFormatServerEcPointFormatsExtension(TlsContext *context,
   uint8_t *p, size_t *written);

error_t tlsFormatServerAlpnExtension(TlsContext *context,
   uint8_t *p, size_t *written);

error_t tlsFormatClientCertTypeExtension(TlsContext *context,
   uint8_t *p, size_t *written);

error_t tlsFormatServerCertTypeExtension(TlsContext *context,
   uint8_t *p, size_t *written);

error_t tlsFormatServerEmsExtension(TlsContext *context,
   uint8_t *p, size_t *written);

error_t tlsFormatServerRenegoInfoExtension(TlsContext *context,
   uint8_t *p, size_t *written);

error_t tlsFormatPskIdentityHint(TlsContext *context,
   uint8_t *p, size_t *written);

error_t tlsFormatServerKeyParams(TlsContext *context,
   uint8_t *p, size_t *written);

error_t tlsGenerateServerKeySignature(TlsContext *context,
   TlsDigitalSignature *signature, const uint8_t *params,
   size_t paramsLen, size_t *written);

error_t tls12GenerateServerKeySignature(TlsContext *context,
   Tls12DigitalSignature *signature, const uint8_t *params,
   size_t paramsLen, size_t *written);

error_t tlsCheckSignalingCipherSuiteValues(TlsContext *context,
   const TlsCipherSuites *cipherSuites);

error_t tlsNegotiateVersion(TlsContext *context, uint16_t clientVersion,
   const TlsSupportedVersionList *supportedVersionList);

error_t tlsNegotiateCipherSuite(TlsContext *context,
   const TlsCipherSuites *cipherSuites, const TlsHelloExtensions *extensions);

error_t tlsSelectCipherSuiteParams(TlsContext *context,
   const TlsHelloExtensions *extensions);

error_t tlsParseCompressMethods(TlsContext *context,
   const TlsCompressMethods *compressMethods);

error_t tlsParseClientSupportedVersionsExtension(TlsContext *context,
   const TlsSupportedVersionList *supportedVersionList);

error_t tlsParseClientSniExtension(TlsContext *context,
   const TlsServerNameList *serverNameList);

error_t tlsParseClientMaxFragLenExtension(TlsContext *context,
   const uint8_t *maxFragLen);

error_t tlsParseClientRecordSizeLimitExtension(TlsContext *context,
   const uint8_t *recordSizeLimit);

error_t tlsParseClientEcPointFormatsExtension(TlsContext *context,
   const TlsEcPointFormatList *ecPointFormatList);

error_t tlsParseClientAlpnExtension(TlsContext *context,
   const TlsProtocolNameList *protocolNameList);

error_t tlsParseClientCertTypeListExtension(TlsContext *context,
   const TlsCertTypeList *clientCertTypeList);

error_t tlsParseServerCertTypeListExtension(TlsContext *context,
   const TlsCertTypeList *serverCertTypeList);

error_t tlsParseClientEmsExtension(TlsContext *context,
   const uint8_t *extendedMasterSecret);

error_t tlsParseClientRenegoInfoExtension(TlsContext *context,
   const TlsRenegoInfo *renegoInfo);

error_t tlsParsePskIdentity(TlsContext *context,
   const uint8_t *p, size_t length, size_t *consumed);

error_t tlsParseClientKeyParams(TlsContext *context,
   const uint8_t *p, size_t length, size_t *consumed);

//C++ guard
#ifdef __cplusplus
   }
#endif

#endif
