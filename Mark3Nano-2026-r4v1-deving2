#include "string.h"
#include <Servo.h>

#define Cont_threshold 3
#define thgain 1
#define thgain2 2
#define thgain3 255
#define Max 255
#define th_max 170
#define th_min 20

char recieve_data[80];
int chkstr[17];
int recieve_cont = 0;
int vector[6] = {0};
bool recieve_end = false;

Servo th[6];

void setup() {
  th[0].attach(2);
  th[1].attach(3);
  th[2].attach(4);
  th[3].attach(5);
  th[4].attach(6);
  th[5].attach(7);

  Serial.begin(115200);

  th[0].write(conv(0, false));
  th[1].write(conv(0, false));
  th[2].write(conv(0, true));
  th[3].write(conv(0, false));
  th[4].write(conv(0, true));
  th[5].write(conv(0, false));
}

void loop() {
  SerialRead();

  if(recieve_end){

    if((Cont_threshold > abs(chkstr[0])) && (Cont_threshold > abs(chkstr[1]))){
      vector[0] = (int)(chkstr[2] * thgain);
      vector[1] = (int)(chkstr[2] * thgain);
      vector[2] = (int)(chkstr[2] * thgain);
      vector[3] = (int)(chkstr[2] * thgain);
      vector[4] = (int)(chkstr[5] * thgain3 - 1 * chkstr[6] * thgain3);
      vector[5] = (int)(chkstr[8] * thgain3 - 1 * chkstr[9] * thgain3);
    }else{
      vector[0] = (int)(chkstr[2] * thgain + map(((-1 * chkstr[0] * thgain2) + (-1 * chkstr[1] * thgain2)),-511,511,-255,255));
      vector[1] = (int)(chkstr[2] * thgain + map(((chkstr[0] * thgain2) + (-1 * chkstr[1] * thgain2)),-511,511,-255,255));
      vector[2] = (int)(chkstr[2] * thgain + map(((chkstr[0] * thgain2) + (chkstr[1] * thgain2)),-511,511,-255,255));
      vector[3] = (int)(chkstr[2] * thgain + map(((-1 * chkstr[0] * thgain2) + (chkstr[1] * thgain2)),-511,511,-255,255));
      vector[4] = (int)(chkstr[5] * thgain3 - 1 * chkstr[6] * thgain3);
      vector[5] = (int)(chkstr[8] * thgain3 - 1 * chkstr[9] * thgain3);
    }

    vector[0] = constrain(vector[0],-Max,Max);
    vector[1] = constrain(vector[1],-Max,Max);
    vector[2] = constrain(vector[2],-Max,Max);
    vector[3] = constrain(vector[3],-Max,Max);
    vector[4] = constrain(vector[4],-Max,Max);
    vector[5] = constrain(vector[5],-Max,Max);

    th[0].write(conv(vector[4], false));
    th[1].write(conv(vector[0], false));
    th[2].write(conv(vector[1], true));
    th[3].write(conv(vector[2], false));
    th[4].write(conv(vector[3], true));
    th[5].write(conv(vector[5], false));
  }
}

int conv(int ins, bool rev) {
  int out;

  if(rev){
    out = map(ins,255,-255,th_min,th_max);
  }else{
    out = map(ins,-255,255,th_min,th_max);
  }

  return out;
}

void SerialRead(){
  if(Serial.available()){
    recieve_data[recieve_cont] = Serial.read();

    if(recieve_data[recieve_cont] == '\n'){
      recieve_data[recieve_cont] = '\0';
      recieve_cont = 0;

      chkstr[0] = atoi(strtok(recieve_data,","));

      for(int counter1 = 1;counter1 <= 16;counter1++)
        chkstr[counter1] = atoi(strtok(NULL,","));

      recieve_end = true;
    }else if(recieve_cont >= 79){
      recieve_cont = 0;
      recieve_end = false;
    }else{
      recieve_cont++;
      recieve_end = false;
    }
  }
}
