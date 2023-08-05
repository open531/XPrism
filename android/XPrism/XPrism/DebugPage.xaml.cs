namespace XPrism;

public partial class DebugPage : ContentPage
{

    public DebugPage()
    {
        InitializeComponent();
    }
    public void OnRefreshClicked(object sender, EventArgs e)
    {
        Console.WriteLine("OnRefreshClicked");
        notinum.Text = MainActivity.serviceConnection?.Binder?.GetActiveNotifications()?.Length.ToString();
    }
}