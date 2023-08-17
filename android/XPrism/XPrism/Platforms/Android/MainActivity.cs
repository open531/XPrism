using Android.App;
using Android.Content;
using Android.Content.PM;
using Android.OS;
using Android.Service.Notification;

namespace XPrism;

[Activity(Theme = "@style/Maui.SplashTheme", MainLauncher = true, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation | ConfigChanges.UiMode | ConfigChanges.ScreenLayout | ConfigChanges.SmallestScreenSize | ConfigChanges.Density)]
public class MainActivity : MauiAppCompatActivity
{
    static public NotificationListener listener { get; private set;}
    protected override void OnCreate(Bundle savedInstanceState)
    {
        base.OnCreate(savedInstanceState);

        // Register the notification listener service
        listener = new NotificationListener();
        var intent = new Intent(NotificationListenerService.ServiceInterface);
        intent.SetComponent(new ComponentName(this, Java.Lang.Class.FromType(listener.GetType())));
        StartService(intent);
    }
}
