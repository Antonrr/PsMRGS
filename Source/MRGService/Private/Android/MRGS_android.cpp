// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "MRGServicePrivatePCH.h"

#if WITH_MRGS

#if PLATFORM_ANDROID

#include "MRGS.h"
#include <stdarg.h>
#include <android/log.h>
#include <jni.h>
#include <string>
#include "AndroidApplication.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <ctype.h>

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOG_TAG    "[CPP] MRGServiceCpp.log"

jmethodID MRGService::AndroidThunkJava_MRGService_initUserWithId;
jmethodID MRGService::AndroidThunkJava_MRGService_admanLoadData;
jmethodID MRGService::AndroidThunkJava_MRGService_admanOpen;
jmethodID MRGService::AndroidThunkJava_MRGService_admanClose;
jmethodID MRGService::AndroidThunkJava_MRGService_admanRelease;
jmethodID MRGService::AndroidThunkJava_MRGService_admanSetTitle;
jmethodID MRGService::AndroidThunkJava_MRGService_showSupport;
jmethodID MRGService::AndroidThunkJava_MRGService_flurryEvent;
jmethodID MRGService::AndroidThunkJava_MRGService_sendGAEvent;
jmethodID MRGService::AndroidThunkJava_MRGService_sendGAScreen;
jmethodID MRGService::AndroidThunkJava_MRGService_sendAFEvent;
jmethodID MRGService::AndroidThunkJava_MRGService_getProductsInfo;
jmethodID MRGService::AndroidThunkJava_MRGService_buyItem;
jmethodID MRGService::AndroidThunkJava_MRGService_addMetric;

//MRGSBillingDelegate* MRGSBilling::delegate = NULL;
////MRGSNotificationServiceDelegate* MRGSNotificationService::delegate = NULL;
//MRGServiceDelegate* MRGService::delegate = NULL;
std::vector<MRGSPurchaseItem> MRGSBilling::PurchaseItems;

class MRGSJniHelper
{
    public:
    /**
     * Получает std-строку из Java-строки
     */
    static std::string jstring2string(jstring str);
    static std::string IntToString(int number);
};

std::string MRGSJniHelper::jstring2string(jstring jstr)
{
	if (jstr == NULL) {
		return "";
	}
	
	JNIEnv *env = 0;
	env = FAndroidApplication::GetJavaEnv(true);
	if (!env)
	{
		return 0;
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
	return ret;
}

std::string IntToString(int number)
{
    std::ostringstream oss;
    oss<< number;
    return oss.str();
}


using namespace std;

    void MRGSLOG(const char* format, ...) {
        va_list args;
        va_start(args, format);
        __android_log_vprint(ANDROID_LOG_DEBUG, LOG_TAG, format, args);
        va_end(args);
    }
    
//    jobject stdMapToJavaMap(JNIEnv* env, const map<string, string>& stdMap) {
//        jclass mapClass = MRGSJniHelper::MRGSJniHelper::getClassID("java/util/HashMap");
//        jmethodID ctrID = env->GetMethodID(mapClass, "<init>", "()V");
//        jobject jmap = env->NewObject(mapClass, ctrID);
//        
//        jmethodID putID = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
//        for (map<string, string>::const_iterator it = stdMap.begin(); it != stdMap.end(); it++) {
//            jstring jkey = env->NewStringUTF((*it).first.c_str());
//            jstring jvalue = env->NewStringUTF((*it).second.c_str());
//            env->CallObjectMethod(jmap, putID, jkey, jvalue);
//            env->DeleteLocalRef(jkey);
//            env->DeleteLocalRef(jvalue);
//        }
//        
//        return jmap;
//    }

/** Common */
    MRGServiceDelegate* MRGService::delegate = NULL;
//    jclass MRGSJniHelper::MRGServiceCppClassId = NULL;

    void MRGService::initUserWithId(const string& uid)
    {
        LOGD("MRGS initWithUid");
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_initUserWithId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_initUserWithId", "(Ljava/lang/String;)V", false);

            LOGD("MRGS initWithUid start");
            jstring Juid = Env->NewStringUTF(uid.c_str());
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_initUserWithId, Juid);
            Env->DeleteLocalRef(Juid);
        }
        else
        {
            LOGD("MRGS initWithUid error");
        }
    }
    
