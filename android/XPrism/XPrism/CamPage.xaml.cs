namespace XPrism;

public partial class CamPage : ContentPage
{
	public CamPage()
	{
		InitializeComponent();
		camWebView.Source = $"http://{ESP32Info.camIPAddress}/";
	}
}