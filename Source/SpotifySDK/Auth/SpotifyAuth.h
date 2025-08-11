// Copyright (c) Harris Barra. (MIT License)

#pragma once

class FSpotifyAuth
{
public:	
	const FString& GetClientId() { return ClientId; }
	const FString& GetClientSecret() { return ClientSecret; }
	
	/**
	 * Get the Logged-in Spotify User Access Token.
	 * This token is given after the user has agreed authentication and
	 * serves as the primary bearer for all higher-level endpoint calls.
	 * @return A shared reference to the User Token string.
	 */
	const FString& GetSpotifyUserToken() { return SpotifyUserToken; }
	void UpdateSpotifyUserToken(const FString& Token) { SpotifyUserToken = Token; }

private:
	FString ClientId = FString("...");
	// This is the Client Secret for the Spotify App, which should be kept private.
	// For security reasons, it is not recommended to hardcode this in the codebase.
	// For Shipping, use a PKCE implementation instead!
	FString ClientSecret = FString("...");

	FString SpotifyUserToken;
};