/** Billing */
    
    MRGSBillingDelegate* MRGSBilling::delegate = NULL;
    
    void MRGSBilling::getProductsInfo(const vector<string>& skuList)
    {
        if (skuList.empty())
            return;
        
        string resultList;
        for(int i = 0; i < skuList.size(); i++)
        {
            string item = skuList[i];
            resultList += item;
            if(i < skuList.size() - 1)
            {
                resultList += ",";
            }
        }
        MRGSLOG("MRGSBilling::products %s", resultList.c_str());
        MRGSLOG("MRGSBilling::getProductsInfo. list size is %d", skuList.size());
 
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_getProductsInfo = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_getProductsInfo", "(Ljava/lang/String;)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_getProductsInfo, Env->NewStringUTF(resultList.c_str()));
            
        }
        else
        {
            LOGD("MRGService::getProductsInfo error");
        }
    }
    
    void MRGSBilling::buyItem(const std::string& sku)
    {
        MRGSLOG("MRGSBilling::buyItem");
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_buyItem = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_buyItem", "(Ljava/lang/String;)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_buyItem, Env->NewStringUTF(sku.c_str()));
            
        }
        else
        {
            LOGD("MRGSBilling::buyItem error");
        }
    }
    
/** Statistic */

    void MRGService::flurryEvent(const std::string& event)
    {
        LOGD("MRGService::flurryEvent");
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_flurryEvent = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_flurryEvent", "(Ljava/lang/String;)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_flurryEvent, Env->NewStringUTF(event.c_str()));
            
        }
        else
        {
            LOGD("MRGService::flurryEvent error");
        }
    }

    void MRGService::sendGAEvent(const std::string& category, const std::string& action, const std::string& label, int value)
    {
        LOGD("MRGService::sendGAEvent");
        std::string tempValue = IntToString(value);
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_sendGAEvent = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_sendGAEvent", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_sendGAEvent, Env->NewStringUTF(category.c_str()), Env->NewStringUTF(action.c_str()), Env->NewStringUTF(label.c_str()), Env->NewStringUTF(tempValue.c_str()));
            
        }
        else
        {
            LOGD("MRGService::sendGAEvent error");
        }
    }

    void MRGService::sendGAScreen(const std::string& screenName)
    {
        LOGD("MRGService::sendGAScreen");
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_sendGAScreen = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_sendGAScreen", "(Ljava/lang/String;)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_sendGAScreen, Env->NewStringUTF(screenName.c_str()));
            
        }
        else
        {
            LOGD("MRGService::sendGAScreen error");
        }
    }

    void MRGService::sendAFEvent(const std::string& eventName, const std::string& value)
    {
        LOGD("MRGService::sendAFEvent");
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_sendAFEvent = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_sendAFEvent", "(Ljava/lang/String;Ljava/lang/String;)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_sendAFEvent, Env->NewStringUTF(eventName.c_str()), Env->NewStringUTF(value.c_str()));
            
        }
        else
        {
            LOGD("MRGService::sendAFEvent error");
        }
    }


/** Support */

    void MRGService::showSupportWithSecret(const std::string& secretKey)
    {
        LOGD("MRGService::showSupportWithSecret");
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_showSupport = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_showSupport", "(Ljava/lang/String;)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_showSupport, Env->NewStringUTF(secretKey.c_str()));
            
        }
        else
        {
            LOGD("MRGService::showSupport error");
        }
    }

	void MRGService::addMetric(int value)
	{
		LOGD("MRGService::addMetric");
		if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
		{
			std::string tempValue = IntToString(value);
			MRGService::AndroidThunkJava_MRGService_addMetric = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_addMetric", "(Ljava/lang/String;)V", false);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_addMetric, Env->NewStringUTF(tempValue.c_str()));
		}
		else
		{
			LOGD("MRGService::addMetric error");
		}
	}

