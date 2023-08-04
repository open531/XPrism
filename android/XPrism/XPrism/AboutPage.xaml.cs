using Microsoft.Maui.Devices;
using System.Windows.Input;
using CommunityToolkit.Maui.Core;
using CommunityToolkit.Maui.Alerts;

namespace XPrism;

public partial class AboutPage : ContentPage
{
    public ICommand TapCommand => new Command<string>(async (url) => await Launcher.OpenAsync(url));

    public AboutPage()
    {
        InitializeComponent();
        BindingContext = this;
        ReadDeviceInfo();
    }

    public async void OnRefreshClicked(object sender, EventArgs e)
    {
        ReadDeviceInfo();
        var toast = Toast.Make("刷新成功", ToastDuration.Short, 14);
        await toast.Show();
    }

    public void ReadDeviceInfo()
    {
        System.Text.StringBuilder sb = new System.Text.StringBuilder();

        sb.AppendLine($"设备型号：{DeviceInfo.Current.Model}");
        sb.AppendLine($"制造厂商：{DeviceInfo.Current.Manufacturer}");
        sb.AppendLine($"设备名称：{DeviceInfo.Current.Name}");
        sb.AppendLine($"设备平台：{DeviceInfo.Current.Platform}");
        sb.AppendLine($"系统版本：{DeviceInfo.Current.VersionString}");
        sb.AppendLine($"设备类型：{DeviceInfo.Current.Idiom}");

        bool isVirtual = DeviceInfo.Current.DeviceType switch
        {
            DeviceType.Physical => false,
            DeviceType.Virtual => true,
            _ => false
        };

        sb.AppendLine($"虚拟设备：{isVirtual}");

        deviceInfo.Text = sb.ToString();
    }
}