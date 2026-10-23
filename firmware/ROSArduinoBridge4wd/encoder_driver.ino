/* *************************************************************
   Encoder definitions
   
   Add an "#ifdef" block to this file to include support for
   a particular encoder board or library. Then add the appropriate
   #define near the top of the main ROSArduinoBridge.ino file.
   
   ************************************************************ */
   
#ifdef USE_BASE

#ifdef ROBOGAIA
  /* The Robogaia Mega Encoder shield */
  #include "MegaEncoderCounter.h"

  /* Create the encoder shield object */
  MegaEncoderCounter encoders = MegaEncoderCounter(4); // Initializes the Mega Encoder Counter in the 4X Count mode
  
  /* Wrap the encoder reading function */
  long readEncoder(int i) {
    if (i == LEFT) return encoders.YAxisGetCount();
    else return encoders.XAxisGetCount();
  }

  /* Wrap the encoder reset function */
  void resetEncoder(int i) {
    if (i == LEFT) return encoders.YAxisReset();
    else return encoders.XAxisReset();
  }
#elif defined(ARDUINO_ENC_COUNTER)
  volatile long left_enc_pos = 0L;
  volatile long right_enc_pos = 0L;
  volatile long left2_enc_pos = 0L;
  volatile long right2_enc_pos = 0L;
  static const int8_t ENC_STATES [] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};  //encoder lookup table
    
  /* Interrupt routine for LEFT encoder, taking care of actual counting */
  ISR (PCINT2_vect){
  	static uint8_t enc0_last=0;
    static uint8_t enc2_last=0;

	enc0_last <<=2; //shift previous state two places
	enc0_last |= (PIND & (3 << 2)) >> 2; //read the current state into lowest 2 bits
  
  	left_enc_pos += ENC_STATES[(enc0_last & 0x0f)];

  	enc2_last <<=2; //shift previous state two places
	enc2_last |= (PIND & (3 << 4)) >> 4; //read the current state into lowest 2 bits
  
  	left2_enc_pos += ENC_STATES[(enc2_last & 0x0f)];


  }
  
  /* Interrupt routine for RIGHT encoder, taking care of actual counting */
  ISR (PCINT1_vect){
        static uint8_t enc1_last=0;
        static uint8_t enc3_last=0;

	enc1_last <<=2; //shift previous state two places
  	enc1_last |= (PINC & (3 << 4)) >> 4; //read the current state into lowest 2 bits
  
  	right_enc_pos += ENC_STATES[(enc1_last & 0x0f)];

  	enc3_last <<=2; //shift previous state two places
  	enc3_last |= (PINC & (3)); //read the current state into lowest 2 bits
  
  	right2_enc_pos += ENC_STATES[(enc3_last & 0x0f)];
  }
  
  /* Wrap the encoder reading function */
  long readEncoder(int i) {
    if (i == LEFT) return left_enc_pos;
    else if (i == RIGHT) return right_enc_pos;
    else if (i == LEFT2) return left2_enc_pos;
    else if (i == RIGHT2) return right2_enc_pos;
  }

  /* Wrap the encoder reset function */
  void resetEncoder(int i) {
    if (i == LEFT){
      left_enc_pos=0L;
      left2_enc_pos=0L;
      return;
    } else { 
      right_enc_pos=0L;
      right2_enc_pos=0L;
      return;
    }
  }
