# **📖 INSTRUKCJA OBSŁUGI: Sterownik Bramy ESP32 (brama40)**

Sterownik umożliwia bezprzewodowe otwieranie i zamykanie bramy (oraz dwóch dodatkowych urządzeń, np. furtki i oświetlenia) za pomocą telefonu, tabletu lub komputera przez sieć Wi-Fi.

## **🟢 1\. Znaczenie diody statusowej (LED)**

Dioda LED na obudowie informuje o bieżącym stanie sterownika:

* **Szybkie miganie:** Sterownik uruchamia się i próbuje połączyć z Twoją domową siecią Wi-Fi.  
* **Wolne miganie (co 1 sekunda):** Sterownik jest połączony z Twoją domową siecią Wi-Fi i działa w trybie zdalnym.  
* **Miganie "podwójne" (krótkie błyski):** Sterownik nie znalazł domowej sieci Wi-Fi i uruchomił własną sieć awaryjną (**Tryb Konfiguracyjny AP**).  
* **Ciągłe świecenie:** Przekaźnik jest właśnie uruchomiony (brama reaguje) lub wgrywane jest nowe oprogramowanie.

## **📱 2\. Codzienna obsługa (w domowej sieci Wi-Fi)**

Jeśli sterownik został już wcześniej skonfigurowany z Twoim domowym routerem:

1. Połącz telefon z **Twoją domową siecią Wi-Fi**.  
2. Otwórz przeglądarkę internetową (np. Chrome, Safari) i wpisz w pasek adresu: `http://brama40.local`  
3. Na ekranie pojawi się panel sterowania:  
   * STATUS: Wyświetla aktualny stan bramy (czytelny ZIELONY napis oznacza bramę OTWARTĄ, a CZERWONY – ZAMKNIĘTĄ).   
   * Przycisk **OPEN / CLOSE**: Uruchamia główną bramę.  
   * Przyciski **AUX1** / **AUX2**: Uruchamiają dodatkowe funkcje (np. furtkę, oświetlenie).

## **⚙️ 3\. Pierwsze uruchomienie lub zmiana sieci Wi-Fi (Tryb Awaryjny)**

Jeśli zmienisz router, hasło do Wi-Fi lub uruchamiasz urządzenie po raz pierwszy, dioda zacznie migać podwójnymi błyskami. Oznacza to, że sterownik czeka na konfigurację.

1. Włącz Wi-Fi w telefonie i wyszukaj dostępne sieci.  
2. Połącz się z siecią o nazwie: **admin**  
3. Wpisz hasło do tej sieci: **adminadmin**  
4. **Ważne (szczególnie dla telefonów Samsung):** Jeśli telefon wyświetli komunikat *"Ta sieć nie ma dostępu do internetu. Czy chcesz utrzymać połączenie?"* – kliknij **TAK**. Na czas konfiguracji możesz też wyłączyć dane komórkowe (LTE/5G) w telefonie.  
5. Otwórz przeglądarkę internetową i wpisz ręcznie adres: `http://192.168.4.1/setup`  
6. Zobaczysz bezpieczny formularz konfiguracji. **Pola są domyślnie puste, ponieważ sterownik ukrywa Twoje obecne hasła ze względów bezpieczeństwa. Zasada działania pól:** Formularz jest inteligentny – wpisujesz tylko to, co chcesz zmienić.  
* Jeśli chcesz zmienić tylko domowe Wi-Fi, wpisz nazwę sieci (SSID) oraz hasło, a pozostałe pola (Nazwa sieci AP, Hasło AP, Nazwa hosta) **zostaw całkowicie puste**. Sterownik zachowa ich dotychczasowe ustawienia.  
* *Uwaga:* Nowe hasło awaryjnej sieci AP musi mieć **minimum 8 znaków**. Jeśli wpiszesz krótsze, sterownik ze względów bezpieczeństwa zignoruje zmianę i zachowa dotychczasowe hasło.  
7. Po zatwierdzeniu ustawień, na ekranie telefonu pojawi się **Tabela Statusu Zapisu**. Zobaczysz na niej dokładny raport:  
* Zmieniono – przy pozycjach, które zostały właśnie zaktualizowane.  
* Bez zmian – przy pozycjach, które zostawiłeś puste (urządzenie zachowało poprzednią konfigurację).  
8. Strona wyświetli odliczanie (6 sekund), po czym sterownik automatycznie się zrestartuje i spróbuje połączyć z nową siecią, a telefon powróci do ekranu głównego.  
   

