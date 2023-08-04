#define OPENWEATHERMAP // OPENWEATHERMAP or ACCUWEATHER

using Android.App;
using CommunityToolkit.Maui.Core;
using CommunityToolkit.Maui.Alerts;
using System.Text.Json;

namespace XPrism;

#if OPENWEATHERMAP
public partial class WeatherPage : ContentPage
{
    public string url = "http://api.openweathermap.org/data/2.5/weather";
    public string city = "";
    public string apiKey = Secrets.OpenWeatherAPI; // Get your own API key from https://openweathermap.org/api
    public string units = "metric";
    public string lang = "zh_cn";
    public string fullUrl = "";

    public WeatherPage()
    {
        InitializeComponent();
        cityLabel.Text = "N/A";
        weatherLabel.Text = "N/A";
        temperatureLabel.Text = "N/A";
        windDirectionLabel.Text = "N/A";
        windSpeedLabel.Text = "N/A";
        humidityLabel.Text = "N/A";
        reportTimeLabel.Text = "N/A";
        sourceLabel.Text = "OpenWeatherMap";
    }

    public async void OnSearchClicked(object sender, System.EventArgs e)
    {
        city = cityEntry.Text;
        if (city != "")
        {
            fullUrl = $"{url}?q={city}&appid={apiKey}&units={units}&lang={lang}";
            HttpClient client = new HttpClient();
            var response = await client.GetAsync(fullUrl);
            if (response.IsSuccessStatusCode)
            {
                var content = await response.Content.ReadAsStringAsync();
                var weather = JsonSerializer.Deserialize<Weather>(content);
                if (weather.cod != 200)
                {
                    await DisplayAlert("错误", "城市名称错误或网络错误", "确定");
                    return;
                }
                cityLabel.Text = weather.name;
                weatherLabel.Text = weather.weather[0].description;
                temperatureLabel.Text = weather.main.temp.ToString() + "℃";
                windDirectionLabel.Text = weather.wind.deg.ToString() + "°";
                windSpeedLabel.Text = weather.wind.speed.ToString() + "m/s";
                humidityLabel.Text = weather.main.humidity.ToString() + "%";
                reportTimeLabel.Text = UnixTimeStampToDateTime(weather.dt).ToString();
                var toast = Toast.Make("查询成功", ToastDuration.Short, 14);
                await toast.Show();
            }
            else
            {
                await DisplayAlert("错误", "城市名称错误或网络错误", "确定");
                return;
            }
        }
    }
    public static DateTime UnixTimeStampToDateTime(double unixTimeStamp)
    {
        // Unix timestamp is seconds past epoch
        DateTime dateTime = new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Utc);
        dateTime = dateTime.AddSeconds(unixTimeStamp).ToLocalTime();
        return dateTime;
    }
}
class Weather
{
    public int cod { get; set; }
    public WeatherMain main { get; set; }
    public WeatherWind wind { get; set; }
    public WeatherWeather[] weather { get; set; }
    public string name { get; set; }
    public int dt { get; set; }
}
class WeatherMain
{
    public float temp { get; set; }
    public int humidity { get; set; }
}
class WeatherWind
{
    public float speed { get; set; }
    public float deg { get; set; }
}
class WeatherWeather
{
    public string description { get; set; }
}
#endif
#if ACCUWEATHER
public partial class WeatherPage : ContentPage
{
    public string url = "http://dataservice.accuweather.com/currentconditions/v1/";
    public string locationKey = "";
    public string apiKey = Secrets.AccuWeatherAPI;
    public string language = "zh-CN";
    public string details = "true";
    public string fullUrl = "";

    public WeatherPage()
    {
        InitializeComponent();
        cityLabel.Text = "N/A";
        weatherLabel.Text = "N/A";
        temperatureLabel.Text = "N/A";
        windDirectionLabel.Text = "N/A";
        windSpeedLabel.Text = "N/A";
        humidityLabel.Text = "N/A";
        reportTimeLabel.Text = "N/A";
    }

    public async void OnSearchClicked(object sender, System.EventArgs e)
    {
        switch (picker.SelectedIndex)
        {
            case 0:
                locationKey = "101924";
                break;
            case 1:
                locationKey = "106577";
                break;
            case 2:
                locationKey = "58194";
                break;
            case 3:
                locationKey = "102255";
                break;
            case 4:
                locationKey = "106832";
                break;
            case 5:
                locationKey = "106774";
                break;
            case 6:
                locationKey = "105570";
                break;
            case 7:
                locationKey = "105571";
                break;
            case 8:
                locationKey = "102144";
                break;
            case 9:
                locationKey = "103847";
                break;
            default:
                locationKey = "";
                fullUrl = "";
                break;
        }
        if (locationKey != "")
        {
            fullUrl = $"{url}{locationKey}?apikey={apiKey}&language={language}&details={details}";
            HttpClient client = new HttpClient();
            var response = await client.GetAsync(fullUrl);
            if (response.IsSuccessStatusCode)
            {
                var content = await response.Content.ReadAsStringAsync();
                if (content == "[]")
                {
                    cityLabel.Text = "N/A";
                    weatherLabel.Text = "N/A";
                    temperatureLabel.Text = "N/A";
                    windDirectionLabel.Text = "N/A";
                    windSpeedLabel.Text = "N/A";
                    humidityLabel.Text = "N/A";
                    reportTimeLabel.Text = "N/A";
                    return;
                }
                else
                {
                    var weather = JsonSerializer.Deserialize<Weather>(content);
                    cityLabel.Text = picker.Items[picker.SelectedIndex];
                    weatherLabel.Text = weather.WeatherText;
                    temperatureLabel.Text = weather.Temperature.Metric.Value.ToString() + "℃";
                    windDirectionLabel.Text = weather.Wind.Direction.Localized;
                    windSpeedLabel.Text = weather.Wind.Speed.Metric.Value.ToString() + "km/h";
                    humidityLabel.Text = weather.RelativeHumidity.ToString() + "%";
                    reportTimeLabel.Text = weather.LocalObservationDateTime.ToString();
                }
            }
        }
    }
}

public class Weather
{
    public DateTime LocalObservationDateTime { get; set; }
    public string WeatherText { get; set; }
    public Temperature_ Temperature { get; set; }
    public Wind_ Wind { get; set; }
    public int RelativeHumidity { get; set; }
    public class Metric_
    {
        public double Value { get; set; }
        public string Unit { get; set; }
    }
    public class Temperature_
    {
        public Metric_ Metric { get; set; }
    }
    public class Direction_
    {
        public string Localized { get; set; }
    }
    public class Speed_
    {
        public Metric_ Metric { get; set; }
    }
    public class Wind_
    {
        public Direction_ Direction { get; set; }
        public Speed_ Speed { get; set; }
    }

}
#endif