//------------------------------------------------------------------------------------------------------ARDUINO_MY_COUNTER  
#elif defined ARDUINO_MY_COUNTER
  //用于实现电机的脉冲计数
  //1.定义计数器
  volatile long left_enc_pos = 0L;
  volatile long right_enc_pos = 0L;
  volatile long left2_enc_pos = 0L;
  volatile long right2_enc_pos = 0L;
  
  // 定义编码器状态查找表
  static const int8_t ENC_STATES[] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};  //编码器查找表
  
  //2.初始化
  void initEncoder() 
  {
    // 设置引脚为输入
    pinMode(LEFT_ENC_PIN_A, INPUT);    // D2
    pinMode(LEFT_ENC_PIN_B, INPUT);    // D3
    pinMode(LEFT2_ENC_PIN_A, INPUT);   // D4
    pinMode(LEFT2_ENC_PIN_B, INPUT);   // D5
    pinMode(RIGHT_ENC_PIN_A, INPUT);   // A4 (C4)
    pinMode(RIGHT_ENC_PIN_B, INPUT);   // A5 (C5)
    pinMode(RIGHT2_ENC_PIN_A, INPUT);  // A0 (C0)
    pinMode(RIGHT2_ENC_PIN_B, INPUT);  // A1 (C1)
    
    // 启用PCINT中断
    // 启用PCINT1和PCINT2中断组
    PCICR |= (1 << PCIE1) | (1 << PCIE2);
    
    // 设置PORTD的中断掩码 (D2, D3, D4, D5)
    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21);
    // 如果你的Arduino的引脚号与PCINT号不一致，请根据实际情况调整
    // 通常D2=PCINT18, D3=PCINT19, D4=PCINT20, D5=PCINT21
    
    // 设置PORTC的中断掩码 (A0, A1, A4, A5)
    PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT12) | (1 << PCINT13);
    // 通常A0=PCINT8, A1=PCINT9, A4=PCINT12, A5=PCINT13
  }
  
  //3.编写中断回调函数
  /* 中断服务程序 for PORTD (D2, D3, D4, D5) */
  ISR(PCINT2_vect) {
    static uint8_t left_enc_last = 0;
    static uint8_t left2_enc_last = 0;
    
    // 读取LEFT编码器状态 (D2, D3)
    uint8_t left_enc_state = (PIND & (3 << 2)) >> 2;
    
    // 更新LEFT编码器状态
    left_enc_last <<= 2;
    left_enc_last |= left_enc_state;
    
    // 根据状态表更新位置计数
    left_enc_pos += ENC_STATES[(left_enc_last & 0x0f)];
    
    // 读取LEFT2编码器状态 (D4, D5)
    uint8_t left2_enc_state = (PIND & (3 << 4)) >> 4;
    
    // 更新LEFT2编码器状态
    left2_enc_last <<= 2;
    left2_enc_last |= left2_enc_state;
    
    // 根据状态表更新位置计数
    left2_enc_pos += ENC_STATES[(left2_enc_last & 0x0f)];
  }
  
  /* 中断服务程序 for PORTC (A0, A1, A4, A5) */
  ISR(PCINT1_vect) {
    static uint8_t right_enc_last = 0;
    static uint8_t right2_enc_last = 0;
    
    // 读取RIGHT编码器状态 (A4, A5 / C4, C5)
    uint8_t right_enc_state = (PINC & (3 << 4)) >> 4;
    
    // 更新RIGHT编码器状态
    right_enc_last <<= 2;
    right_enc_last |= right_enc_state;
    
    // 根据状态表更新位置计数
    right_enc_pos += ENC_STATES[(right_enc_last & 0x0f)];
    
    // 读取RIGHT2编码器状态 (A0, A1 / C0, C1)
    uint8_t right2_enc_state = (PINC & 3);
    
    // 更新RIGHT2编码器状态
    right2_enc_last <<= 2;
    right2_enc_last |= right2_enc_state;
    
    // 根据状态表更新位置计数
    right2_enc_pos += ENC_STATES[(right2_enc_last & 0x0f)];
  }
  
  //4.实现编码器数据读和重置的函数
  /* 4.1 读取编码器函数 */
  long readEncoder(int i) {
    if (i == LEFT) return left_enc_pos;
    else if (i == RIGHT) return right_enc_pos;
    else if (i == LEFT2) return left2_enc_pos;
    else if (i == RIGHT2) return right2_enc_pos;
    return 0;
  }
  
  /* 4.2 重置编码器函数 */
  void resetEncoder(int i) {
    if (i == LEFT){
      left_enc_pos = 0L;
      return;
    } else if (i == RIGHT) { 
      right_enc_pos = 0L;
      return;
    } else if (i == LEFT2) {
      left2_enc_pos = 0L;
      return;
    } else if (i == RIGHT2) {
      right2_enc_pos = 0L;
      return;
    }
  }

  //---------------------------------------------------------------------------------------end----------------------------
#else
  #error A encoder driver must be selected!
#endif

/* Wrap the encoder reset function */
void resetEncoders() {
  resetEncoder(LEFT);
  resetEncoder(RIGHT);
  resetEncoder(LEFT2);
  resetEncoder(RIGHT2);
}

#endif
