#include "WiFi.h"
#include <BleMouse.h>
#include <classes.h>


// GPIOs numbers on ESP32
const int XPin    = 36;
const int YPin    = 39;
const int PushButton  = 34;

const int LED1 = 18;
const int LED2 = 5;
const int LED3 = 4;

int mouse_range = 12; // the max of the mouse change

void test_joystick();
void print_it();

/*
class joystic {
  int range = 20;


  public:
  int x_min_zero = 7777; // the min x still considered zero (becasue joystic is not 100% calibrated)
  int x_max_zero = -777; // the max x still considered zero
  int y_min_zero = 7777; // the min y still considered zero
  int y_max_zero = -777; // the max y still considered zero
  int Xval = 0; // the final calibrated X Y (from -10 to +10) - range value
  int Yval = 0;
  int prev_x ; // keep previous joystic state, transmit only changes
  int prev_y ;
  bool Push_button_state = false;
  bool ON_state = false ;
  bool change_occured; // is this read different than previous?
  

  void estimate_joystic_zeros() {
    for (size_t i = 0; i < 100; i++)
    {
      
      int rdX = analogRead(XPin);
      int rdY = analogRead(YPin);
      if (rdX > x_max_zero)
        x_max_zero = rdX;
      if (rdY > y_max_zero)
        y_max_zero = rdY;
      if (rdX < x_min_zero)
        x_min_zero = rdX;
      if (rdY < y_min_zero)
        y_min_zero = rdY;
    } // of for loop
    
    // add/sub 10 for safety
    x_min_zero -= range; 
    x_max_zero += range; 
    y_min_zero -= range; 
    y_max_zero += range;




  } // of estimate_joystic_zeros

  void read_jostick() {
    prev_x = Xval;
    prev_y = Yval;

    
    int t_rdX = analogRead(XPin);
    int t_rdY = analogRead(YPin);
   
    int butt_tmp =  digitalRead(PushButton);

    if (butt_tmp == HIGH) {
      if (ON_state == true )
        ON_state = false;
      else
        ON_state = false;
    }

    Xval = 0;    
    if (t_rdX < x_min_zero ) {
      Xval = map(t_rdX,0,x_min_zero,-mouse_range,0); }
      else if (t_rdX > x_max_zero ) {
        Xval = map(t_rdX,x_max_zero,4095,0,mouse_range); }

    Yval = 0;
    if (t_rdY < y_min_zero ) {
      Yval = map(t_rdY,0,y_min_zero,-mouse_range,0); }
      else if (t_rdY > y_max_zero ) {
        Yval = map(t_rdY,y_max_zero,4095,0,mouse_range); }
    
    if ((Xval != prev_x) || (Yval != prev_y)) {
      change_occured = true;
      Serial.print("  ------    in class, read_jostick    ");
      print_it();
    }      
    else
      change_occured = false;
  }
}; // of class JOYSTICK
*/



joystic tank_joystick;
BleMouse bleMouse;
void test_hw();

// *** start SETUP
void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup, testing leds");
  test_hw();
  Serial.println("Test Joystick");
  test_joystick();

  pinMode(PushButton, INPUT_PULLUP); // GPIO as input // Problem - not a good GPIO, no pullup. 
  pinMode(LED1, OUTPUT); 
  pinMode(LED2, OUTPUT); 
  pinMode(LED3, OUTPUT); 

  // Before start, get X & Y values to be considered as zero,
  // due to non exact readings
  // then translate X & Y  into values from -10 to +10

  tank_joystick.set_hw(XPin,YPin,PushButton,mouse_range);
  tank_joystick.estimate_joystic_zeros();

  bleMouse.begin();
  Serial.println("Starting BT");
  while (!bleMouse.isConnected()){
    Serial.print(".");
    delay(200);
  }
  Serial.println("Connected");

} // of SETUP



void loop() {


/*
   if(bleMouse.isConnected()) {
    Serial.println("Scroll Down");
    bleMouse.move(0,0,-1);
  }
  delay(500);
  return;
*/

// just for testing
//test_joystick();



  tank_joystick.read_jostick();
  bleMouse.move(tank_joystick.Xval,tank_joystick.Yval,0);

  

  /*
  if (it eas clicked)
    bleMouse.click(MOUSE_LEFT);
*/

  delay(100);

    return;





  // print_it();

  //if ((tank_joystick.Xval!=tank_joystick.prev_x) || (tank_joystick.Yval!=tank_joystick.prev_y)) {
  if (tank_joystick.change_occured) {
    Serial.print(" in loop, change occured ------>    ");
    print_it();
   
  } // of long if


  //delay(300);
  unsigned long time_now = 0;
  int period = 0;
  time_now = millis();      
  while(millis() < time_now + period){
        //wait approx. [period] ms
  }
  


} // of loop






void test_joystick() {
  // read endlessly joystick input, for testing
  int mid = 2047;
  int max_delta_x = 0;
  int max_delta_y = 0;

  Serial.println("don't touch at first - to calibrate drift ");

  while (true) {
    int t_rdX = analogRead(XPin);
    int t_rdY = analogRead(YPin);
    int butt_tmp =  digitalRead(PushButton);

    if (abs(t_rdX-mid) > max_delta_x )
      max_delta_x = abs(t_rdX-mid);

    if (abs(t_rdY-mid) > max_delta_y )
      max_delta_y = abs(t_rdY-mid);

    Serial.print("   X: ");
    Serial.print(t_rdX);
    Serial.print("   Y: ");
    Serial.print(t_rdY);
    Serial.print("  SW: ");
    Serial.print(butt_tmp);
    Serial.print("       Max X drift:  ");
    Serial.print(max_delta_x);
    Serial.print("       Max Y drift:  ");
    Serial.println(max_delta_y);

    delay(300);
  }
} // of test_joystick


void print_it() {
  // DEBUG
  if (tank_joystick.change_occured) {
    Serial.print(" change!   ");  
  }
  else {
    Serial.print("           ");  
  }
  Serial.print("X/Y     ");
  Serial.print(tank_joystick.Xval);
  Serial.print(" / ");
  Serial.print(tank_joystick.Yval);
   Serial.print("      prevX/PrevY   ");
  Serial.print(tank_joystick.prev_x);
  Serial.print(" / ");
  Serial.println(tank_joystick.prev_y);
} // of print_it


void test_hw() {
  for (int i=0;i<3;i++) {
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,LOW);
    digitalWrite(LED3,LOW);
    delay(400);
    digitalWrite(LED1,HIGH);
    delay(400);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED1,LOW);
    delay(400);
    digitalWrite(LED3,HIGH);
    digitalWrite(LED2,LOW);
    delay(400);
  }
  digitalWrite(LED3,LOW);
  

}