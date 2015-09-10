int led = 13;


const static unsigned char led_pins[16]={3,2,5,4,7,6,A1,A2,10,11,12,A0,8,9,A3,A4};
// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  for(int i=0;i<16;i++)
  {
    pinMode(led_pins[i], OUTPUT);
    digitalWrite(led_pins[i],LOW);
  }  
/*  for(int i=0;i<16;i++)
  {
    digitalWrite(led_pins[i],HIGH);
    delay(50);
  }
  for(int i=0;i<16;i++)
  {
    digitalWrite(led_pins[i],LOW);
    delay(50);
  }*/
  
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  if(Serial.available() >= 3)
  {
    unsigned char cmd=Serial.read();
    unsigned short arg=Serial.read();
    arg <<= 8;arg|=Serial.read();
    switch(cmd)
    {
      case 0x01:
       {
         for(int i=0;i<16;i++)
         {
           digitalWrite(led_pins[i],((arg >> i) & 0x1) ? LOW : HIGH);
         }
         break;
       }
    };
  }
  delay(10);
}

