// Copyright (c) Harris Barra. (MIT License)

#pragma once

#include "RequestUtils.h"
#include "SpotifyTracks.generated.h"

USTRUCT(BlueprintType)
struct FTrackProfile
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FString Name;
	UPROPERTY(BlueprintReadWrite)
	FString TrackId;
	UPROPERTY(BlueprintReadWrite)
	int DurationMs;
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> Artists;
	UPROPERTY(BlueprintReadWrite)
	FString AlbumReleaseDate;
	UPROPERTY(BlueprintReadWrite)
	FString AlbumId;
	UPROPERTY(BlueprintReadWrite)
	FString ImgUrl;
};

class FSpotifyTracks
{
public:
	/**
	 * Requests the Track Preview URL for a Spotify track.
	 * WARNING: This method is a bit hacky as it parses the HTML response from the Spotify embed URL.
	 * As of August 2025 there is no official direct API endpoint to get the preview URL.
	 * @param TrackId The ID of the Spotify track.
	 * @param Callback A function that will be called with the preview URL.
	 */
	static void RequestTrackPreviewUrl(const FString& TrackId, TFunction<void(const FString& Url)> Callback)
	{
		FString BaseUrl = FString::Printf(
			TEXT("https://open.spotify.com/embed/track/%s"),
			*TrackId
		);

		TMap<FString, FString> Headers;
		TSharedRef<IHttpRequest> HttpRequest = FRequestUtils::CreateGETRequest(BaseUrl, Headers);

		HttpRequest->OnProcessRequestComplete().BindLambda(
			[Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
			{
				if (bConnectedSuccessfully && Response.IsValid())
				{
					FString ResponseStr = Response->GetContentAsString();
					FString PreviewUrl;

					// Parse the response string to find the preview URL
					// This is a bit hacky, but Spotify's embed URL will return the preview URL
					// We cannot source the URL directly from the API, so we have to parse it from the HTML response.
					const FString SearchKey = TEXT("audioPreview\":{\"url\":\"");
					int32 StartIndex = ResponseStr.Find(SearchKey, ESearchCase::IgnoreCase, ESearchDir::FromStart);

					if (StartIndex != -1)
					{
						StartIndex += SearchKey.Len();
						int32 EndIndex = ResponseStr.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart,
						                                  StartIndex);

						if (EndIndex != -1 && EndIndex > StartIndex)
						{
							PreviewUrl = ResponseStr.Mid(StartIndex, EndIndex - StartIndex);
						}
					}

					UE_LOG(LogTemp, Error, TEXT("Got Preview Url: %s"), *PreviewUrl);

					Callback(PreviewUrl);
				}
				else
				{
					FString ErrorStr = TEXT("Spotify Track Preview Url request failed!!!");
					UE_LOG(LogTemp, Error, TEXT("Request Error: %s"), *ErrorStr);
				}
			}
		);

		HttpRequest->ProcessRequest();
	}
};
