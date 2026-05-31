#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <ElegantOTA.h>

#define LED_PIN           15
#define BUTTON_PIN        0

#define RELAY_MAIN        16
#define RELAY_AUX1        17
#define RELAY_AUX2        18

#define STATUS_PIN        7

Preferences prefs;
WebServer server(80); // Standardowy, stabilny serwer synchroniczny
DNSServer dnsServer;

String sta_ssid;
String sta_pass;
String ap_ssid = "admin";
String ap_pass = "adminadmin";
String hostname = "brama40";

bool apMode = false;
bool staConnected = false;

unsigned long lastWifiCheck = 0;
unsigned long buttonPressStart = 0;

enum LedMode {
  LED_FAST,
  LED_STA,
  LED_AP,
  LED_AP_CLIENT,
  LED_SOLID
};

LedMode ledMode = LED_FAST;

void saveConfig() {
  prefs.begin("wifi", false);
  prefs.putString("sta_ssid", sta_ssid);
  prefs.putString("sta_pass", sta_pass);
  prefs.putString("ap_ssid", ap_ssid);
  prefs.putString("ap_pass", ap_pass);
  prefs.putString("hostname", hostname);
  prefs.end();
}

void loadConfig() {
  prefs.begin("wifi", true);
  sta_ssid = prefs.getString("sta_ssid", "");
  sta_pass = prefs.getString("sta_pass", "");
  ap_ssid = prefs.getString("ap_ssid", "admin");
  ap_pass = prefs.getString("ap_pass", "adminadmin");
  hostname = prefs.getString("hostname", "brama40");
  prefs.end();
}

void clearConfig() {
  prefs.begin("wifi", false);
  prefs.clear();
  prefs.end();
}

void pulseRelay(int pin) {
  ledMode = LED_SOLID;
  digitalWrite(LED_PIN, HIGH);

  digitalWrite(pin, HIGH);
  delay(500); // W synchronicznym serwerze to krótkie opóźnienie jest całkowicie bezpieczne
  digitalWrite(pin, LOW);

  if (staConnected) ledMode = LED_STA;
  else if (apMode) ledMode = LED_AP;
  else ledMode = LED_FAST;
}

String processor() {
  bool gateOpen = digitalRead(STATUS_PIN) == LOW;

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Brama</title>
<style>
body{font-family:Arial,sans-serif;text-align:center;background:#111;color:white;margin:0;padding-top:30px;}
button{width:240px;height:75px;font-size:22px;margin:12px;border:none;border-radius:15px;cursor:pointer;font-weight:bold;transition:background 0.1s ease;}
.main{background:#00aa00;color:white;}
.aux{background:#444;color:white;}
.status{font-size:28px;margin:25px;font-weight:bold;}
.state-open{color:#00ff00;}
.state-closed{color:#ff3333;}
</style>
<script>
function kliknijPrzycisk(element, sciezka, kolorKlikniecia) {
    // 1. Zapamiętaj oryginalny kolor przycisku
    var oryginalnyKolor = element.style.background || window.getComputedStyle(element).backgroundColor;
    
    // 2. Zmień kolor tła na czas kliknięcia
    element.style.background = kolorKlikniecia;
    
    // 3. Wyślij ukryte żądanie HTTP do ESP32 (klikanie przekaźnikiem)
    fetch(sciezka)
        .then(response => {
            if(!response.ok) console.error("Błąd połączenia ze sterownikiem");
        })
        .catch(err => console.error("Błąd sieci:", err));
        
    // 4. Po dokładnie 500 milisekundach przywróć pierwotny kolor
    setTimeout(function() {
        element.style.background = oryginalnyKolor;
    }, 500);
}
</script>
</head>
<body>
<h1>ESP32 Gate Controller</h1>
<div class="status">STATUS: )rawliteral";

  // Dynamiczne nadanie klasy CSS w zależności od stanu krańcówki (ładny zielony lub czerwony tekst)
  if (gateOpen) {
    html += "<span class='state-open'>OTWARTA</span>";
  } else {
    html += "<span class='state-closed'>ZAMKNIĘTA</span>";
  }
  
  html += R"rawliteral(</div>

<button class="main" onclick="kliknijPrzycisk(this, '/main', '#00ff00')">OPEN / CLOSE</button><br>
<button class="aux" onclick="kliknijPrzycisk(this, '/aux1', '#0077ff')">AUX1</button><br>
<button class="aux" onclick="kliknijPrzycisk(this, '/aux2', '#0077ff')">AUX2</button>

<br><br>
<div style="color:#888; font-size:14px; margin-top:20px;">
IP: )rawliteral";
  html += WiFi.localIP().toString();
  html += R"rawliteral(<br>HOST: )rawliteral";
  html += hostname;
  html += R"rawliteral(.local</div></body></html>)rawliteral";

  return html;
}

void startAP() {
  apMode = true;
  staConnected = false;
  ledMode = LED_AP;

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid.c_str(), ap_pass.c_str());

  dnsServer.start(53, "*", WiFi.softAPIP());

  // Przekierowanie dla Captive Portal (jeśli użytkownik połączy się z AP)
  server.onNotFound([]() {
    server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
    server.send(302, "text/plain", "");
  });

  Serial.println("AP STARTED");
  Serial.println(WiFi.softAPIP());
}

bool connectSTA() {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname.c_str());
  WiFi.begin(sta_ssid.c_str(), sta_pass.c_str());

  int retries = 0;
  
  // Bezpieczne, sprzętowe mruganie dedykowane dla etapu setup()
  // Nie obciąża procesora i pozwala stosowi WiFi na stabilną pracę
  while (WiFi.status() != WL_CONNECTED && retries < 30) { // 30 * 300ms = ~9 sekund max
    digitalWrite(LED_PIN, HIGH);
    delay(150);
    digitalWrite(LED_PIN, LOW);
    delay(150);
    
    retries++;
    Serial.print(".");
  }
  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) {
    staConnected = true;
    apMode = false;
    ledMode = LED_STA;

    if (MDNS.begin(hostname.c_str())) {
      Serial.println("MDNS OK");
    }
    Serial.println(WiFi.localIP());
    return true;
  }
  
  return false;
}

