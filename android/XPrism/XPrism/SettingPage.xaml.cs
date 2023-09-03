namespace XPrism;

public partial class SettingPage : ContentPage
{
    public DateTime currentTime;

    public SettingPage()
    {
        InitializeComponent();
        picoIPEntry.Text = ESP32Info.picoIPAddress;
        camIPEntry.Text = ESP32Info.camIPAddress;
    }
    public bool isValidIP(string ip)
    {
        if (ip == null || ip.Length < 7 || ip.Length > 15) return false;
        string[] arrOctets = ip.Split('.');
        if (arrOctets.Length != 4) return false;
        foreach (string octet in arrOctets)
        {
            if (!byte.TryParse(octet, out byte octetValue)) return false;
            if (octetValue < 0 || octetValue > 255) return false;
        }
        return true;
    }
    public void OnPicoIPClicked(object sender, EventArgs e)
    {
        if (isValidIP(picoIPEntry.Text))
        {
            ESP32Info.picoIPAddress = picoIPEntry.Text;
        }
        else
        {
            DisplayAlert("IP无效", "请输入有效的IP地址", "OK");
        }
    }
    public void OnCamIPClicked(object sender, EventArgs e)
    {
        if (isValidIP(camIPEntry.Text))
        {
            ESP32Info.camIPAddress = camIPEntry.Text;
        }
        else
        {
            DisplayAlert("IP无效", "请输入有效的IP地址", "OK");
        }
    }
}