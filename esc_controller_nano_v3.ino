#include <Servo.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
const int Cont_threshold = 3,thgain=1,thgain2=2,thgain3=255,Max=255,th_max=170,th_min=20;
const uint8_t MOTOR_COUNT = 6,INPUT_COUNT = 17,RX_BUFFER_SIZE = 80;const uint8_t motorPins[MOTOR_COUNT] = {2, 3, 4, 5, 6, 7};
const unsigned long COMMAND_TIMEOUT_MS = 300UL;
char receiveData[RX_BUFFER_SIZE];uint8_t receiveCount = 0;
bool discardUntilNewline = false,failsafeActive = true;
int chkstr[INPUT_COUNT],vector[MOTOR_COUNT];Servo th[MOTOR_COUNT];
unsigned long lastCommandAt = 0;
static bool parsePacket(char *line, int *destination),readSerialPacket();
static int fastMixScale(int value),conv(int ins, bool rev);
static void calculateVectors(),applyVectors(),stopMotors();

void setup() {
  for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {th[i].attach(motorPins[i]);}
  stopMotors();Serial.begin(115200);}
void loop() {
  if (readSerialPacket()) {calculateVectors();applyVectors();failsafeActive = false;}
  if (!failsafeActive && (unsigned long)(millis() - lastCommandAt) > COMMAND_TIMEOUT_MS) {stopMotors();failsafeActive = true;}}
static bool parsePacket(char *line, int *destination) {
  char *cursor = line;
  for (uint8_t i = 0; i < INPUT_COUNT; ++i) {
    char *end;
    errno = 0;
    const long value = strtol(cursor, &end, 10);
    if (end == cursor || errno == ERANGE || value < INT_MIN || value > INT_MAX) {return false;}
    if (i + 1U < INPUT_COUNT) {if (*end != ',') {return false;}cursor = end + 1;} else if (*end != '\0') {return false;}
    destination[i] = (int)value;
  }return true;}
static bool readSerialPacket() {
  bool receivedValidPacket = false;
  while (Serial.available() > 0) {
    const char incoming = (char)Serial.read();
    if (incoming == '\r') {continue;}
    if (incoming == '\n') {
      if (!discardUntilNewline && receiveCount > 0) {
        int candidate[INPUT_COUNT];
        receiveData[receiveCount] = '\0';
        if (parsePacket(receiveData, candidate)) {for (uint8_t i = 0; i < INPUT_COUNT; ++i) {chkstr[i] = candidate[i];}lastCommandAt = millis();receivedValidPacket = true;}
      }
      receiveCount = 0;discardUntilNewline = false;
      continue;
    }
    if (discardUntilNewline) {continue;}
    if (receiveCount < RX_BUFFER_SIZE - 1U) {receiveData[receiveCount++] = incoming;} else {receiveCount = 0;discardUntilNewline = true;}
  }return receivedValidPacket;}
static int fastMixScale(int value) {return value / 2;}
static void calculateVectors() {
  if ((Cont_threshold > abs(chkstr[0])) && (Cont_threshold > abs(chkstr[1]))) {
    vector[0] = (int)(chkstr[2] * thgain);
    vector[1] = (int)(chkstr[2] * thgain);
    vector[2] = (int)(chkstr[2] * thgain);
    vector[3] = (int)(chkstr[2] * thgain);
    vector[4] = (int)(chkstr[5] * thgain3 - 1 * chkstr[6] * thgain3);
    vector[5] = (int)(chkstr[8] * thgain3 - 1 * chkstr[9] * thgain3);
  } else {
    vector[0] = (int)(chkstr[2] * thgain + fastMixScale(((-1 * chkstr[0] * thgain2) + (-1 * chkstr[1] * thgain2))));
    vector[1] = (int)(chkstr[2] * thgain + fastMixScale(((chkstr[0] * thgain2) + (-1 * chkstr[1] * thgain2))));
    vector[2] = (int)(chkstr[2] * thgain + fastMixScale(((chkstr[0] * thgain2) + (chkstr[1] * thgain2))));
    vector[3] = (int)(chkstr[2] * thgain + fastMixScale(((-1 * chkstr[0] * thgain2) + (chkstr[1] * thgain2))));
    vector[4] = (int)(chkstr[5] * thgain3 - 1 * chkstr[6] * thgain3);
    vector[5] = (int)(chkstr[8] * thgain3 - 1 * chkstr[9] * thgain3);
  }
  for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {vector[i] = constrain(vector[i], -Max, Max);}}
static int conv(int ins, bool rev) {const int distanceFromMinimum = rev ? (255 - ins) : (ins + 255);return (distanceFromMinimum * 5) / 17 + th_min;}
static void applyVectors() {
  th[0].write(conv(vector[4], false));
  th[1].write(conv(vector[0], false));
  th[2].write(conv(vector[1], true));
  th[3].write(conv(vector[2], false));
  th[4].write(conv(vector[3], true));
  th[5].write(conv(vector[5], false));}
static void stopMotors() {for (uint8_t i = 0; i < MOTOR_COUNT; ++i) {vector[i] = 0;}applyVectors();}