void setupServer() {
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", processor());
  });

  server.on("/main", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    pulseRelay(RELAY_MAIN);
  });

  server.on("/aux1", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    pulseRelay(RELAY_AUX1);
  });

  server.on("/aux2", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    pulseRelay(RELAY_AUX2);
  });

  server.on("/setup", HTTP_GET, []() {
    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Konfiguracja Bramy</title>
<style>
body{font-family:Arial;text-align:center;background:#111;color:white;padding-top:20px;}
form{display:inline-block;text-align:left;background:#222;padding:20px;border-radius:10px;width:280px;}
input{width:100%;padding:8px;margin:8px 0;box-sizing:border-box;border-radius:5px;border:1px solid #444;background:#333;color:white;}
input[type=submit]{background:#00aa00;color:white;border:none;cursor:pointer;font-size:16px;font-weight:bold;margin-top:15px;}
h2{margin-top:15px;font-size:18px;color:#00aa00;border-bottom:1px solid #333;padding-bottom:5px;}
</style>
</head>
<body>
<form action="/save" method="POST">
<h2>Router WiFi</h2>
Nazwa sieci (SSID):<br><input name="sta_ssid" placeholder="Pozostaw puste, aby nie zmieniać"><br>
Hasło sieci:<br><input name="sta_pass" type="password"><br>

<h2>Fallback AP (Awaryjne)</h2>
Nazwa sieci AP:<br><input name="ap_ssid" placeholder="Pozostaw puste, aby nie zmieniać"><br>
Hasło sieci AP:<br><input name="ap_pass" type="password" placeholder="Min. 8 znaków lub puste"><br>

<h2>Hostname (Adres strony)</h2>
Nazwa hosta:<br><input name="hostname" placeholder="Pozostaw puste, aby nie zmieniać"><br>
<small style="color:#aaa;">Wpisz np. brama, aby mieć brama.local</small><br>

<input type="submit" value="Zapisz i restartuj">
</form>
</body>
</html>)rawliteral";
    server.send(200, "text/html", html);
  });

  server.on("/save", HTTP_POST, []() {
    String req_sta_ssid = server.arg("sta_ssid");
    String req_sta_pass = server.arg("sta_pass");
    String req_ap_ssid  = server.arg("ap_ssid");
    String req_ap_pass  = server.arg("ap_pass");
    String req_hostname = server.arg("hostname");

    // Przygotowanie dynamicznej wiadomości HTML dla użytkownika
    String report = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    report += "<style>body{font-family:Arial;background:#111;color:white;text-align:center;padding-top:50px;}";
    report += ".box{display:inline-block;background:#222;padding:25px;border-radius:10px;text-align:left;width:320px;}";
    report += ".ok{color:#00ff00;font-weight:bold;} .warn{color:#ffaa00;} h1{text-align:center;margin-bottom:20px;}</style>";
    // Skrypt JS odlicza 5 sekund, dając czas na przeczytanie, po czym przeglądarka przekieruje na stronę główną
    report += "<script>setTimeout(function(){ window.location.href = '/'; }, 6000);</script>";
    report += "</head><body><div class='box'><h1>Status zapisu</h1><ul>";

    bool changed = false;

    // 1. Logika dla WiFi domowego
    if (req_sta_ssid.length() > 0) {
      sta_ssid = req_sta_ssid;
      sta_pass = req_sta_pass;
      report += "<li>Router WiFi: <span class='ok'>Zmieniono</span></li>";
      changed = true;
    } else {
      report += "<li>Router WiFi: <span class='warn'>Bez zmian</span></li>";
    }

    // 2. Logika dla nazwy sieci AP
    if (req_ap_ssid.length() > 0) {
      ap_ssid = req_ap_ssid;
      report += "<li>Nazwa sieci AP: <span class='ok'>Zmieniono</span></li>";
      changed = true;
    } else {
      report += "<li>Nazwa sieci AP: <span class='warn'>Bez zmian</span></li>";
    }

    // 3. Logika dla hasła AP (z walidacją długości)
    if (req_ap_pass.length() >= 8) {
      ap_pass = req_ap_pass;
      report += "<li>Hasło sieci AP: <span class='ok'>Zmieniono</span></li>";
      changed = true;
    } else if (req_ap_pass.length() > 0) {
      report += "<li>Hasło sieci AP: <span class='warn'>BŁĄD (Za krótkie! Min. 8 znaków. Nie zmieniono)</span></li>";
    } else {
      report += "<li>Hasło sieci AP: <span class='warn'>Bez zmian</span></li>";
    }

    // 4. Logika dla Hostname
    if (req_hostname.length() > 0) {
      hostname = req_hostname;
      report += "<li>Adres (.local): <span class='ok'>Zmieniono</span></li>";
      changed = true;
    } else {
      report += "<li>Adres (.local): <span class='warn'>Bez zmian</span></li>";
    }

    report += "</ul><h3 style='text-align:center;margin-top:25px;color:#aaa;'>Restart urządzenia... (6s)</h3></div></body></html>";

    // Wyślij raport do przeglądarki użytkownika
    server.send(200, "text/html", report);

    // Jeśli cokolwiek faktycznie się zmieniło, zapisujemy pamięć Flash
    if (changed) {
      saveConfig();
    }

    // Bezpieczne opóźnienie w wątku przed fizycznym restartem ESP32
    delay(2000);
    ESP.restart();
  });

  ElegantOTA.begin(&server); 
  server.begin();
}

void handleLED() {
  static unsigned long lastBlink = 0;
  static bool state = false;
  static int phase = 0;
  unsigned long now = millis();

  if (ledMode == LED_SOLID) {
    digitalWrite(LED_PIN, HIGH);
    return;
  }

  switch (ledMode) {
    case LED_FAST:
      if (now - lastBlink > 150) {
        state = !state;
        digitalWrite(LED_PIN, state);
        lastBlink = now;
      }
      break;

    case LED_STA:
      if (now - lastBlink > 1000) {
        state = !state;
        digitalWrite(LED_PIN, state);
        lastBlink = now;
      }
      break;

    case LED_AP:
      if (now - lastBlink > 120) {
        phase++;
        digitalWrite(LED_PIN, (phase == 1 || phase == 3) ? HIGH : LOW);
        if (phase >= 4) phase = 0;
        lastBlink = now;
      }
      break;

    case LED_AP_CLIENT:
      if (now - lastBlink > 400) {
        phase++;
        digitalWrite(LED_PIN, (phase == 1 || phase == 3) ? HIGH : LOW);
        if (phase >= 4) phase = 0;
        lastBlink = now;
      }
      break;
  }
}

void checkButton() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (buttonPressStart == 0)
      buttonPressStart = millis();

    if (millis() - buttonPressStart > 10000) {
      clearConfig();
      delay(1000);
      ESP.restart();
    }
  } else {
    buttonPressStart = 0;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RELAY_MAIN, OUTPUT);
  pinMode(RELAY_AUX1, OUTPUT);
  pinMode(RELAY_AUX2, OUTPUT);
  pinMode(STATUS_PIN, INPUT_PULLUP);

  digitalWrite(RELAY_MAIN, LOW);
  digitalWrite(RELAY_AUX1, LOW);
  digitalWrite(RELAY_AUX2, LOW);

  // Mignięcie testowe, żeby wiedzieć że procesor żyje
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);

  loadConfig();

  // 1. NAJPIERW URUCHAMIAMY SIEĆ WI-FI
  if (sta_ssid.length() > 0) {
    bool ok = false;
    for (int i = 0; i < 3; i++) {
      if (connectSTA()) {
        ok = true;
        break;
      }
    }
    if (!ok) {
      startAP();
    }
  } else {
    startAP();
  }

  // Odczekaj chwilę, aby stos sieciowy (LwIP) ustabilizował adresy IP
  delay(500); 

  // 2. DOPIERO TERAZ URUCHAMIAMY SERWER WWW I OTA
  setupServer();
  
  Serial.println("System gotowy!");
}

void loop() {
  // Kluczowe dla WebServer – obsługa żądań w pętli głównej
  server.handleClient();
  
  if (apMode) {
    dnsServer.processNextRequest();
  }

  handleLED();
  checkButton();

  if (apMode) {
    int clients = WiFi.softAPgetStationNum();
    ledMode = (clients > 0) ? LED_AP_CLIENT : LED_AP;

    if (millis() - lastWifiCheck > 180000) {
      lastWifiCheck = millis();
      int n = WiFi.scanNetworks();
      for (int i = 0; i < n; i++) {
        if (WiFi.SSID(i) == sta_ssid) {
          WiFi.softAPdisconnect(true);
          if (connectSTA()) return;
          startAP();
        }
      }
    }
  }

  if (!apMode && WiFi.status() != WL_CONNECTED) {
    staConnected = false;
    ledMode = LED_FAST;
    if (!connectSTA()) startAP();
  }
}
