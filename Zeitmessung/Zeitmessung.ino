/*
 Name:		Zeitmessung.ino
 Created:	05.12.2018 11:32:21
 Author:	Johannes L.
*/

void interrupt1() {
	interruptCalled(1);
}

void interrupt2() {
	interruptCalled(2);
}

void interrupt3() {
	interruptCalled(3);
}

void interrupt4() {
	interruptCalled(4);
}

const long starttime = millis();
const int bahnenanzahl = 4;
volatile int platzierung = 1;

float werte[bahnenanzahl][2];
float platzierung[bahnenanzahl];
int laserPins[bahnenanzahl] = {};
int readerPins[bahnenanzahl] = {};
void *functions[bahnenanzahl] = {interrupt1, interrupt2 , interrupt3, interrupt4};

boolean started = false;

//Nachrichten
/////////////

String bahnZeiten = "Platz %platz%. Das Solarauto auf der Bahn %bahn% hat %zeit% Sekunden benötigt um das Ziel zu erreichen.";

/////////////

void setup() {
	Serial.begin(9600);

	for (int x = 0; x < sizeof(laserPins) / sizeof(int); x++) {
		pinMode(laserPins[x], OUTPUT);
	}

	for (int x = 0; x < sizeof(readerPins) / sizeof(int); x++) {
		pinMode(readerPins[x], INPUT_PULLUP);
	}

}

void loop() {}

void serialEvent() {
	while (Serial.available()) {
		String input = Serial.readString();
		if (input == "Start") {
			start();
		}
		if (input == "Stop") {
			stop();
		}
	}
}

void start() {
	Serial.println("Zeitmessung gestartet");
	started = true;

	for (int x = 0; x < sizeof(laserPins) / sizeof(int); x++) {
		digitalWrite(laserPins[x], HIGH);
	}

	for (int x = 0; x < sizeof(readerPins) / sizeof(int); x++) {
		attachInterrupt(digitalPinToInterrupt(readerPins[x]), (void(*)()) functions[x], RISING);
	}
}

void stop() {
	ergebnis();
	Serial.println("Zeitmessung gestoppt");
	started = false;

	for (int x = 0; x < sizeof(laserPins) / sizeof(int); x++) {
		digitalWrite(laserPins[x], LOW);
	}

	for (int x = 0; x < sizeof(readerPins) / sizeof(int); x++) {
		detachInterrupt(digitalPinToInterrupt(readerPins[x]));
	}
}

void ergebnis() {
	Serial.println("Ergebnisse des Rennens:");
	for (int x = 0; x < sizeof(werte) / sizeof(int); x++) {
		String message = bahnZeiten;
		message.replace("%bahn%", String(x));
		message.replace("%zeit%", werte[x][0] != NULL ? String(werte[x][0]) : "---");
		message.replace("%platz%", werte[x][1] != NULL ? String(werte[x][1]) : "---");
		Serial.println(message);
	}
}

void interruptCalled(int bahn) {
	float f = (millis() - starttime) / 1000.0;

	if (werte[bahn][0] == NULL) {
		werte[bahn][0] = f;
		werte[bahn][1] = platzierung;
		platzierung++;
	}
}