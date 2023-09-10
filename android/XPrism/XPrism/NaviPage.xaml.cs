using CommunityToolkit.Maui.Core;
using CommunityToolkit.Maui.Alerts;
using System.Text.Json;
using Android.Util;

namespace XPrism;

public partial class NaviPage : ContentPage
{
    public string nominatimUrl = "https://nominatim.openstreetmap.org/search";
    public string graphhopperUrl = "https://graphhopper.com/api/1/route";
    public string addr = "";
    public string profile = "";
    public string latlon1 = "";
    public string latlon2 = "";
    public string apikey = Secrets.GraphHopperAPI; // Get your own API key from https://graphhopper.com/
    public NaviPage()
    {
        InitializeComponent();
    }
    public async void OnSearchClicked(object sender, EventArgs e)
    {
        addr = addrEntry.Text;
        if (addr != "")
        {
            HttpClient client = new HttpClient();
            var response = await client.GetAsync($"{nominatimUrl}?q={addr}&format=json&limit=1");
            if (response.IsSuccessStatusCode)
            {
                var content = await response.Content.ReadAsStringAsync();
                var nominatim = JsonSerializer.Deserialize<List<Nominatim>>(content);
                if (nominatim.Count == 0)
                {
                    await DisplayAlert("错误", "地址错误或网络错误", "确定");
                    return;
                }
                disLabel.Text = nominatim[0].display_name;
                latLabel.Text = nominatim[0].lat;
                lonLabel.Text = nominatim[0].lon;
                var toast = Toast.Make("查询成功", ToastDuration.Short, 14);
                await toast.Show();
            }
            else
            {
                await DisplayAlert("错误", "地址错误或网络错误", "确定");
                return;
            }
        }
        else
        {
            await DisplayAlert("错误", "地址不能为空", "确定");
            return;
        }
    }
    public async void OnNaviClicked(object sender, EventArgs e)
    {
        latlon1 = srcLabel.Text;
        latlon2 = dstLabel.Text;
        if (modePicker.SelectedIndex == -1)
        {
            await DisplayAlert("错误", "请选择出行方式", "确定");
            return;
        }
        profile = modePicker.SelectedItem.ToString();
        if (latlon1 != "" && latlon2 != "")
        {
            HttpClient client = new HttpClient();
            var response = await client.GetAsync($"{graphhopperUrl}?profile={profile}&locale=zh_CN&instructions=true&points_encoded=true&point={latlon1}&point={latlon2}&key={apikey}");
            if (response.IsSuccessStatusCode)
            {
                var content = await response.Content.ReadAsStringAsync();
                var graphhopper = JsonSerializer.Deserialize<Graphhopper>(content);
                if (graphhopper.paths.Count == 0)
                {
                    await DisplayAlert("错误", "经纬度错误或网络错误", "确定");
                    return;
                }
                var toast = Toast.Make("查询成功", ToastDuration.Short, 14);
                await toast.Show();
                var path = graphhopper.paths[0];
                var points = path.points;
                var instructions = path.instructions;
                //System.Text.StringBuilder sb = new System.Text.StringBuilder();
                //sb.Append("距离：");
                //sb.Append(path.distance.ToString());
                //sb.Append("米\n");
                //sb.Append("时间：");
                //sb.Append(path.time.ToString());
                //sb.Append("秒\n");
                //sb.Append("指令：\n");
                //foreach (var instruction in instructions)
                //{
                //    sb.Append(instruction.text);
                //    sb.Append("\n");
                //}
                //sb.Append("坐标：\n");
                //sb.Append(points);
                //naviLabel.Text = sb.ToString();
                string naviUrl = $"&instructionText=";
                foreach (var instruction in instructions)
                {
                    naviUrl += $"{instruction.text}";
                    naviUrl += $"|";
                }
                naviUrl += $"&instructionSign=";
                foreach (var instruction in instructions)
                {
                    naviUrl += $"{instruction.sign}";
                    naviUrl += $"|";
                }
                naviUrl += $"&instructionIntervalBegin=";
                foreach (var instruction in instructions)
                {
                    naviUrl += $"{instruction.interval[0]}";
                    naviUrl += $"|";
                }
                naviUrl += $"&instructionIntervalEnd=";
                foreach (var instruction in instructions)
                {
                    naviUrl += $"{instruction.interval[1]}";
                    naviUrl += $"|";
                }
                //naviLabel.Text = $"http://{ESP32Info.picoIPAddress}/navi?pointEncoded={points}&instructionLength={instructions.Count}{naviUrl}";
                await Clipboard.Default.SetTextAsync($"http://{ESP32Info.picoIPAddress}/navi?pointEncoded={points}&instructionLength={instructions.Count}{naviUrl}");
                //naviWebView.Source = $"http://{ESP32Info.picoIPAddress}/navi?pointEncoded={points}&instructionLength={instructions.Count}{naviUrl}";
                HttpClient client2 = new HttpClient();
                var response2 = await client2.GetAsync($"http://{ESP32Info.picoIPAddress}/navi?pointEncoded={points}&instructionLength={instructions.Count}{naviUrl}");
                if(response2.IsSuccessStatusCode)
                {
                    var content2 = await response2.Content.ReadAsStringAsync();
                    naviLabel.Text = content2;
                }
                else
                {
                    await DisplayAlert("错误", "导航网页加载失败", "确定");
                    return;
                }
            }
            else
            {
                await DisplayAlert("错误", "经纬度错误或网络错误", "确定");
                return;
            }
        }
        else
        {
            await DisplayAlert("错误", "经纬度不能为空", "确定");
            return;
        }
    }

