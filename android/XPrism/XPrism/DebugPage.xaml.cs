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
    }
}