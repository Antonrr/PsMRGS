package ru.mail.mrgservice;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.util.Pair;
import android.content.res.AssetManager;
import android.os.Bundle;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.io.InputStream;
import ru.mail.mrgservice.*;
import com.epicgames.*;
import com.epicgames.ue4.GameActivity;
import android.widget.Toast;
import ru.mail.mrgservice.MRGSMyComSupportDialog;
import ru.mail.mrgservice.MRGSMetrics;

/**
 * Вспомогательный класс для взаимодействия Java кода с C++ кодом
 */

@SuppressWarnings("unused")
public class MRGServiceCpp {
	private final static String LOG_TAG = "[JAVA] MRGServiceCpp.log";
	//support key Jhfd763NMqweYUkGD

	private final static int ADMAN_SHOWCASE = 1;
	private final static int ADMAN_FULLSCREEN_BANNER = 2;
	private final static int ADMAN_INTERSTITIAL = 3;

	public static Context appContext;

	private static MRGSGDPR mGDPR;
	private static native void onUserHasAcceptedGDPR(final boolean withAdvertising);
	private static native void onErrorShowingAgreement();

	public static native void onInitComplete();

	public static native void onUserAuthError();
	private static native void onUserAuthSuccess();

	private static native void onLoadServerDataDidFinished(final String json);
	private static native void onLoadPromoBannersDidFinished(final String json);

	private static native void onMyComSupportHasError(final String error);
	private static native void onMyComHasSupportDidClose();

	public static native void onLoadProductsDidFinished(final List<MRGSPurchaseItem> items);
	private static native void onPurchaseComplete(final String sku, final String transactionId, final String answer);
	private static native void onPurchaseCancel(final String sku, final String transactionId, final String answer);
	private static native void onPurchasePending(final String sku);
	private static native void onPurchaseFail(final String sku, final String answer);

	private static native void onReceivedNotification(final int notificationId, final String title, final String message, final boolean isLocal);
	private static native void onClickOnNotification(final int notificationId, final String title, final String message, final boolean isLocal);

	public static native void onAdmanLoadComplete(int type, boolean notification);
	public static native void onAdmanViewComplete(int type);
	public static native void onAdmanHasNoAdd(int type);


	/**
	 * Позволяет выполнять callback'и от MRGService в потоке, в котором требуется приложению. Например, для
	 * Cocos2d-x это GLThread. По умолчанию все callback'и вызываются в UI-потоке.
	 */
	public static ThreadHelper threadHelper = new ThreadHelper() {
		@Override
		public void runOnNecessaryThread(final Runnable callback) {
			//runOnUiThread(callback);
			// callback.run();
			GameActivity.Get().runOnNecessaryThread(callback);
		}
	};

