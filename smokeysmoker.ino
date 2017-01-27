#include <Wire.h>

#include <LiquidCrystal.h>
#include <avr/interrupt.h>
#include <EEPROM.h>


const int WIFI_TX_PIN = 0;
const int WIFI_RX_PIN = 1;

const int RELAY_PIN = 3;

const int RIGHT_BUTTON_PIN = 4;
const int UP_BUTTON_PIN = 5;
const int LEFT_BUTTON_PIN = 6;
const int DOWN_BUTTON_PIN = 7;

const int LCD_D7_PIN = 8;
const int LCD_D6_PIN = 9;
const int LCD_D5_PIN = 10;
const int LCD_D4_PIN = 11;
const int LCD_E_PIN = 12;
const int LCD_RS_PIN = 13;

const int LCD_CONTRAST_PIN = A0;

const int TEMP_1_PIN = A4;
const int TEMP_2_PIN = A5;

//used be EEPROM
const int TEMP_ADDR = 0 ;
const int TIMELEFT_ADDR = 1;
const int UNITS_ADDR = 2;

const int RIGHTARROW = 0;
const int LEFTARROW = 1;
const int UPARROW = 2;
const int DOWNARROW = 3;
const int UPDOWNARROW = 4;


int menu_timeout = 300; // For future use 

String mainMenu[] = { 
  "Change Temp    ",
  "Current Temp   ",
  "Set Timer      ",
  "Time left      ",
  "Change Units   ",
  "Adj. Contrast  " };



int mainMenuPos = 0; // row to display
int mainMenuSize = sizeof(mainMenu)/sizeof(mainMenu[0]);
String currentMenu;

  
LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

byte upDownArrow[8] = {
  B00100,  B01110,  B11111,  B00000,  B00000,  B11111,  B01110,  B00100,
};
byte upArrow[8] = {
  B00100,  B01110,  B11111,  B00000,  B00000,  B00000,  B00000,  B00000,
};
byte downArrow[8] = {
  B00000,  B00000,  B00000,  B00000,  B00000,  B11111,  B01110,  B00100,
};
byte rightArrow[8] = {
  B00000,  B10000,  B11000,  B11100,  B11110,  B11100,  B11000,  B10000,
};
byte leftArrow[8] = {
  B00000,  B00001,  B00011,  B00111,  B01111,  B00111,  B00011,  B00001,
};



void button_down(){
  if (currentMenu == "mainmenu"){
    mainMenuPos = (mainMenuPos + 1) % (mainMenuSize-1);
  } else if (currentMenu == "changetemp"){
    int temperature = EEPROM.read(TEMP_ADDR);
    if (temperature >100 ){
      EEPROM.write(TEMP_ADDR,temperature - 5 );
    }
  }
}

void button_up(){
  if (currentMenu == "mainmenu"){
    if (mainMenuPos == 0){ // Loop around 
      mainMenuPos = mainMenuSize - 1;
    } else {
      mainMenuPos = mainMenuPos - 1;
    }
  } else if (currentMenu == "changetemp"){
    int temperature = EEPROM.read(TEMP_ADDR);
    if (temperature <250 ){
      EEPROM.write(TEMP_ADDR,temperature + 5 );
    }
  } 
}

void button_right(){
  if (currentMenu == "mainmenu"){
    if (mainMenuPos == 0){
      Serial.println("change menu to changetemp");
      currentMenu = "changetemp";
    } else if (mainMenuPos == 1){
      Serial.println("change menu to currenttemp");
      currentMenu = "currenttemp";
    }
  }
}

void button_left(){
  Serial.println("change back to mainmenu");
  currentMenu = "mainmenu";
}


ISR (PCINT2_vect){

  const byte actPins = PIND;
  Serial.println(actPins, BIN);
  if (actPins == 0b11100011 ){
    Serial.println("Right Pressed ");
    button_right();
  }
  if (actPins == 0b11010011 ){
    Serial.println("Top Pressed ");
    button_up();
  }
  if (actPins == 0b10110011 ){
    Serial.println("Bottom Pressed ");
    button_down();
  }
  if (actPins == 0b01110011 ){
    Serial.println("Left Pressed ");
    button_left();
  }

}
//######################################################################################
//######################################################################################



