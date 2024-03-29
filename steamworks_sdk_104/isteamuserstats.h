//====== Copyright � 1996-2009, Valve Corporation, All rights reserved. =======
//
// Purpose: interface to stats, achievements, and leaderboards 
//
//=============================================================================

#ifndef ISTEAMUSERSTATS_H
#define ISTEAMUSERSTATS_H
#ifdef _WIN32
#pragma once
#endif

#include "isteamclient.h"

// size limit on stat or achievement name (UTF-8 encoded)
enum { k_cchStatNameMax = 128 };

// maximum number of bytes for a leaderboard name (UTF-8 encoded)
enum { k_cchLeaderboardNameMax = 128 };

// maximum number of details int32's storable for a single leaderboard entry
enum { k_cLeaderboardDetailsMax = 64 };

// handle to a single leaderboard
typedef uint64 SteamLeaderboard_t;

// handle to a set of downloaded entries in a leaderboard
typedef uint64 SteamLeaderboardEntries_t;

// type of data request, when downloading leaderboard entries
enum ELeaderboardDataRequest
{
	k_ELeaderboardDataRequestGlobal = 0,
	k_ELeaderboardDataRequestGlobalAroundUser = 1,
	k_ELeaderboardDataRequestFriends = 2,
};

// the sort order of a leaderboard
enum ELeaderboardSortMethod
{
	k_ELeaderboardSortMethodNone = 0,
	k_ELeaderboardSortMethodAscending = 1,	// top-score is lowest number
	k_ELeaderboardSortMethodDescending = 2,	// top-score is highest number
};

// the display type (used by the Steam Community web site) for a leaderboard
enum ELeaderboardDisplayType
{
	k_ELeaderboardDisplayTypeNone = 0, 
	k_ELeaderboardDisplayTypeNumeric = 1,			// simple numerical score
	k_ELeaderboardDisplayTypeTimeSeconds = 2,		// the score represents a time, in seconds
	k_ELeaderboardDisplayTypeTimeMilliSeconds = 3,	// the score represents a time, in milliseconds
};

// a single entry in a leaderboard, as returned by GetDownloadedLeaderboardEntry()
struct LeaderboardEntry_t
{
	CSteamID m_steamIDUser; // user with the entry - use SteamFriends()->GetFriendPersonaName() & SteamFriends()->GetFriendAvatar() to get more info
	int32 m_nGlobalRank;	// [1..N], where N is the number of users with an entry in the leaderboard
	int32 m_nScore;			// score as set in the leaderboard
	int32 m_cDetails;		// number of int32 details available for this entry
};


//-----------------------------------------------------------------------------
// Purpose: Functions for accessing stats, achievements, and leaderboard information
//-----------------------------------------------------------------------------
class ISteamUserStats
{
public:
	// Ask the server to send down this user's data and achievements for this game
	virtual bool RequestCurrentStats() = 0;

	// Data accessors
	virtual bool GetStat( const char *pchName, int32 *pData ) = 0;
	virtual bool GetStat( const char *pchName, float *pData ) = 0;

	// Set / update data
	virtual bool SetStat( const char *pchName, int32 nData ) = 0;
	virtual bool SetStat( const char *pchName, float fData ) = 0;
	virtual bool UpdateAvgRateStat( const char *pchName, float flCountThisSession, double dSessionLength ) = 0;

	// Achievement flag accessors
	virtual bool GetAchievement( const char *pchName, bool *pbAchieved ) = 0;
	virtual bool SetAchievement( const char *pchName ) = 0;
	virtual bool ClearAchievement( const char *pchName ) = 0;

	// Store the current data on the server, will get a callback when set
	// And one callback for every new achievement
	virtual bool StoreStats() = 0;

	// Achievement / GroupAchievement metadata

	// Gets the icon of the achievement, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set
	virtual int GetAchievementIcon( const char *pchName ) = 0;
	// Get general attributes (display name / text, etc) for an Achievement
	virtual const char *GetAchievementDisplayAttribute( const char *pchName, const char *pchKey ) = 0;

	// Achievement progress - triggers an AchievementProgress callback, that is all.
	// Calling this w/ N out of N progress will NOT set the achievement, the game must still do that.
	virtual bool IndicateAchievementProgress( const char *pchName, uint32 nCurProgress, uint32 nMaxProgress ) = 0;

	// Friends stats & achievements