/** Adman */

    MRGSAdmanDelegate* MRGSAdman::delegate = NULL;

    void MRGSAdman::showMyTargetShowcase()
    {
        LOGD("MRGService::showMyTargetShowcase");
        MRGSAdman::loadData(MRGSAdmanEntityType::MRGSAdmanShowcase);
    }

    void MRGSAdman::showMyTargetFullscreen()
    {
        LOGD("MRGService::showMyTargetFullscreen");
        MRGSAdman::loadData(MRGSAdmanEntityType::MRGSAdmanFullscreenBanner);
    }

    void MRGSAdman::setShowcaseTitle(const std::string& title)
    {
        LOGD("MRGSAdman::setShowcaseTitle");
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_admanSetTitle = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanSetTitle", "(Ljava/lang/String;)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_admanSetTitle, Env->NewStringUTF(title.c_str()));
        }
        else
        {
            LOGD("MRGSAdman::setShowcaseTitle error");
        }
    }

    void MRGSAdman::loadData(const MRGSAdmanEntityType& entityType)
    {
        std::string tempType = IntToString((int)entityType);
        LOGD("MRGSAdman::loadData");
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_admanLoadData = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanLoadData", "(Ljava/lang/String;)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_admanLoadData, Env->NewStringUTF(tempType.c_str()));

        }
        else
        {
            LOGD("MRGSAdman::loadData error");
        }
    }
    
    void MRGSAdman::open(const MRGSAdmanEntityType& entityType)
    {
        int tempType = (int)entityType;
        LOGD("MRGSAdman::open(%d)", entityType);
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_admanOpen = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanOpen", "(I)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_admanOpen, (jint)tempType);
        }
        else
        {
            LOGD("MRGSAdman::open error");
        }
    }
    
    void MRGSAdman::close(const MRGSAdmanEntityType& entityType)
    {
        int tempType = (int)entityType;
        LOGD("MRGSAdman::close(%d)", entityType);
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_admanClose = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanClose", "(I)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_admanClose, (jint)tempType);
        }
        else
        {
            LOGD("MRGSAdman::close error");
        }
    }
    
    void MRGSAdman::release(const MRGSAdmanEntityType& entityType)
    {
        int tempType = (int)entityType;
        LOGD("MRGSAdman::release(%d)", entityType);
        if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
        {
            MRGService::AndroidThunkJava_MRGService_admanRelease = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_MRGService_admanRelease", "(I)V", false);
            FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis,  MRGService::AndroidThunkJava_MRGService_admanRelease, (jint)tempType);
        }
        else
        {
            LOGD("MRGSAdman::release error");
        }
    }

/** Adman Delegate */

    void MRGSAdmanDelegate::admanLoadDataComplete(MRGSAdmanEntityType entityType, bool hasNotificaitons)
    {
        LOGD("MRGSAdmanDelegate::admanLoadDataComplete");
        MRGSAdman::open(entityType);
    }

    void MRGSAdmanDelegate::admanLoadDataFail(MRGSAdmanEntityType entityType, const std::string &reason)
    {
        MRGSLOG("MRGSAdmanDelegate::admanLoadDataFail(%s, %s) ", (entityType == MRGSAdmanShowcase) ? "showcase" : "banner", reason.c_str());
    }

    void MRGSAdmanDelegate::admanDidClose(MRGSAdmanEntityType entityType)
    {
        MRGSLOG("MRGSAdmanDelegate::admanDidClose(%s) ", (entityType == MRGSAdmanShowcase) ? "showcase" : "banner");
        MRGSAdman::release(entityType);
    }

