using Android.App;
using Android.Content;
using Android.OS;
using Android.Service.Notification;
using Android.Util;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XPrism
{
    [Service(Name = "io.panxuc.xprism.noti", Label = "XPrism.NotificationListener", Permission = "android.permission.BIND_NOTIFICATION_LISTENER_SERVICE", Exported = false)]
    [IntentFilter(new[] { "android.service.notification.NotificationListenerService" })]
    public class NLService : NotificationListenerService
    {
        static readonly string TAG = typeof(NLService).FullName;
        public IBinder Binder { get; set; }
        public override void OnCreate()
        {
            base.OnCreate();
        }
        public override void OnDestroy()
        {
            base.OnDestroy();
            Binder = null;
        }
        public override void OnNotificationPosted(StatusBarNotification sbn)
        {
            base.OnNotificationPosted(sbn);
            Console.WriteLine(sbn.PackageName);
            Console.WriteLine(sbn.Notification);
        }
        public override void OnNotificationRemoved(StatusBarNotification sbn)
        {
            base.OnNotificationRemoved(sbn);
            Console.WriteLine(sbn.PackageName);
            Console.WriteLine(sbn.Notification);
        }
        public override StatusBarNotification[] GetActiveNotifications()
        {
            return base.GetActiveNotifications();
        }
        public override IBinder OnBind(Intent intent)
        {
            this.Binder = new NLBinder(this);
            return this.Binder;
        }
        public override bool OnUnbind(Intent intent)
        {
            return base.OnUnbind(intent);
        }
        public override void OnListenerConnected()
        {
            base.OnListenerConnected();
        }
    }
    public class NLBinder : Binder
    {
        public NLBinder(NLService service)
        {
            this.Service = service;
        }
        public NLService Service { get; private set; }
        public StatusBarNotification[] GetActiveNotifications()
        {
            return Service?.GetActiveNotifications();
        }
    }
    public class NLServiceConnection : Java.Lang.Object, IServiceConnection
    {
        static readonly string TAG = "NotificationListenerTesting";
        MainActivity mainActivity;
        public NLServiceConnection(MainActivity activity)
        {
            IsConnected = false;
            Binder = null;
            mainActivity = activity;
        }
        public bool IsConnected { get; private set; }
        public NLBinder Binder { get; private set; }
        public void OnServiceConnected(ComponentName name, IBinder service)
        {
            Binder = service as NLBinder;
            IsConnected = this.Binder != null;
            string message = "OnServiceConnected - ";
            Log.Debug(TAG, $"OnServiceConnected - {name.ClassName}");
            if (IsConnected)
            {
                message = message + " bound to service " + name.ClassName;
            }
            else
            {
                message = message + " not bound to service " + name.ClassName;
            }
            Log.Info(TAG, message);
        }
        public void OnServiceDisconnected(ComponentName name)
        {
            Log.Debug(TAG, $"OnServiceDisconnected - {name.ClassName}");
            IsConnected = false;
            Binder = null;
        }
    }
}