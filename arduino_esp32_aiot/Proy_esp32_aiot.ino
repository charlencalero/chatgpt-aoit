#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_camera.h>
#include "Base64.h"

int led=2;

// Parámetros de red
const char* ssid = "Calero";           // Nombre de tu red WiFi
const char* password = "wendy12345";      // Contraseña de tu red WiFi

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define flashLed 4
bool flashEstado = LOW;
int estadoFlash = 0;

// Configuración del servicio POST
const char* post_url = "http://192.168.2.24:5700/predict";  // Reemplaza esto con la URL de tu servicio POST

void setup() {

    Serial.begin(115200);

  // Turn-off the 'brownout detector'
 // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;
 
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();

   
  
  }
  // configurando el led
  pinMode(led,OUTPUT);
  // Inicialización de la conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red WiFi...");
  }
  Serial.println("Conexión WiFi establecida!");
}

void loop() {
 
    camera_fb_t * fb = NULL;
    fb = esp_camera_fb_get();

    if(!fb) {
      Serial.println("Error al capturar la image...");
      delay(1000);
      ESP.restart();
      return;
    } else
    {
        Serial.println("capturar imagen con exito...");
    }

    
  // Codificar la imagen en base64
  size_t image_len = fb->len;
  char* base64_image = new char[image_len * 4 / 3 + 4];
  if (!base64_image) {
    esp_camera_fb_return(fb);
    Serial.println("Error de memoria");
    return;
  }

  char *input = (char *)fb->buf;
    char output[base64_enc_len(3)];

String imagen_base64 = "";

    for (int i=0;i<fb->len;i++) {
      base64_encode(output, (input++), 3);
      if (i%3==0)  imagen_base64 += String(output);
      //URLENCODE
    }

  // Construir el JSON como texto
  String json_str = "{";
  json_str += "\"name\":\"Nombre de la imagen\",";  // Reemplaza "Nombre de la imagen" por el nombre que desees
  json_str += "\"image\":\"" + String( imagen_base64) + "\"";
  json_str += "}";

  // Enviar el JSON al servicio POST
  HTTPClient http;
  http.begin(post_url);
  http.addHeader("Content-Type", "application/json");

  // Enviar el POST y obtener la respuesta
  int httpCode = http.POST(json_str);
  String response = http.getString();

  // Imprimir la respuesta
  Serial.println("Código HTTP: " + String(httpCode));
  Serial.println("Respuesta: " + response);
  digitalWrite(led, HIGH);
  delay(2000);
   digitalWrite(led, LOW);
   
  // Liberar memoria y recursos
  delete[] base64_image;
  esp_camera_fb_return(fb);
  http.end();

  // Esperar un tiempo antes de tomar otra imagen
  delay(2000);
}
