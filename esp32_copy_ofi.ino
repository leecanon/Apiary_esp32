#include <WiFi.h>
#include <DHTesp.h>

// Configuración del DHT11 IAM VILEE
#define DOPIN 4
#define DHTPIN_INTERNAL 17 // Sensor interno (DHT22)
#define DHTPIN_EXTERNAL 16 // Sensor externo (DHT11)

DHTesp dhtInternal; // Sensor interno
DHTesp dhtExternal; // Sensor externo

// Configuración Wi-Fi
const char* ssid = "WLAN_VICH";
const char* password = "*******";

// Crear un servidor web en el puerto 80
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Configuración de los sensores
  dhtInternal.setup(DHTPIN_INTERNAL, DHTesp::DHT22); // Sensor interno
  dhtExternal.setup(DHTPIN_EXTERNAL, DHTesp::DHT11); // Sensor externo
  pinMode(DOPIN, INPUT);
  
  // Conexión a la red Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConexión exitosa a la red Wi-Fi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Iniciar el servidor
  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Nuevo cliente conectado");
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    // Leer datos del sensor interno (DHT22)
    float temperaturaInterna = dhtInternal.getTemperature();
    float humedadInterna = dhtInternal.getHumidity();

    // Leer datos del sensor externo (DHT11)
    float temperaturaExterna = dhtExternal.getTemperature();
    float humedadExterna = dhtExternal.getHumidity();

    // Determinar estado de lluvia
    String colorLluvia = (digitalRead(DOPIN) == HIGH) ? "#27ae60" : "#e74c3c"; // Verde si no hay lluvia, rojo si hay lluvia
    String estadoLluviaTexto = (digitalRead(DOPIN) == HIGH) ? "Sin lluvia detectada" : "Lluvia detectada";

    // Generar contenido HTML con CSS mejorado
    String pagina = "<html><head>";
    pagina += "<title>Monitor de Lluvias, Temperatura y Humedad de Colmenas</title>";
    pagina += "<meta http-equiv='refresh' content='5'>"; // Actualización automática cada 5 segundos
    pagina += "<style>";
    pagina += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; background-color: #f7f9fc; color: #333; margin: 0; padding: 0; }";
    pagina += ".container { max-width: 800px; margin: 0 auto; padding: 40px 20px; }";
    pagina += "h1 { color: #34495e; margin-bottom: 30px; }";
    pagina += ".card { background: linear-gradient(135deg, #74ebd5 0%, #ACB6E5 100%); padding: 20px; margin: 20px 0; box-shadow: 0px 10px 20px rgba(0, 0, 0, 0.1); border-radius: 15px; }";
    pagina += ".temperature { color: #e74c3c; font-size: 3em; font-weight: bold; }";
    pagina += ".humidity { color: #3498db; font-size: 3em; font-weight: bold; }";
    pagina += ".rain-status { color: " + colorLluvia + "; font-size: 2.5em; font-weight: bold; }";
    pagina += "</style></head><body>";
    
    // Contenido del cuerpo
    pagina += "<div class='container'>";
    pagina += "<h1>Monitor de Temperatura y Humedad - ESP32</h1>";

    // Sensor interno
    pagina += "<div class='card'>";
    pagina += "<h2>Temperatura Interna</h2>";
    pagina += "<div class='temperature'>" + String(temperaturaInterna) + " &deg;C</div>";
    pagina += "</div>";
    pagina += "<div class='card'>";
    pagina += "<h2>Humedad Interna</h2>";
    pagina += "<div class='humidity'>" + String(humedadInterna) + " %</div>";
    pagina += "</div>";

    // Sensor externo
    pagina += "<div class='card'>";
    pagina += "<h2>Temperatura Externa</h2>";
    pagina += "<div class='temperature'>" + String(temperaturaExterna) + " &deg;C</div>";
    pagina += "</div>";
    pagina += "<div class='card'>";
    pagina += "<h2>Humedad Externa</h2>";
    pagina += "<div class='humidity'>" + String(humedadExterna) + " %</div>";
    pagina += "</div>";

    // Estado de lluvia
    pagina += "<div class='card'>";
    pagina += "<h2>Detector de Lluvia</h2>";
    pagina += "<div class='rain-status'>" + estadoLluviaTexto + "</div>";
    pagina += "</div>";

    pagina += "</div></body></html>";

    // Enviar respuesta al cliente
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println(pagina);
    client.println();

    client.stop();
    Serial.println("Cliente desconectado");
  }
}