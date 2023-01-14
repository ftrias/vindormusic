/*!
 *  @file		AppleMIDI_EndSession.h
 *  Project		Arduino AppleMIDI Library
 *	@brief		AppleMIDI Library for the Arduino 
 *	Version		0.3
 *  @author		lathoub 
 *	@date		04/04/14
 *  License		Code is open source so please feel free to do anything you want with it; you buy me a beer if you use this and we meet someday (Beerware license).
 */

#pragma once

#include "utility/AppleMidi_Settings.h"
#include "utility/AppleMidi_Defs.h"
#include "utility/AppleMidi_Util.h"

#ifdef __cpp
extern "C" {
#endif

BEGIN_APPLEMIDI_NAMESPACE
	
typedef struct __attribute__((packed)) AppleMIDI_EndSession
{
	uint8_t		signature[2];
	uint8_t		command[2];
	uint32_t	version;
	uint32_t	initiatorToken;
	uint32_t	ssrc;


	inline AppleMIDI_EndSession()
	{
		init();
	}

private:
	void init()
	{
		memcpy(signature, amSignature, sizeof(amSignature));
		memcpy(command, amEndSession, sizeof(amEndSession));
	}
} AppleMIDI_EndSession_t;

END_APPLEMIDI_NAMESPACE

#ifdef __cpp
}
#endif

