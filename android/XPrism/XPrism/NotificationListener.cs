using Android.App;
using Android.Service.Notification;
using Android.Util;

namespace XPrism
{
    [Service(Name = "io.panxuc.xprism.NotificationListener", Label = "XPrismNotificationListener", Permission = "android.permission.BIND_NOTIFICATION_LISTENER_SERVICE")]
    public class NotificationListener : NotificationListenerService
    {
        public override void OnNotificationPosted(StatusBarNotification sbn)
        {
            base.OnNotificationPosted(sbn);

            // Get the notification object from the status bar notification
            var notification = sbn.Notification;

            // Get the notification title, text and time
            var title = notification.Extras.GetString(Notification.ExtraTitle);
            var text = notification.Extras.GetString(Notification.ExtraText);
            var time = notification.When;

            // Log the notification information for debugging
            Log.Debug("NotificationListener", $"Title: {title}, Text: {text}, Time: {time}");

            // TODO: Send the notification information to ESP32 via Bluetooth or Wi-Fi
        }

        public override void OnNotificationRemoved(StatusBarNotification sbn)
        {
            base.OnNotificationRemoved(sbn);

            // Get the notification object from the status bar notification
            var notification = sbn.Notification;

            // Get the notification title, text and time
            var title = notification.Extras.GetString(Notification.ExtraTitle);
            var text = notification.Extras.GetString(Notification.ExtraText);
            var time = notification.When;

            // Log the notification information for debugging
            Log.Debug("NotificationListener", $"Removed: Title: {title}, Text: {text}, Time: {time}");

            // TODO: Handle the notification removal event on ESP32
        }
    }
}