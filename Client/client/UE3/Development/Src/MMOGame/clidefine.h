// SHANDA GAMES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Shanda Games and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1999-2010 Shanda Games.
//      All Rights Reserved.
//
// Shanda Games Limited, Shanghai, China, 201203
// http://www.shandagames.com

#ifndef _CLIDEFINE_H_
#define _CLIDEFINE_H_

#include <sdbase.h>

/// send buffer size
#define SD_CLIENT_SENDBUFFER    64 * 1024
/// maximum message length
#define PACKAGE_LENGTH          64 * 1024

/**
 * @brief network layer message header
 */
#pragma pack(push, 1)
struct STNetMsgHeader
{
    UINT16	wMsgID;         ///< packet header identifier
    UINT32	dwDataLen;      ///< packet data length
};
#pragma pack(pop)

/// maximum message content length
#define PACKAGE_CONTENT_LENGTH  PACKAGE_LENGTH - sizeof(STNetMsgHeader)

/// client version
const SGDP::SSGDPVersion SDCLIENT_MODULE_VERSION = {3, 2, 0, 0};

#endif // #ifndef _CLIDEFINE_H_
