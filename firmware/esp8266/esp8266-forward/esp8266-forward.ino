// These need to be included when using standard Ethernet
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <Dns.h>

#include "AppleMidi.h"

const char *host = "vindores1";
const char *ver = "ESP8266-R1";

int set_wifi = 0;
char ssid[128];
char pass[128];

unsigned long t0 = millis();
bool isConnected = false;

APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI); // see definition in AppleMidi_Defs.h

String readLine(WiFiClient &client) {
  String ret;
  char c, c2;
  while(client.available()) {
    c = client.read();
    if (c == '\r' || c == '\n') break;
    ret += c;
  }
  c2 = client.peek();
  if (c == '\r' && c2 == '\n') client.read();
  if (c == '\n' && c2 == '\r') client.read();
  return ret;
}

String getDate(int debug=0) {
  const char* host = "google.com";
  const char* url   = "/";
//  const char* host = "currentmillis.com";
//  const char* url   = "/time/minutes-since-unix-epoch.php";

  String now("None");

// Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    if (debug) Serial.println("connection failed");
    now = "Connection failed";
    return now;
  }
  
  if (debug) {
    Serial.print("Requesting URL: ");
    Serial.println(url);
  }
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      if (debug) Serial.println(">>> Client Timeout !");
      client.stop();
      now = "Request timeout";
      return now;
    }
  }

  int data = 0;
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = readLine(client);
    if (! data) {
      if (line.length() == 0) {
        if (debug) Serial.println("SRDDATA");
        data = 1; 
        continue;
      }
      if (line.startsWith("Date: ")) {
        now = line.substring(6);
      }
    }
    if (debug) Serial.print("!");
    if (debug) Serial.print(line);
    if (debug) Serial.println("!");
  }
  
  return now;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void initConnect()
{
  Serial.print("SRDCONNECT:");
  Serial.print(ssid);
  Serial.print(",");
  Serial.println(pass);
  WiFi.hostname(host);
  WiFi.begin(ssid, pass);
}

int service_connect = 0;
int service_time = 1;

void findService() {
  if (service_connect) return;
  if (service_time == 0) return;
  if (millis() < service_time) return;
  
  int n = MDNS.queryService("apple-midi", "udp");
  if (n == 0) {
#if 0
    Serial.println("SRDno services found");
#endif
    service_time = millis() + 2000;
  }
  else {
#if 0
    Serial.print(n);
    Serial.println(" service(s) found");
#endif
    for (int i = 0; i < n; ++i) {
      // Print details for each service found
#if 0
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(MDNS.hostname(i));
      Serial.print(" (");
      Serial.print(MDNS.IP(i));
      Serial.print(":");
      Serial.print(MDNS.port(i));
      Serial.println(")");
#endif
      int port = 5004;
      if (MDNS.port(i) > 1) {
        port = MDNS.port(i);
      }
      if (MDNS.IP(i)==IPAddress(0,0,0,0)) {
        int r;
        char host[256];
        MDNS.hostname(i).toCharArray(host, 256);
        IPAddress ip;
        r = WiFi.hostByName(host, ip);
        if (r!=0) {
          AppleMIDI.invite(ip, port);
#if 0
          Serial.print("Invite ");
          Serial.print(host);
          Serial.print(":");
          Serial.println(ip);
#endif
        }      
      }
      else {
#if 0
        Serial.print("Invite ");
        Serial.println(MDNS.IP(i));
#endif
        AppleMIDI.invite(MDNS.IP(i), port);
      }
      service_connect = 1;
    }
  }
}

int wifi_init = 0;

