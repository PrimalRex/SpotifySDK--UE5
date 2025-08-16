// Copyright (c) Harris Barra. (MIT License)

# pragma once

#include "CoreMinimal.h"
#include "RequestUtils.h"
#include "SpotifyUser.generated.h"

USTRUCT(BlueprintType)
struct FUserProfile
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FString Username;
	UPROPERTY(BlueprintReadWrite)
	FString UserId;
	UPROPERTY(BlueprintReadWrite)
	FString Email;
	UPROPERTY(BlueprintReadWrite)
	FString UserUri;
	UPROPERTY(BlueprintReadWrite)
	FString ImgUrl;
};

class FSpotifyUser
{
public:
	// https://developer.spotify.com/documentation/web-api/reference/get-current-users-profile
	static void RequestUserProfile(const FString& UserToken, TFunction<void(const FUserProfile& Profile)> Callback)
	{
		FString BaseUrl = TEXT("https://api.spotify.com/v1/me");
		
		TMap<FString, FString> Headers;
		Headers.Add(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *UserToken));
		TSharedRef<IHttpRequest> HttpRequest = FRequestUtils::CreateGETRequest(BaseUrl, Headers);

		HttpRequest->OnProcessRequestComplete().BindLambda(
			[Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
			{
				if (bConnectedSuccessfully && Response.IsValid())
				{
					FUserProfile Profile;
					FString ResponseStr = Response->GetContentAsString();
					
					FRequestUtils::GetFieldEntry(*ResponseStr, "display_name", Profile.Username);
					FRequestUtils::GetFieldEntry(*ResponseStr, "id", Profile.UserId);
					FRequestUtils::GetFieldEntry(*ResponseStr, "email", Profile.Email);
					FRequestUtils::GetFieldEntry(*ResponseStr, "uri", Profile.UserUri);

					TArray<TSharedPtr<FJsonValue>> ImagesArray;
					FRequestUtils::GetArrayEntry(*ResponseStr, "images", ImagesArray);
					FRequestUtils::GetFieldEntryAtIndex(ImagesArray, "url", 0, Profile.ImgUrl);

					Callback(Profile);
				}
				else
				{
					FString ErrorStr = TEXT("Spotify User Profile request failed!!!");
					UE_LOG(LogTemp, Error, TEXT("Request Error: %s"), *ErrorStr);
				}
			}
		);

		HttpRequest->ProcessRequest();
	}
};
