// Copyright (c) Harris Barra. (MIT License)

#pragma once

#include "RequestUtils.h"
#include "SpotifySDK/Tracks/SpotifyTracks.h"
#include "SpotifyPlaylists.generated.h"

USTRUCT(BlueprintType)
struct FPlaylistProfile
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FString Name;
	UPROPERTY(BlueprintReadWrite)
	FString Description;
	UPROPERTY(BlueprintReadWrite)
	int TrackCount;
	UPROPERTY(BlueprintReadWrite)
	FString PlaylistId;
	UPROPERTY(BlueprintReadWrite)
	FString ImgUrl;
};

USTRUCT(BlueprintType)
struct FPlaylistData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FTrackProfile> Tracks;
	UPROPERTY(BlueprintReadWrite)
	int TrackCount;
};

class FSpotifyPlaylists
{
public:
	// https://developer.spotify.com/documentation/web-api/reference/get-list-users-playlists
	static void RequestUserPlaylists(const FString& UserToken, const FString& UserId, TFunction<void(const TArray<FPlaylistProfile>& Playlists)> Callback)
	{
		// TODO: Temporarily hardcoded to 50 items, should be paginated
		FString BaseUrl = FString::Printf(
			TEXT("https://api.spotify.com/v1/users/%s/playlists?limit=50&offset=0"),
			*UserId
		);
			
		TMap<FString, FString> Headers;
		Headers.Add(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *UserToken));
		TSharedRef<IHttpRequest> HttpRequest = FRequestUtils::CreateGETRequest(BaseUrl, Headers);

		HttpRequest->OnProcessRequestComplete().BindLambda(
			[Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
			{
				if (bConnectedSuccessfully && Response.IsValid())
				{
					TArray<FPlaylistProfile> Playlists;
					FString ResponseStr = Response->GetContentAsString();

					TArray<TSharedPtr<FJsonValue>> PlaylistsArray;
					FRequestUtils::GetArrayEntry(*ResponseStr, "items", PlaylistsArray);

					for (const TSharedPtr<FJsonValue>& PlaylistValue : PlaylistsArray)
					{
						FPlaylistProfile Profile;

						FRequestUtils::GetFieldEntry(PlaylistValue, "name", Profile.Name);
						FRequestUtils::GetFieldEntry(PlaylistValue, "description", Profile.Description);

						TSharedPtr<FJsonObject> TrackObject;
						FRequestUtils::GetObjectEntry(PlaylistValue, "tracks", TrackObject);
						FRequestUtils::GetFieldEntry(TrackObject, "total", Profile.TrackCount);
						
						FRequestUtils::GetFieldEntry(PlaylistValue, "id", Profile.PlaylistId);
						
						TArray<TSharedPtr<FJsonValue>> ImagesArray;
						FRequestUtils::GetArrayEntry(PlaylistValue, "images", ImagesArray);
						FRequestUtils::GetFieldEntryAtIndex(ImagesArray, "url", 0, Profile.ImgUrl);
							
						Playlists.Add(Profile);
					}

					Callback(Playlists);
				}
				else
				{
					FString ErrorStr = TEXT("Spotify User Playlists request failed!!!");
					UE_LOG(LogTemp, Error, TEXT("Request Error: %s"), *ErrorStr);
				}
			}
		);

		HttpRequest->ProcessRequest();
	}

