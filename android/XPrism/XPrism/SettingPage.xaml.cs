namespace XPrism;

public partial class SettingPage : ContentPage
{
	public DateTime currentTime;

	public SettingPage()
	{
		InitializeComponent();
		RefreshTime();
	}

	public void OnTimeRefreshClicked(object sender, System.EventArgs e)
	{
		RefreshTime();
	}

	public void OnTimeSetClicked(object sender, System.EventArgs e)
	{
        currentTime = new DateTime(datePicker.Date.Year, datePicker.Date.Month, datePicker.Date.Day, timePicker.Time.Hours, timePicker.Time.Minutes, timePicker.Time.Seconds);
    }

	public void RefreshTime()
	{
		currentTime = DateTime.Now;
		timePicker.Time = currentTime.TimeOfDay;
		datePicker.Date = currentTime.Date;
	}

    private void datePicker_Focused(object sender, FocusEventArgs e)
    {

    }
}