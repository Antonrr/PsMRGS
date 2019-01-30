// Copyright 2018 Pushkin Studio. All Rights Reserved.

#include "PsMRGSProxyAndroid.h"
#include "PsMRGSSettings.h"
#include "PsMRGSCommon.h"
#include "Async/TaskGraphInterfaces.h"

UPsMRGSProxyAndroid::UPsMRGSProxyAndroid(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bInitComplete = false;
	bUserLoggedin = false;
}

#if PLATFORM_ANDROID

FString MRGSJniHelper::JavaStringToFstring(jstring jstr)
{
	if (jstr == NULL)
	{
		return FString(TEXT(""));
	}
	
	JNIEnv *env = 0;
	env = FAndroidApplication::GetJavaEnv(true);
	if (!env)
	{
		return FString(TEXT(""));
	}
	
	FString ConvertedString;
	const char* chars = env->GetStringUTFChars(jstr, 0);
	ConvertedString = FString(UTF8_TO_TCHAR(chars));
	env->ReleaseStringUTFChars(jstr, chars);
	
	return ConvertedString;
}

void UPsMRGSProxyAndroid::InitModule()
{
	if (IsReady())
	{
		return;
	}
	
	const UPsMRGSSettings* MRGSSettings = GetDefault<UPsMRGSSettings>();
	if (MRGSSettings == nullptr || MRGSSettings->IsValidLowLevel() == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSSettings not initialized or removed"), *PS_FUNC_LINE);
		return;
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		jstring AppId = Env->NewStringUTF(TCHAR_TO_UTF8(*MRGSSettings->AndroidMrgsAppId));
		jstring Secret = Env->NewStringUTF(TCHAR_TO_UTF8(*MRGSSettings->AndroidMrgsSecret));
		static jmethodID InitjMethod = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_initWithAppIdAndSecret", "(Ljava/lang/String;Ljava/lang/String;)V", false);
		
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, InitjMethod, AppId, Secret);
		Env->DeleteLocalRef(AppId);
		Env->DeleteLocalRef(Secret);
	}
}

void UPsMRGSProxyAndroid::InitUser(const FString& UserId)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID InitUserjMethod = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_initUserWithId", "(Ljava/lang/String;)V", false);
		
		jstring jUid = Env->NewStringUTF(TCHAR_TO_UTF8(*UserId));
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, InitUserjMethod, jUid);
		Env->DeleteLocalRef(jUid);
	}
}

void UPsMRGSProxyAndroid::LoadStoreProducts(const TArray<FString>& ProductsList)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	if (ProductsList.Num() == 0)
	{
		UE_LOG(LogMRGS, Warning, TEXT("%s: trying to load empty list of products"), *PS_FUNC_LINE);
		return;
	}
	
	FString ResultList;
	for (int32 i = 0; i < ProductsList.Num(); ++i)
	{
		ResultList += ProductsList[i];
		if(i < ProductsList.Num() - 1)
		{
			ResultList += TEXT(",");
		}
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID LoadProducts = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_getProductsInfo", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, LoadProducts, Env->NewStringUTF(TCHAR_TO_UTF8(*ResultList)));
	}
}

void UPsMRGSProxyAndroid::OnStoreProductsLoaded(TArray<FPsMRGSPurchaseInfo> InLoadedProducts)
{
	LoadedProducts = InLoadedProducts;
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PRODUCTS_LOADED);
		}
	});
}

const TArray<FPsMRGSPurchaseInfo>& UPsMRGSProxyAndroid::GetProducts() const
{
	return LoadedProducts;
}

void UPsMRGSProxyAndroid::BuyProduct(const FString& ProductId, const FString& Payload)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID BuyItem = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_buyItem", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, BuyItem, Env->NewStringUTF(TCHAR_TO_UTF8(*ProductId)));
	}
}

void UPsMRGSProxyAndroid::SendGAScreen(const FString& InScreenName)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID SendGAScreen = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_sendGAScreen", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, SendGAScreen, Env->NewStringUTF(TCHAR_TO_UTF8(*InScreenName)));
	}
}

