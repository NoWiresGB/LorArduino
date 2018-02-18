// LoRa 9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example LoRa9x_RX


// Simple I2C test for ebay 128x64 oled.
// Use smaller faster AvrI2c class in place of Wire.
//

#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Setup dislpay
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

SSD1306AsciiAvrI2c oled;
//------------------------------------------------------------------------------
void setup() {   
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(Adafruit5x7);

  #if INCLUDE_SCROLLING == 0
  #error INCLUDE_SCROLLING must be non-zero. Edit SSD1306Ascii.h
  #elif INCLUDE_SCROLLING == 1
  // Scrolling is not enable by default for INCLUDE_SCROLLING set to one.
  oled.setScroll(true);
  #else  // INCLUDE_SCROLLING
  // Scrolling is enable by default for INCLUDE_SCROLLING greater than one.
  #endif


  uint32_t m = micros();
  oled.clear();  
  oled.set2X();
  oled.println("LORA Tx");
  oled.set1X();
        
  //radio setup
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Arduino LoRa TX Test!");
  oled.println("LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    oled.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
  oled.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    oled.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  oled.print("Set Freq to: "); oled.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
//  rf95.setTxPower(23, false);
  rf95.setTxPower(5, false);
    

}
//------------------------------------------------------------------------------

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop() {
  Serial.println("Sending to rf95_server");
  oled.println("Tx to rf95_server");
  // Send a message to rf95_server
  
  char radiopacket[20] = "Hello World #      ";
  itoa(packetnum++, radiopacket+13, 10);
  Serial.print("Sending "); Serial.println(radiopacket);
  oled.print("TX: "); oled.println(radiopacket);
  radiopacket[19] = 0;
  
  Serial.println("Sending..."); oled.println("Sending...");delay(10);
  rf95.send((uint8_t *)radiopacket, 20);

  Serial.println("Waiting for packet to complete..."); oled.println("Wait TX to complete..."); delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply..."); oled.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: "); oled.print("Got reply: ");
      Serial.println((char*)buf); oled.println((char*)buf);
      Serial.print("RSSI: "); oled.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC); oled.println(rf95.lastRssi(), DEC);     
    }
    else
    {
      Serial.println("Receive failed"); oled.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
    oled.println("No reply.");
  }
  delay(1000);
}