## **🔄 4\. Resetowanie do ustawień fabrycznych**

Jeśli pomylisz się wpisując hasło do domowego Wi-Fi i stracisz kontakt ze sterownikiem, możesz go zresetować fizycznym przyciskiem na obudowie układu.

1. Znajdź przycisk resetu na urządzeniu (oznaczony jako **BUTTON 0**).  
2. Wciśnij go i **przytrzymaj nieprzerwanie przez 10 sekund**.  
3. Sterownik wymaże wszystkie zapisane nazwy sieci oraz hasła, a następnie automatycznie się zrestartuje.  
4. Urządzenie powróci do punktu 3 niniejszej instrukcji (uruchomi sieć awaryjną `admin` i będzie czekać na nową konfigurację).

# **🌐 BEZPRZEWODOWA AKTUALIZACJA (ElegantOTA)**

Z tej funkcji korzystasz, gdy chcesz wgrać poprawki do kodu, nowe funkcje lub zmodyfikowany program.

### **Krok 1: Przygotowanie pliku z programem**

1. Otwórz projekt w programie Arduino IDE.  
2. W górnym menu wybierz: **Szkic** \-\> **Eksportuj skompilowany program** (Szkic \-\> Export compiled Binary).  
3. Arduino IDE utworzy plik z rozszerzeniem **`.bin`** w folderze z Twoim projektem. To jest gotowy plik aktualizacji.

### **Krok 2: Wejście do panelu aktualizacji**

1. Upewnij się, że Twój telefon/komputer jest w tej samej sieci co sterownik bramy.  
2. Otwórz przeglądarkę internetową i dopisz **`/update`** na końcu adresu sterownika:  
   * Jeśli jesteś w sieci domowej: `http://brama40.local/update`  
   * Jeśli jesteś połączony bezpośrednio z siecią awaryjną sterownika: `http://192.168.4.1/update`

### **Krok 3: Wgranie nowego programu**

1. Na ekranie pojawi się elegancki, ciemny panel z napisem **ElegantOTA**.  
2. Upewnij się, że zaznaczona jest opcja **Firmware** (oprogramowanie).  
3. Kliknij przycisk **Choose File** (Wybierz plik) i wskaż wyeksportowany wcześniej plik `.bin`.  
4. Kliknij **Upload** (Prześlij).  
5. Na ekranie pojawi się pasek postępu. Podczas wgrywania dioda LED na sterowniku zacznie świecić światłem ciągłym.

### **Krok 4: Zakończenie**

Po osiągnięciu 100% panel wyświetli komunikat **"Update Successful\! Rebooting..."**. Sterownik automatycznie zrestartuje się w ciągu kilku sekund i uruchomi się już z nowym programem. Wszystkie zapisane wcześniej hasła do Wi-Fi w pamięci `Preferences` zostaną zachowane\!

# **📖 USER MANUAL: ESP32 Gateway Controller (brama40)**

The controller allows you to wirelessly open and close the gate (and two additional devices, e.g. a wicket and lighting) using a phone, tablet or computer via a Wi-Fi network.

## **🟢 1\. Meaning of the status LED**

The LED on the housing indicates the current status of the controller:

* **Fast flashing:**The controller starts and tries to connect to your home Wi-Fi network.  
* **Slow flashing (every 1 second):**The controller is connected to your home Wi-Fi network and operates in remote mode.  
* **Double flashing (short flashes):**The controller did not find the home Wi-Fi network and started its own emergency network (**AP Configuration Mode**).  
* **Continuous light:**The relay is currently activated (the gateway responds) or new software is being loaded.