extern "C" {
    /**************************************/
    /*               Common               */
    /**************************************/
    void Java_ru_mail_mrgservice_MRGServiceCpp_onLoadServerDataDidFinished(JNIEnv* env, jobject obj, jstring json) {
        if (MRGService::delegate) {
            MRGService::delegate->loadServerDataDidFinished(MRGSJniHelper::jstring2string(json));
        }
    }
    
    void Java_ru_mail_mrgservice_MRGServiceCpp_onLoadPromoBannersDidFinished(JNIEnv* env, jobject obj, jstring json) {
        if (MRGService::delegate) {
            MRGService::delegate->loadPromoBannersDidFinished(MRGSJniHelper::jstring2string(json));
        }
    }

    /**************************************/
    /*               Support               */
    /**************************************/
    void Java_ru_mail_mrgservice_MRGServiceCpp_onMyComSupportHasError(JNIEnv* env, jobject obj)
    {
        MRGService::supportDidRecieveError();
    }
    
    void Java_ru_mail_mrgservice_MRGServiceCpp_onMyComHasSupportDidClose(JNIEnv* env, jobject obj)
    {
        MRGService::supportDidClosedCallback();
    }
    
    /***************************************/
    /*               Billing               */
    /***************************************/
    void fillPurchaseItem(JNIEnv* env, jobject jitem, MRGSPurchaseItem* item) {
        jclass itemClass = env->GetObjectClass(jitem);
        
        jfieldID fid = env->GetFieldID(itemClass, "sku", "Ljava/lang/String;");
        jstring jsku = (jstring)env->GetObjectField(jitem, fid);
        item->sku = MRGSJniHelper::jstring2string(jsku);
        
        fid = env->GetFieldID(itemClass, "price", "Ljava/lang/String;");
        jstring jprice = (jstring)env->GetObjectField(jitem, fid);
        item->price = MRGSJniHelper::jstring2string(jprice);
        
        fid = env->GetFieldID(itemClass, "title", "Ljava/lang/String;");
        jstring jtitle = (jstring)env->GetObjectField(jitem, fid);
        item->title = MRGSJniHelper::jstring2string(jtitle);
        
        fid = env->GetFieldID(itemClass, "type", "Ljava/lang/String;");
        jstring jtype = (jstring)env->GetObjectField(jitem, fid);
        item->type = MRGSJniHelper::jstring2string(jtype);
        
        fid = env->GetFieldID(itemClass, "description", "Ljava/lang/String;");
        jstring jdescription = (jstring)env->GetObjectField(jitem, fid);
        item->description = MRGSJniHelper::jstring2string(jdescription);
    }
    
    void Java_ru_mail_mrgservice_MRGServiceCpp_onLoadProductsDidFinished(JNIEnv* env, jobject obj, jobject jItems) {
        if (MRGSBilling::delegate) {
            jclass listClass = env->GetObjectClass(jItems);
            jmethodID mid = env->GetMethodID(listClass, "size", "()I");
            int size = (int)env->CallIntMethod(jItems, mid);
            
            vector<MRGSPurchaseItem> items;
            mid = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
            
            for (int i = 0; i < size; i++) {
                jobject jitem = env->CallObjectMethod(jItems, mid, i);
                MRGSPurchaseItem item;
                fillPurchaseItem(env, jitem, &item);
                items.push_back(item);
                env->DeleteLocalRef(jitem);
            }
			
			MRGSBilling::PurchaseItems = items;
            MRGSBilling::delegate->loadProductsDidFinished(items);
        }
    }
    
    void Java_ru_mail_mrgservice_MRGServiceCpp_onPurchaseComplete(JNIEnv* env, jobject obj, jstring sku, jstring transactionId, jstring answer) {
        if (MRGSBilling::delegate) {
            MRGSBilling::delegate->purchaseComplete(MRGSJniHelper::jstring2string(sku), MRGSJniHelper::jstring2string(transactionId), MRGSJniHelper::jstring2string(answer));
        }
    }
    
    void Java_ru_mail_mrgservice_MRGServiceCpp_onPurchaseFail(JNIEnv* env, jobject obj, jstring sku, jstring answer) {
        if (MRGSBilling::delegate) {
            MRGSBilling::delegate->purchaseFail(MRGSJniHelper::jstring2string(sku), MRGSJniHelper::jstring2string(answer));
        }
    }
    /***************************************/
    
    
    /**************************************/
    /*         Push Notifications         */
    /**************************************/
//    void Java_ru_mail_mrgservice_MRGServiceCpp_onReceivedNotification(JNIEnv* env, jobject obj, jint jnotificationId, jstring jtitle, jstring jmessage, jboolean jIsLocal) {
//        if (MRGSNotificationService::delegate) {
//            MRGSNotificationService::delegate->receivedNotification(jnotificationId, MRGSJniHelper::jstring2string(jtitle), MRGSJniHelper::jstring2string(jmessage), jIsLocal == JNI_TRUE);
//        }
//    }
//    
//    void Java_ru_mail_mrgservice_MRGServiceCpp_onClickOnNotification(JNIEnv* env, jobject obj, jint jnotificationId, jstring jtitle, jstring jmessage, jboolean jIsLocal) {
//        if (MRGSNotificationService::delegate) {
//            MRGSNotificationService::delegate->clickOnNotification(jnotificationId, MRGSJniHelper::jstring2string(jtitle), MRGSJniHelper::jstring2string(jmessage), jIsLocal == JNI_TRUE);
//        }
//    }
    /**************************************/
    
    /*************************/
    /*         Adman         */
    /*************************/
    void Java_ru_mail_mrgservice_MRGServiceCpp_onAdmanLoadComplete(JNIEnv* env, jobject obj, jint jtype, jboolean jnotification) {
        LOGD("MRGSAdman::admanLoadDataComplete");
        if (MRGSAdman::delegate) {
            MRGSAdman::delegate->admanLoadDataComplete((MRGSAdmanEntityType) jtype, jnotification == JNI_TRUE);
        }
    }
    
    void Java_ru_mail_mrgservice_MRGServiceCpp_onAdmanViewComplete(JNIEnv* env, jobject obj, jint jtype) {
        LOGD("MRGSAdman::onAdmanViewComplete");
        if (MRGSAdman::delegate) {
            MRGSAdman::delegate->admanDidClose((MRGSAdmanEntityType) jtype);
        }
    }
    /*************************/
    
    
    //    void MRGSBilling::restoreTransaction() {
    //        MRGSLOG("MRGSBilling::restoreTransaction()");
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "restoreTransaction", "()V")) {
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID);
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    //
    //    void MRGSBilling::closePayment(const std::string& sku, const std::string& transactionId) {
    //        MRGSLOG("MRGSBilling::closePayment(%s, %s)", sku.c_str(), transactionId.c_str());
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "closePayment", "(Ljava/lang/String;Ljava/lang/String;)V")) {
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(sku.c_str()), t.env->NewStringUTF(transactionId.c_str()));
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    //
    //    void MRGSBilling::addSamsungProductPrice(const std::string& groupId, const std::string& itemId, double priceInUSD) {
    //        MRGSLOG("MRGSBilling::addSamsungProductPrice(%s, %s, %f)", groupId.c_str(), itemId.c_str(), priceInUSD);
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "addSamsungProductPrice", "(Ljava/lang/String;Ljava/lang/String;D)V")) {
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(groupId.c_str()), t.env->NewStringUTF(itemId.c_str()), (jdouble) priceInUSD);
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    //

    
/** Notification Service */
    //
    //    void fillPush(JNIEnv* env, jobject jpush, MRGSNotification& push) {
    //        jclass pushClass = env->GetObjectClass(jpush);
    //
    //        jmethodID mid = env->GetMethodID(pushClass, "getId", "()I");
    //        push.notificationId = env->CallIntMethod(jpush, mid);
    //
    //        mid = env->GetMethodID(pushClass, "getMessage", "()Ljava/lang/String;");
    //        push.message = MRGSJniHelper::jstring2string((jstring)env->CallObjectMethod(jpush, mid));
    //
    //        mid = env->GetMethodID(pushClass, "getUnixTimeStamp", "()J");
    //        push.timestamp = (long long)env->CallLongMethod(jpush, mid);
    //    }
    //
    //    MRGSNotificationServiceDelegate* MRGSNotificationService::delegate = NULL;
    //
    //    MRGSNotification::MRGSNotification(int notificationId, const std::string& message, long long timestamp) {
    //        this->notificationId = notificationId;
    //        this->message = message;
    //        this->timestamp = timestamp;
    //    }
    //
    //    void MRGSNotificationService::addLocalNotification(const MRGSNotification& notification) {
    //        MRGSLOG("MRGSNotificationService::addLocalNotification(%d, '%s', %lld)", notification.notificationId, notification.message.c_str(), notification.timestamp);
    //
    //        JNIEnv *env = 0;
    //        if (!MRGSJniHelper::getEnv(&env)) {
    //            return;
    //        }
    //
    //        MRGSJniMethodInfo createInfo;
    //        if (MRGSJniHelper::getStaticMethodInfo(createInfo, "ru/mail/mrgservice/MRGSPushNotification", "create", "(Ljava/lang/String;IJ)Lru/mail/mrgservice/MRGSPushNotification;")) {
    //            jobject jpush = createInfo.env->CallStaticObjectMethod(createInfo.classID, createInfo.methodID,
    //                                                                   env->NewStringUTF(notification.message.c_str()), (jint)notification.notificationId, (jlong)notification.timestamp);
    //            createInfo.env->DeleteLocalRef(createInfo.classID);
    //
    //            MRGSJniMethodInfo addInfo;
    //            if (MRGSJniHelper::getStaticMethodInfo(addInfo, "ru/mail/mrgservice/MRGServiceCpp", "addLocalPush", "(Lru/mail/mrgservice/MRGSPushNotification;)V")) {
    //                addInfo.env->CallStaticVoidMethod(addInfo.classID, addInfo.methodID, jpush);
    //                addInfo.env->DeleteLocalRef(addInfo.classID);
    //            }
    //        }
    //    }
    //
    //    void MRGSNotificationService::removeLocalNotification(const int notificationId) {
    //        MRGSLOG("MRGSNotificationService::removeLocalNotification(%d)", notificationId);
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "removeLocalPush", "(I)V")) {
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID, notificationId);
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    //
    //    void MRGSNotificationService::getLocalNotification(int notificationId, MRGSNotification& push) {
    //        MRGSLOG("MRGSNotificationService::getLocalNotification(%d)", notificationId);
    //
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "getLocalPush", "(I)Lru/mail/mrgservice/MRGSPushNotification;")) {
    //            jobject jpush = t.env->CallStaticObjectMethod(t.classID, t.methodID, notificationId);
    //            t.env->DeleteLocalRef(t.classID);
    //
    //            if (jpush) {
    //                fillPush(t.env, jpush, push);
    //            } else {
    //                push.notificationId = -1;
    //            }
    //        }
    //    }
    //
    //    void MRGSNotificationService::getAllLocalNotifications(std::vector<MRGSNotification>& list) {
    //        MRGSLOG("MRGSNotificationService::getAllLocalNotifications()");
    //
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "getAllLocalPushes", "()Ljava/util/List;")) {
    //            jobject jlist = t.env->CallStaticObjectMethod(t.classID, t.methodID);
    //            t.env->DeleteLocalRef(t.classID);
    //
    //            jclass listClass = t.env->GetObjectClass(jlist);
    //            jmethodID mid = t.env->GetMethodID(listClass, "size", "()I");
    //            int size = (int)t.env->CallIntMethod(jlist, mid);
    //
    //            mid = t.env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
    //            for (int i = 0; i < size; i++) {
    //                jobject jpush = t.env->CallObjectMethod(jlist, mid, i);
    //                MRGSNotification push;
    //                fillPush(t.env, jpush, push);
    //                list.push_back(push);
    //                t.env->DeleteLocalRef(jpush);
    //            }
    //        }
    //    }
    //    
    //
    //    
    //
    

    
    //    void MRGService::flurryEventStart(const std::string& event, const std::map<std::string, std::string> params) {
    //        MRGSLOG("MRGService::flurryEventStart('%s', params.size = %d)", event.c_str(), params.size());
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "flurryEventStart", "(Ljava/lang/String;Ljava/util/Map;)V")) {
    //            jobject jmap = stdMapToJavaMap(t.env, params);
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(event.c_str()), jmap);
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    //
    //    void MRGService::flurryEventStop(const std::string& event, const std::map<std::string, std::string> params) {
    //        MRGSLOG("MRGService::flurryEventStop('%s', params.size = %d)", event.c_str(), params.size());
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "flurryEventStop", "(Ljava/lang/String;Ljava/util/Map;)V")) {
    //            jobject jmap = stdMapToJavaMap(t.env, params);
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(event.c_str()), jmap);
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    //
    //    void MRGService::sendGASocEvent(const std::string& network, const std::string& action, const std::string& target) {
    //        MRGSLOG("MRGService::sendGASocEvent('%s', '%s', '%s')", network.c_str(), action.c_str(), target.c_str());
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "sendGASocEvent", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) {
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(network.c_str()), t.env->NewStringUTF(action.c_str()), t.env->NewStringUTF(target.c_str()));
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    
    //    void MRGService::sendGATimings(const std::string& category, long long interval, const std::string& name, const std::string& label) {
    //        MRGSLOG("MRGService::sendGATimings('%s', %lld, '%s', '%s')", category.c_str(), interval, name.c_str(), label.c_str());
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "sendGATimings", "(Ljava/lang/String;JLjava/lang/String;Ljava/lang/String;)V")) {
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(category.c_str()), (jlong)interval, t.env->NewStringUTF(name.c_str()),
    //                                        t.env->NewStringUTF(label.c_str()));
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    //
    //
    //    void MRGService::sendHandleException(const std::string& description) {
    //        MRGSLOG("MRGService::sendHandleException('%s')", description.c_str());
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "sendHandleException", "(Ljava/lang/String;Ljava/lang/String;)V")) {
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(description.c_str()), NULL);
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    //
    //    void MRGService::sendHandleException(const std::string& description, const std::string& reason) {
    //        MRGSLOG("MRGService::sendHandleException('%s', '%s')", description.c_str(), reason.c_str());
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "sendAFEvent", "(Ljava/lang/String;Ljava/lang/String;)V")) {
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(description.c_str()), t.env->NewStringUTF(reason.c_str()));
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    //
    //    void MRGService::markUserAsCheater(int want, int have) {
    //        MRGSLOG("MRGService::markUserAsCheater(%d, %d)", want, have);
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "markUserAsCheater", "(IILjava/lang/String;)V")) {
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID, (jint)want, (jint)have, NULL);
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    //
    //    void MRGService::markUserAsCheater(int want, int have, const std::string& comment) {
    //        MRGSLOG("MRGService::markUserAsCheater(%d, %d, '%s')", want, have, comment.c_str());
    //        MRGSJniMethodInfo t;
    //        if (MRGSJniHelper::getStaticMethodInfo(t, "ru/mail/mrgservice/MRGServiceCpp", "markUserAsCheater", "(IILjava/lang/String;)V")) {
    //            t.env->CallStaticVoidMethod(t.classID, t.methodID, (jint)want, (jint)have, t.env->NewStringUTF(comment.c_str()));
    //            t.env->DeleteLocalRef(t.classID);
    //        }
    //    }
    //    

}
#endif // PLATFORM_ANDROID

#endif // WITH_MRGS
