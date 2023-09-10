using Plugin.BLE;
using Plugin.BLE.Abstractions;
using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.Exceptions;
using System.Collections.ObjectModel;

namespace XPrism;

public partial class BluetoothPage : ContentPage
{
    public IBluetoothLE ble;
    public IAdapter adapter;
    public ObservableCollection<IDevice> deviceList;

    public BluetoothPage()
	{
		InitializeComponent();
        ble = CrossBluetoothLE.Current;
        adapter = CrossBluetoothLE.Current.Adapter;
        deviceList = new ObservableCollection<IDevice>();
        adapter.DeviceDiscovered += (s, a) => deviceList.Add(a.Device);
        adapter.DeviceConnected += (s, a) => DisplayAlert("Success", "Device connected", "OK");
        adapter.DeviceConnectionLost += (s, a) => DisplayAlert("Warning", "Device connection lost", "OK");
        adapter.DeviceDisconnected += (s, a) => DisplayAlert("Success", "Device disconnected", "OK");
    }
    public async void OnScanClicked(object sender, EventArgs e)
    {
        if (ble.State == BluetoothState.Off)
        {
            await DisplayAlert("Warning", "Please turn on Bluetooth", "OK");
            return;
        }
        deviceList.Clear();
        await adapter.StartScanningForDevicesAsync();
    }
    public async void OnDeviceListViewItemTapped(object sender, ItemTappedEventArgs e)
    {
        // Get the tapped device
        var device = e.Item as IDevice;

        // Check if the device is already connected
        if (device.State == DeviceState.Connected)
        {
            // Disconnect the device
            await adapter.DisconnectDeviceAsync(device);
        }
        else
        {
            // Stop scanning for devices
            await adapter.StopScanningForDevicesAsync();

            // Connect the device
            await adapter.ConnectToDeviceAsync(device);
        }
    }
}