## **📱 2\. Daily operation (on your home Wi-Fi network)**

If the driver has already been configured with your home router:

1. Connect your phone to **Your home Wi-Fi network**.  
2. Open your web browser (e.g. Chrome, Safari) and enter the following in the address bar:`http://brama40.local`  
3. The control panel will appear on the screen:  
   * STATUS: Displays the current status of the gate (a legible GREEN inscription means the gate is OPEN, and RED – CLOSED).   
   * Button **OPEN / CLOSE**: Activates the main gate.  
   * Buttons **AUX1** / **AUX2**: They activate additional functions (e.g. gate, lighting).

## **⚙️ 3\. First time launch or change Wi-Fi network (Failsafe Mode)**

If you change your router or Wi-Fi password, or are starting the device for the first time, the LED will flash twice. This indicates that the controller is waiting for configuration.

1. Turn on Wi-Fi on your phone and search for available networks.  
2. Connect to the network called:**admin**  
3. Enter the password for this network:**adminadmin**  
4. **Important (especially for Samsung phones):**If your phone displays a message*"This network does not have internet access. Would you like to stay connected?"*– click **YES** You can also turn off cellular data (LTE/5G) on your phone during setup.  
5. Open your web browser and enter the address manually:`http://192.168.4.1/setup`  
6. You will see a secure setup form.**The fields are blank by default because the controller hides your current passwords for security reasons. Principle of operation of fields:**The form is intelligent – ​​you only enter what you want to change.  
* If you only want to change your home Wi-Fi, enter the network name (SSID) and password, and the remaining fields (AP Network Name, AP Password, Host Name)**leave completely blank**The controller will retain their current settings.  
* *Attention:*The new fallback AP password must have **minimum 8 characters**If you enter a shorter password, the controller will ignore the change for security reasons and keep the current password.  
7. After confirming the settings, the phone screen will appear **Write Status Table** You will see a detailed report there:  
* Changed – for items that have just been updated.  
* No changes – for items you left blank (the device retained the previous configuration).  
8. The page will display a countdown (6 seconds), after which the controller will automatically restart and attempt to connect to the new network, and the phone will return to the home screen.  
   

## **🔄 4\. Factory Reset**

If you make a mistake when entering your home Wi-Fi password and lose contact with the controller, you can reset it using the physical button on the system housing.

1. Locate the reset button on your device (marked as **BUTTON 0**).  
2. Press it and hold **continuously for 10 seconds**.  
3. The driver will erase all saved network names and passwords and then automatically restart.  
4. The device will return to point 3 of this manual (it will start the emergency network`admin`and will wait for a new configuration).

# **🌐 OVER THE AIR UPDATE (ElegantOTA)**

You use this function when you want to upload code fixes, new features, or a modified program.

### **Step 1: Preparing the program file**

1. Open the project in the Arduino IDE.  
2. In the top menu, select:**Sketch** \-\> **Export compiled program** (Szkic \-\> Export compiled Binary).  
3. The Arduino IDE will create a file with the extension**`.bin`**in your project folder. This is the finished update file.

### **Step 2: Entering the update panel**

1. Make sure your phone/computer is on the same network as the gateway controller.  
2. Open your web browser and add**`/update`**at the end of the controller address:  
   * If you are on a home network:`http://brama40.local/update`  
   * If you are connected directly to the controller's emergency network:`http://192.168.4.1/update`

### **Step 3: Uploading a new program**

1. An elegant, dark panel with the inscription will appear on the screen **ElegantOTA**.  
2. Make sure the option is checked **Firmware**(software).  
3. Click the button**Choose File**(Choose file) and select the previously exported file`.bin`.  
4. Click**Upload**(Submit).  
5. A progress bar will appear on the screen. During the upload, the LED on the controller will glow solid.

### **Step 4: Finishing**

Once it reaches 100%, the panel will display a message**"Update Successful\! Rebooting..."**The controller will automatically restart within a few seconds and launch with the new program. All previously saved Wi-Fi passwords are stored in memory.`Preferences`will be preserved\!

