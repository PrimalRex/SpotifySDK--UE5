// Copyright (c) Harris Barra. (MIT License)

#include <SpotifySDK.h>

#define LOCTEXT_NAMESPACE "FSpotifySDKModule"

FSpotifySDKModule* FSpotifySDKModule::Singleton = nullptr;

void FSpotifySDKModule::StartupModule()
{
	// This will execute at runtime as soon as the module is loaded into mem
	Singleton = this;
}

void FSpotifySDKModule::ShutdownModule()
{
	Singleton = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSpotifySDKModule, SpotifySDK)