	// downloads stats for the user
	// returns a UserStatsReceived_t received when completed
	// if the other user has no stats, UserStatsReceived_t.m_eResult will be set to k_EResultFail
	// these stats won't be auto-updated; you'll need to call RequestUserStats() again to refresh any data
	virtual SteamAPICall_t RequestUserStats( CSteamID steamIDUser ) = 0;

	// requests stat information for a user, usable after a successful call to RequestUserStats()
	virtual bool GetUserStat( CSteamID steamIDUser, const char *pchName, int32 *pData ) = 0;
	virtual bool GetUserStat( CSteamID steamIDUser, const char *pchName, float *pData ) = 0;
	virtual bool GetUserAchievement( CSteamID steamIDUser, const char *pchName, bool *pbAchieved ) = 0;

	// Reset stats 
	virtual bool ResetAllStats( bool bAchievementsToo ) = 0;

	// Leaderboard functions

	// asks the Steam back-end for a leaderboard by name, and will create it if it's not yet
	// This call is asynchronous, with the result returned in LeaderboardFindResult_t
	virtual SteamAPICall_t FindOrCreateLeaderboard( const char *pchLeaderboardName, ELeaderboardSortMethod eLeaderboardSortMethod, ELeaderboardDisplayType eLeaderboardDisplayType ) = 0;

	// as above, but won't create the leaderboard if it's not found
	// This call is asynchronous, with the result returned in LeaderboardFindResult_t
	virtual SteamAPICall_t FindLeaderboard( const char *pchLeaderboardName ) = 0;

	// returns the name of a leaderboard
	virtual const char *GetLeaderboardName( SteamLeaderboard_t hSteamLeaderboard ) = 0;

	// returns the total number of entries in a leaderboard, as of the last request
	virtual int GetLeaderboardEntryCount( SteamLeaderboard_t hSteamLeaderboard ) = 0;

	// returns the sort method of the leaderboard
	virtual ELeaderboardSortMethod GetLeaderboardSortMethod( SteamLeaderboard_t hSteamLeaderboard ) = 0;

	// returns the display type of the leaderboard
	virtual ELeaderboardDisplayType GetLeaderboardDisplayType( SteamLeaderboard_t hSteamLeaderboard ) = 0;

	// Asks the Steam back-end for a set of rows in the leaderboard.
	// This call is asynchronous, with the result returned in LeaderboardScoresDownloaded_t
	// LeaderboardScoresDownloaded_t will contain a handle to pull the results from GetDownloadedLeaderboardEntries() (below)
	// You can ask for more entries than exist, and it will return as many as do exist.
	// k_ELeaderboardDataRequestGlobal requests rows in the leaderboard from the full table, with nRangeStart & nRangeEnd in the range [1, TotalEntries]
	// k_ELeaderboardDataRequestGlobalAroundUser requests rows around the current user, nRangeStart being negate
	//   e.g. DownloadLeaderboardEntries( hLeaderboard, k_ELeaderboardDataRequestGlobalAroundUser, -3, 3 ) will return 7 rows, 3 before the user, 3 after
	// k_ELeaderboardDataRequestFriends requests all the rows for friends of the current user 
	virtual SteamAPICall_t DownloadLeaderboardEntries( SteamLeaderboard_t hSteamLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd ) = 0;

	// Returns data about a single leaderboard entry
	// use a for loop from 0 to LeaderboardScoresDownloaded_t::m_cEntryCount to get all the downloaded entries
	// e.g.
	//		void OnLeaderboardScoresDownloaded( LeaderboardScoresDownloaded_t *pLeaderboardScoresDownloaded )
	//		{
	//			for ( int index = 0; index < pLeaderboardScoresDownloaded->m_cEntryCount; index++ )
	//			{
	//				LeaderboardEntry_t leaderboardEntry;
	//				int32 details[3];		// we know this is how many we've stored previously
	//				GetDownloadedLeaderboardEntry( pLeaderboardScoresDownloaded->m_hSteamLeaderboardEntries, index, &leaderboardEntry, details, 3 );
	//				assert( leaderboardEntry.m_cDetails == 3 );
	//				...
	//			}
	// once you've accessed all the entries, the data will be free'd, and the SteamLeaderboardEntries_t handle will become invalid
	virtual bool GetDownloadedLeaderboardEntry( SteamLeaderboardEntries_t hSteamLeaderboardEntries, int index, LeaderboardEntry_t *pLeaderboardEntry, int32 *pDetails, int cDetailsMax ) = 0;

