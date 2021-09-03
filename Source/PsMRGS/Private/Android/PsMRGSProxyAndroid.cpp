// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "PsMRGSProxyAndroid.h"

#include "PsMRGSCommon.h"
#include "PsMRGSSettings.h"

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

	JNIEnv* env = 0;
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

//////////////////////////////////////////////////////////////////////////
// GDPR

void UPsMRGSProxyAndroid::ShowDefaultGDPRAgreement(bool bOnlyEU, bool bWithAdvertising, FString LocalizationLanguage)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		const UPsMRGSSettings* MRGSSettings = GetDefault<UPsMRGSSettings>();
		if (MRGSSettings == nullptr)
		{
			UE_LOG(LogMRGS, Error, TEXT("%s: invalid MRGSSettings"), *PS_FUNC_LINE);
			return;
		}

		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_ShowDefaultGDPRAgreement", "(Ljava/lang/String;ZZLjava/lang/String;)V", false);

		jstring AppId = Env->NewStringUTF(TCHAR_TO_UTF8(*MRGSSettings->AndroidMrgsAppId));
		jstring LocalizationLanguageJstring = Env->NewStringUTF(TCHAR_TO_UTF8(*LocalizationLanguage));
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, methodId, AppId, bOnlyEU, bWithAdvertising, LocalizationLanguageJstring);
		Env->DeleteLocalRef(AppId);
		Env->DeleteLocalRef(LocalizationLanguageJstring);
	}
	else
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: invalid JNIEnv"), *PS_FUNC_LINE);
	}
}

void UPsMRGSProxyAndroid::ShowGDPRAgreement(bool bOnlyEU, bool bWithAdvertising, FString LocalizationLanguage)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		const UPsMRGSSettings* MRGSSettings = GetDefault<UPsMRGSSettings>();
		if (MRGSSettings == nullptr)
		{
			UE_LOG(LogMRGS, Error, TEXT("%s: invalid MRGSSettings"), *PS_FUNC_LINE);
			return;
		}

		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_ShowGDPRAgreement", "(Ljava/lang/String;ZZLjava/lang/String;)V", false);

		jstring AppId = Env->NewStringUTF(TCHAR_TO_UTF8(*MRGSSettings->AndroidMrgsAppId));
		jstring LocalizationLanguageJstring = Env->NewStringUTF(TCHAR_TO_UTF8(*LocalizationLanguage));
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, methodId, AppId, bOnlyEU, bWithAdvertising, LocalizationLanguageJstring);
		Env->DeleteLocalRef(AppId);
		Env->DeleteLocalRef(LocalizationLanguageJstring);
	}
	else
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: invalid JNIEnv"), *PS_FUNC_LINE);
	}
}

int32 UPsMRGSProxyAndroid::GetGDPRAcceptedVersion()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_GetGDPRAcceptedVersion", "()I", false);

		return FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, methodId);
	}
	else
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: invalid JNIEnv"), *PS_FUNC_LINE);
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// CCPA

bool UPsMRGSProxyAndroid::ShouldShowCCPA()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_ShouldShowCCPA", "()Z", false);

		return FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, methodId);
	}
	else
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: invalid JNIEnv"), *PS_FUNC_LINE);
		return false;
	}
}

EPsMRGSCCPASetting UPsMRGSProxyAndroid::GetCCPASettingValue()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_GetCCPASettingValue", "()I", false);

		/**
		 public abstract class MRGSCCPA {
		   public static final int USER_PREFERENCE_SHARE = 0;
		   public static final int USER_PREFERENCE_NOT_SHARING = 1;
		 }
		 */

		const int32 Result = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, methodId);
		if (Result == 0)
		{
			return EPsMRGSCCPASetting::Share;
		}
		else if (Result == 1)
		{
			return EPsMRGSCCPASetting::DontShare;
		}
		else
		{
			UE_LOG(LogMRGS, Error, TEXT("%s: unhandled MRGSCCPAUserPreference value"), *PS_FUNC_LINE);
			return EPsMRGSCCPASetting::Share;
		}
	}
	else
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: invalid JNIEnv"), *PS_FUNC_LINE);
		return EPsMRGSCCPASetting::Share;
	}
}

