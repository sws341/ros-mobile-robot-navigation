/* *************************************************************
   Encoder driver function definitions - by James Nugen
   ************************************************************ */
   
   
#ifdef ARDUINO_ENC_COUNTER

  #define LEFT_ENC_PIN_A PD2  //pin 2
  #define LEFT_ENC_PIN_B PD3  //pin 3
  
  #define LEFT2_ENC_PIN_A PD4  //pin 4
  #define LEFT2_ENC_PIN_B PD5   //pin 5

  #define RIGHT_ENC_PIN_A PC4  //pin A4
  #define RIGHT_ENC_PIN_B PC5   //pin A5

  #define RIGHT2_ENC_PIN_A PC0  //pin A0
  #define RIGHT2_ENC_PIN_B PC1   //pin A1

#elif defined ARDUINO_MY_COUNTER
  //定义引脚
  //------------------------unoR3版本
  #define LEFT_ENC_PIN_A PD2  //pin 2
  #define LEFT_ENC_PIN_B PD3  //pin 3
  
  #define LEFT2_ENC_PIN_A PD4  //pin 4
  #define LEFT2_ENC_PIN_B PD5   //pin 5

  #define RIGHT_ENC_PIN_A PC4  //pin A4
  #define RIGHT_ENC_PIN_B PC5   //pin A5

  #define RIGHT2_ENC_PIN_A PC0  //pin A0
  #define RIGHT2_ENC_PIN_B PC1   //pin A1

  //-------------------------------------------mega2560版本
  // // Mega 2560上的正确端口映射
  // // 使用Arduino引脚编号更简单
  // #define LEFT_ENC_PIN_A 2  // 数字引脚2
  // #define LEFT_ENC_PIN_B 3  // 数字引脚3
  // #define LEFT2_ENC_PIN_A 4  // 数字引脚4
  // #define LEFT2_ENC_PIN_B 5  // 数字引脚5

  // #define RIGHT_ENC_PIN_A A4  // 模拟引脚A4
  // #define RIGHT_ENC_PIN_B A5  // 模拟引脚A5
  // #define RIGHT2_ENC_PIN_A A0  // 模拟引脚A0
  // #define RIGHT2_ENC_PIN_B A1  // 模拟引脚A1
//--------------------------------------------------------下面的函数声明不要动！！
  
  //声明函数
  //1.初始化函数：设置引脚的操作模式，添加终端
  void initEncoder();
  //2.中断函数
  void leftEncoderEventA();
  void leftEncoderEventB();
  void rightEncoderEventA();
  void rightEncoderEventB();
  void left2EncoderEventA();
  void left2EncoderEventB();
  void right2EncoderEventA();
  void right2EncoderEventB();
#endif
   
long readEncoder(int i);
void resetEncoder(int i);
void resetEncoders();

