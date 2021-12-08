/*
 * Websocket Project for DC105
 * 
 * Group Members:
 * Bryle Anthony Orevillo
 * Glenn Gerald Dapitilla
 * Melchizedek Cruel
 * Lord Son Pabriga
 * R.A. Patricia Corage
 * 
 * Hardware used:
 * 
 * NodeMCU ESP32 Dev Module
 * WS2812b LED 16leds 
 * 
*/

// Imported Libraries
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <FastLED.h>

#define NUM_LEDS 16

CRGBArray<NUM_LEDS> leds;

const char *ssid = "DC105-Websocket";
const char *password = "12345678";

const char *msg_toggle_led = "toggleLED";
const char *msg_get_led = "getLEDState";

const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;
const int led_pin = 2;

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);

char msg_buf[10];

int led_state = 0;
int recent_led_state = 1;
int clients = 0;

/***********************************************************
 * Functions
 */
// Callback WebSocket events
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t *payload,
                      size_t length)
{

    switch (type)
    {

    // Client has disconnected
    case WStype_DISCONNECTED:
    {
        Serial.printf("[%u] Disconnected!\n", client_num);

        clients--;
        sprintf(msg_buf, "%d", clients);
        for (int i = 0; i < clients; i++)
        {
            Serial.printf("Sending to [%u]: %s\n", i, msg_buf);
            webSocket.sendTXT(i, msg_buf);
        }
    }
    break;
    // New client has connected
    case WStype_CONNECTED:
    {
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());

        clients++;
        sprintf(msg_buf, "%d", clients);
        for (int i = 0; i < clients; i++)
        {
            Serial.printf("Sending to [%u]: %s\n", i, msg_buf);
            webSocket.sendTXT(i, msg_buf);
        }
    }
    break;

    // Handle text messages from client
    case WStype_TEXT:
        // Print out raw message
        Serial.printf("[%u] Received text: %s\n", client_num, payload);

        // Toggle LED
        if (strcmp((char *)payload, "toggleLED") == 0)
        {
            led_state = led_state ? 0 : 1;
            Serial.printf("Toggling LED to %u\n", led_state);
            digitalWrite(led_pin, led_state);
            if (led_state == 0)
            {
                ledOFF();
            }
            else
            {
                switch (recent_led_state)
                {
                case 1:
                    ledRED();
                    recent_led_state = 1;
                    break;
                case 2:
                    ledGREEN();
                    recent_led_state = 2;
                    break;
                case 3:
                    ledBLUE();
                    recent_led_state = 3;
                    break;
                }
            }
        }
        // Send the state of the LED to the clients
        else if (strcmp((char *)payload, "getLEDState") == 0)
        {
            if (led_state == 0)
            {
                sprintf(msg_buf, "%d", led_state);
            }
            else
            {
                sprintf(msg_buf, "%d", recent_led_state);
            }

            for (int i = 0; i < clients; i++)
            {
                Serial.printf("Sending to [%u]: %s\n", i, msg_buf);
                webSocket.sendTXT(i, msg_buf);
            }

            // Message not recognized
        }
        else
        {
            Serial.println("[%u] Message not recognized");
        }

        if (strcmp((char *)payload, "ledRED") == 0)
        {
            if (led_state == 0)
                return;
            ledRED();
            recent_led_state = 1;
        }
        if (strcmp((char *)payload, "ledGREEN") == 0)
        {
            if (led_state == 0)
                return;
            ledGREEN();
            recent_led_state = 2;
        }
        if (strcmp((char *)payload, "ledBLUE") == 0)
        {
            if (led_state == 0)
                return;
            ledBLUE();
            recent_led_state = 3;
        }
        break;

    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
        break;
    }
}

// Callbacks
void onIndexRequest(AsyncWebServerRequest *request)
{
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() +
                   "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/index.html", "text/html");
}
void onJSRequest(AsyncWebServerRequest *request)
{
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() +
                   "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/server.js", "text/javascript");
}
void onCSSRequest(AsyncWebServerRequest *request)
{
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() +
                   "] HTTP GET request of " + request->url());
    request->send(SPIFFS, "/style.css", "text/css");
}
void onPageNotFound(AsyncWebServerRequest *request)
{
    IPAddress remote_ip = request->client()->remoteIP();
    Serial.println("[" + remote_ip.toString() +
                   "] HTTP GET request of " + request->url());
    request->send(404, "text/plain", "Not found");
}

/***********************************************************
 * LED functions
 */
void ledOFF()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB(0, 0, 0);
    }
    FastLED.show();
}
void ledRED()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB(255, 0, 0);
    }
    FastLED.show();
}
void ledGREEN()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB(0, 255, 0);
    }
    FastLED.show();
}
void ledBLUE()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB(0, 0, 255);
    }
    FastLED.show();
}

/***********************************************************
 * Main | Setup and Loop
 */

void setup()
{
    // Initialize LED and turn off
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, LOW);

    // Start serial port | just for monitoring the server on the serial monitor
    Serial.begin(115200);

    //Adding all LEDs
    FastLED.addLeds<NEOPIXEL, 15>(leds, NUM_LEDS);

    // Testing SPIFFS to load the index.html,server.js,style.css files
    if (!SPIFFS.begin())
    {
        Serial.println("Error mounting SPIFFS");
        while (1)
            ;
    }
    // Start wifi access point for the server
    WiFi.softAP(ssid, password);

    Serial.println();
    Serial.println("AP running");
    Serial.print("My IP address: ");
    Serial.println(WiFi.softAPIP());

    // On HTTP request for index.html, server.js, style.css
    server.on("/", HTTP_GET, onIndexRequest);
    server.on("/server.js", HTTP_GET, onJSRequest);
    server.on("/style.css", HTTP_GET, onCSSRequest);

    server.onNotFound(onPageNotFound);

    // Start web server
    server.begin();

    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);

    ledOFF();
}

void loop()
{
    webSocket.loop();
}