	private final static MRGSGDPR.MRGSGDPRDelegate mGDPRDelegate = new MRGSGDPR.MRGSGDPRDelegate() {
		@Override
		public void userHasAcceptedGDPR(boolean withAdvertising) {
			Log.v(LOG_TAG, "userHasAcceptedGDPR");
			final boolean finalWithAdvertising = withAdvertising;
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onUserHasAcceptedGDPR(finalWithAdvertising);
				}
			});
		}

		@Override
		public void errorShowingAgreement() {
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onErrorShowingAgreement();
				}
			});
		}
	};

	private final static MRGSServerData.MRGSServerDataDelegate mServerDataDelegate = new MRGSServerData.MRGSServerDataDelegate() {
		@Override
		public void loadServerDataDidFinished(final MRGSMap serverData) {
			Log.v(LOG_TAG, "loadServerDataDidFinished");
			final String serverDataJson = MRGSJson.stringWithMap(serverData);
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onLoadServerDataDidFinished(serverDataJson);
				}
			});
		}

		@Override
		public void loadPromoBannersDidFinished(final MRGSMap promoBanners) {
			Log.v(LOG_TAG, "loadPromoBannersDidFinished");
			final String promoBannersDataJson = MRGSJson.stringWithMap(promoBanners);
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onLoadPromoBannersDidFinished(promoBannersDataJson);
				}
			});
		}
	};


	private final static MRGSBillingDelegateEx mBillingDelegate = new MRGSBillingDelegateEx() {
		@Override
		public void onReceiveProductsResponce(final MRGSBillingEntities.MRGSBankProductsResponse productsResponce)
		{
			Log.v(LOG_TAG, String.format("MRGSBillingDelegate.onReceiveProductsResponce(%s, items.count = %d)", MRGSBilling.instance().getBillingName(), productsResponce.validItems.size()));
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onLoadProductsDidFinished(productsResponce.validItems);
				}
			});
		}

	    @Override
		public void onReceiveProductsError(final MRGSBillingEntities.MRGSBankProductsResponse productsResponce)
	    {

	    }

	    @Override
		public void onReceiveSuccessfullPurchase(final MRGSBillingEntities.MRGSBankPurchaseResult purchase)
	    {
	    	Log.v(LOG_TAG, String.format("MRGSBillingDelegate.onReceiveSuccessfullPurchase(%s, %s, %s)", MRGSBilling.instance().getBillingName(), purchase.purchaseItem, purchase.developerPayload));
			if (purchase.purchaseItem.resultCode != 1) // BILLING_RESPONSE_RESULT_USER_CANCELED = 1
			{
				threadHelper.runOnNecessaryThread(new Runnable() {
					@Override
					public void run() {
						onPurchaseComplete(purchase.purchaseItem.sku, purchase.purchaseItem.transactionId, purchase.developerPayload);
					}
				});
			}
			else
			{
				threadHelper.runOnNecessaryThread(new Runnable() {
					@Override
					public void run() {
						onPurchaseCancel(purchase.purchaseItem.sku, purchase.purchaseItem.transactionId, purchase.developerPayload);
					}
				});
			}
	    }

	    @Override
		public void onReceiveFailedPurchase(final MRGSBillingEntities.MRGSBankPurchaseResult purchase)
	    {
	    	Log.v(LOG_TAG, String.format("MRGSBillingDelegate.onReceiveFailedPurchase(%s, %s, %s)", MRGSBilling.instance().getBillingName(), purchase.purchaseItem, purchase.developerPayload));
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onPurchaseFail((purchase.purchaseItem != null ? purchase.purchaseItem.sku : null), purchase.developerPayload);
				}
			});
	    }

	    @Override
		public void onReceivePendingPurchase(final MRGSBillingEntities.MRGSBankPurchaseResult purchase)
	    {
	    	Log.v(LOG_TAG, String.format("MRGSBillingDelegate.onReceivePendingPurchase(%s, %s)", MRGSBilling.instance().getBillingName(), purchase.purchaseItem));
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onPurchasePending((purchase.purchaseItem != null ? purchase.purchaseItem.sku : null));
				}
			});
	    }

	    @Override
		public void onReceiveCancelledPurchase(final MRGSBillingEntities.MRGSBankPurchaseResult purchase)
	    {
	    	Log.v(LOG_TAG, String.format("MRGSBillingDelegate.onReceiveCancelledPurchase(%s, %s)", MRGSBilling.instance().getBillingName(), purchase.purchaseItem));
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onPurchaseCancel(purchase.purchaseItem.sku, purchase.purchaseItem.transactionId, purchase.developerPayload);
				}
			});
	    }

	    @Override
		public void onTransactionsRestoreCompleted()
	    {

	    }
	};

	private final static MRGSMyComSupportDialog.MyComListener mSupportDelegate = new MRGSMyComSupportDialog.MyComListener() {
		@Override
		public boolean onMyComSupportError(final Exception ex) {
			Log.e("[MYCOM]", "My.com support error: " + ex.getLocalizedMessage());
			onMyComSupportHasError(ex.getLocalizedMessage());
			// true - если не надо показывать стандартное окно с ошибкой, false - если нужно
			return false;
		}

		@Override
		public void onMyComSupportClose() {
			Log.v("[MYCOM]", "My.com support is closed");
			onMyComHasSupportDidClose();
		}
	};

	public static void ShowDefaultGDPRAgreement(final String appId, final boolean bOnlyEU, final boolean bWithAdvertising) {
		GameActivity activity = GameActivity.Get();
		activity.runOnUiThread(new Runnable() 
		{
			@Override
			public void run() 
			{
				GameActivity thisActivity = GameActivity.Get();
				mGDPR = MRGSGDPR.MRGSGDPRFactory.getMRGSGDPR();
				mGDPR.setDelegate(mGDPRDelegate);
				mGDPR.onlyEU(bOnlyEU);
				mGDPR.withAdvertising(bWithAdvertising);
				mGDPR.showDefaultAgreementAtActivity(thisActivity, appId);
			}
		});
	}

	public static void ShowGDPRAgreement(final String appId, final boolean bOnlyEU, final boolean bWithAdvertising) {
		GameActivity activity = GameActivity.Get();
		activity.runOnUiThread(new Runnable() 
		{
			@Override
			public void run() 
			{
				GameActivity thisActivity = GameActivity.Get();
				AssetManager AssetManagerInstance = thisActivity.getAssets();

				mGDPR = MRGSGDPR.MRGSGDPRFactory.getMRGSGDPR();
				mGDPR.setDelegate(mGDPRDelegate);
				mGDPR.onlyEU(bOnlyEU);
				mGDPR.withAdvertising(bWithAdvertising);
				mGDPR.showAgreementAtActivity(thisActivity,
                             appId,
                             "gdpr/gdpr.html");
			}
		});
	}

	public static int GetGDPRAcceptedVersion() {
		mGDPR = MRGSGDPR.MRGSGDPRFactory.getMRGSGDPR();
		GameActivity activity = GameActivity.Get();
		return mGDPR.getAgreedVersion(activity);
	}

	public static boolean ShouldShowCCPA() {
		mGDPR = MRGSGDPR.MRGSGDPRFactory.getMRGSGDPR();
		GameActivity activity = GameActivity.Get();
		return mGDPR.shouldShowCCPAButton(activity);
	}

	public static int GetCCPASettingValue() {
		mGDPR = MRGSGDPR.MRGSGDPRFactory.getMRGSGDPR();
		GameActivity activity = GameActivity.Get();
		return mGDPR.getCurrentCCPAUserPreference(activity);
	}

	public static void SetCCPASettingValue(int Value) {
		mGDPR = MRGSGDPR.MRGSGDPRFactory.getMRGSGDPR();
		GameActivity activity = GameActivity.Get();
		mGDPR.setUserChangedCCPAPreferences(activity, Value);
	}

	public static void showSupport(final String secretKey) {
		Log.v(LOG_TAG, String.format("showSupport(%s)", secretKey));

		GameActivity activity = GameActivity.Get();
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				try {
					Log.v(LOG_TAG, "showSupport try start");		
					GameActivity tempActivity = GameActivity.Get();
					MRGSMyComSupportDialog dialog = new MRGSMyComSupportDialog(tempActivity, secretKey, true);
					dialog.setListener(mSupportDelegate);
					dialog.setErrorMessage("Не удалось загрузить страницу");
					dialog.setErrorTitle("Ошибка подключения");
					dialog.setErrorButton("Ок");
					dialog.setFullscreen(true);
					dialog.show();
					Log.v(LOG_TAG, "showSupportStart");			
				}	
				catch (Throwable e) {
    				e.printStackTrace();
				}
			}
		});
	}

	public static void addMetric(int metricId){
		MRGSMetrics.addMetric(metricId);
	}

	public static void addMetric(String metricCode, int value, int level, int objectId){
		MRGSMetrics.addMetric(metricCode, value, level, objectId);
	}

	public static void checkIntegration() {
		MRGService.instance().checkIntegration();
	}

	public static void initWithAppidAndSecret(final String appId, final String appSecret, final boolean bDebug) {

		Log.v(LOG_TAG, String.format("MRGServiceCPP:init (%s, %s, %b)", appId, appSecret, bDebug));

		GameActivity activity = GameActivity.Get();
		activity.runOnUiThread(new Runnable() 
		{
			@Override
			public void run() 
			{
				Log.v(LOG_TAG, String.format("MRGServiceCPP:init context (%s)", appContext.getPackageCodePath()));
				init(appContext, appId, appSecret, bDebug);
			}
		});
	}

	public static void initUser(final String userId) {
		Log.v(LOG_TAG, String.format("MRGServiceCPP:initWithUserId started"));
		MRGSUsers.instance().setUserId(userId);
		Log.v(LOG_TAG, String.format("MRGServiceCPP:initWithUserId registered and authorized new user %s", userId));
		onUserAuthSuccess();
	}

	public static void init(final Context context,final String appId, final String appSecret, final boolean bDebug) {
		Log.v(LOG_TAG, String.format("init MRGS started"));

		Bundle mrgsSettings = new Bundle();
		mrgsSettings.putBoolean("debug", bDebug);
		mrgsSettings.putString("billing", "google");
		mrgsSettings.putBoolean("testDevice", bDebug);
		mrgsSettings.putBoolean("crashReports", false);
		mrgsSettings.putBoolean("localPushNotifications", false);
		mrgsSettings.putString("utmSource", "test-utm-source");

		MRGService.service(context, mServerDataDelegate, appId, appSecret, mrgsSettings);
		MRGSMyComSupport.setTicketListener(SupportTicketListener.instance());
		MRGSBilling.instance().setDelegateEx(mBillingDelegate);

		GameActivity activity = GameActivity.Get();
		activity.OnMrgsInitComplete();
		threadHelper.runOnNecessaryThread(new Runnable() 
		{
			@Override
			public void run() 
			{
				Log.v(LOG_TAG, String.format("MRGServiceCPP:init complete"));
				onInitComplete();
			}
		});
	}

	public static void initPush() {
		MRGSPushNotifications.getInstance().initPushNotifications(PushDelegate.instance());
	}

	public static void initLocalPush() {
		MRGSLocalPushService.setDelegate(PushDelegate.instance());
	}

	public static void getProductsInfo(final String skuList) {
		Log.v(LOG_TAG, String.format("getProductsInfo(%s)", skuList));

		ArrayList<String> skuListTemp = new ArrayList(Arrays.asList(skuList.split(",")));
		ArrayList<Pair<String,String>> skuListToLoad = new ArrayList<>();
		for (int i = 0; i < skuListTemp.size(); i++)
		{
			skuListToLoad.add(Pair.create(skuListTemp.get(i), "cons"));
		}
		MRGSBilling.instance().getProductsInfoWithTypes(skuListToLoad);
	}

	public static void buyItem(final String sku) {
		Log.v(LOG_TAG, String.format("buyItem(%s)", sku));
		MRGSBilling.instance().buyItem(sku);
	}

	public static void restoreTransaction() {
		Log.v(LOG_TAG, "restoreTransaction()");
		MRGSBilling.instance().restoreTransaction();
	}

	public static void closePayment(final String sku, final String transactionId) {
		Log.v(LOG_TAG, String.format("closePayment(%s, %s)", sku, transactionId));
		MRGSPurchaseItem item = new MRGSPurchaseItem();
		item.sku = sku;
		item.transactionId = transactionId;
		MRGSBilling.instance().closePayment(item);
	}

	public static void addLocalPush(final MRGSPushNotification notification) {
		Log.v(LOG_TAG, String.format("addLocalPush(%s)", notification.getJson()));
		MRGSLocalPushService.addLocalPush(notification);
	}

	public static void removeLocalPush(final int notificationId) {
		Log.v(LOG_TAG, String.format("removeLocalPush(%d)", notificationId));
		MRGSLocalPushService.removeLocalPush(notificationId);
	}

	public static MRGSPushNotification getLocalPush(final int notificationId) {
		Log.v(LOG_TAG, String.format("getLocalPush(%d)", notificationId));
		return MRGSLocalPushService.getLocalPush(notificationId);
	}

	public static List<MRGSPushNotification> getAllLocalPushes() {
		Log.v(LOG_TAG, "getAllLocalPushes()");
		MRGSList mrgsList = MRGSLocalPushService.getAllLocalPushes();

		List<MRGSPushNotification> result = new ArrayList<MRGSPushNotification>(mrgsList.size());
		for (int i = 0; i < mrgsList.size(); i++) {
			result.add((MRGSPushNotification)mrgsList.get(i));
		}
		return result;
	}

	/*************************************************/
	/*                   AppsFlyer                   */
	/*************************************************/
	public static void sendAFEvent(final String eventName, final String value) {
		Log.v(LOG_TAG, String.format("sendAFEvent('%s', '%s')", eventName, value));
		MRGSMap params = new MRGSMap();
		params.put("param1", value);
		MRGSAnalytics.getInstance().getAppsFlyer().sendEvent(eventName, params);
	}
	/*************************************************/

	public static void sendHandleException(final String description, final String reason) {
		if (reason == null || reason.length() == 0) {
			Log.v(LOG_TAG, String.format("sendHandleException('%s')", description));
			MRGService.sendHandleException(description);
		} else {
			Log.v(LOG_TAG, String.format("sendHandleException('%s', '%s')", description, reason));
			MRGService.sendHandleException(description, reason);
		}
	}

	public static void markUserAsCheater(final int want, final int have, final String comment) {
		if (comment == null || comment.length() == 0) {
			Log.v(LOG_TAG, String.format("markUserAsCheater(%d, %d)", want, have));
			MRGSUsers.instance().markUserAsCheater(want, have);
		} else {
			Log.v(LOG_TAG, String.format("markUserAsCheater(%d, %d, '%s')", want, have, comment));
			MRGSUsers.instance().markUserAsCheater(want, have, comment);
		}
	}

	private static void invokeUiThreadIfNeeded(final Runnable runnable) {
		if (Looper.getMainLooper().getThread() == Thread.currentThread()) { // UI-thread
			runnable.run();
		} else {
			final Handler mainHandler = new Handler(Looper.getMainLooper());
			mainHandler.post(runnable);
		}
	}

	private static class PushDelegate implements MRGSPushNotificationHandler.MRGSPushNotificationDelegate {
		private PushDelegate() {
		}

		private static PushDelegate mInstance;

		synchronized static PushDelegate instance() {
			if (mInstance == null) {
				mInstance = new PushDelegate();
			}
			return mInstance;
		}

		@Override
		public void clickOnNotification(final int notificationId, final String title, final String message, final boolean isLocal) {
			Log.v(LOG_TAG, String.format("PushDelegate.clickOnNotification(%d, '%s', '%s', %s)", notificationId, title, message, (isLocal ? "local" : "server")));
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onClickOnNotification(notificationId, title, message, isLocal);
				}
			});
		}

		@Override
		public void receivedNotification(final int notificationId, final String title, final String message, final boolean isLocal) {
			Log.v(LOG_TAG, String.format("PushDelegate.receivedNotification(%d, '%s', '%s', %s)", notificationId, title, message, (isLocal ? "local" : "server")));
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onReceivedNotification(notificationId, title, message, isLocal);
				}
			});
		}
	}

	/**
	 * Выполняет callback'и в необходимом потоке. Например, в случае Cocos2d-x это должен быть GL-thread.
	 */
	public interface ThreadHelper {
		void runOnNecessaryThread(final Runnable callback);
	}

	private static class SupportTicketListener implements MRGSMyComSupport.TicketListener {
		private SupportTicketListener() {
		}

		private static SupportTicketListener mInstance;

		synchronized static SupportTicketListener instance() {
			if (mInstance == null) {
				mInstance = new SupportTicketListener();
			}
			return mInstance;
		}

		@Override
		public void onTicketResponse(final boolean hasNotifications) {
			// String message = (hasNotifications ? "Есть новые ответы по тикетам" : "Нет новых ответов на тикеты");
			// Toast.makeText(GameActivity.Get(), message, Toast.LENGTH_LONG).show();
		}
	}
}
