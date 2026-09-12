#include "string.h"
#define Cont_threshold 3
#define thgain 1
#define thgain2 2
#define thgain3 255
#define Max 255


char recieve_data[80];//受信可能文字数
int chkstr[17];//17入力 (X,Y,Z,rZ,B1~~~B12,POV)
int recieve_cont = 0;//文字カウント用変数
int vector[6] ={0};
bool recieve_end = false;//命令が更新されているか（文字列の終端まで到達しているか）

void setup() {

  pinMode(A0,OUTPUT);//Motor1
  pinMode(A1,OUTPUT);//Motor2
  pinMode(A2,OUTPUT);//Motor3
  pinMode(A3,OUTPUT);//Motor4
  pinMode(A4,OUTPUT);//Motor5
  pinMode(A5,OUTPUT);//Motor6

  
  Serial.begin(115200);

    MotorDrive(2,0);
    MotorDrive(3,0);
    MotorDrive(4,0);
    MotorDrive(5,0);
    MotorDrive(1,0);
    MotorDrive(6,0);
}

void loop() {
  SerialRead();
  if(recieve_end){//SerialRead関数で命令系が受信されるとフラグが立ち、こちらのループに移動する。
  
  //動作する内容を変更する
  //コントローラから得られた数値から機体を動作させるための推力値に変換する
  //PC上のソフトから得られたコントローラの値はSerialReadで読みこまれ、Chkstr[]に格納される。対応は下記。
  //17項目を読み込み，int型で格納する
  //chkstr[0]=左軸横（右が正）    //chkstr[1]=左軸縦（上が正）  //chkstr[2]=右軸縦（上が正）    //chkstr[3]=右軸横（右が正）
  //chkstr[4]=A（押下で1)         //chkstr[5]=B（押下で1)       //chkstr[6]=X（押下で1)         //chkstr[7]=Y（押下で1)
  //chkstr[8]=LB                  //chkstr[9]=RB                //chkstr[10]=Back               //chkstr[11]=Start
  //chkstr[12]=左軸おしこみ       //chkstr[13]=右軸おしこみ     //chkstr[14]=NC                 //chkstr[15]=NC
  //chkstr[16]=方向キー　デフォルト-1 時計回り8方向　0-4500-9000-13500-18000-22500-27000-31500
  //ボタンにする際は、chkstr[?] * thgain3　もしくは　chkstr[?] * maxにする 方向キーには手出ししない。

    //コントローラ命令を書き込む部分ここから
    if ((Cont_threshold > abs(chkstr[0])) && (Cont_threshold > abs(chkstr[1]))){
        vector[0] = (int)(chkstr[2] * thgain);
        vector[1] = (int)(chkstr[2] * thgain);
        vector[2] = (int)(chkstr[2] * thgain);
        vector[3] = (int)(chkstr[2] * thgain);
        vector[4] = (int)(chkstr[5] * thgain3 -1* chkstr[6] *thgain3);
        vector[5] = (int)(chkstr[8] * thgain3 -1* chkstr[9] *thgain3);
        //vector = (int)(chkstr[4] * thgain);むし
    }else{
        vector[0] = (int)(chkstr[2] * thgain + map((((-1*  chkstr[0] * thgain2) + (-1*  chkstr[1] * thgain2))),-511,511,-255,255));
        vector[1] = (int)(chkstr[2] * thgain + map((((     chkstr[0] * thgain2) + (-1*  chkstr[1] * thgain2))),-511,511,-255,255));
        vector[2] = (int)(chkstr[2] * thgain + map((((     chkstr[0] * thgain2) + (     chkstr[1] * thgain2))),-511,511,-255,255));
        vector[3] = (int)(chkstr[2] * thgain + map((((-1*  chkstr[0] * thgain2) + (     chkstr[1] * thgain2))),-511,511,-255,255));
        vector[4] = (int)(chkstr[5] * thgain3 -1* chkstr[6] *thgain3);
        vector[5] = (int)(chkstr[8] * thgain3 -1* chkstr[9] *thgain3);
    }
    //M1はvector4,M6はvector5に代入すると回転する（入力値は-255～255）

    
    //コントローラ命令を書き込む部分ここまで

    //限界値を超えた場合は足切り
    vector[0] = constrain(vector[0],-1 * Max,Max);
    vector[1] = constrain(vector[1],-1 * Max,Max);
    vector[2] = constrain(vector[2],-1 * Max,Max);
    vector[3] = constrain(vector[3],-1 * Max,Max);
    vector[4] = constrain(vector[4],-1 * Max,Max);
    vector[5] = constrain(vector[5],-1 * Max,Max);

    //格納したデータを処理して出力
    MotorDrive(2,vector[0]); //M2 
    MotorDrive(3,vector[1]); //M3
    MotorDrive(4,vector[2]); //M4
    MotorDrive(5,vector[3]); //M5
    
    MotorDrive(1,vector[4]);//M1 デフォルト無効
    MotorDrive(6,vector[5]);//M6 デフォルト無効
    
  }
  
}

//モータを回転させる関数
//接続されたモータ（1～4），回転数(-254～254)で指定する
void MotorDrive(char motornom,int motorspeed){
  switch(motornom){
    case 1:
      analogWrite(3,abs(motorspeed));
      digitalWrite(A0,motorspeed > 0 ? HIGH : LOW);
      break;

    case 2:
      analogWrite(5,abs(motorspeed));
      digitalWrite(A1,motorspeed > 0 ? HIGH : LOW);
      break;

    case 3:
      analogWrite(6,abs(motorspeed));
      digitalWrite(A2,motorspeed > 0 ? HIGH : LOW);
      break;

    case 4:
      analogWrite(9,abs(motorspeed));
      digitalWrite(A3,motorspeed > 0 ? HIGH : LOW);
      break;

    case 5:
      analogWrite(10,abs(motorspeed));
      digitalWrite(A4,motorspeed > 0 ? HIGH : LOW);
      break;

    case 6:
      analogWrite(11,abs(motorspeed));
      digitalWrite(A5,motorspeed > 0 ? HIGH : LOW);
      break;
  }
}
//以下は基本いじらない。
//文字列を受信する関数
void SerialRead(){
    if(Serial.available()){
        recieve_data[recieve_cont] = Serial.read();

        if(recieve_data[recieve_cont] == '\n'){//終端判定置換
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
