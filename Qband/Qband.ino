#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>  // Cambia de SPIFFS.h a LittleFS.h

const char* apSSID = "ResQBand";  // Nombre de la red Wi-Fi del ESP
const char* apPassword = "12345678";  // Contraseña de la red Wi-Fi

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Configurar el ESP8266 en modo Access Point
  WiFi.softAP(apSSID, apPassword);
  Serial.println("Modo AP iniciado");
  Serial.print("IP de la red: ");
  Serial.println(WiFi.softAPIP());  // Imprimir la IP de la red creada por el ESP

  if (!LittleFS.begin()) {  
    Serial.println("Error al montar LittleFS");
    return;
  }

  // Servir el archivo HTML desde LittleFS
  server.on("/", HTTP_GET, []() {
    File file = LittleFS.open("/index.html", "r");  // Cambiar SPIFFS por LittleFS
    if (!file) {
      server.send(500, "text/plain", "Error al cargar la página");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();
  });

  // Ruta para obtener los datos en formato JSON
  server.on("/datos", HTTP_GET, []() {
    File file = LittleFS.open("/datos.json", "r");  // Cambiar SPIFFS por LittleFS
    if (!file) {
      server.send(500, "application/json", "{\"error\": \"No se encontró el archivo\"}");
      return;
    }
    server.streamFile(file, "application/json");
    file.close();
  });

  // Ruta para guardar los datos
  server.on("/guardar", HTTP_POST, []() {
    if (!server.hasArg("plain")) {
      server.send(400, "application/json", "{\"error\": \"No se recibieron datos\"}");
      return;
    }

    File file = LittleFS.open("/datos.json", "w");  // Cambiar SPIFFS por LittleFS
    if (!file) {
      server.send(500, "application/json", "{\"error\": \"No se pudo guardar el archivo\"}");
      return;
    }

    file.print(server.arg("plain"));
    file.close();
    
    server.send(200, "application/json", "{\"mensaje\": \"Datos guardados correctamente\"}");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
