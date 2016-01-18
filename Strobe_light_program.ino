const int light1pin = 12;
const int laser = 8;
const int potpin = A1;
const int auxpin = A0;
int fspeed, scaledspeed, aux, auxscaled;  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
int ptr=0;
#define AVG_VALS 1
#define CUTOFF 20
int vals[AVG_VALS];
int i;
int avg;
void setup () {
//Serial.begin(9600);
}

void loop () {
  fspeed = analogRead(potpin);
  scaledspeed = fspeed/2;
  aux = analogRead(auxpin);
  auxscaled = aux/5;
  vals[ptr] = aux;
  ptr++;
  ptr %= AVG_VALS;
  avg=0;
  for(i=0; i<AVG_VALS; i++){
    avg+= vals[i];
  }
  avg/=AVG_VALS;
  //Serial.println(avg);
  if(avg>CUTOFF) {
    digitalWrite(laser, HIGH);
    //Serial.println("laser is on");
  }
  else if(avg<=CUTOFF) {
    digitalWrite(laser, LOW);
    //Serial.print("laser is off");
  }
  //delay(20);
  /*
  digitalWrite(light1pin, HIGH);
  digitalWrite(light2pin, HIGH);
  delay(scaledspeed);
  digitalWrite(light1pin, LOW);
  digitalWrite(light2pin, LOW);
  Serial.println(brightness);
  delay(scaledspeed);
  */

  
}