void UPsMRGSProxyAndroid::SendGAEvent(const FString& InCategory, const FString& InAction, const FString& InLabel)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID SendGAEvent = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_sendGAEvent", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, SendGAEvent, Env->NewStringUTF(TCHAR_TO_UTF8(*InCategory)), Env->NewStringUTF(TCHAR_TO_UTF8(*InAction)), Env->NewStringUTF(TCHAR_TO_UTF8(*InLabel)));
	}
}

void UPsMRGSProxyAndroid::SendFlurryEvent(const FString& InAction)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID SendflurryEvent = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_flurryEvent", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, SendflurryEvent, Env->NewStringUTF(TCHAR_TO_UTF8(*InAction)));
	}
}

void UPsMRGSProxyAndroid::SendAFEvent(const FString& InEventName, const FString& InValue)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID SendAFEvent = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_sendAFEvent", "(Ljava/lang/String;Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, SendAFEvent, Env->NewStringUTF(TCHAR_TO_UTF8(*InEventName)), Env->NewStringUTF(TCHAR_TO_UTF8(*InValue)));
	}
}

void UPsMRGSProxyAndroid::AddMetricWithId(int32 MetricId)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}

	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID AddMetric = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_addMetricWithId", "(I)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AddMetric, MetricId);
	}
}

void UPsMRGSProxyAndroid::AddMetricWithCode(const FString& MetricCode, int32 Value, int32 Level, int32 ObjectId)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID AddMetric = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_addMetricWithCode", "(Ljava/lang/String;III)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AddMetric, Env->NewStringUTF(TCHAR_TO_UTF8(*MetricCode)), Value, Level, ObjectId);
	}
}

void UPsMRGSProxyAndroid::ShowMyTargetShowcase()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	FString AdType;
	AdType.AppendInt(static_cast<int32>(MRGSAdmanEntityType::MRGSAdmanShowcase));
	ProcessAdmanCase(AdType);
}

void UPsMRGSProxyAndroid::ShowMyTargetFullscreen()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	FString AdType;
	AdType.AppendInt(static_cast<int32>(MRGSAdmanEntityType::MRGSAdmanFullscreenBanner));
	ProcessAdmanCase(AdType);
}

void UPsMRGSProxyAndroid::ShowMyTargetInterstitialSlider()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	FString AdType;
	AdType.AppendInt(static_cast<int32>(MRGSAdmanEntityType::MRGSAdmanInterstitialSlider));
	ProcessAdmanCase(AdType);
}

void UPsMRGSProxyAndroid::ProcessAdmanCase(FString AdType)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID InterstitialLoadFullscreenData = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanLoadData", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, InterstitialLoadFullscreenData, Env->NewStringUTF(TCHAR_TO_UTF8(*AdType)));
	}
}

void UPsMRGSProxyAndroid::ShowSupport()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	const UPsMRGSSettings* MRGSSettings = GetDefault<UPsMRGSSettings>();
	if (MRGSSettings == nullptr || MRGSSettings->IsValidLowLevel() == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSSettings not initialized or removed"), *PS_FUNC_LINE);
		return;
	}
	
	FString SupportSecretKey = MRGSSettings->AndroidMrgsSupportSecretKey;
	if (SupportSecretKey.IsEmpty())
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: AndroidMrgsSupportSecretKey is empty"), *PS_FUNC_LINE);
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID ShowSupport = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_showSupport", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, ShowSupport, Env->NewStringUTF(TCHAR_TO_UTF8(*MRGSSettings->AndroidMrgsSupportSecretKey)));
	}
}

const bool UPsMRGSProxyAndroid::IsReady() const
{
	return bInitComplete;
}

const bool UPsMRGSProxyAndroid::UserLoggedIn() const
{
	return bUserLoggedin;
}

void UPsMRGSProxyAndroid::OnInitComplete()
{
	bInitComplete = true;
	
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_INIT_COMPLETE);
		}
	});
}

void UPsMRGSProxyAndroid::OnFullscreenClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_FULLSCREEN_CLOSED);
		}
	});
}

void UPsMRGSProxyAndroid::OnInterstitialSliderClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_INTERSTITIAL_SLIDER_CLOSED);
		}
	});
}

void UPsMRGSProxyAndroid::OnInterstitialDataRecieveError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_INTERSTITIAL_DATA_ERROR);
		}
	});
}

void UPsMRGSProxyAndroid::OnFullscreenDataRecieveError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_FULLSCREEN_DATA_ERROR);
		}
	});
}

