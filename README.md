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

