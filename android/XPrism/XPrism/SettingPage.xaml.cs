using System.Net.NetworkInformation;

namespace XPrism;

public partial class SettingPage : ContentPage
{
    public DateTime currentTime;

    public SettingPage()
    {
        InitializeComponent();
        picoIPEntry.Text = ESP32Info.picoIPAddress;
        camIPEntry.Text = ESP32Info.camIPAddress;
        GetIP();
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
    public void GetIP()
    {
        Boolean hasAP = false;
        String APIP = "";
        NetworkInterface[] nis = NetworkInterface.GetAllNetworkInterfaces();
        foreach (NetworkInterface ni in nis)
        {
            //IPInterfaceProperties ipps = ni.GetIPProperties();
            //UnicastIPAddressInformationCollection ipcs = ipps.UnicastAddresses;
            //foreach (UnicastIPAddressInformation ipc in ipcs)
            //{
            //    Console.WriteLine("IP: " + ipc.Address.ToString() + ", Interface: " + ni.Name);
            //}
            if (ni.Name == "ap0")
            {
                hasAP = true;
                IPInterfaceProperties ipps = ni.GetIPProperties();
                UnicastIPAddressInformationCollection ipcs = ipps.UnicastAddresses;
                foreach (UnicastIPAddressInformation ipc in ipcs)
                {
                    if (ipc.Address.ToString().IndexOf('.') != -1)
                    {
                        APIP = ipc.Address.ToString();
                        break;
                    }
                }
                break;
            }
        }
        if (hasAP)
        {
            suggestLabel.Text = "您似乎已打开热点😊\n热点的IP地址为：\n" + APIP + "\n供您参考。";
        }
        else
        {
            suggestLabel.Text = "您似乎未打开热点🤔\n建议您打开热点以便于连接。";
        }
    }
}