void UPsMRGSProxyAndroid::OnShowcaseDataRecieveError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SHOWCASE_DATA_ERROR);
		}
	});
}

void UPsMRGSProxyAndroid::OnShowCaseDataHasNoAds()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SHOWCASE_DATA_EMPTY);
		}
	});
}

void UPsMRGSProxyAndroid::OnSupportClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_CLOSED);
		}
	});
}

void UPsMRGSProxyAndroid::OnSupportReceivedError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_ERROR);
		}
	});
}

void UPsMRGSProxyAndroid::OnSupportTicketsFailWithError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_TICKETS_ERROR);
		}
	});
}

void UPsMRGSProxyAndroid::OnUserAuthSuccess()
{
	bUserLoggedin = true;
	
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_USERINIT_COMPLETE);
		}
	});
}

void UPsMRGSProxyAndroid::OnUserAuthError()
{
	bUserLoggedin = false;

	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_USERINIT_ERROR);
		}
	});
}

void UPsMRGSProxyAndroid::OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_COMPLETE);
		}
	});
}

void UPsMRGSProxyAndroid::OnPurchaseFailed(const FString& ProductId, const FString& Answer)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_FAILED);
		}
	});
}

void UPsMRGSProxyAndroid::OnPurchaseCanceled(const FString& ProductId, const FString& Answer)
{
	AsyncTask(ENamedThreads::GameThread, [this]() {
		if(MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_CANCELED);
		}
	});
}

FString UPsMRGSProxyAndroid::GetDevicePlatform() const
{
	return FString(TEXT(""));
}

FString UPsMRGSProxyAndroid::GetOpenUDID() const
{
	return FString(TEXT(""));
}

