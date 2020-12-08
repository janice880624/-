#define PULZ 2
#define DIRZ 3
#define ENAZ 4
#define PULC 5
#define DIRC 6
#define ENAC 7
#define Xcomplete 8
#define Ycomplete 9
#define Zcomplete 10
#define RESET 12
#define COMPLETE 11
#define HARDRESET 13
#define ZLIMIT A0
#define PUMP A1
#define PULZ_HIGH         PORTD |=   0b00000100;
#define PULZ_LOW          PORTD &=  ~0b00000100;
#define PULC_HIGH         PORTD |=   0b00100000;
#define PULC_LOW          PORTD &=  ~0b00100000;
int dtl[50] = {440, 297, 228, 191, 164, 143, 127, 117, 106, 101, 95, 90, 85, 80, 74, 69, 67, 66, 65, 64, 62, 60, 58, 56, 54, 53, 51, 49, 48, 46, 44, 43 , 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 30, 30, 30, 30, 30};
int STEPZ[12] = {1900, 1900, 1900, 1900, 1900, 1900, 1900, 1900, 1900, 1900, 1900, 1900};
int catchstep = 1500;
String stringC[12];
int STEPC[12];
String datareceive;
String datasend;
int MaxSpeed ;
int rampuptime = 20;
int robotscore = 0;
int remainstep;
int check;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setTimeout(10);
  pinMode(Ycomplete, INPUT);
  pinMode(Zcomplete, OUTPUT);
  pinMode(Xcomplete, INPUT);
  pinMode(PULC , OUTPUT);
  pinMode(DIRC , OUTPUT);
  pinMode(ENAC , OUTPUT);
  pinMode(PULZ , OUTPUT);
  pinMode(DIRZ , OUTPUT);
  pinMode(ENAZ , OUTPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(HARDRESET , INPUT_PULLUP);
  pinMode(COMPLETE , INPUT_PULLUP);
  pinMode(RESET , INPUT_PULLUP);
  pinMode(ZLIMIT , INPUT_PULLUP);
  digitalWrite(Zcomplete, LOW);
  digitalWrite(ENAZ, HIGH);
  digitalWrite(ENAC, HIGH);
  digitalWrite(DIRZ, LOW);
  while (Read(RESET) == 1);
  Zreturn();
}

