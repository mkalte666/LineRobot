#include<Wire.h>

#define F_CPU 16000000

 int a;
 int b;
 int c;
 int d;
 int e;
 int f;


void SetFlag(unsigned char* dst, unsigned char flag)
{
  *dst |= (1<<flag);
}

void ResetFlag(unsigned char* dst, unsigned char flag)
{
  *dst &= ~(1<<flag);
}

boolean CheckFlag(unsigned char* dst, unsigned char flag)
{
  return (*dst & flag);
}

void setup() 
{
 Wire.begin();
 Serial.begin(9600); 
 pinMode(24, INPUT); 
 pinMode(25, INPUT);
 pinMode(26, INPUT);
 pinMode(27, INPUT);
 pinMode(6,OUTPUT);
 pinMode(7,OUTPUT);
 pinMode(8,OUTPUT);
 pinMode(9,OUTPUT);
 pinMode(14,OUTPUT);
 }

void raw_drive(char l, char r, unsigned char spd)
{
  digitalWrite(8, l);
  digitalWrite(9, r);
  analogWrite(6, spd);
  analogWrite(7, spd);
}

void fahren(int x,int y)       // x = 0..6 , y = 0 .. 255
{
  
  switch (x)
  {
    case 0:                    //Motoren stop
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      analogWrite(6, 0);
      analogWrite(7, 0);
      break;
    case 1:                    //Motoren vorwärts
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      analogWrite(6, y);
      analogWrite(7, y);
      break; 
    case 2:                    //Motoren rückwärts
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      analogWrite(6, y);
      analogWrite(7, y);
      break;
    case 3:                    //Lenken links schnell
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
      analogWrite(6, y);
      analogWrite(7, y);
      break;  
    case 4:                    //Lenken rechts schnell
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
      analogWrite(6, y);
      analogWrite(7, y);
      break;  
    case 5:                    //Lenken links langsam
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
      analogWrite(6, 0);
      analogWrite(7, y);
      break;  
    case 6:                    //Lenken rechts langsam
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
      analogWrite(6, y);
      analogWrite(7, 0);
      break;    
   }
 }

unsigned char parse_linesensors()
{
  unsigned char result = 0;
  (LOW == digitalRead(24)) ? SetFlag(&result, 0) : ResetFlag(&result, 0);
  (LOW == digitalRead(25)) ? SetFlag(&result, 1) : ResetFlag(&result, 1);
  (LOW == digitalRead(26)) ? SetFlag(&result, 2) : ResetFlag(&result, 2);
  (LOW == digitalRead(27)) ? SetFlag(&result, 3) : ResetFlag(&result, 3);
  return result;
}

void srf02()
{
  Wire.beginTransmission(112);    //entspricht Adresse 0xE0
  Wire.write(byte(0x00));         //Befehlsregister 0x00
  Wire.write(byte(0x51));         //Werte in cm
  Wire.endTransmission();
  delay(70);
  Wire.beginTransmission(112);    //entspricht Adresse 0xE0
  Wire.write(byte(0x02));         //Befehlsregister 0x00
  Wire.endTransmission();
  Wire.requestFrom(112,2);        //2 bytes von Adresse 112
  if (2<=Wire.available())
  {
    f  = Wire.read();
    f  = f<<8;
    f |= Wire.read();
  }
}

void loop() 
{
  unsigned char flags = parse_linesensors();
  srf02();
  Serial.println(flags);
  // delay(150);           //wegen Anzeige im Hyperterminal
  /*switch (flags)
  {
    case 0:
     fahren(1,60);
     break;
   case 1:
     fahren(6,60);
     break;
   case 2:
     fahren(6,60);
     break;  
   case 3:
     fahren(6,60);
     break;
   case 4:
     fahren(5,60);
     break; 
   case 8:
     fahren(5,60);
     break; 
   case 12:
      fahren(5,60);
     break;    
   default:
     fahren(1,60);
     break;  
  }  
  */
  
}


