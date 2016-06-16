#define FORWARD_PIN 0
#define REVERSE_TURN_PIN 1
#define SENSOR_PIN 2
#define SENSOR_ANALOG_CHANNEL 1

#define SIP_THRESHOLD 450 //analog val
#define PUFF_THRESHOLD 550 //analog val

#define POLL_INTERVAL 10 //ms

//
void setup()
{
	pinMode(FORWARD_PIN, OUTPUT);
	pinMode(REVERSE_TURN_PIN, OUTPUT);

	while (1)
	{
		int16_t val = analogRead(SENSOR_ANALOG_CHANNEL);

		if (val > PUFF_THRESHOLD)
		{
			pinMode(FORWARD_PIN, OUTPUT);
			digitalWrite(FORWARD_PIN, HIGH);
		}
		else
		{
			pinMode(FORWARD_PIN, INPUT);
			digitalWrite(FORWARD_PIN, LOW);
		}

		if (val < SIP_THRESHOLD)
		{
			pinMode(REVERSE_TURN_PIN, OUTPUT);
			digitalWrite(REVERSE_TURN_PIN, HIGH);
		}
		else
		{
			pinMode(REVERSE_TURN_PIN, INPUT);
			digitalWrite(REVERSE_TURN_PIN, LOW);
		}
		
		delay(POLL_INTERVAL);
	}
}

//
void loop() {}
