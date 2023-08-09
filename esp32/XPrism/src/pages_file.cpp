#include "pages_file.h"
#include "pages_file_ui.h"
#include "pages_main.h"
#include "system.h"
#include "ESP32FtpServer.h"

#define FILE_APP_NAME "文件"

#define FILE_REFRESH_INTERVAL 2000UL // 刷新间隔（2s）
#define RECV_BUFFER_SIZE 2000        // 接收缓冲区大小
#define SEND_BUFFER_SIZE 2000        // 发送缓冲区大小
#define SERVER_PORT 8081             // 设置监听端口
#define SHARE_WIFI_ALIVE 20000UL     // 维持Wifi心跳的时间（20s）

FtpServer ftpSrv; // FTP服务器

struct FileRunData
{
    boolean tcp_server_started; // TCP服务器是否已启动
    boolean request_sent;       // 是否已发送请求
    unsigned long last_refresh; // 上次刷新时间
    unsigned long last_alive;   // 上次维持Wifi心跳的时间
    char *recv_buffer;          // 接收缓冲区
    char *send_buffer;          // 发送缓冲区
};

static FileRunData *run_data = NULL;

static int file_init(XPages *sys)
{
    file_gui_init();
    // run_data = new FileRunData;
    // run_data->tcp_server_started = false;
    // run_data->request_sent = false;
    // run_data->last_refresh = 0;
    // run_data->last_alive = 0;
    // run_data->recv_buffer = new char[RECV_BUFFER_SIZE];
    // run_data->send_buffer = new char[SEND_BUFFER_SIZE];
    return 0;
}

static void click_BACK()
{
    xpages->app_exit();
}

static void file_process(XPages *sys, Buttons *btns_info)
{
    lv_scr_load_anim_t anim_type = LV_SCR_LOAD_ANIM_NONE;

    btns_info->attachClick(BACK, click_BACK);

    // if (run_data->tcp_server_started == false && run_data->request_sent == false)
    // {
    //     display_file("文件", WiFi.softAPIP().toString().c_str(), "8081", "请在浏览器中输入地址访问", anim_type);
    //     sys->send_to(FILE_APP_NAME, XPAGES_NAME, APP_MESSAGE_WIFI_CONN, NULL, NULL);
    //     run_data->request_sent = true;
    // }
    // else if (run_data->tcp_server_started == true)
    // {
    //     if (millis() - run_data->last_refresh > FILE_REFRESH_INTERVAL)
    //     {
    //         sys->send_to(FILE_APP_NAME, XPAGES_NAME, APP_MESSAGE_WIFI_CONN, NULL, NULL);
    //     }
    //     ftpSrv.handleFTP();
    // }
}

static void file_background_task(XPages *sys, Buttons *btns_info)
{
}

static int file_exit_callback(void *param)
{
    file_gui_del();

    if (run_data->recv_buffer != NULL)
    {
        delete[] run_data->recv_buffer;
        run_data->recv_buffer = NULL;
    }
    if (run_data->send_buffer != NULL)
    {
        delete[] run_data->send_buffer;
        run_data->send_buffer = NULL;
    }
    if (run_data != NULL)
    {
        delete run_data;
        run_data = NULL;
    }
}

static void file_message_handle(const char *from, const char *to, APP_MESSAGE_TYPE type, void *message, void *ext_info)
{
    // switch (type)
    // {
    // case APP_MESSAGE_WIFI_CONN:
    // {
    //     Serial.print(F("APP_MESSAGE_WIFI_CONN enable\n"));
    //     display_file("文件", WiFi.softAPIP().toString().c_str(), "8081", "请在浏览器中输入地址访问", LV_SCR_LOAD_ANIM_NONE);
    //     run_data->tcp_server_started = true;
    //     ftpSrv.begin("xprism", "xprism");
    //     break;
    // }
    // case APP_MESSAGE_WIFI_AP:
    // {
    //     Serial.print(F("APP_MESSAGE_WIFI_AP enable\n"));
    //     display_file("文件", WiFi.softAPIP().toString().c_str(), "8081", "请在浏览器中输入地址访问", LV_SCR_LOAD_ANIM_NONE);
    //     run_data->tcp_server_started = true;
    //     break;
    // }
    // case APP_MESSAGE_WIFI_ALIVE:
    // {
    //     break;
    // }
    // default:
    //     break;
    // }
}

APP_OBJ file_app = {FILE_APP_NAME,
                    &app_file,
                    "",
                    file_init,
                    file_process,
                    file_background_task,
                    file_exit_callback,
                    file_message_handle};