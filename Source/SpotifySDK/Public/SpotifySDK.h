// Copyright (c) Harris Barra. (MIT License)

#pragma once

#include "RequestUtils.h"
#include "Modules/ModuleManager.h"
#include "SpotifySDK/Auth/SpotifyAuth.h"
#include "SpotifySDK/Playlists/SpotifyPlaylists.h"
#include "SpotifySDK/Tracks/SpotifyTracks.h"
#include "SpotifySDK/UserClient/SpotifyUser.h"

class FSpotifySDKModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	SPOTIFYSDK_API virtual void StartupModule() override;
	SPOTIFYSDK_API virtual void ShutdownModule() override;

	static SPOTIFYSDK_API FSpotifySDKModule& Get()
	{
		return Singleton ? *Singleton : FModuleManager::LoadModuleChecked<FSpotifySDKModule>("SpotifySDK");
	}

	///////////////////////////////////////

	SPOTIFYSDK_API const FSpotifyAuth& GetSpotifyAuth() const { return *SpotifyAuth; }
	SPOTIFYSDK_API FSpotifyAuth& GetSpotifyAuth() { return *SpotifyAuth; }

	SPOTIFYSDK_API void UpdateSpotifyUserToken(const FString& Token) { GetSpotifyAuth().UpdateSpotifyUserToken(Token); }

	///////////////////////////////////////

	SPOTIFYSDK_API void RequestUserProfile(TFunction<void(const FUserProfile& Profile)> Callback)
	{
		FSpotifyUser::RequestUserProfile(GetSpotifyUserToken(), Callback);
	}

	SPOTIFYSDK_API void RequestUserPlaylists(const FString& UserId, const TPair<int, int> LimitOffset, const TFunction<void(const TArray<FPlaylistProfile>& Playlists)>& Callback)
	{
		FSpotifyPlaylists::RequestUserPlaylists(GetSpotifyUserToken(), UserId, LimitOffset, Callback);
	}

	SPOTIFYSDK_API void RequestPlaylist(const FString& PlaylistId, const TFunction<void(const FPlaylistProfile& Playlist)>& Callback)
	{
		FSpotifyPlaylists::RequestPlaylist(GetSpotifyUserToken(), PlaylistId, Callback);
	}

	SPOTIFYSDK_API void BatchRequestPlaylists(const TArray<FString>& PlaylistIds, const TFunction<void(const TArray<FPlaylistProfile>& Playlists)>& Callback)
	{
		FSpotifyPlaylists::BatchRequestPlaylists(GetSpotifyUserToken(), PlaylistIds, Callback);
	}

	SPOTIFYSDK_API void RequestPlaylistTracks(const FString& PlaylistId, const TPair<int, int> LimitOffset, const TFunction<void(const FPlaylistData& PlaylistData)>& Callback)
	{
		FSpotifyPlaylists::RequestPlaylistTracks(GetSpotifyUserToken(), PlaylistId, LimitOffset, Callback);
	}

	SPOTIFYSDK_API void RequestTrackPreviewUrl(const FString& TrackId, const TFunction<void(const FString& Url)>& Callback)
	{
		FSpotifyTracks::RequestTrackPreviewUrl(TrackId, Callback);
	}

	///////////////////////////////////////

	SPOTIFYSDK_API const FString& GetClientId() { return GetSpotifyAuth().GetClientId(); }
	SPOTIFYSDK_API const FString& GetClientSecret() { return GetSpotifyAuth().GetClientSecret(); }

	/**
	 * Get the Logged-in Spotify User Access Token.
	 * This token is given after the user has agreed authentication and
	 * serves as the primary bearer for all higher-level endpoint calls.
	 * @return A reference to the User Token string.
	 */
	SPOTIFYSDK_API const FString& GetSpotifyUserToken() { return GetSpotifyAuth().GetSpotifyUserToken(); }

private:
	static FSpotifySDKModule* Singleton;

	/**
	 * Authentication instance for Spotify SDK.
	 * Holds all necessary credentials and tokens.
	 * This is a singleton instance that can be accessed statically.
	 */
	TUniquePtr<FSpotifyAuth> SpotifyAuth = MakeUnique<FSpotifyAuth>();
};