    public async void OnGetGPSClicked(object sender, EventArgs e)
    {
        CancellationTokenSource _cancelTokenSource;
        bool _isCheckingLocation;
        try
        {
            _cancelTokenSource = new CancellationTokenSource();
            _isCheckingLocation = true;
            GeolocationRequest request = new GeolocationRequest(GeolocationAccuracy.Best, TimeSpan.FromSeconds(10));
            Location location = await Geolocation.Default.GetLocationAsync(request, _cancelTokenSource.Token);
            if (location != null)
            {
                var toast = Toast.Make("查询成功", ToastDuration.Short, 14);
                await toast.Show();
                srcLabel.Text = location.Latitude.ToString() + "," + location.Longitude.ToString();
            }
        }
        catch (FeatureNotSupportedException fnsEx)
        {
            await DisplayAlert("错误", "设备不支持定位", "确定");
            return;
        }
        catch (FeatureNotEnabledException fneEx)
        {
            await DisplayAlert("错误", "定位未启用", "确定");
            return;
        }
        catch (PermissionException pEx)
        {
            await DisplayAlert("错误", "定位权限未授予", "确定");
            return;
        }
        catch (Exception ex)
        {
            await DisplayAlert("错误", "定位失败", "确定");
            return;
        }
        finally
        {
            _isCheckingLocation = false;
        }
    }

    public async void OnCopyClicked(object sender, EventArgs e)
    {
        if (latLabel.Text != "" && lonLabel.Text != "")
        {
            dstLabel.Text = latLabel.Text + "," + lonLabel.Text;
        }
        else
        {
            await DisplayAlert("错误", "经纬度不能为空", "确定");
        }
    }
    public class Nominatim
    {
        public string lat { get; set; }
        public string lon { get; set; }
        public string display_name { get; set; }
    }
    public class Graphhopper
    {
        public List<Path> paths { get; set; }
    }
    public class Path
    {
        public double distance { get; set; }
        public double weight { get; set; }
        public long time { get; set; }
        public string points { get; set; }
        public List<Instruction> instructions { get; set; }
    }
    public class Point
    {
        public List<List<int>> coordinates { get; set; }
    }
    public class Instruction
    {
        public double distance { get; set; }
        public double heading { get; set; }
        public int sign { get; set; }
        public List<int> interval { get; set; }
        public string text { get; set; }
        public int time { get; set; }
        public string street_name { get; set; }
    }
}