#include <Servo.h>

// --- أطراف المواتير (العربية) ---
#define in1 6
#define in2 7
#define in3 8
#define in4 9   // موتور
int ENA = A0;
int ENB = A1;

// --- أطراف السلة الذكية (تعديل الأطراف لمنع التداخل) ---
const int trigDoor = 4;    
const int echoDoor = 12;
const int trigLevel = 2;   
const int echoLevel = 3;  // تم تغييره من 5 لـ 12 لتجنب التداخل
const int ledFull = 13;    
const int servoPin = 11;   // تم تغييره من 9 لـ 11 لأن 9 محجوز للموتور

Servo myServo;

// --- متغيرات ---
int Speed = 130;
int command;
const int distToOpen = 25;    
const int distIsFull = 7;      
const int closedAngle = 0;     
const int openedAngle = 100;   

void setup() {
  Serial.begin(9600);
  
  // إعدادات العربية
  pinMode(in1, OUTPUT); pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT); pinMode(in4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  // إعدادات السلة
  pinMode(trigDoor, OUTPUT); pinMode(echoDoor, INPUT);
  pinMode(trigLevel, OUTPUT); pinMode(echoLevel, INPUT);
  pinMode(ledFull, OUTPUT);
  
  myServo.attach(servoPin);
  myServo.write(closedAngle); 
  
  Serial.println("Combined System Ready!");
}

void loop() {
  // 1. التحكم في سرعة المواتير
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);

  // 2. استقبال أوامر البلوتوث (العربية)
  if (Serial.available() > 0) {
    command = Serial.read();
    handleCar(command);
  }

  // 3. منطق السلة الذكية (بدون Delay معطل)
  checkBinLogic();
}

// --- دالة التحكم في العربية ---
void handleCar(char cmd) {
  switch (cmd) {
    case 'F': forward(); break;
    case 'B': back();    break;
    case 'L': left();    break;
    case 'R': right();   break;
    case 'S': Stop();    break;
  }
}

// --- دالة السلة الذكية ---
void checkBinLogic() {
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 100) { // فحص كل 100 مللي ثانية عشان العربية ما تقفش
    int personDistance = getDistance(trigDoor, echoDoor);
    int wasteLevel = getDistance(trigLevel, echoLevel);

    if (personDistance > 0 && personDistance < distToOpen) {
      openDoor();       
      delay(1000); // وقت رمي الحاجة     
      closeDoor();      
    }

    if (wasteLevel > 0 && wasteLevel < distIsFull) {
      digitalWrite(ledFull, HIGH);
    } else {
      digitalWrite(ledFull, LOW);
    }
    lastCheck = millis();
  }
}

// --- دالة قياس المسافة ---
int getDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 20000); // إضافة timeout لسرعة الاستجابة
  int distance = duration * 0.034 / 2;
  return (distance == 0) ? 999 : distance;
}

// --- حركات السيرفو ---
void openDoor() {
  for (int pos = closedAngle; pos <= openedAngle; pos += 3) {
    myServo.write(pos); delay(5); 
  }
}

void closeDoor() {
  for (int pos = openedAngle; pos >= closedAngle; pos -= 3) {
    myServo.write(pos); delay(5);
  }
}

// --- حركات العربية المظبوطة ---
void forward() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
}
void back() {
  digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);  digitalWrite(in4, HIGH); 
}
void left() {
  digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
}
void right() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);  digitalWrite(in4, HIGH);
}
void Stop() {
  digitalWrite(in1, LOW);  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);  digitalWrite(in4, LOW);
}