void create_navigation(){
  lcd.createChar(RIGHTARROW, rightArrow);
  lcd.createChar(LEFTARROW, leftArrow);
  lcd.createChar(UPARROW, upArrow);
  lcd.createChar(DOWNARROW, downArrow);
  lcd.createChar(UPDOWNARROW, upDownArrow);
}



void print_main_menu(){
  lcd.setCursor(0, 0);
  lcd.print("Main Menu      ");
  lcd.write(byte(RIGHTARROW));  //Up and down arrows

  lcd.setCursor(0, 1);
  lcd.write(byte(UPDOWNARROW));  //Up and down arrows

  lcd.print(mainMenu[mainMenuPos]);
}



void print_changetemp_menu(){
  lcd.setCursor(0, 0);
  lcd.print("Change Temp     ");
  lcd.setCursor(0, 1);
  
  lcd.write(byte(LEFTARROW));
  lcd.print(" ");
  lcd.print(EEPROM.read(TEMP_ADDR));
  lcd.write(byte(UPDOWNARROW));
  lcd.print("F         ");
  
}

double print_temp(int pin_id){
  int value = analogRead(pin_id); 
  double Temp;
  Temp = log(((10240000/value) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;              // Convert Kelvin to Celsius
  if (EEPROM.read(UNITS_ADDR) == 'F'){
    Temp = (Temp * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit - comment out this line if you need Celsius
  }
  return Temp;
}

void print_currenttemp_menu(){
  lcd.setCursor(0, 0);
  lcd.print("Current Temp   ");
  lcd.setCursor(0, 1);
  lcd.write(byte(LEFTARROW));
  lcd.print(" ");
  lcd.print(print_temp(TEMP_1_PIN)); 
  lcd.print(EEPROM.read(UNITS_ADDR));
  lcd.print(" ");
  lcd.print(print_temp(TEMP_1_PIN)); 
  lcd.print(EEPROM.read(UNITS_ADDR));
  
  lcd.print("   ");
  

}
void print_settimer_menu(){}
void print_timeleft_menu(){}
void print_changeunits_menu(){}
void print_adjcontrast_menu(){}
// the loop routine runs over and over again forever:





//####################### Setup ##########################
void setup() {
  Serial.begin(9600);
  // initialize the digital pin as an output.
  lcd.begin(16, 2);

  create_navigation();
  
  analogWrite(LCD_CONTRAST_PIN, 100); // a sane default.
    
  pinMode(DOWN_BUTTON_PIN,INPUT_PULLUP);
  pinMode(UP_BUTTON_PIN,INPUT_PULLUP);
  pinMode(LEFT_BUTTON_PIN,INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN,INPUT_PULLUP);

  // This should enable the ISR for our buttons
  

  PCMSK2 |= bit (PCINT20);  // want pin 4
  PCMSK2 |= bit (PCINT21);  // want pin 5
  PCMSK2 |= bit (PCINT22);  // want pin 6
  PCMSK2 |= bit (PCINT23);  // want pin 7

    
  PCIFR  |= bit (PCIF2);    // clear any outstanding interrupts
  PCICR  |= bit (PCIE2);    // enable pin change interrupts for D0 to D7
  
  currentMenu = "mainmenu";
  
  if (EEPROM.read(TEMP_ADDR) == 0){
    // Internally always use farenheit since it doesn't need decimals.
  	EEPROM.write(TEMP_ADDR, 250);
  }
  if (EEPROM.read(TIMELEFT_ADDR) == 0){
    // Units are in seconds: 18000 seconds => 300 minutes => 5 hours 
  	EEPROM.write(TIMELEFT_ADDR, 18000);
  }
  if (EEPROM.read(UNITS_ADDR) == 0){
    // Internally always use farenheit since it doesn't need decimals.
  	EEPROM.write(UNITS_ADDR, 'F');
  }
  interrupts();
}



//============ Loop ==============
void loop() {
  
  if (currentMenu == "mainmenu"){
    print_main_menu();
  }else if (currentMenu == "changetemp"){
  	print_changetemp_menu();
    
  }else if (currentMenu == "currenttemp"){
    Serial.println("current temp");
  	print_currenttemp_menu();
    
  }else if (currentMenu == "settimer"){
  	print_settimer_menu();
    
  }else if (currentMenu == "timeleft"){
  	print_timeleft_menu();
    
  }else if (currentMenu == "changeunits"){
  	print_changeunits_menu();
    
  }else if (currentMenu == "adjcontrast"){
  	print_adjcontrast_menu();
  }
  
  
  
}
