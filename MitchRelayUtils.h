#define RELAY_IN_1 5
#define RELAY_IN_2 6

void initRelay() {
  pinMode(RELAY_IN_1, OUTPUT);
  digitalWrite(RELAY_IN_1, HIGH);
}

/** 
  Turns on relay with given relay pin
**/  
void relayOn(int relayPin) {
  digitalWrite(relayPin, LOW);
  delay(5000);
}

/** 
  Turns off relay with given relay pin
**/  
void relayOff(int relayPin) {
  digitalWrite(relayPin, HIGH);
  delay(1000);
}
