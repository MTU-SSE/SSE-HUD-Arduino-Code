//This code flashes a HC-05 bluetooth module with new settings.
//See the README.md file for more information on how to use this program.
//This program will not work for the HC-06
void setup() {

  Serial2.begin(38400); // HC-05 default AT mode baud rate
  delay(2000);

  //set the name of the bluetooth device to "GREENTOOTH"
  Serial2.print("AT+NAME=GREENTOOTH\r\n");
  delay(1000);

  //set the password for this device to "0000"
  Serial2.print("AT+PSWD=0000\r\n");
  delay(1000);

  //Set the baud rate for this device to 115200
  Serial2.print("AT+UART=115200,0,0\r\n");
  delay(1000);
}

void loop(){}