void UPsMRGSProxyAndroid::SetCCPASettingValue(EPsMRGSCCPASetting Value)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_SetCCPASettingValue", "(I)V", false);

		/**
		 public abstract class MRGSCCPA {
		   public static final int USER_PREFERENCE_SHARE = 0;
		   public static final int USER_PREFERENCE_NOT_SHARING = 1;
		 }
		 */

		if (Value == EPsMRGSCCPASetting::Share)
		{
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, methodId, 0);
		}
		else if (Value == EPsMRGSCCPASetting::DontShare)
		{
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, methodId, 1);
		}
		else
		{
			UE_LOG(LogMRGS, Error, TEXT("%s: unhandled EPsMRGSCCPASetting value"), *PS_FUNC_LINE);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, methodId, 1);
		}
	}
	else
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: invalid JNIEnv"), *PS_FUNC_LINE);
	}
}

//////////////////////////////////////////////////////////////////////////
// MRGS

void UPsMRGSProxyAndroid::CheckIntegration()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID CheckjMethod = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_checkIntegration", "()V", false);

		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, CheckjMethod);
	}
}

void UPsMRGSProxyAndroid::InitModule()
{
	if (IsReady())
	{
		AsyncTask(ENamedThreads::GameThread, [this]() {
			if (MRGSDelegate.IsBound())
			{
				MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_INIT_COMPLETE);
			}
		});
		return;
	}

	const UPsMRGSSettings* MRGSSettings = GetDefault<UPsMRGSSettings>();
	if (MRGSSettings == nullptr)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSSettings not initialized or removed"), *PS_FUNC_LINE);
		return;
	}

#if !UE_BUILD_SHIPPING
	bool bDebug = MRGSSettings->bDebugInDevelopment;
#else
	bool bDebug = false;
#endif

	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		jstring AppId = Env->NewStringUTF(TCHAR_TO_UTF8(*MRGSSettings->AndroidMrgsAppId));
		jstring Secret = Env->NewStringUTF(TCHAR_TO_UTF8(*MRGSSettings->AndroidMrgsSecret));
		jstring SupportSecret = Env->NewStringUTF(TCHAR_TO_UTF8(*MRGSSettings->AndroidMrgsSupportSecretKey));
		jstring MyTrackerAppId = Env->NewStringUTF(TCHAR_TO_UTF8(*MRGSSettings->AndroidMyTrackerAppId));
		jstring AppsFlyerKey = Env->NewStringUTF(TCHAR_TO_UTF8(*MRGSSettings->AndroidAppsFlyerDevKey));
		static jmethodID InitjMethod = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_initWithAppIdAndSecret", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)V", false);

		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, InitjMethod, AppId, Secret, SupportSecret, MyTrackerAppId, AppsFlyerKey, bDebug);
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

void UPsMRGSProxyAndroid::SetAutoRestoreTransactions(bool bRestore)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}

	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID jMethod = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_autoRestoreTransactions", "(Z)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, jMethod, bRestore);
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
		if (i < ProductsList.Num() - 1)
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
		static jmethodID BuyItem = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_buyItem", "(Ljava/lang/String;Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, BuyItem, Env->NewStringUTF(TCHAR_TO_UTF8(*ProductId)), Env->NewStringUTF(TCHAR_TO_UTF8(*Payload)));
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

/* Helper function */
template <typename T>
void SetEventField(JNIEnv* Env, const T& FieldValue, const char* SetFieldMethodName, const char* SetFieldMethodSignature, jclass EventJavaClass, jobject EventJavaObject, jclass FieldJavaClass, jmethodID FieldCtor)
{
	jobject fieldValueObject = Env->NewObject(FieldJavaClass, FieldCtor, FieldValue);
	jmethodID setFieldMethodId = FJavaWrapper::FindMethod(Env, EventJavaClass, SetFieldMethodName, SetFieldMethodSignature, false);
	FJavaWrapper::CallVoidMethod(Env, EventJavaObject, setFieldMethodId, fieldValueObject);
	Env->DeleteLocalRef(fieldValueObject);
};