int testConnect() {
  if (set_wifi == 0) return 0;
  if (wifi_init) return 1;
  if (WiFi.status() != WL_CONNECTED) return 0;
  wifi_init = 1;
  
  // Create a session and wait for a remote host to connect to us
  AppleMIDI.begin("vindor-midi-session");

  if (!MDNS.begin("vindor-midi")) {
//    Serial.println("Error setting up MDNS responder!");
  }
  MDNS.addService("apple-midi", "udp", 5004);

  AppleMIDI.OnConnected(OnAppleMidiConnected);
  AppleMIDI.OnDisconnected(OnAppleMidiDisconnected);

  AppleMIDI.OnReceiveNoteOn(OnAppleMidiNoteOn);
  AppleMIDI.OnReceiveNoteOff(OnAppleMidiNoteOff);
  AppleMIDI.OnReceiveControlChange(OnAppleMidiControlChange);
  AppleMIDI.OnReceiveSystemExclusive(OnAppleMidiSystemExclusive);
//  AppleMIDI.OnReceiveAfterTouchChannel(OnAppleMidiAfterTouchChannel);
//  AppleMIDI.OnReceiveAfterTouchPoly(OnAppleMidiAfterTouchPoly);
  
  findService();
//  service_time = 1;
  
  return 1;
}

int timex;

#define DBG1

#ifdef OTA

#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>

ESP8266HTTPUpdateServer httpUpdater;
ESP8266WebServer httpServer;

void startOTAService() {
  #ifdef DBG1
  strcpy(ssid, "gw2");
  strcpy(pass, "elephant");
  set_wifi = 1;
//  delay(1000);
  initConnect();
//  delay(2000);
  #endif

  MDNS.begin(host);
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
}

#endif

void setup() {
  // Serial communications and wait for port to open:
  #ifdef DBG1
  Serial.begin(115200);
  #else
  Serial.begin(31250);
  #endif
  timex = millis();
  Serial.println("SRDBEGIN");
  #ifdef OTA
  startOTAService();
  #endif
}

int getByte() {
  int timeout = millis()+1000;
  while(Serial.available() == 0) {
    if (millis() > timeout) return -1;
  }
  return Serial.read();
}

int inByte;
int cmd;

uint8_t buffer[256];
int idx = 0;
int count = -1;

const int sleepSec = 60 * 60; // one hour

void process_special() {
  int c2 = buffer[1];
  if (c2 == 0x01) { // set ssid
    int len = buffer[2];
    strncpy(ssid, (char*)buffer+3, len);
    ssid[len] = 0;
  }
  if (c2 == 0x02) { // set pass
    int len = buffer[2];
    strncpy(pass, (char*)buffer+3, len);
    pass[len] = 0;
  }
  if (c2 == 0x03) { // connect
    set_wifi = 1;
    initConnect();
  }
  if (c2 == 0x04) { // revision
    Serial.print("SRV");
    Serial.println(ver);
  }
  if (c2 == 0x06) { // sleep
    ESP.deepSleep(0); // (sleepSec * 1000000);
  }
  if (c2 == 0x07) { // speed up
    Serial.begin(115200);
  }
}

void process() {
  int note, velocity, channel;
//  int cmdx = buffer[0] & 0xF0;
  channel = (buffer[0] & 0x0F) + 1;
  switch(cmd) {
    case 0x90: // NoteOn
      note = buffer[1];
      velocity = buffer[2];
      AppleMIDI.noteOn(note, velocity, channel);
      break;
    case 0x80: // NoteOff
      note = buffer[1];
      velocity = buffer[2];
      AppleMIDI.noteOff(note, velocity, channel);
      break;
    case 0xB0: // Control (BC, Expr, etc.)
      note = buffer[1];
      velocity = buffer[2];
      AppleMIDI.controlChange(note, velocity, channel);
      break;
    case 0xD0: // After touch channel
      velocity = buffer[1];
      AppleMIDI.afterTouch(velocity, channel);
      break;
    case 0xA0: // After touch poly
      note = buffer[1];
      velocity = buffer[2];
      AppleMIDI.polyPressure(note, velocity, channel);
      break;
    case 0xF0: // SysEx
      if (buffer[0] == 0xF0) {
        AppleMIDI.sysEx(idx+1, buffer, true);
      }
      else {
        process_special();      
      }
      break;
  }
  idx = 0;
  count = 0;
  cmd = 0;
}

