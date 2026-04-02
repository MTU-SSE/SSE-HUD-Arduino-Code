
void setup() {
Serial2.begin(38400); // HC-05 default AT mode baud rate
delay(2000);

Serial2.print("AT+NAME=GREENTOOTH\r\n");
delay(1000);

Serial2.print("AT+PSWD=0000\r\n");
delay(1000);

Serial2.print("AT+UART=115200,0,0\r\n"); // Set to 115200 baud
delay(1000);
}

void loop(){}
