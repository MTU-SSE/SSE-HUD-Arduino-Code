#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(32);

//wheel speed sensor
//int hallEffectPin = A0;
//int counter = 0;
//int MAG_HIGH = 0;
//int MAG_LOW = 1;
//int state = MAG_LOW;

//wheel diameter (in)
//float wheel_diameter = 16.5;
//float circumference = PI * wheel_diameter;
////calculate the distance between each magnet along the wheel rim
//float mag_dist = circumference / 2;

//int lap_pin = D26;
int laps = 0;

int number = 0;

#define good_serial Serial2

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  // if (!SD.begin(38)) {
  //   Serial.println("Something went wrong initalizing SD card!");
  // }
  
  mcp2515.reset();
  int res = mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ);
  if(res != MCP2515::ERROR_OK) {
    Serial.println("Something went wrong setting bitrate!");
  }
  
  if(mcp2515.setNormalMode() != MCP2515::ERROR_OK) {
    Serial.println("Something went wrong setting normal mode!");
  }


  //Serial.println("------- CAN Read ----------");
  //Serial.println("ID  DLC   DATA");

  pinMode(13, OUTPUT);

}

void loop() {
  //Serial2.print("Hello\n");
  // Human Readable (Values printed in hex)
    int res = mcp2515.readMessage(&canMsg);
    if (res == MCP2515::ERROR_OK) {
      if (canMsg.can_id == 0x460) {
        return;
      }
      if (canMsg.can_id == 0x118 ||
          canMsg.can_id == 0x640 ||
          canMsg.can_id == 0x64C ||
          canMsg.can_id == 0x641 ||
          canMsg.can_id == 0x649) {
        good_serial.print("[");
        good_serial.print("1] "); 
        good_serial.print(canMsg.can_id, HEX); // print ID
        // Serial.print(canMsg.can_dlc, HEX); // print DLC
        // Serial.print(" ");
        
        for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
          good_serial.print(" ");
          good_serial.print(canMsg.data[i],HEX);
        }

        good_serial.print("\n");      
      }
    }
    
    //test_data();

    //lap_button_update();
    //wheel_speed_update();
}

void lap_button_update() {
  if (false) {
    good_serial.print("[1] ");
    good_serial.print(2, HEX);
    good_serial.print(" ");
    good_serial.print(laps, HEX);
    good_serial.print("\n");
  }
}

int prev_hall_reading = 0;

//check the hall effect sensor for change in reading
//void wheel_speed_update() {
//  int mag_val = analogRead(hallEffectPin);
//  if (mag_val < 480 && state == MAG_LOW) {
//    state = MAG_HIGH;
//  }
//  else if (mag_val > 500 && state == MAG_HIGH) {
//    state = MAG_LOW;
//    float wheelspeed = mag_passed_speed();
//    Serial.print("[1] ");
//    Serial.print(1, HEX);
//    Serial.print(" ");
//    Serial.print(wheelspeed, HEX);
//    Serial.print("\n");
//  }
//}
//
////returns a speed in miles/hr
//float mag_passed_speed() {
//  int now = millis();
//  //calculate speed in in/ms and convert to mi/hr
//  float result = (mag_dist / (double) (now - prev_hall_reading)) * 56.818;
//  prev_hall_reading = now;
//  return result;
//}

void test_data() {
   if (number % 50 == 0) {
    number = 0;
   }
   good_serial.print("[");
   good_serial.print("1");
   good_serial.print("] ");
   good_serial.print("640 ");
   good_serial.print(number, HEX);
   good_serial.print(" ");
   number++;
   good_serial.print(number, HEX);
   good_serial.print(" ");
   number++;
   good_serial.print(number, HEX);
   good_serial.print(" ");
   number++;
   good_serial.print(number, HEX);
   good_serial.print(" ");
   number++;
   good_serial.print(number, HEX);
   good_serial.print(" ");
   number++;
   good_serial.print(number, HEX);
   good_serial.print(" ");
   number++;
   good_serial.print(number, HEX);
   good_serial.print(" ");
   number++;
   good_serial.print(number, HEX);
   number++;

   good_serial.print("\n");
}
