/* Functions and type-defs for PID control.

   Taken mostly from Mike Ferguson's ArbotiX code which lives at:
   
   http://vanadium-ros-pkg.googlecode.com/svn/trunk/arbotix/
*/

/* PID setpoint info For a Motor */
typedef struct {
  double TargetTicksPerFrame;    // target speed in ticks per frame
  long Encoder;                  // encoder count
  long PrevEnc;                  // last encoder count

  /*
  * Using previous input (PrevInput) instead of PrevError to avoid derivative kick,
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-derivative-kick/
  */
  int PrevInput;                // last input
  //int PrevErr;                   // last error
  int PrevEncD;
  /*
  * Using integrated term (ITerm) instead of integrated error (Ierror),
  * to allow tuning changes,
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
  */
  int ITerm;                    //integrated term

  long output;                    // last motor setting
}
SetPointInfo;

// initiate four wheels
SetPointInfo leftPID, rightPID,left2PID,right2PID;

/* init PID Parameters*/
/*
 * Keep fractional gains as configured in arduino_params.yaml.  The archived
 * v0.4 source used int here, silently turning 8.835 into 8 and 0.9 into 0.
 * These values still require bench tuning before powered-wheel testing.
 */
float Kp = 2.0f;
float Kd = 8.835f;
float Ki = 0.9f;
float Ko = 50.0f;

unsigned char moving = 0; // is the base in motion?

/*
* Initialize PID variables to zero to prevent startup spikes
* when turning PID on to start moving
* In particular, assign both Encoder and PrevEnc the current encoder value
* See http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-initialization/
* Note that the assumption here is that PID is only turned on
* when going from stop to moving, that's why we can init everything on zero.
*/
void resetPID(){
   leftPID.TargetTicksPerFrame = 0.0;
   leftPID.Encoder = readEncoder(LEFT);
   leftPID.PrevEnc = leftPID.Encoder;
   leftPID.output = 0;
   leftPID.PrevInput = 0; 
   leftPID.ITerm = 0;
   leftPID.PrevEncD =0;

   rightPID.TargetTicksPerFrame = 0.0;
   rightPID.Encoder = readEncoder(RIGHT);
   rightPID.PrevEnc = rightPID.Encoder;
   rightPID.output = 0;
   rightPID.PrevInput = 0;
   rightPID.ITerm = 0;
   rightPID.PrevEncD =0;

   left2PID.TargetTicksPerFrame = 0.0;
   left2PID.Encoder = readEncoder(LEFT2);
   left2PID.PrevEnc = left2PID.Encoder;
   left2PID.output = 0;
   left2PID.PrevInput = 0;
   left2PID.ITerm = 0;
   left2PID.PrevEncD =0;

   right2PID.TargetTicksPerFrame = 0.0;
   right2PID.Encoder = readEncoder(RIGHT2);
   right2PID.PrevEnc = right2PID.Encoder;
   right2PID.output = 0;
   right2PID.PrevInput = 0;
   right2PID.ITerm = 0;
   right2PID.PrevEncD =0;

}

/* PID routine to compute the next motor commands */
void doPID(SetPointInfo * p) {
  long Perror;
  long output;
  int input;

  input = p->Encoder - p->PrevEnc;
  // Low Pass filter
  // If the encoder reading is noisy, try average filter or median filter.
  int enc_change = 0.5*input+0.5*p->PrevEncD;
  Perror = p->TargetTicksPerFrame - enc_change;
  p->PrevEncD = enc_change;
  //Serial.println(input);
  /*
  * Avoid derivative kick and allow tuning changes,
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-derivative-kick/
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
  */
  //output = (Kp * Perror + Kd * (Perror - p->PrevErr) + Ki * p->Ierror) / Ko;
  // p->PrevErr = Perror;
  output = (Kp * Perror - Kd * (input - p->PrevInput) + p->ITerm) / Ko;
  p->PrevEnc = p->Encoder;

  output += p->output;
  // Accumulate Integral error *or* Limit output.
  // Stop accumulating when output saturates(anti wind-up)
  if (output >= MAX_PWM)
    output = MAX_PWM;
  else if (output <= -MAX_PWM)
    output = -MAX_PWM;
  else
  /*
  * allow turning changes, see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
  */
    p->ITerm += Ki * Perror;

  p->output = output;
  p->PrevInput = input;
}

/* Read the encoder values and call the PID routine */
void updatePID() {
  /* Read the encoders */
  //获取编码器计数
  leftPID.Encoder = readEncoder(LEFT);
  rightPID.Encoder = readEncoder(RIGHT);
  left2PID.Encoder = readEncoder(LEFT2);
  right2PID.Encoder = readEncoder(RIGHT2);
  /* If we're not moving there is nothing more to do */
  if (!moving){
    /*
    如果没运行
    * Reset PIDs once, to prevent startup spikes,
    * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-initialization/
    * PrevInput is considered a good proxy to detect
    * whether reset has already happened
    */
    if (leftPID.PrevInput != 0 || rightPID.PrevInput != 0) resetPID();
    return;
  }

  /* Compute PID update for each motor */
  //分别调试四轮
  doPID(&rightPID);
  doPID(&leftPID);
  doPID(&right2PID);
  doPID(&left2PID);

  /* Set the motor speeds accordingly */
  setMotorSpeeds(leftPID.output, rightPID.output,left2PID.output, right2PID.output);
}

