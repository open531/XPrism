namespace XPrism;

public partial class MainPage : ContentPage
{
    public MainPage()
    {
        InitializeComponent();
        Application.Current.UserAppTheme = AppTheme.Light;
    }
    public async void OnWeatherClicked(object sender, System.EventArgs e)
    {
        // 打开天气页面
        await Navigation.PushAsync(new WeatherPage());
    }
    public async void OnNaviClicked(object sender, System.EventArgs e)
    {
        // 打开导航页面
        await Navigation.PushAsync(new NaviPage());
    }
    public async void OnCamClicked(object sender, System.EventArgs e)
    {
        // 打开摄像头页面
        await Navigation.PushAsync(new CamPage());
    }
    public async void OnSerialClicked(object sender, System.EventArgs e)
    {
        // 打开发送页面
        await Navigation.PushAsync(new SerialPage());
    }
    public async void OnBluetoothClicked(object sender, System.EventArgs e)
    {
        // 打开蓝牙页面
        await Navigation.PushAsync(new BluetoothPage());
    }
    public async void OnSettingClicked(object sender, System.EventArgs e)
    {
        // 打开设置页面
        await Navigation.PushAsync(new SettingPage());
    }
    public async void OnAboutClicked(object sender, System.EventArgs e)
    {
        // 打开关于页面
        await Navigation.PushAsync(new AboutPage());
    }
    public async void OnDebugClicked(object sender, System.EventArgs e)
    {
        // 打开调试页面
        await Navigation.PushAsync(new DebugPage());
    }
}

