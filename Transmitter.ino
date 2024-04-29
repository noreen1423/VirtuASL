#include <RH_ASK.h>
#include <SPI.h>

#define pwmPin 14

RH_ASK driver(2000, 0, 4);

int previousPercentage = -1;

void setup() {
  Serial.begin(115200);
  pinMode(pwmPin, INPUT);
  if (!driver.init()) {
    Serial.println("RF driver init failed");
  }
}

void loop() {
  // Read the PWM value from the pin
  int pwmValue = analogRead(pwmPin);

  // Map the PWM value to a percentage (0-100)
  int currentPercentage = map(pwmValue, 0, 1023, 0, 100);

  // Print the PWM value and percentage for debugging
  Serial.print("PWM Value: ");
  Serial.print(pwmValue);
  Serial.print(" | Percentage: ");
  Serial.println(currentPercentage);

  // Check if the current percentage has changed and is not within the range 90-100
  if (currentPercentage != previousPercentage && (currentPercentage < 90 || currentPercentage > 100)) {
    // Send the signal
    sendSignal();
    // Update the previous percentage
    previousPercentage = currentPercentage;
  }

  // Delay before reading PWM again
  delay(1000);
}

void sendSignal() {
  const char *msg = "Controls On!";

    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
  // Print a message indicating that the signal has been sent
  Serial.println("Signal sent");
}