/* Helper function */
void SetEventStringField(JNIEnv* Env, const FString& FieldValue, const char* SetFieldMethodName, jclass EventJavaClass, jobject EventJavaObject)
{
	jmethodID setFieldMethodId = FJavaWrapper::FindMethod(Env, EventJavaClass, SetFieldMethodName, "(Ljava/lang/String;)V", false);
	jstring fieldJstringValue = Env->NewStringUTF(TCHAR_TO_UTF8(*FieldValue));
	FJavaWrapper::CallVoidMethod(Env, EventJavaObject, setFieldMethodId, fieldJstringValue);
	Env->DeleteLocalRef(fieldJstringValue);
};

void UPsMRGSProxyAndroid::AddTrackerEvent(const FPsMRGSTrackerEvent& Event)
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}

	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_createTrackerEvent", "(Ljava/lang/String;)Lru/mail/mrgservice/tracker/MRGSTrackerEvent;", false);
		jstring eventNameJstring = Env->NewStringUTF(TCHAR_TO_UTF8(*Event.EventName));
		jobject CustomEvent = FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, methodId, eventNameJstring);
		Env->DeleteLocalRef(eventNameJstring);

		jclass EventClassID = FAndroidApplication::FindJavaClassGlobalRef("ru/mail/mrgservice/tracker/MRGSTrackerEvent");

		jclass IntegerClassID = FJavaWrapper::FindClassGlobalRef(Env, "java/lang/Integer", false);
		jmethodID integerInitMethodId = FJavaWrapper::FindMethod(Env, IntegerClassID, "<init>", "(I)V", false);
		const char* SetIntMethodSignature = "(Ljava/lang/Integer;)V";
		SetEventField(Env, Event.Level, "setLevel", SetIntMethodSignature, EventClassID, CustomEvent, IntegerClassID, integerInitMethodId);
		SetEventField(Env, Event.customInt1, "setCustomInt1", SetIntMethodSignature, EventClassID, CustomEvent, IntegerClassID, integerInitMethodId);
		SetEventField(Env, Event.customInt2, "setCustomInt2", SetIntMethodSignature, EventClassID, CustomEvent, IntegerClassID, integerInitMethodId);
		SetEventField(Env, Event.customInt3, "setCustomInt3", SetIntMethodSignature, EventClassID, CustomEvent, IntegerClassID, integerInitMethodId);
		SetEventField(Env, Event.customInt4, "setCustomInt4", SetIntMethodSignature, EventClassID, CustomEvent, IntegerClassID, integerInitMethodId);
		SetEventField(Env, Event.customInt5, "setCustomInt5", SetIntMethodSignature, EventClassID, CustomEvent, IntegerClassID, integerInitMethodId);
		SetEventField(Env, Event.customInt6, "setCustomInt6", SetIntMethodSignature, EventClassID, CustomEvent, IntegerClassID, integerInitMethodId);
		SetEventField(Env, Event.customInt7, "setCustomInt7", SetIntMethodSignature, EventClassID, CustomEvent, IntegerClassID, integerInitMethodId);
		SetEventField(Env, Event.customInt8, "setCustomInt8", SetIntMethodSignature, EventClassID, CustomEvent, IntegerClassID, integerInitMethodId);
		SetEventField(Env, Event.customInt9, "setCustomInt9", SetIntMethodSignature, EventClassID, CustomEvent, IntegerClassID, integerInitMethodId);
		SetEventField(Env, Event.customInt10, "setCustomInt10", SetIntMethodSignature, EventClassID, CustomEvent, IntegerClassID, integerInitMethodId);

		jclass FloatClassID = FJavaWrapper::FindClassGlobalRef(Env, "java/lang/Float", false);
		jmethodID floatInitMethodId = FJavaWrapper::FindMethod(Env, FloatClassID, "<init>", "(F)V", false);
		const char* SetFloatMethodSignature = "(Ljava/lang/Float;)V";
		SetEventField(Env, Event.customFloat1, "setCustomFloat1", SetFloatMethodSignature, EventClassID, CustomEvent, FloatClassID, floatInitMethodId);
		SetEventField(Env, Event.customFloat2, "setCustomFloat2", SetFloatMethodSignature, EventClassID, CustomEvent, FloatClassID, floatInitMethodId);
		SetEventField(Env, Event.customFloat3, "setCustomFloat3", SetFloatMethodSignature, EventClassID, CustomEvent, FloatClassID, floatInitMethodId);
		SetEventField(Env, Event.customFloat4, "setCustomFloat4", SetFloatMethodSignature, EventClassID, CustomEvent, FloatClassID, floatInitMethodId);
		SetEventField(Env, Event.customFloat5, "setCustomFloat5", SetFloatMethodSignature, EventClassID, CustomEvent, FloatClassID, floatInitMethodId);
		SetEventField(Env, Event.customFloat6, "setCustomFloat6", SetFloatMethodSignature, EventClassID, CustomEvent, FloatClassID, floatInitMethodId);
		SetEventField(Env, Event.customFloat7, "setCustomFloat7", SetFloatMethodSignature, EventClassID, CustomEvent, FloatClassID, floatInitMethodId);
		SetEventField(Env, Event.customFloat8, "setCustomFloat8", SetFloatMethodSignature, EventClassID, CustomEvent, FloatClassID, floatInitMethodId);
		SetEventField(Env, Event.customFloat9, "setCustomFloat9", SetFloatMethodSignature, EventClassID, CustomEvent, FloatClassID, floatInitMethodId);
		SetEventField(Env, Event.customFloat10, "setCustomFloat10", SetFloatMethodSignature, EventClassID, CustomEvent, FloatClassID, floatInitMethodId);

		SetEventStringField(Env, Event.customString1, "setCustomString1", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString2, "setCustomString2", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString3, "setCustomString3", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString4, "setCustomString4", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString5, "setCustomString5", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString6, "setCustomString6", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString7, "setCustomString7", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString8, "setCustomString8", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString9, "setCustomString9", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString10, "setCustomString10", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString11, "setCustomString11", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString12, "setCustomString12", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString13, "setCustomString13", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString14, "setCustomString14", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString15, "setCustomString15", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString16, "setCustomString16", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString17, "setCustomString17", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString18, "setCustomString18", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString19, "setCustomString19", EventClassID, CustomEvent);
		SetEventStringField(Env, Event.customString20, "setCustomString20", EventClassID, CustomEvent);

		static jmethodID sendMethodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_sendTrackerEvent", "(Lru/mail/mrgservice/tracker/MRGSTrackerEvent;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, sendMethodId, CustomEvent);

		Env->DeleteLocalRef(CustomEvent);
		Env->DeleteGlobalRef(EventClassID);
		Env->DeleteGlobalRef(IntegerClassID);
		Env->DeleteGlobalRef(FloatClassID);
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
	if (MRGSSettings == nullptr)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSSettings not initialized or removed"), *PS_FUNC_LINE);
		return;
	}

	FString SupportSecretKey = MRGSSettings->AndroidMrgsSupportSecretKey;
	if (SupportSecretKey.IsEmpty())
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: AndroidMrgsSupportSecretKey is empty"), *PS_FUNC_LINE);
		return;
	}

	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID ShowSupport = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_showSupport", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, ShowSupport, Env->NewStringUTF(TCHAR_TO_UTF8(*MRGSSettings->AndroidMrgsSupportSecretKey)));
		OnSupportOpened();
	}
}

