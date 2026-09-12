#include "string.h"
#include <Servo.h>
//一先ず書き込みが出来る状態です。
#define Cont_threshold 3  //コントローラの0近傍の切り捨て
#define thgain 1          //推力コントロールのピーキーさを上げる
#define thgain2 1         //同上
#define th_max 170        //前進方向の推力最大値
#define th_min 20         //後進方向の推力最大値

char *recieve_data;  //受信可能文字数
char cmpstr[10];        //チェック用文字列格納用
int chkstr[17];         //17入力 (X,Y,Z,rZ,B1~~~B12,POV)
int recieve_cont = 0;   //文字カウント用変数

int vector0 = 0;
int vector1 = 0;
int vector2 = 0;
int vector3 = 0;
int vector4 = 0;
int vector5 = 0;
bool recieve_end = false;        //命令が更新されているか（文字列の終端まで到達しているか）
bool USB_Communication = false;  //電話線接続モード（USB時接続テスト時にはTrueにする）

Servo th1;
Servo th2;
Servo th3;
Servo th4;

void setup() {
  // put your setup code here, to run once:
  th1.attach(2);
  th2.attach(3);
  th3.attach(4);
  th4.attach(5);

  //  if(USB_Communication){ //接続方法は何にする？
  Serial.begin(115200);
  //  }else{
  //    Serial1.begin(115200);
  //  }
}

void loop() {

  // put your main code here, to run repeatedly:
  SerialRead();
  //動作する内容を変更する
  //コントローラから得られた数値から機体を動作させるための推力値に変換する
  //PC上のソフトから得られたコントローラの値はSerialReadで読みこまれ、Chkstr[]に格納される。対応は下記。
  //17項目を読み込み，int型で格納する
  //chkstr[0]=左軸横（右が正）    //chkstr[1]=左軸縦（上が正）  //chkstr[2]=右軸縦（上が正）    //chkstr[3]=右軸横（右が正）
  //chkstr[4]=A（押下で1)         //chkstr[5]=B（押下で1)       //chkstr[6]=X（押下で1)         //chkstr[7]=Y（押下で1)
  //chkstr[8]=LB                  //chkstr[9]=RB                //chkstr[10]=Back               //chkstr[11]=Start
  //chkstr[12]=左軸おしこみ       //chkstr[13]=右軸おしこみ     //chkstr[14]=NC                 //chkstr[15]=NC
  //chkstr[16]=方向キー　デフォルト-1 時計回り8方向　0-4500-9000-13500-18000-22500-27000-31500

  //コントローラ命令を書き込む部分ここから
  if ((Cont_threshold > abs(chkstr[0])) && (Cont_threshold > abs(chkstr[1]))) {
    vector0 = (int)(chkstr[2] * thgain);
    vector1 = (int)(chkstr[2] * thgain);
    vector2 = (int)(chkstr[2] * thgain);
    vector3 = (int)(chkstr[2] * thgain);
  } else {
    vector0 = (int)(chkstr[2] * thgain + map((((-1 * chkstr[0] * thgain2) + (-1 * chkstr[1] * thgain2))), -511, 511, -255, 255));
    vector1 = (int)(chkstr[2] * thgain + map((((chkstr[0] * thgain2) + (-1 * chkstr[1] * thgain2))), -511, 511, -255, 255));
    vector2 = (int)(chkstr[2] * thgain + map((((chkstr[0] * thgain2) + (chkstr[1] * thgain2))), -511, 511, -255, 255));
    vector3 = (int)(chkstr[2] * thgain + map((((-1 * chkstr[0] * thgain2) + (chkstr[1] * thgain2))), -511, 511, -255, 255));
  }
  //コントローラ命令を書き込む部分ここまで

  //conv関数で足切りとマッピングをする→それを推力値としてドライバに書き込む
  th1.write(conv(vector0, false));
  th2.write(conv(vector1, true));
  th3.write(conv(vector2, false));
  th4.write(conv(vector3, true));
}

int conv(int ins, bool rev) {  //-255~255で入れた指示値をサーボ関数に入れるために20～170に丸める
  int out;
  //rev =Trueの場合は逆回転になるようマッピングする
  if (rev) {
    out = map(ins, 255, -255, th_min, th_max);
  } else {
    out = map(ins, -255, 255, th_min, th_max);
  }
  return out;
}


//文字列を受信する関数
void SerialRead() {
  if (Serial.available()) {//Serialの受信があったか確認する                      
    //memset((void*)recieve_data, NULL, sizeof(recieve_data));
    String test = Serial.readStringUntil('\n');
    recieve_data = (char*)test.c_str();
    if(strncmp(recieve_data,"Send Data",3)!=0){
      //文字列を切り取り，各値を確認する
      //Serial.println(recieve_data);
      chkstr[0] = atoi(strtok(recieve_data, ","));                                                   //命令セット文字列を格納
      for (int counter1 = 1; counter1 < 16; counter1++) chkstr[counter1] = atoi(strtok(NULL, ","));  //受け取る数値は右(a1,a2,a3,a4,p1,p2,p3,p4)
      memset((void*)recieve_data, NULL, sizeof(recieve_data));
      //Serial.println(chkstr[0]);
      //Serial.println(chkstr[1]);
      //Serial.println(chkstr[2]);
      //Serial.println(chkstr[3]);
    }
  }
}
