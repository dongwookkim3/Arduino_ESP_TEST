#include <Arduino.h>
#include <ESP8266WebServer.h>

#include "secrets.h"

#include <FS.h>
#include <LittleFS.h>


#define UNUSED __attribute__((unused))


#define TRACE(...) Serial.printf(__VA_ARGS__)


#define HOSTNAME "webserver"


#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"


ESP8266WebServer server(80);


#include "builtinfiles.h"






void handleRedirect() {
  TRACE("Redirect...");
  String url = "/index.htm";

  if (!LittleFS.exists(url)) {
    url = "/$update.htm";
  }

  server.sendHeader("Location", url, true);
  server.send(302);
}




void handleListFiles() {
  Dir dir = LittleFS.openDir("/");
  String result;

  result += "[\n";
  while (dir.next()) {
    if (result.length() > 4) {
      result += ",";
    }
    result += "  {";
    result += " \"name\": \"" + dir.fileName() + "\", ";
    result += " \"size\": " + String(dir.fileSize()) + ", ";
    result += " \"time\": " + String(dir.fileTime());
    result += " }\n";

  }
  result += "]";
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/javascript; charset=utf-8", result);
}



void handleSysInfo() {
  String result;

  FSInfo fs_info;
  LittleFS.info(fs_info);

  result += "{\n";
  result += "  \"flashSize\": " + String(ESP.getFlashChipSize()) + ",\n";
  result += "  \"freeHeap\": " + String(ESP.getFreeHeap()) + ",\n";
  result += "  \"fsTotalBytes\": " + String(fs_info.totalBytes) + ",\n";
  result += "  \"fsUsedBytes\": " + String(fs_info.usedBytes) + ",\n";
  result += "}";

  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/javascript; charset=utf-8", result);
}





class FileServerHandler : public RequestHandler {
  public:




    FileServerHandler() {
      TRACE("FileServerHandler is registered\n");
    }






    bool canHandle(HTTPMethod requestMethod, const String UNUSED &_uri) override {
      return ((requestMethod == HTTP_POST) || (requestMethod == HTTP_DELETE));
    }


    bool canUpload(const String &uri) override {

      return (uri == "/");
    }


    bool handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) override {

      String fName = requestUri;
      if (!fName.startsWith("/")) {
        fName = "/" + fName;
      }

      if (requestMethod == HTTP_POST) {


      } else if (requestMethod == HTTP_DELETE) {
        if (LittleFS.exists(fName)) {
          LittleFS.remove(fName);
        }
      }

      server.send(200);
      return (true);
    }



    void upload(ESP8266WebServer UNUSED &server, const String UNUSED &_requestUri, HTTPUpload &upload) override {

      String fName = upload.filename;
      if (!fName.startsWith("/")) {
        fName = "/" + fName;
      }

      if (upload.status == UPLOAD_FILE_START) {

        if (LittleFS.exists(fName)) {
          LittleFS.remove(fName);
        }
        _fsUploadFile = LittleFS.open(fName, "w");

      } else if (upload.status == UPLOAD_FILE_WRITE) {

        if (_fsUploadFile) {
          _fsUploadFile.write(upload.buf, upload.currentSize);
        }

      } else if (upload.status == UPLOAD_FILE_END) {

        if (_fsUploadFile) {
          _fsUploadFile.close();
        }
      }
    }

  protected:
    File _fsUploadFile;
};



void setup(void) {
  delay(3000);


  Serial.begin(115200);
  Serial.setDebugOutput(false);

  TRACE("Starting WebServer example...\n");

  TRACE("Mounting the filesystem...\n");
  if (!LittleFS.begin()) {
    TRACE("could not mount the filesystem...\n");
    delay(2000);
    ESP.restart();
  }


  WiFi.mode(WIFI_STA);
  if (strlen(ssid) == 0) {
    WiFi.begin();
  } else {
    WiFi.begin(ssid, passPhrase);
  }


  WiFi.setHostname(HOSTNAME);

  TRACE("Connect to WiFi...\n");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    TRACE(".");
  }
  TRACE("connected.\n");


  TRACE("Setup ntp...\n");
  configTime(TIMEZONE, "pool.ntp.org");

  TRACE("Register service handlers...\n");


  server.on("/$upload.htm", []() {
    server.send(200, "text/html", FPSTR(uploadContent));
  });


  server.on("/", HTTP_GET, handleRedirect);


  server.on("/$list", HTTP_GET, handleListFiles);
  server.on("/$sysinfo", HTTP_GET, handleSysInfo);


  server.addHandler(new FileServerHandler());


  server.enableCORS(true);


  server.enableETag(true);


  server.serveStatic("/", LittleFS, "/");


  server.onNotFound([]() {

    server.send(404, "text/html", FPSTR(notFoundContent));
  });

  server.begin();
  TRACE("hostname=%s\n", WiFi.getHostname());
}



void loop(void) {
  server.handleClient();
}

