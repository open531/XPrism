namespace XPrism;

public partial class SendPage : ContentPage
{
	public SendPage()
	{
		InitializeComponent();
	}

    public void OnEditorTextChanged(object sender, TextChangedEventArgs e)
    {
        //string oldText = e.OldTextValue;
        //string newText = e.NewTextValue;
        //string myText = editor.Text;
    }

    public void OnEditorCompleted(object sender, EventArgs e)
    {
        //string text = ((Editor)sender).Text;
    }
}