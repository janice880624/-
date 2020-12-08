#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Keypad.h>
#include <FastLED.h>
/*-------Define PIN------*/
#define PULX 2
#define DIRX 3
#define ENAX 4
#define FLASH1 5
#define SETTING 6
#define START 7
#define Xcomplete 8
#define Ycomplete 9
#define Zcomplete 10
#define RESET 12
#define COMPLETE 11
#define HARDRESET 13
#define XLIMIT A0
#define NUM_LEDS 16
#define PULX_HIGH         PORTE |=  0b00010000;
#define PULX_LOW          PORTE &= ~0b00010000;
byte colPins[4] = {44, 42, 40, 38}; //鍵盤腳位
byte rowPins[4] = {52, 50, 48, 46};//鍵盤腳位
///LCD腳位(SDA,SCL,5V,GND)
int dtl[50] = {440, 297, 228, 191, 164, 143, 127, 117, 106, 101, 95, 90, 85, 80, 74, 69, 67, 66, 65, 64, 62, 60, 58, 56, 54, 53, 51, 49, 48, 46, 44, 43 , 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 30, 30, 30, 30, 30};
/*-------End define PIN------*/
/*-------Define Variable-------*/
char keymap[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
String datasend;
String datareceive;
String level;
int MaxSpeed;
String grid;
String numobj;
String typerobot;
String typeplayer;
int rampuptime = 20;
int STEPX[12];
String stringX[12];
int remainstep;
int check;

/*-------End define variable-------*/
CRGB leds[NUM_LEDS];
LiquidCrystal_I2C lcd(0x27, 20, 4); //設定LCD位置0x27,設定LCD大小為16*2
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, 4, 4);
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(15);
  FastLED.addLeds<WS2811, FLASH1, RGB>(leds, NUM_LEDS);
  /*-------Setup PIN Mode------*/
  pinMode(FLASH1 , OUTPUT);
  pinMode(Zcomplete, INPUT);
  pinMode(Xcomplete, OUTPUT);
  pinMode(PULX , OUTPUT);
  pinMode(DIRX , OUTPUT);
  pinMode(ENAX , OUTPUT);
  digitalWrite(Xcomplete, LOW);
  pinMode(SETTING , INPUT_PULLUP);
  pinMode(START , INPUT_PULLUP);
  pinMode(HARDRESET , INPUT_PULLUP);
  pinMode(COMPLETE , INPUT_PULLUP);
  pinMode(RESET , INPUT_PULLUP);
  pinMode(XLIMIT , INPUT_PULLUP);
  digitalWrite(ENAX, HIGH);
  while (Read(RESET) == 1);
  Xreturn();
  /*-------End setup PIN Mode------*/
}
void loop() {
  check = 0;
  //  /*-------LCD Welcome------*/
  lcd.init();
  lcd.backlight();
  while (Read(SETTING) != 0) //;
  {
    lcd.setCursor(0, 0);
    lcd.print("Welcome to matching game!");
    lcd.setCursor(0, 1);
    lcd.print("Press <Setting> to begin now!");
    lcd.scrollDisplayLeft();
    delay(300);
  }
  /*-------End LCD Welcome------*/
  /*-------Get game MaxSpeed------*/
  String line1 = "Choose game diffi";
  String line2 = "culty";
  level = getinput(line1, line2, 1, 10);
  lcd.clear();
  lcd.print("MaxSpeed=");
  lcd.print(level);
  /*-------End get game MaxSpeed------*/
  delay(1000);
  /*-------Get grid number------*/
  line1 = "Input number of";
  line2 = "grid";
  grid = getinput(line1, line2, 3, 5);
  lcd.clear();
  lcd.print("Grid=");
  lcd.print(grid);
  /*-------End get grid number------*/
  delay(1000);
  /*-------Get number of object------*/
  line1 = "Input number of";
  line2 = "object";
  numobj = getinput(line1, line2, 3, 5);
  lcd.clear();
  lcd.print("Num of Object=");
  lcd.print(numobj);
  /*-------End get number of object------*/
  delay(1000);
  /*-------Get type of object for robot arm------*/
  line1 = "Input type for";
  line2 = "robot";
  typerobot = getinput(line1, line2, 1, 6);
  lcd.clear();
  lcd.print("Type for robot=");
  lcd.print(typerobot);
  /*-------End get type of object for robot arm------*/
  delay(1000);
  /*-------Get type of object for robot arm------*/
  line1 = "Input type for";
  line2 = "player";
  typeplayer = getinput(line1, line2, 1, 6);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Type for");
  lcd.setCursor(3, 1);
  lcd.print("player=");
  lcd.print(typeplayer);
  /*-------End get type of object for robot arm------*/
  delay(1000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Game setting");
  lcd.setCursor(3, 1);
  lcd.print("complete!");
  delay(1000);
  /*-------Send data to Matlab-------*/
  datasend = level + " " + grid + " " + numobj + " " + typerobot + " " + typeplayer;
  MaxSpeed = level.toInt();
  if (MaxSpeed <= 6)
  {
    MaxSpeed = MaxSpeed - 1;
  }
  else if (MaxSpeed == 7)
  {
    MaxSpeed = 9;
  }
  else if (MaxSpeed == 8)
  {
    MaxSpeed = 13;
  }
  else if (MaxSpeed == 9)
  {
    MaxSpeed = 27;
  }
  else
  {
    MaxSpeed = 49;
  }
  Serial.println(datasend);
  lcd.clear();
  lcd.print("Sendding data");
  for (int j = 1; j < 4; j++) {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    String dot = ".";
    for (int i = 0; i < 16; i++) {
      lcd.setCursor(0, 1);
      lcd.print(dot);
      dot = dot + ".";
      delay(50);
    }
  }
  /*-------End send data to Matlab-------*/
  lcd.clear();
  lcd.print("Press <Start> to");
  lcd.setCursor(3, 1);
  lcd.print("play now!");
  while (Read(START) != 0)
  {
    if (Read(HARDRESET) == 0) {
      Serial.println("Hard reset");
      lcd.clear();
      lcd.print("Reseting....");
      delay(2000);
      return;
    }
  }
  Serial.println("Start");
  lcd.clear();
  lcd.print("Ready...");
  while (Serial.available() == 0);
  datareceive = Serial.readString();
  /*--------Open flash1--------*/
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
    leds[whiteLed] = CRGB::White;
    FastLED.show();
  }
  delay(2000);
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
    leds[whiteLed] = CRGB::Black;
    FastLED.show();
  }
  /*--------Close flash1--------*/
  /*--------Read data X--------*/
  while (Serial.available() == 0);
  datareceive = Serial.readString();
  int MoveNum = 1;
  for (int i = 0; i < datareceive.length(); i++) {
    if (isSpace(datareceive[i])) {
      STEPX[MoveNum] = stringX[MoveNum].toInt();
      MoveNum++;
      continue;
    }
    stringX[MoveNum] += datareceive[i];
  }
  datareceive = "";
  digitalWrite(Xcomplete, LOW);
  /*--------End read data X--------*/
  /*--------Robot arm X axis matching --------*/
  delay(5000);
  lcd.clear();
  lcd.print("Go...");
  for (int i = 1; i <= MoveNum; i++)
  {
    if (STEPX[i] < 0) {
      digitalWrite(DIRX, HIGH);
    }
    else {
      digitalWrite(DIRX, LOW);
    }
    if (abs(STEPX[i]) < 2 * MaxSpeed * rampuptime) {
      for (int j = 1; j < abs(STEPX[i]); j++) {
        check = checkStop();
        if (check != 0)
          break;
        Xstep(350);
      }
    }
    else {
      for (int rampup = 0; rampup < MaxSpeed; rampup++)
      {
        if (check != 0)
          break;
        for (int rampupcount = 0; rampupcount < rampuptime; rampupcount++)
        {
          check = checkStop();
          if (check != 0)
            break;
          Xstep(dtl[rampup]);
        }
      }
      if (check != 0)
        break;
      remainstep = abs(STEPX[i]) - 2 * MaxSpeed * rampuptime;
      for (int constSpeed = 0; constSpeed < remainstep; constSpeed++)
      {
        check = checkStop();
        if (check != 0)
          break;
        Xstep(dtl[MaxSpeed]);
      }
      if (check != 0)
        break;
      for (int rampdown = MaxSpeed; rampdown > 0; rampdown--)
      {
        if (check != 0)
          break;
        for (int rampdowncount = 0; rampdowncount < rampuptime; rampdowncount++)
        {
          check = checkStop();
          if (check != 0)
            break;
          Xstep(dtl[rampdown]);
        }
      }
    }
    digitalWrite(Xcomplete, HIGH);
    while (true) {
      if (Read(Ycomplete) == 1 && Read(Zcomplete) == 1)
        break;
    }
    digitalWrite(Xcomplete, LOW);
    if (check != 0)
      break;
  }
  digitalWrite(Xcomplete, HIGH);
  if (Read(HARDRESET) == 1 && Read(RESET) == 1 && check == 0) {
    Xreturn();
  }
  for (int i = 1; i <= MoveNum; i++) {
    stringX[i] = "";
    STEPX[i] = 0;
  }
  /*---------End matching-------*/
  if (check != 0) {
    while (Read(HARDRESET) == 0);
    while (Read(RESET) == 1);
    Xreturn();
    lcd.clear();
    lcd.print("Reseting....");
    delay(2000);
    return;
  }
  else {
    lcd.clear();
    lcd.print("Reseting....");
    Xreturn();
  }
}
int Read(int PIN)
{
  return digitalRead(PIN);
}
String getinput(String s1, String s2, int num1, int num2)
{
  String string = "";
  String note = "Pls input " + String(num1) + "~" + String(num2) + "!";
  s2 = s2 + "(" + String(num1) + "~" + String(num2) + "):";
  lcd.clear();
  lcd.print(s1);
  lcd.setCursor(0, 1);
  lcd.print(s2);
  while (true) {
    char key = myKeypad.getKey();
    if (key) {
      if (key == '*')
      {
        if (string.toInt() < num1 || string.toInt() > num2)
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(note);
          lcd.setCursor(2, 1);
          lcd.print("Input again!");
          delay(2000);
          lcd.clear();
          lcd.print(s1);
          lcd.setCursor(0, 1);
          lcd.print(s2);
          string = "";
          continue;
        }
        break;
      }
      else if (key == 'A' || key == 'B' || key == 'C' || key == 'D' || key == '#')
      {
        string = "";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Invalid value!");
        delay(2000);
        lcd.clear();
        lcd.print(s1);
        lcd.setCursor(0, 1);
        lcd.print(s2);
        continue;
      }
      string = string + key;
      lcd.setCursor(s2.length() + 1, 1);
      lcd.print(string);
    }
  }
  return string;
}
void Xstep(int Delay)
{
  PULX_HIGH;
  delayMicroseconds(Delay);
  PULX_LOW;
  delayMicroseconds(Delay);
}
int checkStop()
{
  if (Read(COMPLETE) == 0)
    return 1;
  else if (Read(HARDRESET) == 0)
    return 2;
  else
    return 0;
}
void Xreturn()
{
  digitalWrite(DIRX, HIGH);
  while (true) {
    int test = Read(XLIMIT);
    if (test == 0)
      break;
    Xstep(200);
  }
}
