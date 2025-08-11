# SpotifySDK--UE5
Spotify Web API SDK built with native Unreal Engine wrappers

Note: The current SDK does not provide any logic on creating authentication tokens. This requires an intermediate step with a web-url callback. However there is support on Client Id and Client Secret to get you started with parsing data to a Launch URL.

# How to call the plugin

Once you've included the module in your Build.cs, you can call the module with:

```
SpotifySDKModule = &FSpotifySDKModule::Get();
```

You can then call functions to the module directly. All operations are thread-safe and uses a event-driven system:

```
SpotifySDKModule->RequestTrackPreviewUrl(TEXT("1G1ZxaxFZQI9DArt6UzlrF"), [&](const FString& PreviewUrl)
  {	
    UE_LOG(LogTemp, Error, TEXT("Track Preview URL: %s"), *PreviewUrl);`
  });
```

The under-the-hood uses the HTTP thread to asynchronously dispatch calls and uses a delegate binding for when the response has been returned.