void UPsMRGSProxyAndroid::CheckSupportTickets()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}

	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID CheckTickets = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_checkTickets", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, CheckTickets);
	}
}

bool UPsMRGSProxyAndroid::IsReady() const
{
	return bInitComplete;
}

bool UPsMRGSProxyAndroid::UserLoggedIn() const
{
	return bUserLoggedin;
}

void UPsMRGSProxyAndroid::RequestOpenUDID()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID methodID = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_requestOpenUDID", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, methodID);
	}
}

void UPsMRGSProxyAndroid::LoadAdvertising()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}

	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_loadAdvertisingContent", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, methodId);
	}
}

bool UPsMRGSProxyAndroid::IsAdvertisingLoaded()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return false;
	}

	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_isAdvertisingLoaded", "()Z", false);
		return FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, methodId);
	}
	else
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: invalid JNIEnv"), *PS_FUNC_LINE);
		return false;
	}
}

void UPsMRGSProxyAndroid::ShowAdvertising()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}

	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_showAdvertising", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, methodId);

		AsyncTask(ENamedThreads::GameThread, [this]() {
			if (MRGSDelegate.IsBound())
			{
				MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_ADVERTISING_SHOW);
			}
		});
	}
}

void UPsMRGSProxyAndroid::OpenShowcase()
{
	if (bInitComplete == false)
	{
		UE_LOG(LogMRGS, Error, TEXT("%s: UPsMRGSProxyAndroid not initialized"), *PS_FUNC_LINE);
		return;
	}

	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_openShowcase", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, methodId);
	}
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

