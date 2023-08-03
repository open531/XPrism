namespace XPrism;

public partial class MainPage : ContentPage
{

	public MainPage()
	{
		InitializeComponent();
	}

	public async void OnSendClicked(object sender, System.EventArgs e)
	{
        // 打开发送页面
        await Navigation.PushAsync(new SendPage());
    }

	public async void OnAboutClicked(object sender, System.EventArgs e)
	{
		// 打开关于页面
		await Navigation.PushAsync(new AboutPage());
	}
}

