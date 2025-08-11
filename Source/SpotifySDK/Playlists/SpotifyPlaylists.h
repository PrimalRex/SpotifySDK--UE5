// Copyright (c) Harris Barra. (MIT License)

#pragma once

#include "RequestUtils.h"
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
	FString PlaylistId;
	UPROPERTY(BlueprintReadWrite)
	FString ImgUrl;
};

class FSpotifyPlaylists
{
public:
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
					UE_LOG(LogTemp, Error, TEXT("Playlists got: %s"), *ResponseStr);

					TArray<TSharedPtr<FJsonValue>> PlaylistsArray;
					FRequestUtils::GetArrayEntry(*ResponseStr, "items", PlaylistsArray);

					for (const TSharedPtr<FJsonValue>& PlaylistValue : PlaylistsArray)
					{
						FPlaylistProfile Profile;

						FRequestUtils::GetFieldEntry(PlaylistValue, "name", Profile.Name);
						FRequestUtils::GetFieldEntry(PlaylistValue, "description", Profile.Description);
						FRequestUtils::GetFieldEntry(PlaylistValue, "id", Profile.PlaylistId);
						
						TArray<TSharedPtr<FJsonValue>> ImagesArray;
						FRequestUtils::GetArrayEntry(PlaylistValue, "images", ImagesArray);
						FRequestUtils::GetFieldEntryAtIndex(ImagesArray, "url", 0, Profile.ImgUrl);

						UE_LOG(LogTemp, Error, TEXT("Spotify Profile: %s"), *Profile.Name);
						UE_LOG(LogTemp, Error, TEXT("Spotify ID: %s"), *Profile.PlaylistId);
						UE_LOG(LogTemp, Error, TEXT("Spotify Description: %s"), *Profile.Description);
						UE_LOG(LogTemp, Error, TEXT("Spotify Image URL: %s"), *Profile.ImgUrl);
							
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
	
};
