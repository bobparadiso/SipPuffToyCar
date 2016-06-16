#define SENSOR_PIN A0
#define FORWARD_PIN 8
#define REVERSE_PIN 9
#define LEFT_PIN 10
#define RIGHT_PIN 11

#define SIP_THRESHOLD 450 //analog val
#define PUFF_THRESHOLD 550 //analog val

#define POLL_INTERVAL 10 //ms

#define SHORT_ACTION_MIN 100 //ms
#define SHORT_ACTION_MAX 750 //ms
#define ACTION_SPACE_MIN 50 //ms
#define ACTION_SPACE_MAX 750 //ms

uint32_t sipStartTime;
uint8_t sipStarted = 0;
uint32_t lastSipTime;
uint32_t lastSipDuration;

uint32_t puffStartTime;
uint8_t puffStarted = 0;
uint32_t lastPuffTime;
uint32_t lastPuffDuration;

enum DRIVE_STATE {FORWARD, REVERSE, IDLE};
char *DRIVE_STATE_STRS[] = {"forward", "reverse", "idle"};
uint8_t driveState = IDLE;

//
void setDrive(uint8_t state)
{
	if (driveState == state)
		return;

	if (state == FORWARD)
	{
		pinMode(FORWARD_PIN, OUTPUT);
		pinMode(REVERSE_PIN, INPUT);
	}
	else if (state == REVERSE)
	{
		pinMode(FORWARD_PIN, INPUT);
		pinMode(REVERSE_PIN, OUTPUT);
	}
	else
	{
		pinMode(FORWARD_PIN, INPUT);
		pinMode(REVERSE_PIN, INPUT);
	}
	
	driveState = state;
}

//
void toggleReverse()
{
	if (driveState == IDLE)
		setDrive(REVERSE);
	else
		setDrive(IDLE);
}

//
void toggleForward()
{
	if (driveState == IDLE)
		setDrive(FORWARD);
	else
		setDrive(IDLE);
}

enum STEERING_STATE {LEFT, RIGHT, CENTER};
char *STEERING_STATE_STRS[] = {"left", "right", "center"};
uint8_t steeringState = CENTER;

//
void setSteering(uint8_t state)
{
	if (steeringState == state)
		return;

	if (state == LEFT)
	{
		pinMode(LEFT_PIN, OUTPUT);
		pinMode(RIGHT_PIN, INPUT);
	}
	else if (state == RIGHT)
	{
		pinMode(LEFT_PIN, INPUT);
		pinMode(RIGHT_PIN, OUTPUT);
	}
	else
	{
		pinMode(LEFT_PIN, INPUT);
		pinMode(RIGHT_PIN, INPUT);
	}
	
	steeringState = state;
}

//
void setup()
{
	Serial.begin(9600);

	while (1)
	{
		int16_t val = analogRead(SENSOR_PIN);
		if (!sipStarted && val < SIP_THRESHOLD)
		{
			sipStarted = 1;
			sipStartTime = millis();
		}
		else if (sipStarted && val > SIP_THRESHOLD)
		{
			sipStarted = 0;
			uint32_t duration = millis() - sipStartTime;
			if (duration > SHORT_ACTION_MIN)
			{
				uint32_t prevLastSipTime = lastSipTime;
				uint32_t prevLastSipDuration = lastSipDuration;
				
				lastSipTime = millis();
				lastSipDuration = duration;
				
				uint32_t space = sipStartTime - prevLastSipTime;

				//two shorts in a row
				if (prevLastSipDuration < SHORT_ACTION_MAX &&
					lastSipDuration < SHORT_ACTION_MAX &&
					space > ACTION_SPACE_MIN && space < ACTION_SPACE_MAX)
					{
						toggleReverse();
					}
			}
		}
		
		if (!puffStarted && val > PUFF_THRESHOLD)
		{
			puffStarted = 1;
			puffStartTime = millis();
		}
		else if (puffStarted && val < PUFF_THRESHOLD)
		{
			puffStarted = 0;
			uint32_t duration = millis() - puffStartTime;
			if (duration > SHORT_ACTION_MIN)
			{
				uint32_t prevLastPuffTime = lastPuffTime;
				uint32_t prevLastPuffDuration = lastPuffDuration;
				
				lastPuffTime = millis();
				lastPuffDuration = duration;
				
				uint32_t space = puffStartTime - prevLastPuffTime;
				
				//two shorts in a row
				if (prevLastPuffDuration < SHORT_ACTION_MAX &&
					lastPuffDuration < SHORT_ACTION_MAX &&
					space > ACTION_SPACE_MIN && space < ACTION_SPACE_MAX)
					{
						toggleForward();
					}
			}
		}
	
		//update steering
		if (sipStarted)
			setSteering(LEFT);
		else if (puffStarted)
			setSteering(RIGHT);
		else
			setSteering(CENTER);

		Serial.print("drive: ");		
		Serial.print(DRIVE_STATE_STRS[driveState]);		
		Serial.print("   steering: ");		
		Serial.println(STEERING_STATE_STRS[steeringState]);		
		delay(POLL_INTERVAL);
	}
}

//
void loop() {}