void loop() {
  check = 0;
  /*--------Read data C--------*/
  robotscore = 0;
  while (Serial.available() == 0) {
    if (Read(Xcomplete) == 1)
      digitalWrite(Zcomplete, HIGH);
  }
  datareceive = Serial.readString();
  MaxSpeed = datareceive.toInt();
  if (MaxSpeed<=6)
  {
    MaxSpeed=MaxSpeed-1;
  }
  else if(MaxSpeed==7)
  {
    MaxSpeed=9;
  }
  else if(MaxSpeed==8)
  {
    MaxSpeed=13;
  }
  else if(MaxSpeed==9)
  {
    MaxSpeed=27;
  }
  else
  {
    MaxSpeed=49;
  }
  while (Serial.available() == 0);
  datareceive = Serial.readString();
  int MoveNum = 1;
  for (int i = 0; i < datareceive.length(); i++) {
    if (isSpace(datareceive[i])) {
      STEPC[MoveNum] = stringC[MoveNum].toInt();
      MoveNum++;
      continue;
    }
    stringC[MoveNum] += datareceive[i];
  }
  datareceive = "";
  /*--------End read data Y--------*/
  digitalWrite(Zcomplete, LOW);
  /*--------Robot arm ZC axis matching --------*/
  delay(4800);
  for (int i = 1; i <= (MoveNum * 2 - 1); i++)
  {
    if (i == (MoveNum * 2 - 1)) {
      digitalWrite(Zcomplete, HIGH);
      digitalWrite(PUMP, LOW);
      delay(100);
      break;
    }
    /*--------Z going down---------*/
    digitalWrite(DIRZ, HIGH);
    digitalWrite(Zcomplete, LOW);
    for (int rampup = 0; rampup < MaxSpeed; rampup++)
    {
      if (check != 0)
        break;
      for (int rampupcount = 0; rampupcount < rampuptime; rampupcount++)
      {
        check = checkStop();
        if (check != 0)
          break;
        Zstep(dtl[rampup]);
      }
    }
    if (check != 0)
      break;
    remainstep = abs(STEPZ[i]) - 2 * MaxSpeed * rampuptime;
    for (int constSpeed = 0; constSpeed < remainstep; constSpeed++)
    {
      check = checkStop();
      if (check != 0)
        break;
      Zstep(dtl[MaxSpeed]);
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
        Zstep(dtl[rampdown]);
      }
    }
    if (check != 0)
      break;
    /*--------End Z going down---------*/
    while (true)
    {
      if (Read(Xcomplete) == 1 && Read(Ycomplete) == 1)
        break;
      check = checkStop();
      if (check != 0)
        break;
    }
    if (check != 0)
      break;
    for (int j = 0; j < catchstep; j++) {
      check = checkStop();
      if (check != 0)
        break;
      Zstep(200);
    }
    if (check != 0)
      break;
    if (i % 2 == 1)
      digitalWrite(PUMP, HIGH);
    else {
      digitalWrite(PUMP, LOW);
      robotscore++;
    }
    delay(100);
    digitalWrite(DIRZ, LOW);
    for (int j = 0; j < catchstep; j++) {
      check = checkStop();
      if (check != 0)
        break;
      Zstep(200);
    }
    if (check != 0)
      break;
    digitalWrite(Zcomplete, HIGH);
    while (true) {
      if (Read(Xcomplete) == 0 && Read(Ycomplete) == 0)
        break;
      }
    digitalWrite(Zcomplete, LOW);
    /*--------C axis turn an angle---------*/
    if (i % 2 == 1) {//Turn
      digitalWrite(DIRC, HIGH);
      for (int j = 0; j < abs(STEPC[(i + 1) / 2]); j++) {
        check = checkStop();
        if (check != 0)
          break;
        Cstep(100);
      }
    }
    else
    { //Return
      digitalWrite(DIRC, LOW);
      for (int j = 0; j < abs(STEPC[i / 2]); j++) {
        check = checkStop();
        if (check != 0)
          break;
        Cstep(100);
      }
    }
    if (check != 0)
      break;
    /*--------End C axis turn an angle---------*/
    /*--------Z going up---------*/
    for (int rampup = 0; rampup < MaxSpeed; rampup++)
    {
      if (check != 0)
        break;
      for (int rampupcount = 0; rampupcount < rampuptime; rampupcount++)
      {
        check = checkStop();
        if (check != 0)
          break;
        Zstep(dtl[rampup]);
      }
    }
    if (check != 0)
      break;
    remainstep = abs(STEPZ[i]) - 2 * MaxSpeed * rampuptime;
    for (int constSpeed = 0; constSpeed < remainstep; constSpeed++)
    {
      check = checkStop();
      if (check != 0)
        break;
      Zstep(dtl[MaxSpeed]);
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
        Zstep(dtl[rampdown]);
      }
    }
    /*--------End Z going up---------*/
  }
  for (int i = 1; i <= MoveNum; i++) {
    stringC[i] = "";
    STEPC[i] = 0;
  }
  check = checkStop();
  /*---------End matching-------*/
  if (check == 0)
    Serial.println("1");
  else if (check == 1) {
    digitalWrite(Zcomplete, HIGH);
    digitalWrite(PUMP, LOW);
    Serial.println("2");
    delay(100);
    Serial.println(robotscore);
    while (Read(RESET) == 1);
    Zreturn();
  }
  else {
    /*---------Reset and go to origin-----*/
    digitalWrite(Zcomplete, HIGH);
    Serial.println("Hard reset");
    digitalWrite(PUMP, LOW);
    while (Read(HARDRESET) == 0);
    while (Read(RESET) == 1);
    Zreturn();
    /*---------End reset--------*/
  }
}
int Read(int PIN)
{
  return digitalRead(PIN);
}
void Zstep(int Delay)
{
  PULZ_HIGH;
  delayMicroseconds(Delay);
  PULZ_LOW;
  delayMicroseconds(Delay);
}
void Cstep(int Delay)
{
  PULC_HIGH;
  delayMicroseconds(Delay);
  PULC_LOW;
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
void Zreturn()
{
  digitalWrite(DIRZ, LOW);
  while (Read(ZLIMIT) == 1) {
    Zstep(200);
  }
}
