/*
   아두이노 나노, 프로미니, 메가 등에서 사용할 수 있는 샘플코드 입니다.
   다른 MCU에서 사용할 시에는 본 코드에 구현된 소프트웨어시리얼 쪽을
   사용하시는 MCU에  맞춰서 변경해서 사용하시면 됩니다.
   자세한 내용은 코드 라인별 주석 참고해주시고, 별도의 문의 사항이 있으시면
   아래의 이메일로 문의 부탁드립니다.
   담당자 이메일: ykkim@exsen.co.kr, 김용광 팀장

   This code is provided by EXSEN
   You can ask this via email (ykkim@exsen.co.kr)
   tested B/D: Arduino Promini, Nano, Uno, Mega 2580
   if you want to use this code to another board,
   check the softwareserial part,
   you can edit that part according to your MCU.
*/

#include "SoftwareSerial.h"
// 소프트웨어 시리얼의 사용
// Use softwareserial to connect the CO2 sensor via Promini or nano, uno
// Promini and nano, uno don't have enough hardware serial port.
// so connect Rx, Tx of hardware serial port to PC to debug,
// connect Software Serial to CO2 sensor

SoftwareSerial N1_Serial(4, 5); // Rx: 4, Tx 5 connect Tx of sensor to PIN 4, Rx of sensor to PIN 5
/* if you use that pin already to other purpose, you can use other pin
   Digital Pin: 2,3,4,5,6,7,8,9,12,13 was tested to software serial
*/

byte read_co2[] = {0x42, 0x4d, 0xe3, 0x00, 0x00, 0x01, 0x72};   //co2 value reading command
byte reset_sensor[] = {0x43, 0x57, 0x4D, 0x00, 0x00, 0x00, 0xE7}; //co2 reset command

//data
int FB_CO2[12] = {0,};
int data_ppm = 0;

// Timing
unsigned long current_time = 0;
unsigned long prev_time = 0;
int set_time = 1; //every 1 sec

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // arduino to PC
  N1_Serial.begin(9600); //arduino to CO2 sensor, 9600 bps
}

void loop() {
  // put your main code here, to run repeatedly:
  current_time = millis() / 1000; // 1 second count up
  if (current_time - prev_time >= set_time) { // every 1 second
    N1_data_read();
    //N1_reset(); // if you make sure the circumstance is clear, use N1_reset code to make sensor output value as clear once.
    data_print();
    prev_time = current_time;
  }
}

void N1_data_read() {
  int sum = 0;
  N1_Serial.setTimeout(100); // you can ignore this code
  int timeout = 0;
  while (!N1_Serial.available()) {
    timeout++;
    N1_Serial.write(read_co2, 7);
    delay(1);
    if (timeout > 10) {
      break;
    }
    else {
      //do nothing
    }
  }
  //N1_Serial.listen(); // if you connect the more sensor (>= 2pcs) via software serial, use this code
  for (int i = 0; i < 12; i++) {
    FB_CO2[i] = N1_Serial.read();
    sum = sum + FB_CO2[i];
    delay(1);
  }
  sum = sum - FB_CO2[10] - FB_CO2[11];
  if (sum == ((FB_CO2[10] * 256) + FB_CO2[11])) {
    data_ppm = FB_CO2[4] * 256 + FB_CO2[5];
  }
}

void N1_reset() {
  N1_Serial.setTimeout(100);
  int timeout = 0;
  while (!N1_Serial.available()) {
    timeout++;
    N1_Serial.write(reset_sensor, 7);
    delay(1);
    if (timeout > 10) {
      break;
    }
  }
}

void data_print() {
  Serial.print("CO2: ");
  Serial.println(data_ppm);
}