void UPsMRGSProxyAndroid::OnClickOnNotification(int32 NotificationId, const FString& Title, const FString& Message, const FString& DeveloperPayload, bool bIsLocal)
{
	AsyncTask(ENamedThreads::GameThread, [this, DeveloperPayload]() {
		NotificationDeveloperPayload = DeveloperPayload;
		if (MRGSDelegate.IsBound())
		{
			MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_NOTIFICATION_CLICKED);
		}
	});
}

FString UPsMRGSProxyAndroid::GetNotificationDeveloperPayload() const
{
	return NotificationDeveloperPayload;
}

void UPsMRGSProxyAndroid::OnPurchaseComplete(const FString& PaymentId, const FString& TransactionId, const FString& Payload)
{
	AsyncTask(ENamedThreads::GameThread, [this, PaymentId, TransactionId, Payload]() {
		MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_COMPLETE);
		MRGSIAPDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_COMPLETE, PaymentId, TransactionId, Payload);
	});
}

void UPsMRGSProxyAndroid::OnPurchasePending(const FString& ProductId)
{
	AsyncTask(ENamedThreads::GameThread, [this, ProductId]() {
		MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_PENDING);
		MRGSIAPDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_PENDING, ProductId, "", "");
	});
}

void UPsMRGSProxyAndroid::OnPurchaseFailed(const FString& ProductId, const FString& Answer)
{
	AsyncTask(ENamedThreads::GameThread, [this, ProductId, Answer]() {
		MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_FAILED);
		MRGSIAPDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_FAILED, ProductId, "", Answer);
	});
}

void UPsMRGSProxyAndroid::OnPurchaseCanceled(const FString& ProductId, const FString& Answer)
{
	AsyncTask(ENamedThreads::GameThread, [this, ProductId, Answer]() {
		MRGSDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_CANCELED);
		MRGSIAPDelegate.Broadcast(EPsMRGSEventsTypes::MRGS_PURCHASE_CANCELED, ProductId, "", Answer);
	});
}

FString UPsMRGSProxyAndroid::GetDevicePlatform() const
{
	return TEXT("Android");
}

