#define PULY 2
#define DIRY 3
#define ENAY 4
#define Xcomplete 8
#define Ycomplete 9
#define Zcomplete 10
#define RESET 12
#define COMPLETE 11
#define HARDRESET 13
#define YLIMIT A0
#define PULY_HIGH         PORTD |=   0b00000100;
#define PULY_LOW          PORTD &=  ~0b00000100;
#define NUM_LEDS 16
int dtl[50] = {440, 297, 228, 191, 164, 143, 127, 117, 106, 101, 95,  90,  85,  80,  74,  69,  67,  66,  65,  64, 62,  60,  58,  56,  54,  53,  51,  49,  48,  46, 44,  43,  42,  41,  40,  39,  38,  37,  36,  35, 34,  33,  32,  31,  30,  30,  30,  30,  30,  30};
String stringY[12];
int STEPY[12];
String datareceive = "";
String datasend;
int MaxSpeed;
int rampuptime = 20;
int remainstep;
int check;
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(14);
  pinMode(Ycomplete, OUTPUT);
  pinMode(Zcomplete, INPUT);
  pinMode(PULY , OUTPUT);
  pinMode(DIRY , OUTPUT);
  pinMode(ENAY , OUTPUT);
  digitalWrite(Ycomplete, LOW);
  digitalWrite(ENAY, HIGH);
  pinMode(HARDRESET , INPUT_PULLUP);
  pinMode(COMPLETE , INPUT_PULLUP);
  pinMode(RESET , INPUT_PULLUP);
  pinMode(YLIMIT , INPUT_PULLUP);
  while (Read(RESET) == 1);
  Yreturn();
}

void loop() {
  check = 0;

  /*--------Read data Y--------*/
  while (Serial.available() == 0) {
    if (Read(Xcomplete) == 1)
      digitalWrite(Zcomplete, HIGH);
  }
  datareceive = Serial.readString();
  digitalWrite(Ycomplete, LOW);
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
  /*--------End read data Y--------*/
  /*--------Robot arm Y axis matching --------*/
  datareceive = "";
  while (Serial.available() == 0);
  datareceive = Serial.readString();
  int MoveNum = 1;
  for (int i = 0; i < datareceive.length(); i++) {
    if (isSpace(datareceive[i])) {
      STEPY[MoveNum] = stringY[MoveNum].toInt();
      MoveNum++;
      continue;
    }
    stringY[MoveNum] += datareceive[i];
  }
  datareceive = "";
  delay(5000);
  /*--------End read data X--------*/
  /*--------Robot arm X axis matching --------*/
  for (int i = 1; i <= MoveNum; i++)
  {
    if (STEPY[i] < 0) {
      digitalWrite(DIRY, LOW);
    }
    else {
      digitalWrite(DIRY, HIGH);
    }
    if (abs(STEPY[i]) < 2 * MaxSpeed * rampuptime;) {
      for (int j = 1; j < abs(STEPY[i]); j++) {
        check = checkStop();
        if (check != 0)
          break;
        Ystep(300);
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
          Ystep(dtl[rampup]);
        }
      }
      if (check != 0)
        break;
      remainstep = abs(STEPY[i]) - 2 * MaxSpeed * rampuptime;
      for (int constSpeed = 0; constSpeed < remainstep; constSpeed++)
      {
        check = checkStop();
        if (check != 0)
          break;
        Ystep(dtl[MaxSpeed]);
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
          Ystep(dtl[rampdown]);
        }
      }
    }
    digitalWrite(Ycomplete, HIGH);
    while (true) {
      if (Read(Xcomplete) == 1 && Read(Zcomplete) == 1)
        break;
    }
    digitalWrite(Ycomplete, LOW);
    delay(100);
    if (check != 0)
      break;
  }
  /*---------End matching-------*/
  for (int i = 1; i <= MoveNum; i++) {
    stringY[i] = "";
    STEPY[i] = 0;
  }
  digitalWrite(Ycomplete, HIGH);
  if (check != 0) {
    while (Read(HARDRESET) == 0);
    while (Read(RESET) == 1);
    Yreturn();
  }
}

int Read(int PIN)
{
  return digitalRead(PIN);
}
void Ystep(int Delay)
{
  PULY_HIGH;
  delayMicroseconds(Delay);
  PULY_LOW;
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
void Yreturn()
{
  digitalWrite(DIRY, HIGH);
  while (Read(YLIMIT) == 1) {
    Ystep(200);
  }
}
