using Plugin.LocalNotification;

namespace XPrism;

public partial class MemoPage : ContentPage
{
	public MemoPage()
	{
		InitializeComponent();
	}
	public async void OnAddMemoClicked(object sender, EventArgs e)
	{
        if(titleEntry.Text == null)
		{
            await DisplayAlert("标题为空", "请输入备忘标题", "OK");
        }
        else
        {
            var request = new NotificationRequest
            {
                NotificationId = 114514,
                Title = "备忘："+titleEntry.Text,
                Description = contentEditor.Text,
                ReturningData = "Dummy data",
                BadgeNumber = 42,
            };

            await LocalNotificationCenter.Current.Show(request);
        }
    }
}