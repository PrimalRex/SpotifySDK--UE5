# (WIP) SpotifySDK--UE5
Spotify Web API SDK built with native Unreal Engine wrappers and is event-driven (delegate system)

Note: The current SDK does not provide any logic on creating authentication tokens. This requires an intermediate step with a web-url callback. However there is support on Client Id and Client Secret to get you started with parsing data to a Launch URL.

# How to call the plugin

Once you've included the module in your Build.cs, you can call the module with:

```
SpotifySDKModule = &FSpotifySDKModule::Get();
```

Before calling any functions, you must set the authorization bearer token. The [Spotify Docs](https://developer.spotify.com/documentation/web-api/tutorials/code-flow) has instructions in how to facilitiate the token.

It is not recommended, but if you wish to set Client Id and Client Secret values, you may do so in `SpotifyAuth.h` and you can call these functions as follows:

```
SpotifySDKModule->GetClientId();
SpotifySDKModule->GetClientSecret();
```


Once you have obtained the token, simply call the `UpdateSpotifyToken()` function.

```
SpotifySDKModule->UpdateSpotifyUserToken("ABCDEFToken");
```

You can then call any endpoint functions. All operations are thread-safe and uses a event-driven system:

```
SpotifySDKModule->RequestTrackPreviewUrl(TEXT("1G1ZxaxFZQI9DArt6UzlrF"), [&](const FString& PreviewUrl)
  {	
    UE_LOG(LogTemp, Error, TEXT("Track Preview URL: %s"), *PreviewUrl);`
  });
```

Under-the-hood it uses the HTTP thread to asynchronously dispatch calls and uses a delegate binding for when the response has been returned.
