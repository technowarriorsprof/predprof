#include <pcmRF.h>
#include <pcmConfig.h>
#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>
#include <EEPROM.h>

#define SD_ChipSelectPin 10  //
#define KN 9
#define ledStop 3  /* PIN LED Stop */
#define MicPin A1  // Аналоговый пин, к которму подключен микрофон



char NameRecord[10];  // Имя нового - записываемого файла на SD-карту
unsigned int RecordNumber;     // Номер записи - храним в EEPROM. в диапазоне от 0 до 32767



unsigned int sample;
unsigned int signalMax = 0;
unsigned int signalMin = 256;
unsigned int peakToPeak = 0;
#define peakToPeakMinLevel 200  // Уровень срабатывания на звук. Значение подбирается индивидуально!
                                // Максимальное значение - 255

TMRpcm audio;  // create an object for use in this sketch

void setup() {
  Serial.begin(9600);
  pinMode(ledStop, OUTPUT);
  //audio.speakerPin = 11; //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
  pinMode(10, OUTPUT);  //Pin pairs: 9,10 Mega: 5-2,6-7,11-12,46-45
  analogReference(EXTERNAL);
  // The audio library needs to know which CS pin to use for recording
  audio.CSPin = SD_ChipSelectPin;
  if (!SD.begin(SD_ChipSelectPin)) {
    digitalWrite(ledStop, LOW);
    Serial.println("Не работает SD");
    return;
  } 
  digitalWrite(ledStop, LOW);

  RecordNumber = EEPROM.read(0);
}

int const STATE_WAIT_FOR_BUTTON = 0;
int const STATE_BUTTON_PRESSED = 1;
int const STATE_RECORDING = 2;
int const STATE_RECORDING_BUTTON_PRESSED = 3;
int button_counter = 0;
int const BUTTON_COUNTER_MAX = 1000;

int state = STATE_WAIT_FOR_BUTTON;

void loop() {
  switch (state) {
  case STATE_WAIT_FOR_BUTTON:
    if (digitalRead(KN)) { state = STATE_BUTTON_PRESSED; button_counter = 0; }
    break;

  case STATE_BUTTON_PRESSED:
    if (not digitalRead(KN)) { state = STATE_WAIT_FOR_BUTTON; }
    else { button_counter += 1; }

    if (button_counter >= BUTTON_COUNTER_MAX) { 
      Serial.print("Start recording! File: ");
      Serial.print(RecordNumber + 1);
      Serial.println(".wav");
      
      state = STATE_RECORDING; 
      StartRec();
    }

    break;

  case STATE_RECORDING:
    if (digitalRead(KN)) { state = STATE_RECORDING_BUTTON_PRESSED; button_counter = 0; }

    break;

  case STATE_RECORDING_BUTTON_PRESSED:
    if (not digitalRead(KN)) { state = STATE_RECORDING; }
    else { button_counter += 1; }

    if (button_counter >= BUTTON_COUNTER_MAX) { 
      Serial.print("Start recording! File: ");
      Serial.print(RecordNumber + 1);
      Serial.println(".wav");

      state = STATE_WAIT_FOR_BUTTON; 
      StopRec();
    }

    break;
  }



  /*while (not digitalRead(KN)) { continue; }
  delay(500);
  Serial.print("Start recording! File: ");
  Serial.print(RecordNumber + 1);
  Serial.println(".wav");
  digitalWrite(ledStop, HIGH);
  StartRec();
  delay(1000);
  while (not digitalRead(KN)) {}
  analogRead(MicPin);
  StopRec();
  delay(500);*/
}



void StartRec() {  // begin recording process
  digitalWrite(ledStop, HIGH);
  RecordNumber++;
  EEPROM.write(0, RecordNumber);                // Сохранение в EEPROM номера последнего аудиофайла
  sprintf(NameRecord, "%d.wav", RecordNumber);  // создаем название файла из номера и расширения ".wav"
  
  audio.startRecording(NameRecord, 16000, MicPin, 0);  // Старт записи
  Serial.println("START");
}

void StopRec() {                    // stop recording process, close file
  audio.stopRecording(NameRecord);  // Стоп записи
  digitalWrite(ledStop, LOW);

  Serial.println("END");
}
