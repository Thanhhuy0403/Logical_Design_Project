#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

// Led RGB
#include <Adafruit_NeoPixel.h>

// Timer
// #include "Ticker.h"

#define WIFI_SSID "ACLAB"
#define WIFI_PASSWORD "ACLAB2023"

#define TXD_PIN (GPIO_NUM_44)  // ESP32 TX to STM32 RX (PA9)
#define RXD_PIN (GPIO_NUM_43)  // ESP32 RX to STM32 TX (PA10)

#define API_KEY "AIzaSyAp-8pUjTQjmUHwIZgMIv0Jx8o7uHV-TM4"
#define DATABASE_URL "https://app-api-3b3ff-default-rtdb.firebaseio.com/"

#define PIN_NEO_PIXEL 6 // Chân kết nối với NeoPixel
#define NUM_PIXELS 4    // Số LED trên NeoPixel
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
unsigned long fetchDataPrevMillis = 0;
bool signupOK = false;

// Sensor data values - data send
float temp = 0.0;
float humi = 0.0;

// led RGB - data recive
int stateLed = 0;
float R = 0.0, G = 0.0, B = 0.0;

// Ticker timer;

void fetchData() {
  // Lấy giá trị R
  if (Firebase.RTDB.getFloat(&fbdo, "LED/color/r")) {
    R = fbdo.floatData();
    Serial.println("R: " + String(R));
  } else {
    Serial.println("Failed to get R: " + fbdo.errorReason());
  }

  // Lấy giá trị G
  if (Firebase.RTDB.getFloat(&fbdo, "LED/color/g")) {
    G = fbdo.floatData();
    Serial.println("G: " + String(G));
  } else {
    Serial.println("Failed to get G: " + fbdo.errorReason());
  }

  // Lấy giá trị B
  if (Firebase.RTDB.getFloat(&fbdo, "LED/color/b")) {
    B = fbdo.floatData();
    Serial.println("B: " + String(B));
  } else {
    Serial.println("Failed to get B: " + fbdo.errorReason());
  }

  // Trạng thái led
  if (Firebase.RTDB.getBool(&fbdo, "LED/state")) { // Thay đổi phương thức để lấy boolean
    stateLed = fbdo.boolData(); // Lấy dữ liệu kiểu boolean
    Serial.println("State LED: " + String(stateLed));
  } else {
    Serial.println("Failed to get data: " + fbdo.errorReason());
  }
}

void setup() {
  Serial.begin(9600);

  // setup led RGB
  NeoPixel.begin();
  NeoPixel.show();

  // config uart to esp32
  const uart_port_t uart_num = UART_NUM_2;
  uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,  // Disable flow control
  };

  const int uart_buffer_size = (1024 * 2);
  QueueHandle_t uart_queue;
  // Install UART driver using an event queue
  ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

  // Configure UART parameters
  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
  ESP_ERROR_CHECK(uart_set_pin(uart_num, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected with IP: ");
  Serial.println(WiFi.localIP()); 
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if(Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("signUp OK");
    signupOK = true;
  } else {
    Serial.println(config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
}

void loop() {

  // connect uart recieve data
  const uart_port_t uart_num = UART_NUM_2;
  uint8_t data[128];

  int length = 0;
  ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
  length = uart_read_bytes(uart_num, data, length, 100);
  if (length > 0) {
      char *test = (char*) data;
      // Serial.println(test);

      char *token = strtok(test, "-");

      if (token != NULL) {
        temp = atof(token);
      }
      
      token = strtok(NULL, "-");

      if (token != NULL) {
          humi = atof(token);
      }

      Serial.println("check");
      Serial.println(temp);
      Serial.println(humi);
  }

  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    // Send temperature data to Firebase
    if(Firebase.RTDB.setFloat(&fbdo, "Sensor/temp", temp)) {
      Serial.println();
      Serial.println(temp);
      Serial.println("--- Successfully saved to: " + fbdo.dataPath());
      Serial.println("(" + fbdo.dataType() + ")");
    } else {
      Serial.println("Failed: " + fbdo.errorReason());
    }

    // Send humidity data to Firebase
    if(Firebase.RTDB.setFloat(&fbdo, "Sensor/humi", humi)) {
      Serial.println();
      Serial.println(humi);
      Serial.println("--- Successfully saved to: " + fbdo.dataPath());
      Serial.println("(" + fbdo.dataType() + ")");
    } else {
      Serial.println("Failed: " + fbdo.errorReason());
    }

    // Recive data state led from database
    // timer.attach(2, fetchData);
    fetchData();
    // delay(3000);
  }

  // fetch data - handle led RGB
  if (Firebase.ready() && signupOK && (millis() - fetchDataPrevMillis > 3000 || fetchDataPrevMillis == 0)) {
    fetchDataPrevMillis = millis();  // Cập nhật lại thời gian
    fetchData();
    if (stateLed == 0) {
      NeoPixel.clear();  // Xóa toàn bộ màu LED
      NeoPixel.show();   // Cập nhật trạng thái tắt
    }else{
      for (int pixel = 0; pixel < NUM_PIXELS; pixel++) {
        NeoPixel.setPixelColor(pixel, NeoPixel.Color(R, G, B));
      }
      NeoPixel.show();   // Cập nhật trạng thái tắt
    }
  }
}

