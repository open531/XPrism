//this code should be used in this way:
//input a website like "http://......", then it will send "Ready"
//you can type "http://....../get?data=HelloWorld" to input what you want to send

#include <WiFi.h>
#include <WebServer.h>

WebServer Server(80);

void SendRoot()
{
    Server.send(200,"text/plain","Ready");
}

void SendGet()
{
    if(Server.hasArg("data"))
    {
        String data=Server.arg("data");
        Serial.println("data:"+data);
    }

    Server.send(200,"text/plain","Received");
}

void SendPost()
{
    Server.send(200,"text/plain","Processing");
}

void SendUpload()
{
    HTTPUpload& upload=Server.upload();
    if(upload.status==UPLOAD_FILE_START)
    {
        Serial.println("Receiving...");
    }
    else if(upload.status==UPLOAD_FILE_WRITE)
    {
        Serial.write(upload.buf,upload.currentSize);
    }
    else if(upload.status==UPLOAD_FILE_END)
    {
        Server.send(200,"text/plain","Data:");
    }
}

void initialSend()
{
    //begin Serial before this
    WiFi.softAP("ESP32");
    Server.on("/",SendRoot);
    Server.on("/get",HTTP_GET,SendGet);
    Server.on("/post",HTTP_POST,SendPost,SendUpload);
    Server.begin();
}

void SendRoutine()
{
    while(1)
    {
        Server.handleClient();
    }
}