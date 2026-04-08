#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(32);

//IMPORTANT
//the Serial port defined after good_serial
//is the port that all data read from the CAN network will be sent to
//  "Serial" is the USB output of the arduino
//  "Serial2" is the bluetooth module (if it is connected)
#define good_serial Serial2

//lap button pin
int lap_pin = 23;

void setup() {
  Serial.begin(115200);
  //this higher baud rate can only be used with bluetooth modules flashed with
  //the flash_bluetooth_settings program
  Serial2.begin(115200);

  //This sets an interrupt on the lap button pin
  //This means that when the arduino detects a RISING edge on that pin
  //it will pause what it's currently doing, run lap_interrupt()
  //and then when that is done it will resume what it was previously doing
  attachInterrupt(digitalPinToInterrupt(lap_pin), lap_interrupt, RISING);
  
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
  // Human Readable (Values printed in hex)
    int res = mcp2515.readMessage(&canMsg);
    if (res == MCP2515::ERROR_OK) {
      //filter out packets from the O2 sensor
      if (canMsg.can_id == 0x460) {
        return;
      }
      //list of allowed packets (data we actually want)
      //see this doc for more info: https://docs.google.com/spreadsheets/d/1KqUCvgxujewzqeYJbRDF5o1ScjHsWxG1/edit?usp=sharing&ouid=108212624486532605840&rtpof=true&sd=true
      //note: I don't think we get the 0x118 packet because we do not have a PDM
      if (canMsg.can_id == 0x640 ||
          canMsg.can_id == 0x64C ||
          canMsg.can_id == 0x641 ||
          canMsg.can_id == 0x648 ||
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
    // Causes the arduino to send out test data to verify that its connection to the HUD display is stable. This should cause the "RPM" value to rapidly increase
    //test_data();
    
    // This is reads the lap button from the flag that the interrupt sets
    // and sends an appropriate signal to the HUD display.
    // Uses software debouncing
    lap_button_update();

    // This code works. It will calculate wheel speed each time a magnet passes the sensor, and send that updated speed to the HUD display
    //wheel_speed_update();
}

//flag for if the button was pressed (set by interrupt)
//0 is false, 1 is true
int lap_button_pressed = 0;
//timestamp of when the last sucessful press happened for software debounce
long last_press_time = 0;
//how long the debounce should wait (in ms)
//should be longer than the press->release time of the button
//to catch falling edge debounce
const int DEBOUNCE_WAIT = 2000;
//count of how many laps we have done
//(unlikely to be useful as it will reset when the arduino is power cycled
int laps = 0;

//This function gets called as an interrupt when the lap button is pressed.
//It is important that is does as little as possible
//with most of the processing being handled in the main loop
void lap_interrupt() {
  lap_button_pressed = 1;
}

//TODO: COMMENT
void lap_button_update() {
  if (lap_button_pressed == 1) {
    lap_button_pressed = 0;
    //TODO: COMMENT
    if (millis() - last_press_time > DEBOUNCE_WAIT) {
      last_press_time = millis();
      good_serial.print("[1] ");
      good_serial.print(2, HEX);
      good_serial.print(" ");
      good_serial.print(1, HEX);
      good_serial.print("\n");
    }
  }
  
  //TODO:change to check the pin, likely need to do some form of debounce or interrupt here
  //and increment the lap counter
  if (false) {
    good_serial.print("[1] ");
    good_serial.print(2, HEX);
    good_serial.print(" ");
    good_serial.print(laps, HEX);
    good_serial.print("\n");
  }
}

//wheel speed sensor code
//edit these constants for the wheel parameters
const int hallEffectPin = A0; //arduino pin connected to the hall effect sensor
const float WHEEL_DIAMETER = 16.5; // the diameter of the wheel being read in inches
const int NUMBER_OF_MAGNETS_ON_WHEEL = 2; //the number of EQUALLY SPACED magnets on the wheel
                                          //more magnets will give increased accuracy
                                          //if not equally spaced, values will be inaccurate

const int MAGNET_PRESENT_THRESHOLD = 480; //a measured value above which a magnet is for sure present
const int MAGNET_LEFT_THRESHOLD = 500; //a measured value below which a magnet is for sure no longer present
//note that using the negative side of the magnet will push the reading in the opposite direction
//all magnets need to have the same alignment for this system to work properly

//An easy way to measure what these values should be is to add Serial.println(analogRead(hallEffectPin)) to the main arduino loop

//Used to make the code tracking hall effect state more readable
//(budget enums)
const int MAG_HIGH = 0;
const int MAG_LOW = 1;
int state = MAG_LOW;
int prev_hall_reading = 0;

const float circumference = PI * WHEEL_DIAMETER;
//calculate the distance between each magnet along the wheel rim
const float mag_dist = circumference / NUMBER_OF_MAGNETS_ON_WHEEL;

//Get a new reading from the hall effect sensor
//compare that reading to the mag present and 
void wheel_speed_update() {
  int mag_val = analogRead(hallEffectPin);
  if (mag_val < MAGNET_PRESENT_THRESHOLD && state == MAG_LOW) {
    state = MAG_HIGH;
  }
  else if (mag_val > MAGNET_LEFT_THRESHOLD && state == MAG_HIGH) {
    state = MAG_LOW;
    float wheelspeed = mag_passed_speed();
    Serial.print("[1] ");
    Serial.print(1, HEX);
    Serial.print(" ");
    Serial.print(wheelspeed, HEX);
    Serial.print("\n");
  }
}

//returns a speed in miles/hr
//called each time a magnet passes the sensor
float mag_passed_speed() {
  int now = millis();
  //calculate speed in in/ms and convert to mi/hr
  float result = (mag_dist / (double) (now - prev_hall_reading)) * 56.818;
  prev_hall_reading = now;
  return result;
}


int number = 0;

//sends test data to the HUD display
//This should cause the "RPM" data value to rapidly increase
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