FString UPsMRGSProxyAndroid::GetCountryCode() const
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv(true);
	if (Env)
	{
		static jmethodID methodId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_getCountry", "()Ljava/lang/String;", false);
		jstring jCountryCode = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, methodId);
		return MRGSJniHelper::JavaStringToFstring(jCountryCode);
	}
	return FString();
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
		OutItem.FormattedPrice = MRGSJniHelper::JavaStringToFstring(jprice);

		static jmethodID getPriceMicrosMethodId = FJavaWrapper::FindMethod(env, itemClass, "getPriceMicros", "()Ljava/lang/String;", false);
		jstring jpricemicros = (jstring)env->CallObjectMethod(jitem, getPriceMicrosMethodId);
		OutItem.PriceMicros = MRGSJniHelper::JavaStringToFstring(jpricemicros);

		fid = env->GetFieldID(itemClass, "title", "Ljava/lang/String;");
		jstring jtitle = (jstring)env->GetObjectField(jitem, fid);
		OutItem.Title = MRGSJniHelper::JavaStringToFstring(jtitle);

		fid = env->GetFieldID(itemClass, "type", "Ljava/lang/String;");
		jstring jtype = (jstring)env->GetObjectField(jitem, fid);
		OutItem.Type = MRGSJniHelper::JavaStringToFstring(jtype);

		fid = env->GetFieldID(itemClass, "description", "Ljava/lang/String;");
		jstring jdescription = (jstring)env->GetObjectField(jitem, fid);
		OutItem.Description = MRGSJniHelper::JavaStringToFstring(jdescription);

		static jmethodID getCurrencyMethodId = FJavaWrapper::FindMethod(env, itemClass, "getCurrency", "()Ljava/lang/String;", false);
		jstring jcurrency = (jstring)env->CallObjectMethod(jitem, getCurrencyMethodId);
		OutItem.Currency = MRGSJniHelper::JavaStringToFstring(jcurrency);

		env->DeleteLocalRef(jcurrency);
		env->DeleteLocalRef(jsku);
		env->DeleteLocalRef(jprice);
		env->DeleteLocalRef(jpricemicros);
		env->DeleteLocalRef(jtitle);
		env->DeleteLocalRef(jtype);
		env->DeleteLocalRef(jdescription);
		env->DeleteLocalRef(itemClass);
	}

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onUserHasAcceptedGDPR(JNIEnv* env, jboolean withAdvertising)
	{
		AsyncTask(ENamedThreads::GameThread, [withAdvertising]() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnGDPRAccepted(withAdvertising);
			}
		});
	}

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onErrorShowingAgreement(JNIEnv* env)
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnGDPRError();
			}
		});
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

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onPurchasePending(JNIEnv* env, jobject obj, jstring sku)
	{
		FString Sku = MRGSJniHelper::JavaStringToFstring(sku);

		AsyncTask(ENamedThreads::GameThread, [Sku]() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnPurchasePending(Sku);
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

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onUserAuthSuccess(JNIEnv* env, jobject obj)
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnUserAuthSuccess();
			}
			else
			{
				UE_LOG(LogMRGS, Error, TEXT("%s: invalid MRGSProxy"), *PS_FUNC_LINE);
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
			else
			{
				UE_LOG(LogMRGS, Error, TEXT("%s: invalid MRGSProxy"), *PS_FUNC_LINE);
			}
		});
	}

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onClickOnNotification(JNIEnv* env, jobject obj, int notificationId, jstring title, jstring message, jstring developerPayload, jboolean isLocal)
	{
		AsyncTask(ENamedThreads::GameThread, [notificationId, title, message, developerPayload, isLocal]() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnClickOnNotification(notificationId, MRGSJniHelper::JavaStringToFstring(title), MRGSJniHelper::JavaStringToFstring(message), MRGSJniHelper::JavaStringToFstring(developerPayload), isLocal);
			}
			else
			{
				UE_LOG(LogMRGS, Error, TEXT("%s: invalid MRGSProxy"), *PS_FUNC_LINE);
			}
		});
	}

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onSupportTicketResponse(JNIEnv* env, jobject obj, jboolean bHasTickets)
	{
		AsyncTask(ENamedThreads::GameThread, [bHasTickets]() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				if (bHasTickets)
				{
					Proxy->OnSupportTicketsReceived();
				}
			}
			else
			{
				UE_LOG(LogMRGS, Error, TEXT("%s: invalid MRGSProxy"), *PS_FUNC_LINE);
			}
		});
	}

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onAdvertisingLoadedCallback(JNIEnv* env, jobject obj)
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnAdvertisingLoaded();
			}
		});
	}

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onAdvertisingLoadingErrorCallback(JNIEnv* env, jobject obj)
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnAdvertisingLoadingError();
			}
		});
	}

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onAdvertisingFinishedCallback(JNIEnv* env, jobject obj, jboolean bSkipped)
	{
		AsyncTask(ENamedThreads::GameThread, [bSkipped]() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnAdvertisingFinished(bSkipped);
			}
			else
			{
				UE_LOG(LogMRGS, Error, TEXT("%s: invalid MRGSProxy"), *PS_FUNC_LINE);
			}
		});
	}

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onShowcaseNewContentCallback(JNIEnv* env, jobject obj)
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnNewShowcaseContent();
			}
		});
	}

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onShowcaseShowFinishedCallback(JNIEnv* env, jobject obj)
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnShowcaseShowFinished();
			}
		});
	}

	JNIEXPORT void Java_ru_mail_mrgservice_MRGServiceCpp_onReceivedOpenUDID(JNIEnv* env, jobject obj, jstring judid)
	{
		FString Udid = MRGSJniHelper::JavaStringToFstring(judid);
		AsyncTask(ENamedThreads::GameThread, [Udid]() {
			auto* Proxy = UPsMRGSLibrary::GetMRGSProxy();
			if (Proxy)
			{
				Proxy->OnReceivedOpenUDID(Udid);
			}
		});
	}
}

#endif
