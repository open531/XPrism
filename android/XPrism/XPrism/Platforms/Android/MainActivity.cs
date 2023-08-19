using Android.App;
using Android.Content;
using Android.Content.PM;
using Android.OS;
using Android.Service.Notification;

namespace XPrism;

[Activity(Theme = "@style/Maui.SplashTheme", MainLauncher = true, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation | ConfigChanges.UiMode | ConfigChanges.ScreenLayout | ConfigChanges.SmallestScreenSize | ConfigChanges.Density)]
public class MainActivity : MauiAppCompatActivity
{
    static public NotificationListener listener { get; private set; }
    protected override void OnCreate(Bundle savedInstanceState)
    {
        base.OnCreate(savedInstanceState);

        startNotificationListener();
    }
    public void startNotificationListener()
    {
        // Register the notification listener service
        listener = new NotificationListener();
        var intent = new Intent(NotificationListenerService.ServiceInterface);
        intent.SetComponent(new ComponentName(this, Java.Lang.Class.FromType(listener.GetType())));
        StartService(intent);
    }
    public void startMediaController()
    {
        if (Build.VERSION.SdkInt >= BuildVersionCodes.O)
        {
            //8.0以上系统启动为前台服务, 否则在后台, 测试中发现过几分钟后MediaController监听不到音乐信息
            StartForegroundService(new Intent(this, MediaController.Class));
        }
        else
        {
            StartService(new Intent(this, MediaController.Class));
        }
    }
}
