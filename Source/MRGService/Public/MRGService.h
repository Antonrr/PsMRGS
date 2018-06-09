// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Runtime/Launch/Resources/Version.h"

#if ENGINE_MINOR_VERSION >= 15
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#endif

#include "ModuleManager.h"
#include <string>

#include "MRGService.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMRGSDelegate, uint8, MyEventName);

//USTRUCT()
//struct FPurchaseItem
//{
//    GENERATED_USTRUCT_BODY()
//    
//    UPROPERTY(EditAnywhere, Category = "Awm|MRGS")
//    FString sku;
//    
//    UPROPERTY(EditAnywhere, Category = "Awm|MRGS")
//    FString price;
//    
//    UPROPERTY(EditAnywhere, Category = "Awm|MRGS")
//    FString title;
//    
//    UPROPERTY(EditAnywhere, Category = "Awm|MRGS")
//    FString type;
//    
//    UPROPERTY(EditAnywhere, Category = "Awm|MRGS")
//    FString description;
//};
DECLARE_LOG_CATEGORY_EXTERN(LogAwmMrgs, Display, All);



USTRUCT(BlueprintType)
struct FAwmGameCenterAuthInfo
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FString PlayerId;
	
	UPROPERTY(BlueprintReadOnly)
	FString PublicKeyUrl;
	
	UPROPERTY(BlueprintReadOnly)
	FString Signature;
	
	UPROPERTY(BlueprintReadOnly)
	FString Salt;
	
	UPROPERTY(BlueprintReadOnly)
	FString Timestamp;
	
	UPROPERTY(BlueprintReadOnly)
	FString Nick;
	
	FAwmGameCenterAuthInfo()
	{
		PlayerId = "";
		PublicKeyUrl = "";
		Signature = "";
		Salt = "";
		Timestamp = "";
		Nick = "";
	}
	
	void Reset()
	{
		PlayerId = "";
		PublicKeyUrl = "";
		Signature = "";
		Salt = "";
		Timestamp = "";
		Nick = "";
	}
};

USTRUCT(BlueprintType)
struct FAwmGooglePlayAuthInfo
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FString AuthToken;
	
	FAwmGooglePlayAuthInfo()
	{
		AuthToken = "";
	}
	
	void Reset()
	{
		AuthToken = "";
	}
};

UENUM(BlueprintType)
enum class EMrgsEventsTypes:uint8
{
    TYPE_INIT_COMPLETE = 0,
    TYPE_PRODUCTS_LOADED,
    TYPE_PURCHASE_COMPLETE,
    TYPE_PURCHASE_FAILED,
    TYPE_SUPPORT_CLOSED,
    TYPE_SUPPORT_ERROR,
    TYPE_FULLSCREEN_CLOSED,
    TYPE_USERINIT_COMPLETE,
	TYPE_LOGIN_COMPLETE,
	TYPE_LOGIN_ERROR
};

USTRUCT()
struct FAwmPurchaseInfo
{
	GENERATED_USTRUCT_BODY()

	FString Sku;

	FString Price;

	FString Title;

	FString Type;
	
	FString Description;
	
	FAwmPurchaseInfo()
	{
		Sku = "";
		Price = "";
		Title = "";
		Type = "";
		Description = "";
	}
};

class FMRGServiceModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
    void InitModule();
    void InitModuleWithUid(const FString& UserId);
    void InitUser(const FString& UserId);
    void SendGAScreen(const FString& ScreenName);
    void ShowSupport();
    void SendGAEvent(const FString& Category, const FString& Action, const FString& Label, int32 Value);
    void SendFlurryEvent(const FString& Action);
    void SendAFEvent(const FString& EventName, const FString& Value);

    void ShowMyTargetShowcase();
    void ShowMyTargetFullscreen();
    void LoadBankProducts(const TArray<FString>& ProductsList);
    void ByProduct(const FString& ProductId);
	TArray<FAwmPurchaseInfo>& GetProducts();
	
	FString GetDeviceModel() const;
	FString GetDevicePlatform() const;
	
	void UpdateAuthToken(const FString& AuthToken);
	FString GetAuthToken() const;
	void SaveTokenToKeychain(const FString& TokenToSave);
	
	/** Save token to iCloud */
	void SaveTokenToICloud(const FString& AuthToken);
	
	/** Get saved token from iCloud */
	FString GetTokenFromICloud() const;
	
	void AddMetric(int32 MetricId);
    
    bool IsReady() const;
//    TArray<FPurchaseItem*> GetPurchaseItems() const;
    
//    DECLARE_EVENT_OneParam(FMRGServiceModule, FMRGSEvent, uint8)
    
    UPROPERTY()
    FMRGSDelegate MRGSEvent;
    
    UPROPERTY()
    EMrgsEventsTypes MrgsEventsTypes;
	
	UPROPERTY()
	TArray<FAwmPurchaseInfo> Products;

	UPROPERTY()
	class UMRGServiceSettings* MrgsSettings;
	
	void UpdateGameCenterAuthInfo();
	void UpdateGooglePlayAuthInfo(const FString& AuthToken);
	
	FAwmGameCenterAuthInfo GetGameCenterAuthInfo();
	FAwmGooglePlayAuthInfo GetGooglePlayAuthInfo();
	
	FString GetOpenUDID() const;
	
	void NavigateToUrl(FString Url);

protected:
    void MRGSInitCompleteCallback();
    void ProductsLoaded();
    void ProductsPurchaseComplete(const std::string& sku, const std::string& transactionId);
    void ProductsPurchaseFailed(const std::string& sku, const std::string& transactionId);
    void SupportDidClosed();
    void SupportDidReceiveError();
    void FullscreenDidClosed();
    bool bIsReady = false;
	
	FString LastUrl;
	
	FAwmGameCenterAuthInfo GameCenterAuthInfo;
	FAwmGooglePlayAuthInfo GooglePlayAuthInfo;
//    #if PLATFORM_IOS
//    static void ListenOpenURL(UIApplication* application, NSURL* url, NSString* sourceApplication, id annotation)
//    #endif
};
