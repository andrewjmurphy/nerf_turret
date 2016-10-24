
#define IR_IN  8

int turntable_powerpin = 4;
int turntable_vccpin = 3;
int turntable_gndpin = 2;

int tilt_powerpin = 7;
int tilt_vccpin = 6;
int tilt_gndpin = 5;

int flywheelpin = 9;
int firepin = 11;
int flywheelboostpin = 10;

boolean flywheel_on = false;

boolean button_held = false;

int Pulse_Width = 0;
int ir_code = 0x00;
char  adrL_code = 0x00;
char  adrH_code = 0x00;

void tilt_up()
{
    Serial.println("UP");
    // Turn off power first, to prevent short circuit.
    digitalWrite(tilt_powerpin, HIGH);
    
    // Set both to low to tilt up
    digitalWrite(tilt_vccpin, HIGH);
    digitalWrite(tilt_gndpin, HIGH);
    
    // Turn on power first
    digitalWrite(tilt_powerpin, LOW);
}

void tilt_down()
{
    Serial.println("DOWN");
    // Turn off power first, to prevent short circuit.
    digitalWrite(tilt_powerpin, HIGH);
    
    // Set both to low to tilt down
    digitalWrite(tilt_vccpin, LOW);
    digitalWrite(tilt_gndpin, LOW);
    
    // Turn on power first
    digitalWrite(tilt_powerpin, LOW);
}

void turn_left()
{
    Serial.println("LEFT");
    // Turn off power first, to prevent short circuit.
    digitalWrite(turntable_powerpin, HIGH);
    
    // Set both to high to turn anticlockwise
    digitalWrite(turntable_vccpin, LOW);
    digitalWrite(turntable_gndpin, LOW);
    
    // Turn on power first
    digitalWrite(turntable_powerpin, LOW);
    
}

void turn_right()
{
    Serial.println("RIGHT");
    // Turn off power first, to prevent short circuit.
    digitalWrite(turntable_powerpin, HIGH);
    
    // Set both to low to turn clockwise
    digitalWrite(turntable_vccpin, HIGH);
    digitalWrite(turntable_gndpin, HIGH);
    
    // Turn on power first
    digitalWrite(turntable_powerpin, LOW);
}

void stop_all()
{
    //Serial.println("STOP");
    // Put power to both tilt and turn motors
    digitalWrite(turntable_powerpin, HIGH);
    digitalWrite(tilt_powerpin, HIGH);
    
    // stop firing and cut the flywheel boost
    digitalWrite(firepin, HIGH);
    digitalWrite(flywheelboostpin, HIGH);
    
    // set remaining relays to NC state
    digitalWrite(turntable_vccpin, HIGH);
    digitalWrite(turntable_gndpin, HIGH);
    digitalWrite(tilt_vccpin, HIGH);
    digitalWrite(tilt_gndpin, HIGH);
    
    
}

void toggle_flywheel()
{
  // has the button been released since we last set the flywheel state
  // if not then do nothing.
  if (button_held)
  {
    return;
    //Serial.println("Button_held is true");
  }
  else {
    //Serial.println("Button_held is false");
  }
    
  if (flywheel_on)
  {
    Serial.println("flywheel off");
    digitalWrite(flywheelpin, HIGH);
    flywheel_on = false;
  }
  else
  {
    Serial.println("flywheel on");
    digitalWrite(flywheelpin, LOW);
    flywheel_on = true;
  }
  button_held = true;
  //Serial.println("Button_held set to true");

}

void fire()
{
  // only fire if the flywheel has been spun up
  if (flywheel_on)
  {
    Serial.println("FIRE!!!!!!!!");
    digitalWrite(flywheelboostpin, LOW);
    digitalWrite(firepin, LOW);
  }

}

