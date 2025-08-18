// Copyright (c) Harris Barra. (MIT License)

#pragma once

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

class FRequestUtils
{
public:
	// This is a utility class for creating HTTP requests and parsing JSON responses.
	// TODO: Need to rework how frequently deserialization is called to improve perf.

	static TSharedRef<IHttpRequest> CreatePOSTRequest(const FString& Url, const FString& RequestContent)
	{
		TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

		HttpRequest->SetURL(Url);
		HttpRequest->SetVerb(TEXT("POST"));
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
		HttpRequest->SetContentAsString(RequestContent);

		return HttpRequest;
	}

	static TSharedRef<IHttpRequest> CreateGETRequest(const FString& Url, const TMap<FString, FString>& Headers = {})
	{
		TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

		HttpRequest->SetURL(Url);
		HttpRequest->SetVerb(TEXT("GET"));

		for (const auto& HeaderPair : Headers)
		{
			HttpRequest->SetHeader(HeaderPair.Key, HeaderPair.Value);
		}
		return HttpRequest;
	}

	//////////// JSON Parsing ////////////

	static bool ParseResponseString(const FString& ResponseString, TSharedPtr<FJsonObject>& JsonObject)
	{
		JsonObject = MakeShareable(new FJsonObject());
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			return true;
		}

		return false;
	}

	static bool GetObjectEntry(const TSharedPtr<FJsonValue>& ResponseValue, const FString& ObjectName, TSharedPtr<FJsonObject>& OutValue)
	{
		TSharedPtr<FJsonObject> JsonObject = ResponseValue->AsObject();

		if (!JsonObject.IsValid())
		{
			return false;
		}

		if (JsonObject->HasField(ObjectName))
		{
			OutValue = JsonObject->GetObjectField(ObjectName);
			return true;
		}

		return false;
	}

	static bool GetObjectEntry(const TSharedPtr<FJsonObject>& ResponseObject, const FString& ObjectName, TSharedPtr<FJsonObject>& OutValue)
	{
		if (!ResponseObject.IsValid())
		{
			return false;
		}

		if (ResponseObject->HasField(ObjectName))
		{
			OutValue = ResponseObject->GetObjectField(ObjectName);
			return true;
		}

		return false;
	}

	static bool GetObjectEntry(const FString& ResponseString, const FString& ObjectName, TSharedPtr<FJsonObject>& OutValue)
	{
		TSharedPtr<FJsonObject> JsonObject;
		ParseResponseString(ResponseString, JsonObject);

		if (!JsonObject.IsValid())
		{
			return false;
		}

		if (JsonObject->HasField(ObjectName))
		{
			OutValue = JsonObject->GetObjectField(ObjectName);
			return true;
		}

		return false;
	}

	static bool GetArrayEntry(const FString& ResponseString, const FString& ArrayName, TArray<TSharedPtr<FJsonValue>>& OutValue)
	{
		TSharedPtr<FJsonObject> JsonObject;

		if (ParseResponseString(ResponseString, JsonObject) && JsonObject->HasField(ArrayName))
		{
			OutValue = JsonObject->GetArrayField(ArrayName);
			return true;
		}

		return false;
	}

	static bool GetArrayEntry(const TSharedPtr<FJsonValue>& ResponseValue, const FString& ArrayName, TArray<TSharedPtr<FJsonValue>>& OutValue)
	{
		TSharedPtr<FJsonObject> JsonObject = ResponseValue->AsObject();

		if (!JsonObject.IsValid())
		{
			return false;
		}

		if (JsonObject->HasField(ArrayName))
		{
			OutValue = JsonObject->GetArrayField(ArrayName);
			return true;
		}

		return false;
	}

	static bool GetArrayEntry(const TSharedPtr<FJsonObject>& ResponseObject, const FString& ArrayName, TArray<TSharedPtr<FJsonValue>>& OutValue)
	{
		if (!ResponseObject.IsValid())
		{
			return false;
		}

		if (ResponseObject->HasField(ArrayName))
		{
			OutValue = ResponseObject->GetArrayField(ArrayName);
			return true;
		}

		return false;
	}

	static bool GetFieldEntryAtIndex(const TArray<TSharedPtr<FJsonValue>>& JsonArray, const FString& FieldName, const int32 Index, FString& OutValue)
	{
		if (!JsonArray.IsValidIndex(Index))
		{
			OutValue = TEXT("Error: Out of bounds");
			return false;
		}

		TSharedPtr<FJsonObject> JsonObject = JsonArray[Index]->AsObject();

		if (!JsonObject->HasField(FieldName))
		{
			OutValue = TEXT("Error: Field not found");
			return false;
		}

		OutValue = JsonObject->GetStringField(FieldName);
		return true;
	}

	static bool GetFieldEntry(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, FString& OutValue)
	{
		if (!JsonObject->HasField(FieldName))
		{
			OutValue = TEXT("Error: Field not found");
			return false;
		}

		OutValue = JsonObject->GetStringField(FieldName);
		return true;
	}

	static bool GetFieldEntry(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, int& OutValue)
	{
		if (!JsonObject->HasField(FieldName))
		{
			OutValue = -1;
			return false;
		}

		OutValue = JsonObject->GetIntegerField(FieldName);
		return true;
	}

	static bool GetFieldEntry(const FString& ResponseString, const FString& FieldName, FString& OutValue)
	{
		TSharedPtr<FJsonObject> JsonObject;

		if (ParseResponseString(ResponseString, JsonObject) && JsonObject->HasField(FieldName))
		{
			OutValue = JsonObject->GetStringField(FieldName);
			return true;
		}
		else
		{
			OutValue = TEXT("Error");
			return false;
		}
	}

	static bool GetFieldEntry(const TSharedPtr<FJsonValue>& ResponseValue, const FString& FieldName, FString& OutValue)
	{
		TSharedPtr<FJsonObject> JsonObject = ResponseValue->AsObject();

		if (!JsonObject.IsValid())
		{
			return false;
		}

		if (JsonObject->HasField(FieldName))
		{
			OutValue = JsonObject->GetStringField(FieldName);
			return true;
		}
		else
		{
			OutValue = TEXT("Error");
			return false;
		}
	}

	static bool GetFieldEntry(const TSharedPtr<FJsonValue>& ResponseValue, const FString& FieldName, bool& OutValue)
	{
		TSharedPtr<FJsonObject> JsonObject = ResponseValue->AsObject();

		if (!JsonObject.IsValid())
		{
			return false;
		}

		if (JsonObject->HasField(FieldName))
		{
			OutValue = JsonObject->GetBoolField(FieldName);
			return true;
		}
		else
		{
			OutValue = false;
			return OutValue;
		}
	}

	static bool GetFieldEntry(const FString& ResponseString, const FString& FieldName, int& OutValue)
	{
		TSharedPtr<FJsonObject> JsonObject;

		if (ParseResponseString(ResponseString, JsonObject) && JsonObject->HasField(FieldName))
		{
			OutValue = JsonObject->GetIntegerField(FieldName);
			return true;
		}
		else
		{
			OutValue = -1;
			return false;
		}
	}

private:
};
