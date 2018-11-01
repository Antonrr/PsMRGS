// Copyright 2018 Pushkin Studio. All Rights Reserved.

#include "PsMRGSProxyAndroid.h"
#include "PsMRGSSettings.h"
#include "PsMRGSCommon.h"

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
	
	const jclass stringClass = env->GetObjectClass(jstr);
	const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
	const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jstr, getBytes, env->NewStringUTF("UTF-8"));
	
	size_t length = (size_t) env->GetArrayLength(stringJbytes);
	jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);
	
	std::string ret = std::string((char *)pBytes, length);
	env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);
	
	env->DeleteLocalRef(stringJbytes);
	env->DeleteLocalRef(stringClass);
	return FString(ret.c_str());
}

void UPsMRGSProxyAndroid::InitModule()
{
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
	int32 Index = 0;
	for (auto ListItem : ProductsList)
	{
		ResultList += ListItem;
		if(Index < ProductsList.Num() - 1)
		{
			ResultList += ",";
		}
		Index++;
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID LoadProducts = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_getProductsInfo", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, LoadProducts, Env->NewStringUTF(TCHAR_TO_UTF8(*ResultList)));
	}
}

void UPsMRGSProxyAndroid::OnStoreProductsLoaded(TArray<FPsMRGSPurchaseInfo>& InLoadedProducts)
{
	LoadedProducts = InLoadedProducts;
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
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

void UPsMRGSProxyAndroid::AddMetric(int32 MetricId)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}
	
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		FString MetricString;
		MetricString.AppendInt(MetricId);
		static jmethodID AddMetric = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_addMetric", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AddMetric, Env->NewStringUTF(TCHAR_TO_UTF8(*MetricString)));
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
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID AdmanLoadShowcaseData = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanLoadData", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AdmanLoadShowcaseData, Env->NewStringUTF(TCHAR_TO_UTF8(*AdType)));
	}
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
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID AdmanLoadFullscreenData = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanLoadData", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AdmanLoadFullscreenData, Env->NewStringUTF(TCHAR_TO_UTF8(*AdType)));
	}
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
	
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if (MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_INIT_COMPLETE);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnFullscreenClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if (MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_FULLSCREEN_CLOSED);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnInterstitialSliderClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if (MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_INTERSTITIAL_SLIDER_CLOSED);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnInterstitialDataRecieveError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if (MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_INTERSTITIAL_DATA_ERROR);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnFullscreenDataRecieveError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if (MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_FULLSCREEN_DATA_ERROR);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnShowcaseDataRecieveError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if (MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SHOWCASE_DATA_ERROR);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnShowCaseDataHasNoAds()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if (MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SHOWCASE_DATA_EMPTY);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnSupportClosed()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if(MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_CLOSED);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnSupportReceivedError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if (MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_ERROR);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnSupportTicketsFailWithError(const FString& Error)
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if (MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_SUPPORT_TICKETS_ERROR);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnUserAuthSuccess()
{
	bUserLoggedin = true;
	
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if(MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_USERINIT_COMPLETE);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnUserAuthError()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if(MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_USERINIT_ERROR);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload)
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if(MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_COMPLETE);
				  }
			  });
}

void UPsMRGSProxyAndroid::OnPurchaseFailed(const FString& ProductId, const FString& Answer)
{
	AsyncTask(ENamedThreads::GameThread, [this]()
			  {
				  if(MRGSDelegate.IsBound())
				  {
					  MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_FAILED);
				  }
			  });
}

extern "C"
{
	void fillPurchaseItem(JNIEnv* env, jobject jitem, FPsMRGSPurchaseInfo& item)
	{
		jclass itemClass = env->GetObjectClass(jitem);
		
		jfieldID fid = env->GetFieldID(itemClass, "sku", "Ljava/lang/String;");
		jstring jsku = (jstring)env->GetObjectField(jitem, fid);
		item.Sku = MRGSJniHelper::JavaStringToFstring(jsku);
		
		fid = env->GetFieldID(itemClass, "price", "Ljava/lang/String;");
		jstring jprice = (jstring)env->GetObjectField(jitem, fid);
		item.Price = MRGSJniHelper::JavaStringToFstring(jprice);
		
		fid = env->GetFieldID(itemClass, "title", "Ljava/lang/String;");
		jstring jtitle = (jstring)env->GetObjectField(jitem, fid);
		item.Title = MRGSJniHelper::JavaStringToFstring(jtitle);
		
		fid = env->GetFieldID(itemClass, "type", "Ljava/lang/String;");
		jstring jtype = (jstring)env->GetObjectField(jitem, fid);
		item.Type = MRGSJniHelper::JavaStringToFstring(jtype);
		
		fid = env->GetFieldID(itemClass, "description", "Ljava/lang/String;");
		jstring jdescription = (jstring)env->GetObjectField(jitem, fid);
		item.Description = MRGSJniHelper::JavaStringToFstring(jdescription);
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onInitComplete(JNIEnv* env, jobject obj)
	{
		UE_LOG(LogMRGS, Log, TEXT("%s"), *PS_FUNC_LINE);
		auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
		if (Proxy)
		{
			Proxy->OnInitComplete();
		}
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
		auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
		if (Proxy)
		{
			Proxy->OnSupportReceivedError(MRGSJniHelper::JavaStringToFstring(error));
		}
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onMyComHasSupportDidClose(JNIEnv* env, jobject obj)
	{
		auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
		if (Proxy)
		{
			Proxy->OnSupportClosed();
		}
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onLoadProductsDidFinished(JNIEnv* env, jobject obj, jobject jItems)
	{
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
		
		auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
		if (Proxy)
		{
			Proxy->OnStoreProductsLoaded(Items);
		}
	}
	
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onPurchaseComplete(JNIEnv* env, jobject obj, jstring sku, jstring transactionId, jstring answer)
	{
		auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
		if (Proxy)
		{
			Proxy->OnPurchaseComplete(MRGSJniHelper::JavaStringToFstring(sku), MRGSJniHelper::JavaStringToFstring(transactionId), MRGSJniHelper::JavaStringToFstring(answer));
		}
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onPurchaseFail(JNIEnv* env, jobject obj, jstring sku, jstring answer)
	{
		auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
		if (Proxy)
		{
			Proxy->OnPurchaseFailed(MRGSJniHelper::JavaStringToFstring(sku), MRGSJniHelper::JavaStringToFstring(answer));
		}
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
		auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
		if (Proxy)
		{
			Proxy->OnShowCaseDataHasNoAds();
		}
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onAdmanViewComplete(JNIEnv* env, jobject obj, jint jtype)
	{
		JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
		if (Env)
		{
			static jmethodID AdmanClose = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanClose", "(I)V", false);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AdmanClose, jtype);
			
			MRGSAdmanEntityType AdmanActionType = static_cast<MRGSAdmanEntityType>((int)jtype);
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			
			static jmethodID AdmanRelease = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanRelease", "(I)V", false);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AdmanRelease, jtype);
			
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
					break;
			}
		}
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onUserAuthSuccess(JNIEnv* env, jobject obj)
	{
		auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
		if (Proxy)
		{
			Proxy->OnUserAuthSuccess();
		}
	}
	
	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onUserAuthError(JNIEnv* env, jobject obj)
	{
		auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
		if (Proxy)
		{
			Proxy->OnUserAuthError();
		}
	}
}

#endif


