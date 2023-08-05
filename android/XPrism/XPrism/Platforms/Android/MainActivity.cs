using Android.App;
using Android.Content;
using Android.Content.PM;
using Android.OS;

namespace XPrism;

[Activity(Theme = "@style/Maui.SplashTheme", MainLauncher = true, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation | ConfigChanges.UiMode | ConfigChanges.ScreenLayout | ConfigChanges.SmallestScreenSize | ConfigChanges.Density)]
public class MainActivity : MauiAppCompatActivity
{
    public static NLServiceConnection? serviceConnection;
    protected override void OnCreate(Bundle? savedInstanceState)
    {
        base.OnCreate(savedInstanceState);
    }
    protected override void OnStart()
    {
        base.OnStart();
        if (serviceConnection == null)
        {
            serviceConnection = new NLServiceConnection(this);
        }
        Intent serviceToStart = new Intent(this, typeof(NLService));
        BindService(serviceToStart, serviceConnection, Bind.AutoCreate);
    }
}