int bytecount = 0;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{  
  if (testConnect()) AppleMIDI.run();

#ifdef OTA
  httpServer.handleClient();
#endif

  //findService();

#if 1
  if (millis() - timex > 1000) {
//    pinMode(2, INPUT);
//    int x = digitalRead(2);
//    if (x) Serial.write("H");
//    else Serial.write("L");
    Serial.write('S');
    if (wifi_init) Serial.write('W');
    else Serial.write('X');
    if (isConnected) Serial.write('1');
    else Serial.write("0");
    timex = millis();

    AppleMIDI.activeSensing();

    static int cc = 0;
    if (cc++ > 10) {
      Serial.print("SRDping:");
      Serial.println(bytecount);
      cc=0;
    }

//    Serial.printf("SRDS%d\n", WiFi.status());
  }
#endif

  if (Serial.available() > 0) {
    bytecount++;
    int inByte = Serial.read();
    if (count > 0) {
      buffer[++idx] = inByte;
      
      Serial.print("SRDGOT:");
      for(int i=0; i<idx; i++) {
        Serial.print(buffer[i], HEX);
      }
      Serial.println();
      
      if (--count == 0) process();
      else if (cmd == 0xF0) { // SysEx & Special
        if (inByte == 0xF7) { // end
          process();
        }
      }
    }
    else {
      Serial.print("SRDSTART:"); Serial.println(inByte);
      cmd = inByte & 0xF0;
      buffer[0] = inByte;
      idx = 0;
      switch(cmd) {
        case 0x90: // NoteOn
          count = 2;
          break;
        case 0x80: // NoteOff
          count = 2;
          break;
        case 0xB0: // Control (BC, Expr, etc.)
          count = 2;
          break;
        case 0xD0: // After touch channel
          count = 1;
          break;
        case 0xA0: // After touch poly
          count = 2;
          break;
        case 0xF0: // SysEx & Custom
          count = sizeof(buffer);
          break;
        default:
          Serial.print("SRDUNK:");
          Serial.println(cmd, HEX);
          break;
      }
    }
  }
}

// ====================================================================================
// Event handlers for incoming MIDI messages
// ====================================================================================

// -----------------------------------------------------------------------------
// rtpMIDI session. Device connected
// -----------------------------------------------------------------------------
void OnAppleMidiConnected(uint32_t ssrc, char* name) {
  isConnected  = true;
}

// -----------------------------------------------------------------------------
// rtpMIDI session. Device disconnected
// -----------------------------------------------------------------------------
void OnAppleMidiDisconnected(uint32_t ssrc) {
  isConnected  = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOn(byte channel, byte note, byte velocity) {
  Serial.write(0x80 + (channel-1));
  Serial.write(note);
  Serial.write(velocity);
#if 0
  Serial.print("Incoming NoteOn from channel:");
  Serial.print(channel);
  Serial.print(" note:");
  Serial.print(note);
  Serial.print(" velocity:");
  Serial.print(velocity);
  Serial.println();
  Serial.write(0x90 + (channel-1));
  Serial.write(note);
  Serial.write(velocity);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOff(byte channel, byte note, byte velocity) {
  Serial.write(0x80 + (channel-1));
  Serial.write(note);
  Serial.write(velocity);
#if 0
  Serial.print("Incoming NoteOff from channel:");
  Serial.print(channel);
  Serial.print(" note:");
  Serial.print(note);
  Serial.print(" velocity:");
  Serial.print(velocity);
  Serial.println();
  Serial.write(0x80 + (channel-1));
  Serial.write(note);
  Serial.write(velocity);
#endif
}

void OnAppleMidiControlChange(byte channel, byte number, byte value) {
  Serial.write(0xB0 + (channel-1));
  Serial.write(number);
  Serial.write(value);
}

void OnAppleMidiSystemExclusive(byte *array, byte size) {
  if (array[0] != 0xF0) Serial.write(0xF0);
  for(int i=0; i<size; i++) {
    Serial.write(array[i]);
  }
  if (array[size-1] != 0xF7) Serial.write(0xF7);
}

void OnAppleMidiAfterTouchChannel(byte channel, byte pressure) {
  Serial.write(0xD0 + (channel-1));
  Serial.write(pressure);
}

void OnAppleMidiAfterTouchPoly(byte channel, byte note, byte pressure) {
  Serial.write(0xA0 + (channel-1));
  Serial.write(note);
  Serial.write(pressure);
}