	// Uploads a user score to the Steam back-end.
	// This call is asynchronous, with the result returned in LeaderboardScoreUploaded_t
	// If the score passed in is no better than the existing score this user has in the leaderboard, then the leaderboard will not be updated.
	// Details are extra game-defined information regarding how the user got that score
	// pScoreDetails points to an array of int32's, cScoreDetailsCount is the number of int32's in the list
	virtual SteamAPICall_t UploadLeaderboardScore( SteamLeaderboard_t hSteamLeaderboard, int32 nScore, int32 *pScoreDetails, int cScoreDetailsCount ) = 0;
};

#define STEAMUSERSTATS_INTERFACE_VERSION "STEAMUSERSTATS_INTERFACE_VERSION005"


//-----------------------------------------------------------------------------
// Purpose: called when the latests stats and achievements have been received
//			from the server
//-----------------------------------------------------------------------------
struct UserStatsReceived_t
{
	enum { k_iCallback = k_iSteamUserStatsCallbacks + 1 };
	uint64		m_nGameID;		// Game these stats are for
	EResult		m_eResult;		// Success / error fetching the stats
	CSteamID	m_steamIDUser;	// The user for whom the stats are retrieved for
};


//-----------------------------------------------------------------------------
// Purpose: result of a request to store the user stats for a game
//-----------------------------------------------------------------------------
struct UserStatsStored_t
{
	enum { k_iCallback = k_iSteamUserStatsCallbacks + 2 };
	uint64		m_nGameID;		// Game these stats are for
	EResult		m_eResult;		// success / error
};


//-----------------------------------------------------------------------------
// Purpose: result of a request to store the achievements for a game, or an 
//			"indicate progress" call. If both m_nCurProgress and m_nMaxProgress
//			are zero, that means the achievement has been fully unlocked.
//-----------------------------------------------------------------------------
struct UserAchievementStored_t
{
	enum { k_iCallback = k_iSteamUserStatsCallbacks + 3 };

	uint64		m_nGameID;				// Game this is for
	bool		m_bGroupAchievement;	// if this is a "group" achievement
	char		m_rgchAchievementName[k_cchStatNameMax];		// name of the achievement
	uint32		m_nCurProgress;			// current progress towards the achievement
	uint32		m_nMaxProgress;			// "out of" this many
};


//-----------------------------------------------------------------------------
// Purpose: call result for finding a leaderboard, returned as a result of FindOrCreateLeaderboard() or FindLeaderboard()
//			use CCallResult<> to map this async result to a member function
//-----------------------------------------------------------------------------
struct LeaderboardFindResult_t
{
	enum { k_iCallback = k_iSteamUserStatsCallbacks + 4 };
	SteamLeaderboard_t m_hSteamLeaderboard;	// handle to the leaderboard serarched for, 0 if no leaderboard found
	uint8 m_bLeaderboardFound;				// 0 if no leaderboard found
};


//-----------------------------------------------------------------------------
// Purpose: call result indicating scores for a leaderboard have been downloaded and are ready to be retrieved, returned as a result of DownloadLeaderboardEntries()
//			use CCallResult<> to map this async result to a member function
//-----------------------------------------------------------------------------
struct LeaderboardScoresDownloaded_t
{
	enum { k_iCallback = k_iSteamUserStatsCallbacks + 5 };
	SteamLeaderboard_t m_hSteamLeaderboard;
	SteamLeaderboardEntries_t m_hSteamLeaderboardEntries;	// the handle to pass into GetDownloadedLeaderboardEntries()
	int m_cEntryCount; // the number of entries downloaded
};


//-----------------------------------------------------------------------------
// Purpose: call result indicating scores has been uploaded, returned as a result of UploadLeaderboardScore()
//			use CCallResult<> to map this async result to a member function
//-----------------------------------------------------------------------------
struct LeaderboardScoreUploaded_t
{
	enum { k_iCallback = k_iSteamUserStatsCallbacks + 6 };
	uint8 m_bSuccess;			// 1 if the call was successful
	SteamLeaderboard_t m_hSteamLeaderboard;	// the leaderboard handle that was
	int32 m_nScore;				// the score that was attempted to set
	uint8 m_bScoreChanged;		// true if the score in the leaderboard change, false if the existing score was better
	int m_nGlobalRankNew;		// the new global rank of the user in this leaderboard
	int m_nGlobalRankPrevious;	// the previous global rank of the user in this leaderboard; 0 if the user had no existing entry in the leaderboard
};


#endif // ISTEAMUSER_H