void timer1_init(void)
{
  TCCR1A = 0X00; 
  TCCR1B = 0X05;
  TCCR1C = 0X00;
  TCNT1 = 0X00;
  TIMSK1 = 0X00;
}
void remote_deal(void)
{   
    if(adrL_code!=0){
      //Serial.println(ir_code,HEX);
      Serial.println(adrL_code,HEX);
    }
    if(adrL_code==8 || adrL_code==0X1C)
      turn_left();
    else if(adrL_code==0X5A || adrL_code==0X1B)
      turn_right();
    else if(adrL_code==0X18 || adrL_code==0XB)
      tilt_down();
    else if(adrL_code==0X52 || adrL_code==0XC)
      tilt_up();
    else if(adrL_code==0X45 || adrL_code==0X1E)
      toggle_flywheel();
    else if(adrL_code==0X1c || adrL_code==0X15)
      fire();
    else
      stop_all();
      
    if (adrL_code==0) {
      button_held = false;
      //Serial.println("Button released. Button_held set to false");
    }
      
}
char logic_value()
{
  TCNT1 = 0X00;
  while(!(digitalRead(IR_IN)));
  Pulse_Width=TCNT1;
  TCNT1=0;
  if(Pulse_Width>=7&&Pulse_Width<=10)
  {
    while(digitalRead(IR_IN));
    Pulse_Width=TCNT1;
    TCNT1=0;
    if(Pulse_Width>=7&&Pulse_Width<=10)
      return 0;
    else if(Pulse_Width>=25&&Pulse_Width<=27) 
      return 1;
  }
  return -1;
}
void pulse_deal()
{
  int i;
  int j;
  ir_code=0x00;
  adrL_code=0x00;
  adrH_code=0x00;

  
  for(i = 0 ; i < 16; i++)
  {
    if(logic_value() == 1)
        ir_code |= (1<<i);
  }

  for(i = 0 ; i < 8; i++)
  {
    if(logic_value() == 1)
      adrL_code |= (1<<i);
  }

  for(j = 0 ; j < 8; j++)
  {
    if(logic_value() == 1)
        adrH_code |= (1<<j);
  }
}
void remote_decode(void)
{
  TCNT1=0X00;       
  while(digitalRead(IR_IN))
  {
    if(TCNT1>=1563)  //100ms
    {
      ir_code=0x00ff;
      adrL_code=0x00;
      adrH_code=0x00;
      return;
    }  
  }

  //100ms
  TCNT1=0X00;

  while(!(digitalRead(IR_IN)));
  Pulse_Width=TCNT1;
  TCNT1=0;
  if(Pulse_Width>=140&&Pulse_Width<=141)//9ms
  {

    while(digitalRead(IR_IN));
    Pulse_Width=TCNT1;
    TCNT1=0;
    if(Pulse_Width>=68&&Pulse_Width<=72)//4.5ms
    {  
      pulse_deal();
      return;
    }
    else if(Pulse_Width>=34&&Pulse_Width<=36)//2.25ms
    {
      while(!(digitalRead(IR_IN)));
      Pulse_Width=TCNT1;
      TCNT1=0;
      if(Pulse_Width>=7&&Pulse_Width<=10)//560us
      {
        return; 
      }
    }
  }
}
void setup()
{
  Serial.begin(9600);
  pinMode(IR_IN,INPUT);
  pinMode(turntable_powerpin,OUTPUT);
  pinMode(turntable_vccpin,OUTPUT);
  pinMode(turntable_gndpin,OUTPUT);
  pinMode(tilt_powerpin,OUTPUT);
  pinMode(tilt_vccpin,OUTPUT);
  pinMode(tilt_gndpin,OUTPUT);
  pinMode(flywheelpin,OUTPUT);
  pinMode(flywheelboostpin,OUTPUT);
  pinMode(firepin,OUTPUT);
  
  // set the power pin to high to turn it off by default;
  digitalWrite(turntable_powerpin, HIGH);
  digitalWrite(tilt_powerpin, HIGH);
  
  // set the flywheel and fire pins to off;
  digitalWrite(flywheelpin, HIGH);
  digitalWrite(firepin, HIGH);
  digitalWrite(flywheelboostpin, HIGH);
  
  
  Serial.flush();
}
void loop()
{  
  timer1_init();
  while(1)
  {
    remote_decode();
    remote_deal();
  }  
}
