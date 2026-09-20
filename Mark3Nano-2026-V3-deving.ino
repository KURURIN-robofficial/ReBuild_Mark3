#include <Arduino.h>
#include <Servo.h>

constexpr uint8_t MOTOR_COUNT = 6;
constexpr uint8_t INPUT_COUNT = 17;

constexpr int16_t CONT_THRESHOLD = 3;
constexpr int16_t TH_GAIN  = 1;
constexpr int16_t TH_GAIN2 = 2;
constexpr int16_t TH_GAIN3 = 255;

constexpr int16_t POWER_MAX = 255;
constexpr uint8_t TH_MIN = 20;
constexpr uint8_t TH_MAX = 170;

constexpr uint16_t SERIAL_BAUD = 115200;
constexpr uint16_t FAILSAFE_MS = 300;

constexpr uint8_t RX_BUFFER_SIZE = 80;

const uint8_t motorPins[MOTOR_COUNT] = {2, 3, 4, 5, 6, 7};
const bool motorReverse[MOTOR_COUNT] = {
  false,  // motor 0
  false,  // motor 1
  true,   // motor 2
  false,  // motor 3
  true,   // motor 4
  false   // motor 5
};

Servo motors[MOTOR_COUNT];

char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxIndex = 0;

int16_t ch[INPUT_COUNT];
int16_t vectorPower[MOTOR_COUNT];

uint32_t lastPacketMs = 0;

uint8_t convertPowerToServoAngle(int16_t power, bool reverse) {
  power = constrain(power, -POWER_MAX, POWER_MAX);

  if (reverse) {
    return map(power, POWER_MAX, -POWER_MAX, TH_MIN, TH_MAX);
  }

  return map(power, -POWER_MAX, POWER_MAX, TH_MIN, TH_MAX);
}

void writeMotor(uint8_t index, int16_t power) {
  motors[index].write(convertPowerToServoAngle(power, motorReverse[index]));
}

void stopAllMotors() {
  for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
    writeMotor(i, 0);
  }
}

bool parsePacket(char *line, int16_t *out, uint8_t count) {
  uint8_t index = 0;
  char *p = line;

  while (index < count) {
    char *endPtr;
    long value = strtol(p, &endPtr, 10);

    if (p == endPtr) {
      return false;
    }

    out[index++] = constrain(value, -32768, 32767);

    if (*endPtr == ',') {
      p = endPtr + 1;
    } else if (*endPtr == '\0') {
      break;
    } else {
      return false;
    }
  }

  return index == count;
}

bool readSerialPacket() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      rxBuffer[rxIndex] = '\0';
      rxIndex = 0;

      return parsePacket(rxBuffer, ch, INPUT_COUNT);
    }

    if (rxIndex < RX_BUFFER_SIZE - 1) {
      rxBuffer[rxIndex++] = c;
    } else {
      rxIndex = 0;
      return false;
    }
  }

  return false;
}

int16_t mixAxis(int16_t base, int16_t x, int16_t y) {
  int32_t mix = base * TH_GAIN;
  mix += map((x + y) * TH_GAIN2, -511, 511, -255, 255);

  return constrain(mix, -POWER_MAX, POWER_MAX);
}

void calculateVectors() {
  const bool stickCenter =
    abs(ch[0]) < CONT_THRESHOLD &&
    abs(ch[1]) < CONT_THRESHOLD;

  const int16_t base = ch[2];

  if (stickCenter) {
    vectorPower[0] = base * TH_GAIN;
    vectorPower[1] = base * TH_GAIN;
    vectorPower[2] = base * TH_GAIN;
    vectorPower[3] = base * TH_GAIN;
  } else {
    vectorPower[0] = mixAxis(base, -ch[0], -ch[1]);
    vectorPower[1] = mixAxis(base,  ch[0], -ch[1]);
    vectorPower[2] = mixAxis(base,  ch[0],  ch[1]);
    vectorPower[3] = mixAxis(base, -ch[0],  ch[1]);
  }

  vectorPower[4] = (ch[5] - ch[6]) * TH_GAIN3;
  vectorPower[5] = (ch[8] - ch[9]) * TH_GAIN3;

  for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
    vectorPower[i] = constrain(vectorPower[i], -POWER_MAX, POWER_MAX);
  }
}

void applyVectors() {
  // 元コードと同じ出力対応:
  // th[0] <- vector[4]
  // th[1] <- vector[0]
  // th[2] <- vector[1]
  // th[3] <- vector[2]
  // th[4] <- vector[3]
  // th[5] <- vector[5]
  writeMotor(0, vectorPower[4]);
  writeMotor(1, vectorPower[0]);
  writeMotor(2, vectorPower[1]);
  writeMotor(3, vectorPower[2]);
  writeMotor(4, vectorPower[3]);
  writeMotor(5, vectorPower[5]);
}

void setup() {
  Serial.begin(SERIAL_BAUD);

  for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
    motors[i].attach(motorPins[i]);
  }

  stopAllMotors();
  lastPacketMs = millis();
}

void loop() {
  if (readSerialPacket()) {
    calculateVectors();
    applyVectors();
    lastPacketMs = millis();
  }

  if (millis() - lastPacketMs > FAILSAFE_MS) {
    stopAllMotors();
  }
}
