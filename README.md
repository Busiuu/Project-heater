# Projekt Regulacji Temperatury Grzałki

Ten projekt przedstawia kompleksowe rozwiązanie regulacji temperatury grzałki z wykorzystaniem mikrokontrolera STM32, programowania w Matlab oraz graficznego interfejsu użytkownika (GUI).

## Kluczowe elementy projektu

### 1. **Modelowanie procesu**
- Opracowanie matematycznego modelu obiektu w Matlab z wykorzystaniem metody ARMAX.
- Analiza i porównanie modeli z rzeczywistym procesem w celu wybrania najbardziej odpowiedniego modelu.

### 2. **Dobór regulatora PI**
- Stworzenie układu regulacji w Matlab Simulink.
- Optymalizacja parametrów regulatora PI (Kp=5000, Ki=400) z uwzględnieniem efektu wind-up.

### 3. **Implementacja mikrokontrolera STM32**
- Obsługa czujnika BMP280 (SPI) oraz enkodera.
- Generowanie sygnałów PWM do sterowania grzałką i wentylatorem.
- Dwukierunkowa komunikacja UART do przesyłania danych i sterowania temperaturą zadaną.

### 4. **Graficzny interfejs użytkownika (GUI)**
- Stworzony w STM32CubeMonitor z wykorzystaniem Node-Red.
- Wizualizacja danych w czasie rzeczywistym oraz możliwość ich zapisu.
- Interaktywne sterowanie temperaturą zadaną za pomocą UART.

## Efekt końcowy
- Stabilny układ regulacji temperatury o wysokiej precyzji.
- Szybki czas reakcji (1% uchybu w 75 sekundach przy zadanej temperaturze 60°C).
- Maksymalne przeregulowanie wynosiło jedynie 3%.

## Zasoby
- Kod projektu: [Project-heater na GitHub](https://github.com/Busiuu/Project-heater)
- Wymagania sprzętowe: Mikrokontroler STM32, czujnik BMP280, enkoder, grzałka, wentylator.

## Dokumentacja
Więcej szczegółów znajduje się w [dokumentacji projektu](https://github.com/Busiuu/Project-heater).

PS. Proszę zignorować pliki .DS_Store, są to pliki odpowiadające za wygląd folderu w systemie macOS. 
