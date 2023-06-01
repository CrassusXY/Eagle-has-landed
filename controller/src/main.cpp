#include <DFRobot_BMI160.h>

#define DT 0.1
#define ALPHA 0.6
DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;

float results = 0.0;
float gyro = 0.0;

uint32_t historic = 0;

void setup(){
  Serial.begin(115200);

  pinMode(17, INPUT_PULLDOWN);
  pinMode(18, INPUT_PULLDOWN);

  //init the hardware bmin160
  while(bmi160.softReset() != BMI160_OK){
    Serial.println("reset false");
    delay(1000);
  }

  //set and init the bmi160 i2c address
  while(bmi160.I2cInit(i2c_addr) != BMI160_OK){
    Serial.println("init false");
    delay(1000);
  }
}

void loop(){
  int16_t accG[6]={0};
  float acc[3] = {0.0, 0.0, 0.0};
  float tmp[3] = {0.0, 0.0, 0.0};

  if (millis() - historic > 10){
    bmi160.getAccelGyroData(accG);

    //raw gyro to angles
    gyro = accG[1] * M_PI/180.0 * DT;

    //raw acc to G with hard iron
    tmp[0] = accG[3]/16384.0*9.81 + 0.088301;
    tmp[1] = accG[4]/16384.0*9.81 - 0.307944;
    tmp[2] = accG[5]/16384.0*9.81 + 0.008029;

    //soft iron
    acc[0] = 1.003601 * tmp[0] + 0.000220 * tmp[1] - 0.004024 * tmp[2];
    acc[1] = 0.000220 * tmp[0] + 1.001191 * tmp[1] + 0.001550 * tmp[2];
    acc[2] = -0.004024 * tmp[0] + 0.001550 * tmp[1] + 0.981899 * tmp[2];
    
    //acc to angles
	  tmp[1] = (float)((atan2(acc[2], acc[0])) + M_PI ) * (180 / M_PI);

    //angles recalculate
    tmp[1] -= 270;
    if (tmp[1] < -180)
      tmp[1] += 360;

    results = ALPHA * (gyro * DT + results) + (1 - ALPHA) * tmp[1];

    Serial.print("X ");
    Serial.print(digitalRead(17));
    Serial.print(" ");  Serial.print(digitalRead(18));
    Serial.print(" ");  Serial.print(results);
    Serial.println();
    historic = millis();
  }
}