extern "C"
{
	void fillPurchaseItem(JNIEnv* env, jobject jitem, FPsMRGSPurchaseInfo& OutItem)
	{
		if (!env)
		{
			return;
		}
		
		jclass itemClass = env->GetObjectClass(jitem);
		
		jfieldID fid = env->GetFieldID(itemClass, "sku", "Ljava/lang/String;");
		jstring jsku = (jstring)env->GetObjectField(jitem, fid);
		OutItem.Sku = MRGSJniHelper::JavaStringToFstring(jsku);
		
		fid = env->GetFieldID(itemClass, "price", "Ljava/lang/String;");
		jstring jprice = (jstring)env->GetObjectField(jitem, fid);
		OutItem.Price = MRGSJniHelper::JavaStringToFstring(jprice);
		
		fid = env->GetFieldID(itemClass, "title", "Ljava/lang/String;");
		jstring jtitle = (jstring)env->GetObjectField(jitem, fid);
		OutItem.Title = MRGSJniHelper::JavaStringToFstring(jtitle);
		
		fid = env->GetFieldID(itemClass, "type", "Ljava/lang/String;");
		jstring jtype = (jstring)env->GetObjectField(jitem, fid);
		OutItem.Type = MRGSJniHelper::JavaStringToFstring(jtype);
		
		fid = env->GetFieldID(itemClass, "description", "Ljava/lang/String;");
		jstring jdescription = (jstring)env->GetObjectField(jitem, fid);
		OutItem.Description = MRGSJniHelper::JavaStringToFstring(jdescription);
		
		env->DeleteLocalRef(jsku);
		env->DeleteLocalRef(jprice);
		env->DeleteLocalRef(jtitle);
		env->DeleteLocalRef(jtype);
		env->DeleteLocalRef(jdescription);
		env->DeleteLocalRef(itemClass);
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onInitComplete(JNIEnv* env, jobject obj)
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnInitComplete();
			}
		});
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onLoadServerDataDidFinished(JNIEnv* env, jobject obj, jstring json)
	{
		UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onLoadPromoBannersDidFinished(JNIEnv* env, jobject obj, jstring json)
	{
		UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onMyComSupportHasError(JNIEnv* env, jobject obj, jstring error)
	{
		FString Error = MRGSJniHelper::JavaStringToFstring(error);
		AsyncTask(ENamedThreads::GameThread, [Error]() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnSupportReceivedError(Error);
			}
		});
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onMyComHasSupportDidClose(JNIEnv* env, jobject obj)
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnSupportClosed();
			}
		});
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onLoadProductsDidFinished(JNIEnv* env, jobject obj, jobject jItems)
	{
		if (!env)
		{
			return;
		}
		
		jclass ListClass = env->GetObjectClass(jItems);
		jmethodID mid = env->GetMethodID(ListClass, "size", "()I");
		int Size = (int)env->CallIntMethod(jItems, mid);
		
		TArray<FPsMRGSPurchaseInfo> Items;
		mid = env->GetMethodID(ListClass, "get", "(I)Ljava/lang/Object;");
		
		for (int i = 0; i < Size; i++)
		{
			jobject jitem = env->CallObjectMethod(jItems, mid, i);
			FPsMRGSPurchaseInfo Item;
			fillPurchaseItem(env, jitem, Item);
			Items.Add(Item);
			env->DeleteLocalRef(jitem);
		}
		
		env->DeleteLocalRef(ListClass);
		
		AsyncTask(ENamedThreads::GameThread, [Items]() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnStoreProductsLoaded(Items);
			}
		});
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onPurchaseComplete(JNIEnv* env, jobject obj, jstring sku, jstring transactionId, jstring answer)
	{
		FString Sku = MRGSJniHelper::JavaStringToFstring(sku);
		FString Transaction = MRGSJniHelper::JavaStringToFstring(transactionId);
		FString Answer = MRGSJniHelper::JavaStringToFstring(answer);
		
		AsyncTask(ENamedThreads::GameThread, [Sku, Transaction, Answer]() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnPurchaseComplete(Sku, Transaction, Answer);
			}
		});
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onPurchaseFail(JNIEnv* env, jobject obj, jstring sku, jstring answer)
	{
		FString Sku = MRGSJniHelper::JavaStringToFstring(sku);
		FString Answer = MRGSJniHelper::JavaStringToFstring(answer);
		
		AsyncTask(ENamedThreads::GameThread, [Sku, Answer]() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnPurchaseFailed(Sku, Answer);
			}
		});
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onPurchaseCancel(JNIEnv* env, jobject obj, jstring sku, jstring answer)
	{
		FString Sku = MRGSJniHelper::JavaStringToFstring(sku);
		FString Answer = MRGSJniHelper::JavaStringToFstring(answer);
		
		AsyncTask(ENamedThreads::GameThread, [Sku, Answer]() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnPurchaseCanceled(Sku, Answer);
			}
		});
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onAdmanLoadComplete(JNIEnv* env, jobject obj, jint jtype, jboolean jnotification)
	{
		JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
		if (Env)
		{
			static jmethodID AdmanOpen = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanOpen", "(I)V", false);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AdmanOpen, jtype);
		}
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onAdmanHasNoAdd(JNIEnv* env, jobject obj)
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnShowCaseDataHasNoAds();
			}
		});
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onAdmanViewComplete(JNIEnv* env, jobject obj, jint jtype)
	{
		if (env)
		{
			MRGSAdmanEntityType AdmanActionType = static_cast<MRGSAdmanEntityType>((int)jtype);
			static jmethodID AdmanClose = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanClose", "(I)V", false);
			FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, AdmanClose, jtype);
			
			static jmethodID AdmanRelease = FJavaWrapper::FindMethod(env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanRelease", "(I)V", false);
			FJavaWrapper::CallVoidMethod(env, FJavaWrapper::GameActivityThis, AdmanRelease, jtype);
			
			AsyncTask(ENamedThreads::GameThread, [AdmanActionType]() {
				auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
				switch (AdmanActionType)
				{
					case MRGSAdmanEntityType::MRGSAdmanFullscreenBanner:
						if (Proxy)
						{
							Proxy->OnFullscreenClosed();
						}
						break;
						
					case MRGSAdmanEntityType::MRGSAdmanInterstitialSlider:
						if (Proxy)
						{
							Proxy->OnInterstitialSliderClosed();
						}
						break;
						
					default:
						UE_LOG(LogMRGS, Error, TEXT("%s: Unrecognized error"), *PS_FUNC_LINE);
						break;
				}
			});
		}
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onUserAuthSuccess(JNIEnv* env, jobject obj)
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnUserAuthSuccess();
			}
		});
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onUserAuthError(JNIEnv* env, jobject obj)
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnUserAuthError();
			}
		});
	}
}

#endif
