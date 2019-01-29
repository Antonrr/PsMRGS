package ru.mail.mrgservice;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.util.Pair;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import ru.mail.mrgservice.*;
import com.epicgames.*;
import com.epicgames.ue4.GameActivity;
import android.widget.Toast;
import ru.mail.mrgservice.MRGSMyComSupportDialog;
import com.my.target.nativeads.banners.NativeAppwallBanner;
import ru.mail.mrgservice.MRGSMetrics;

/**
 * Вспомагательный класс для взаимодействия Java кода с C++ кодом
 */
@SuppressWarnings("unused")
public class MRGServiceCpp {
	private final static String LOG_TAG = "[JAVA] MRGServiceCpp.log";
	//support key Jhfd763NMqweYUkGD

	private final static int ADMAN_SHOWCASE = 1;
	private final static int ADMAN_FULLSCREEN_BANNER = 2;
	private final static int ADMAN_INTERSTITIAL = 3;

	public static Context appContext;

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


	private final static MRGSBillingDelegate mBillingDelegate = new MRGSBillingDelegate() {
		@Override
		public void loadProductsDidFinished(final MRGSBilling mrgsBilling, final ArrayList<MRGSPurchaseItem> mrgsPurchaseItems) {
			Log.v(LOG_TAG, String.format("MRGSBillingDelegate.loadProductsDidFinished(%s, items.count = %d)", MRGSBilling.getBillingName(), mrgsPurchaseItems.size()));
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onLoadProductsDidFinished(mrgsPurchaseItems);
				}
			});
		}

		@Override
		public void purchaseComplete(final MRGSBilling mrgsBilling, final MRGSPurchaseItem item, final String answer) {
			Log.v(LOG_TAG, String.format("MRGSBillingDelegate.purchaseComplete(%s, %s, %s)", MRGSBilling.getBillingName(), item, answer));
			if (item.resultCode != 1) // BILLING_RESPONSE_RESULT_USER_CANCELED = 1
			{
				threadHelper.runOnNecessaryThread(new Runnable() {
					@Override
					public void run() {
						onPurchaseComplete(item.sku, item.transactionId, answer);
					}
				});
			}
			else
			{
				threadHelper.runOnNecessaryThread(new Runnable() {
					@Override
					public void run() {
						onPurchaseCancel(item.sku, item.transactionId, answer);
					}
				});
			}
		}

		@Override
		public void purchaseFail(final MRGSBilling mrgsBilling, final MRGSPurchaseItem item, final String answer) {
			Log.v(LOG_TAG, String.format("MRGSBillingDelegate.purchaseFail(%s, %s, %s)", MRGSBilling.getBillingName(), item, answer));
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onPurchaseFail((item != null ? item.sku : null), answer);
				}
			});
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
					Log.v(LOG_TAG, "catch error");	
					Log.e(LOG_TAG, "showSupportStart error", e);
    				e.printStackTrace();
				}
			}
		});
	}

	public static void addMetric(final String metricd) {
		MRGSMetrics.addMetric(Integer.parseInt(metricd));
	}

	public static void initWithAppidAndSecret(final String appId, final String appSecret) {

		Log.v(LOG_TAG, String.format("MRGServiceCPP:init (%s, %s)", appId, appSecret));

		GameActivity activity = GameActivity.Get();
		activity.runOnUiThread(new Runnable() 
		{
			@Override
			public void run() 
			{
				Log.v(LOG_TAG, String.format("MRGServiceCPP:init context (%s)", appContext.getPackageCodePath()));
				init(appContext, appId, appSecret);
			}
		});
	}

	public static void initUser(final String userId) {
		Log.v(LOG_TAG, String.format("MRGServiceCPP:initWithUserId started"));
		MRGSList users = MRGSUsers.instance().getAllUsers();
		int count = users.size();
		if (count > 0) {
			for (int i = 0; i < count; i++) {
				String tempUserId = ((MRGSMap) users.get(i)).objectForKey("userId").toString();
				MRGSLog.vp("userID = " + tempUserId);
				if(tempUserId.equals(userId)) {
					boolean bResult = MRGSUsers.instance().authorizationUserWithId(userId);
					if (bResult == false) {
						Log.v(LOG_TAG, String.format("MRGServiceCPP:initWithUserId found user but couldn't authorize %s", userId));
						onUserAuthError();
					}
					else {
						Log.v(LOG_TAG, String.format("MRGServiceCPP:initWithUserId found user and authorized %s", userId));
						onUserAuthSuccess();
					}
					return;
				}
			}
		}

		String RegistredUserId = MRGSUsers.instance().registerNewUser(userId);
		if (!RegistredUserId.equals(userId)) {
			Log.v(LOG_TAG, String.format("MRGServiceCPP:initWithUserId couldn't register new user %s. Result = %s", userId, RegistredUserId));
		}

		boolean bResult = MRGSUsers.instance().authorizationUserWithId(userId);
		if (bResult == false) {
			Log.v(LOG_TAG, String.format("MRGServiceCPP:initWithUserId couldn't authorize new user %s", userId));
			onUserAuthError();
			return;
		}

		Log.v(LOG_TAG, String.format("MRGServiceCPP:initWithUserId registered and authorized new user %s", userId));
		onUserAuthSuccess();
	}

	public static void init(final Context context,final String appId, final String appSecret) {
		Log.v(LOG_TAG, String.format("init MRGS started"));

		MRGService.service(context, mServerDataDelegate, appId, appSecret);
		MRGSMyComSupport.setTicketListener(SupportTicketListener.instance());
		MRGSBilling.instance().setDelegate(mBillingDelegate);

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
		MRGService.instance().initPush(PushDelegate.instance());
	}

	public static void initLocalPush() {
		MRGSLocalPushService.setDelegate(PushDelegate.instance());
	}

	public static void registerUserWithId(final String userId) {
		
	}

	public static void authorizeUserWithId(final String userId) {
		MRGSUsers.instance().authorizationUserWithId(userId);
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

	public static void addSamsungProductPrice(final String groupId, final String itemId, final double priceInUSD) {
		Log.v(LOG_TAG, String.format("addSamsungProductPrice(%s, %s, %f)", groupId, itemId, priceInUSD));
		MRGSSamsungPrice.instance().addProductPrice(groupId, itemId, priceInUSD);
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

	public static void admanLoadData(final String type) {
		Log.v(LOG_TAG, String.format("admanLoadData(%s)", type));
		final int tempType = Integer.parseInt(type);
		threadHelper.runOnNecessaryThread(new Runnable() {
			@Override
			public void run() {
				switch (tempType) {
					case ADMAN_SHOWCASE:
						getAdvertising().loadShowcase(AdmanDelegate.instance());
						break;
					case ADMAN_FULLSCREEN_BANNER:
						getAdvertising().loadFullscreen(AdmanDelegate.instance());
						break;
					case ADMAN_INTERSTITIAL:
						getAdvertising().loadInterstitial(AdmanDelegate.instance());
					break;
				}
			}
		});
	}

	public static void admanOpen(final int type) {
		Log.v(LOG_TAG, String.format("admanOpen(%d)", type));
		threadHelper.runOnNecessaryThread(new Runnable() {
			@Override
			public void run() {
				switch (type) {
					case ADMAN_SHOWCASE:
						getAdvertising().openShowcase();
						break;
					case ADMAN_FULLSCREEN_BANNER:
						getAdvertising().openFullscreen();
						break;
					case ADMAN_INTERSTITIAL:
						getAdvertising().openInterstitial();
						break;
				}
			}
		});
	}

	public static void admanClose(final int type) {
		Log.v(LOG_TAG, String.format("admanClose(%d)", type));
		threadHelper.runOnNecessaryThread(new Runnable() {
			@Override
			public void run() {
				switch (type) {
					case ADMAN_SHOWCASE:
						getAdvertising().closeShowcase();
						break;
					case ADMAN_FULLSCREEN_BANNER:
						getAdvertising().closeFullscreen();
						break;
					case ADMAN_INTERSTITIAL:
						getAdvertising().closeInterstitial();
						break;
				}
			}
		});
	}

	public static void admanRelease(final int type) {
		Log.v(LOG_TAG, String.format("admanRelease(%d)", type));
		threadHelper.runOnNecessaryThread(new Runnable() {
			@Override
			public void run() {
				switch (type) {
					case ADMAN_SHOWCASE:
						getAdvertising().releaseShowcase();
						break;
					case ADMAN_FULLSCREEN_BANNER:
						getAdvertising().releaseFullscreen();
						break;

					case ADMAN_INTERSTITIAL:
						break;
						
				}
			}
		});
	}

	public static void admanSetTitle(final String title) {
		Log.v(LOG_TAG, "admanSetTitle()");
		threadHelper.runOnNecessaryThread(new Runnable() {
			@Override
			public void run() {
				getAdvertising().setShowcaseTitle(title);
			}
		});
	}

	/************************************************/
	/*               Flurry Analytics               */
	/************************************************/
	public static void flurryEvent(final String event) {
		Log.v(LOG_TAG, String.format("flurryEvent('%s')", event));
		Log.v(LOG_TAG, String.format("flurryEvent start"));
		MRGService.instance().flurryEvent(event);
		Log.v(LOG_TAG, String.format("flurryEvent start"));
	}

	public static void flurryEvent(final String event, final Map<String, String> params) {
		Log.v(LOG_TAG, String.format("flurryEvent('%s', %s)", event, params));
		MRGService.instance().flurryEvent(event, params);
	}

	public static void flurryEventStart(final String event, final Map<String, String> params) {
		Log.v(LOG_TAG, String.format("flurryEventStart('%s', %s)", event, params));
		MRGService.instance().flurryEventStart(event, params);
	}

	public static void flurryEventStop(final String event, final Map<String, String> params) {
		Log.v(LOG_TAG, String.format("flurryEventStop('%s', %s)", event, params));
		MRGService.instance().flurryEventStop(event, params);
	}
	/************************************************/


	/************************************************/
	/*               Google Analytics               */
	/************************************************/
	public static void sendGAEvent(final String category, final String action, final String label) {
		Log.v(LOG_TAG, String.format("sendGAEvent('%s', '%s', '%s')", category, action, label));
		MRGService.instance().sendGAEvent(category, action, label, 1L);
	}

	public static void sendGAScreen(final String screenName) {
		Log.v(LOG_TAG, String.format("sendGAScreen('%s')", screenName));
		MRGService.instance().sendGAScreen(screenName);
	}

	public static void sendGASocEvent(final String network, final String action, final String target) {
		Log.v(LOG_TAG, String.format("sendGASocEvent('%s', '%s', '%s')", network, action, target));
		MRGService.instance().sendGASocEvent(network, action, target);
	}

	public static void sendGATimings(final String category, final long interval, final String name, final String label) {
		Log.v(LOG_TAG, String.format("sendGATimings('%s', %d, '%s', '%s'", category, interval, name, label));
		MRGService.instance().sendGATimings(category, interval, name, label);
	}
	/************************************************/


	/*************************************************/
	/*                   AppsFlyer                   */
	/*************************************************/
	public static void sendAFEvent(final String eventName, final String value) {
		Log.v(LOG_TAG, String.format("sendAFEvent('%s', '%s')", eventName, value));
		MRGService.instance().sendAFEvent(eventName, value);
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
			MRGService.markUserAsCheater(want, have);
		} else {
			Log.v(LOG_TAG, String.format("markUserAsCheater(%d, %d, '%s')", want, have, comment));
			MRGService.markUserAsCheater(want, have, comment);
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

	private static MRGSAdvertising getAdvertising() {
		MRGSAdvertising advertising = MRGSAdvertising.getLastInstance();
		if (advertising == null) {
			advertising = MRGSAdvertising.createInstance(MRGService.instance().getCurrentActivity());
		}
		return advertising;
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

	private static class AdmanDelegate implements MRGSAdvertising.LoadDelegate {
		private AdmanDelegate() {
		}

		private static AdmanDelegate mInstance;

		synchronized static AdmanDelegate instance() {
			if (mInstance == null) {
				mInstance = new AdmanDelegate();
			}
			return mInstance;
		}

		public void onVideoComplete(final MRGSAdvertising.AdvertisingType type) {
			// Log.d("ADMAN++", String.format("onVideoComplete(%s)", type));
		}

		/**
		 * Метод протокола, срабатывает при получении данных о баннерах для Витрины
		 * @param notification Если True, то нужно показать бабл на кнопке
		 */
		public void onLoadComplete(final MRGSAdvertising.AdvertisingType advertisingType, final boolean notification, final List<NativeAppwallBanner> BannersList) {
			Log.v(LOG_TAG, String.format("AdmanDelegate.onLoadComplete(%s, %s)", advertisingType, notification));
			final int type;
			switch (advertisingType) {
				case SHOWCASE:
				default:
					type = ADMAN_SHOWCASE;
					break;
				case FULLSCREEN:
					type = ADMAN_FULLSCREEN_BANNER;
					break;
				case INTERSTITIAL :
					type = ADMAN_INTERSTITIAL;
					break;
			}
			threadHelper.runOnNecessaryThread(new Runnable() {
				@Override
				public void run() {
					onAdmanLoadComplete(type, notification);
				}
			});
		}

		public void onViewComplete(final MRGSAdvertising.AdvertisingType advertisingType) {
			Log.v(LOG_TAG, String.format("AdmanDelegate.onViewComplete(%s)", advertisingType));
			final int type;
			switch (advertisingType) {
				case SHOWCASE:
				default:
					type = ADMAN_SHOWCASE;
					break;
				case FULLSCREEN:
					type = ADMAN_FULLSCREEN_BANNER;
					break;

				case INTERSTITIAL:
					type = ADMAN_INTERSTITIAL;
					break;
			}
			onAdmanViewComplete(type);
		}

		public void onNoAd(final MRGSAdvertising.AdvertisingType advertisingType) {
			final int type;
			switch (advertisingType) {
				case SHOWCASE:
				default:
					type = ADMAN_SHOWCASE;
					break;
				case FULLSCREEN:
					type = ADMAN_FULLSCREEN_BANNER;
					break;

				case INTERSTITIAL:
					type = ADMAN_INTERSTITIAL;
					break;
			}

			onAdmanHasNoAdd(type);
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