	// https://developer.spotify.com/documentation/web-api/reference/get-playlists-tracks
	// Note: This function retrieves TRACKS and IGNORES Episodes
	static void RequestPlaylistTracks(const FString& UserToken, const FString& PlaylistId, const TPair<int, int> LimitOffset, TFunction<void(const FPlaylistData& PlaylistData)> Callback)
	{
		TSharedRef<FPlaylistData> PlaylistData = MakeShareable(new FPlaylistData());
		TSharedPtr<TFunction<void(TPair<int, int>)>> ExpandPlaylist = MakeShared<TFunction<void(TPair<int, int>)>>();

		*ExpandPlaylist = [=](TPair<int,int> LambdaLimitOffset)
		{
			RequestPlaylistTracksImpl(UserToken, PlaylistId, LambdaLimitOffset, [=](const FString& ResponseStr)
			{
				int TotalTracks;
				FRequestUtils::GetFieldEntry(*ResponseStr, "total", TotalTracks);

				TArray<TSharedPtr<FJsonValue>> ItemsArray;
				FRequestUtils::GetArrayEntry(*ResponseStr, "items", ItemsArray);
				
				for (const TSharedPtr<FJsonValue>& ItemsValue : ItemsArray)
				{
					// We need to verify that the track is not a locally added track as there are no preview URLs
					// for those tracks.
					bool bIsLocalTrack = false;
					FRequestUtils::GetFieldEntry(ItemsValue, "is_local", bIsLocalTrack);
					if (!bIsLocalTrack)
					{
						FTrackProfile Profile;

						TSharedPtr<FJsonObject> TrackObject;
						FRequestUtils::GetObjectEntry(ItemsValue, "track", TrackObject);
					
						FRequestUtils::GetFieldEntry(TrackObject, "name", Profile.Name);
						FRequestUtils::GetFieldEntry(TrackObject, "id", Profile.TrackId);
						FRequestUtils::GetFieldEntry(TrackObject, "duration_ms", Profile.DurationMs);

						TArray<TSharedPtr<FJsonValue>> ArtistsArray;
						FRequestUtils::GetArrayEntry(TrackObject, "artists", ArtistsArray);
					
						for (const TSharedPtr<FJsonValue>& ArtistValue : ArtistsArray)
						{
							TArray<FString>& Artists = Profile.Artists;
							FString Artist;
						
							FRequestUtils::GetFieldEntry(ArtistValue, "name", Artist);
							Artists.Add(Artist);
						}
					
						TSharedPtr<FJsonObject> AlbumObject;
						FRequestUtils::GetObjectEntry(TrackObject, "album", AlbumObject);

						FRequestUtils::GetFieldEntry(AlbumObject, "id", Profile.AlbumId);
						FRequestUtils::GetFieldEntry(AlbumObject, "release_date", Profile.AlbumReleaseDate);
					
						TArray<TSharedPtr<FJsonValue>> ImagesArray;
						FRequestUtils::GetArrayEntry(AlbumObject, "images", ImagesArray);
						FRequestUtils::GetFieldEntryAtIndex(ImagesArray, "url", 0, Profile.ImgUrl);
				
						PlaylistData->Tracks.Add(Profile);
					}
				}

				int CurrentTrackIndex = LambdaLimitOffset.Key + LambdaLimitOffset.Value;
				if (CurrentTrackIndex < TotalTracks)
				{
					(*ExpandPlaylist)(TPair<int, int>(100, CurrentTrackIndex));
				}
				else
				{
					PlaylistData->TrackCount = PlaylistData->Tracks.Num();
					Callback(*PlaylistData);
				}
			});
		};

		(*ExpandPlaylist)(LimitOffset);
	}

	static void RequestPlaylistTracksImpl(const FString& UserToken, const FString& PlaylistId, const TPair<int, int> LimitOffset, TFunction<void(const FString& Response)> Callback)
	{
		FString BaseUrl = FString::Printf(
			TEXT("https://api.spotify.com/v1/playlists/%s/tracks?limit=%d&offset=%d"),
			*PlaylistId,
			LimitOffset.Key,
			LimitOffset.Value
		);
			
		TMap<FString, FString> Headers;
		Headers.Add(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *UserToken));
		TSharedRef<IHttpRequest> HttpRequest = FRequestUtils::CreateGETRequest(BaseUrl, Headers);

		HttpRequest->OnProcessRequestComplete().BindLambda(
			[&, Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
			{
				if (bConnectedSuccessfully && Response.IsValid())
				{
					FString ResponseStr = Response->GetContentAsString();
					Callback(ResponseStr);
				}
				else
				{
					FString ErrorStr = TEXT("Spotify Playlist Tracks request failed!!!");
					UE_LOG(LogTemp, Error, TEXT("Request Error: %s"), *ErrorStr);
				}
			}
		);

		HttpRequest->ProcessRequest();
	}
private:
	
};
