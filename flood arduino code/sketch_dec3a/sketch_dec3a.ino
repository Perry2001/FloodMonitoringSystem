const int buttonPin = 2;
const int buttonPin2 = 3;
const int buttonPin3 = 4;

int buttonState = 0, buttonState2 = 0, buttonState3= 0;

void setup() {
  Serial.begin(115200); // Initialize serial communication
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);

  int tot = buttonState + buttonState2 + buttonState3;

  Serial.print("SwitchesOn:");
  Serial.println(tot);


  delay(1000);
}
