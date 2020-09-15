#include <ADCTouch.h> // Capacitive touch sensor
#include <Servo.h> // Servo motor

// TODO: add led for pot, reorganize breadboard.

// Mode Switch ///////////////////////////////////////////////////////////////
// Switches between photo resistor and touch sensor
int pin_switch = 5;

// Photo Resistor ////////////////////////////////////////////////////////////
int pin_led_photo_senese = 3; // Indicates that the light sensor is being used
int pin_photo_resistor = A2; // Pin for the light sensor input

// Touch sensor //////////////////////////////////////////////////////////////
int pin_led_touch_sense = 2; // Indicates that the touch sensor is being used
int pin_touch_sensor = A0; // Pin for the touch sensor input

// Potentiometer + Button /////////////////////////////////////////////////////
int pin_button = 6; // Button which overrides the switch and uses the potentiometer
int pin_led_potentiometer = 4; // Indicates that the potentiometer is being used
int pin_potentiometer = A1; // Pin for the potentiometer input

Servo dial_servo_motor; // The servo motor used for the dial
float servo_angle; // Stores the motor angle

int cap_touch_ref; // Stores the zero-offset refrence for the touch sensor

int cap_touch_reading, photo_resistor_reading, potentiometer_reading;

void setup() {
	// Start the serial connection
	Serial.begin(115200);

	// Attach the servo motor to pin 9.
	dial_servo_motor.attach(9);

	// Mode Switch ///////////////////////////////////////////////////////////////
	// The switch is set to a pullup input, which will weakly keep the pin on
	// until the switch turns it off. This is generally only good for digital inputs
	pinMode(pin_switch, INPUT_PULLUP);

	// Photo Resistor ////////////////////////////////////////////////////////////
	// Photo resistor is an analog input, so it should be a regular input
	pinMode(pin_photo_resistor, INPUT);
	// The LED is an output that the arduino will control.
	pinMode(pin_led_photo_senese, OUTPUT);

	// Touch sensor //////////////////////////////////////////////////////////////
	pinMode(pin_led_touch_sense, OUTPUT);
	// We don't need to set the mode for pin_touch_sensor, ADCTouch will do that for us.
	// This will get the zero offset, using 1000 readings
	cap_touch_ref = ADCTouch.read(pin_touch_sensor, 1000);

	// Potentiometer + Button /////////////////////////////////////////////////////
	pinMode(pin_button, INPUT_PULLUP);
	pinMode(pin_potentiometer, INPUT);
	pinMode(pin_led_potentiometer, OUTPUT);
} // setup

void loop() {
	if (digitalRead(pin_button) == LOW) {
		// Potentiometer ////////////////////////////////////////////////////////////
		digitalWrite(pin_led_touch_sense, LOW);
		digitalWrite(pin_led_photo_senese, LOW);
		digitalWrite(pin_led_potentiometer, HIGH);

		potentiometer_reading = analogRead(pin_potentiometer);
		Serial.print("Potentiometer: ");
		Serial.println(potentiometer_reading);
		servo_angle = map(potentiometer_reading, 0, 1024, 0, 180);
	}	else if (digitalRead(pin_switch) == HIGH) {
		// Photo Resistor ///////////////////////////////////////////////////////////
		digitalWrite(pin_led_touch_sense, LOW);
		digitalWrite(pin_led_photo_senese, HIGH);
		digitalWrite(pin_led_potentiometer, LOW);

		photo_resistor_reading = analogRead(pin_photo_resistor);
		Serial.print("Light sensor: ");
		Serial.println(photo_resistor_reading);
		servo_angle = map(photo_resistor_reading, 0, 1024, 0, 180);
	}	else {
		// Touch sensor /////////////////////////////////////////////////////////////
		digitalWrite(pin_led_touch_sense, HIGH);
		digitalWrite(pin_led_photo_senese, LOW);
		digitalWrite(pin_led_potentiometer, LOW);

		cap_touch_reading = ADCTouch.read(pin_touch_sensor); // Read the input
		cap_touch_reading -= cap_touch_ref; // Refrence the reading to zero

		Serial.print("Touch sensor: ");
		Serial.println(cap_touch_reading); // Print reading to the serial connection for
		                                   // troubleshooting

		// Keep the value below 40 for hte mapping later.
		if (cap_touch_reading > 40)
			cap_touch_reading = 40;
		// Ignore negative numbers
		if (cap_touch_reading < 0)
			cap_touch_reading = 0;

		// Scale the values into degrees for the servo motor
		servo_angle = map(cap_touch_reading, 0, 40, 0, 180);
	}
	// After all readings are done, set the value to the servo
	dial_servo_motor.write(servo_angle); // Write angle to motor
	// If you are powering the motor from the arduino that is doing the readings,
	// you'll need this delay to prevent the motor's motion from changing readings.
	// delay(50);
} // loop
