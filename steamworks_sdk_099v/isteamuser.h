//====== Copyright � 1996-2008, Valve Corporation, All rights reserved. =======
//
// Purpose: interface to user account information in Steam
//
//=============================================================================

#ifndef ISTEAMUSER_H
#define ISTEAMUSER_H
#ifdef _WIN32
#pragma once
#endif

#include "isteamclient.h"

// structure that contains client callback data
// see callbacks documentation for more details
struct CallbackMsg_t
{
	HSteamUser m_hSteamUser;
	int m_iCallback;
	uint8 *m_pubParam;
	int m_cubParam;
};

// reference to a steam call, to filter results by
typedef int32 HSteamCall;
enum EConfigSubTree {
    EConfigSubTree_stub
};

enum ELogonState {
    ELogonState_stub
};

//-----------------------------------------------------------------------------
// Purpose: Functions for accessing and manipulating a steam account
//			associated with one client instance
//-----------------------------------------------------------------------------
class ISteamUser
{
public:
	virtual HSteamUser GetHSteamUser() = 0;
	virtual void LogOn( CSteamID steamID ) = 0;
	virtual void LogOff() = 0;
	virtual bool BLoggedOn() = 0;
	virtual ELogonState GetLogonState() = 0;
	virtual bool BConnected() = 0;
	virtual CSteamID GetSteamID() = 0;
	virtual bool IsVACBanned( int nGameID ) = 0;
	virtual bool RequireShowVACBannedMessage( int nAppID ) = 0;
	virtual void AcknowledgeVACBanning( int nAppID ) = 0;
	virtual void SetSteam2Ticket( uint8 *pubTicket, int cubTicket ) = 0;
	virtual void AddServerNetAddress( uint32 unIP, uint16 unPort ) = 0;
	virtual bool SetEmail( const char *pchEmail ) = 0;
	virtual bool SetRegistryString( EConfigSubTree eRegistrySubTree, const char *pchKey, const char *pchValue ) = 0;
	virtual bool GetRegistryString( EConfigSubTree eRegistrySubTree, const char *pchKey, char *pchValue, int cbValue ) = 0;
	virtual bool SetRegistryInt( EConfigSubTree eRegistrySubTree, const char *pchKey, int iValue ) = 0;
	virtual bool GetRegistryInt( EConfigSubTree eRegistrySubTree, const char *pchKey, int *piValue ) = 0;
	virtual int InitiateGameConnection( void *pBlob, int cbMaxBlob, CSteamID steamID, CGameID gameID, uint32 unIPServer, uint16 usPortServer, bool bSecure ) = 0;
	virtual void TerminateGameConnection( uint32 unIPServer, uint16 usPortServer ) = 0;
	virtual void SetSelfAsPrimaryChatDestination() = 0;
	virtual bool IsPrimaryChatDestination() = 0;
	virtual void RequestLegacyCDKey( uint32 nAppID ) = 0;
	virtual bool SendGuestPassByEmail( const char *pchEmailAccount, GID_t gidGuestPassID, bool bResending ) = 0;
	virtual bool SendGuestPassByAccountID( uint32 uAccountID, GID_t gidGuestPassID, bool bResending ) = 0;
	virtual bool AckGuestPass(const char *pchGuestPassCode) = 0;
	virtual bool RedeemGuestPass(const char *pchGuestPassCode) = 0;
	virtual uint32 GetGuestPassToGiveCount() = 0;
	virtual uint32 GetGuestPassToRedeemCount() = 0;
	virtual RTime32 GetGuestPassLastUpdateTime() = 0;
	virtual bool GetGuestPassToGiveInfo( uint32 nPassIndex, GID_t *pgidGuestPassID, PackageId_t *pnPackageID, RTime32 *pRTime32Created, RTime32 *pRTime32Expiration, RTime32 *pRTime32Sent, RTime32 *pRTime32Redeemed, char *pchRecipientAddress, int cRecipientAddressSize ) = 0;
	virtual bool GetGuestPassToRedeemInfo( uint32 nPassIndex, GID_t *pgidGuestPassID, PackageId_t *pnPackageID, RTime32 *pRTime32Created, RTime32 *pRTime32Expiration, RTime32 *pRTime32Sent, RTime32 *pRTime32Redeemed) = 0;
	virtual bool GetGuestPassToRedeemSenderAddress( uint32 nPassIndex, char* pchSenderAddress, int cSenderAddressSize ) = 0;
	virtual bool GetGuestPassToRedeemSenderName( uint32 nPassIndex, char* pchSenderName, int cSenderNameSize ) = 0;
	virtual void AcknowledgeMessageByGID( const char *pchMessageGID ) = 0;
	virtual bool SetLanguage( const char *pchLanguage ) = 0;
	virtual void TrackAppUsageEvent( CGameID gameID, int eAppUsageEvent, const char *pchExtraInfo = "" ) = 0;
	virtual void SetAccountName( const char *pchAccountName ) = 0;
	virtual void SetPassword( const char *pchPassword ) = 0;
	virtual void SetAccountCreationTime( RTime32 rt ) = 0;
};

#define STEAMUSER_INTERFACE_VERSION "SteamUser005"


// callbacks


//-----------------------------------------------------------------------------
// Purpose: called when a connections to the Steam back-end has been established
//			this means the Steam client now has a working connection to the Steam servers
//			usually this will have occurred before the game has launched, and should
//			only be seen if the user has dropped connection due to a networking issue
//			or a Steam server update
//-----------------------------------------------------------------------------
struct SteamServersConnected_t
{
	enum { k_iCallback = k_iSteamUserCallbacks + 1 };
};

//-----------------------------------------------------------------------------
// Purpose: called when a connection attempt has failed
//			this will occur periodically if the Steam client is not connected, 
//			and has failed in it's retry to establish a connection
//-----------------------------------------------------------------------------
struct SteamServerConnectFailure_t
{
	enum { k_iCallback = k_iSteamUserCallbacks + 2 };
	EResult m_eResult;
};


//-----------------------------------------------------------------------------
// Purpose: called if the client has lost connection to the Steam servers
//			real-time services will be disabled until a matching SteamServersConnected_t has been posted
//-----------------------------------------------------------------------------
struct SteamServersDisconnected_t
{
	enum { k_iCallback = k_iSteamUserCallbacks + 3 };
	EResult m_eResult;
};


//-----------------------------------------------------------------------------
// Purpose: Sent by the Steam server to the client telling it to disconnect from the specified game server,
//			which it may be in the process of or already connected to.
//			The game client should immediately disconnect upon receiving this message.
//			This can usually occur if the user doesn't have rights to play on the game server.
//-----------------------------------------------------------------------------
struct ClientGameServerDeny_t
{
	enum { k_iCallback = k_iSteamUserCallbacks + 13 };

	uint32 m_uAppID;
	uint32 m_unGameServerIP;
	uint16 m_usGameServerPort;
	uint16 m_bSecure;
	uint32 m_uReason;
};


//-----------------------------------------------------------------------------
// Purpose: called when the callback system for this client is in an error state (and has flushed pending callbacks)
//			When getting this message the client should disconnect from Steam, reset any stored Steam state and reconnect.
//			This usually occurs in the rare event the Steam client has some kind of fatal error.
//-----------------------------------------------------------------------------
struct CallbackPipeFailure_t
{
	enum { k_iCallback = k_iSteamUserCallbacks + 17 };
};


#endif // ISTEAMUSER_H
