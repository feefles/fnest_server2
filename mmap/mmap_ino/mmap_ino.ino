
#include "./mmap.h"

using namespace std;

/* assume /tmp mounted on /tmpfs -> all operation in memory */
/* we can use just any file in tmpfs. assert(file size not modified && file permissions left readable) */
mmap_Data* p_mmapData; // here our mmapped data will be accessed

#include <Servo.h>


// Tmperature measurement
int temp_meas_raw = 0;
float temp_meas = 0.;
#define BETA    0.95
#define TEMP_PIN    5  // A5


// Buttons
#define B_DOWN_PIN  7
#define B_UP_PIN    6
unsigned char b_down_state = 0;
unsigned char b_up_state = 0;


// Servo
#define SERVO_PIN  9
Servo therm_servo;
#define SERVO_MIN_ANG    10      //15
#define SERVO_MAX_ANG    142    //152
#define SERVO_FET_PIN  8
#define SERVO_DISABLE_COUNTS  10
char servo_disable_counter = 0;

// Other state
#define MAX_TEMP  80
#define MIN_TEMP  50
char temp_set = 64;
char temp_change_flag = 0;


void exitError(const char* errMsg) {
//  /* print to the serial Arduino is attached to, i.e. /dev/ttyGS0 */
//  String s_cmd("echo 'error: ");
//  s_cmd = s_cmd + errMsg + " - exiting' > /dev/ttyGS0";
//  system(s_cmd.c_str()); 
  exit(EXIT_FAILURE);
}  

void setup() {
  int fd_mmapFile; // file descriptor for memory mapped file
  /* open file and mmap mmapData*/
  fd_mmapFile = open(mmapFilePath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
//  if (fd_mmapFile == -1) exitError("couldn't open mmap file"); 
  /* make the file the right size - exit if this fails*/
  if (ftruncate(fd_mmapFile, sizeof(mmap_Data)) == -1) exitError("couldn' modify mmap file");
  /* memory map the file to the data */
  /* assert(filesize not modified during execution) */
  p_mmapData = static_cast<mmap_Data*>(mmap(NULL, sizeof(mmap_Data), PROT_READ | PROT_WRITE, MAP_SHARED, fd_mmapFile, 0));  
  if (p_mmapData == MAP_FAILED) exitError("couldn't mmap"); 
  
 
  // Servo Object
  therm_servo.attach(SERVO_PIN);
    
  // Pins
  pinMode(B_DOWN_PIN,INPUT);
  pinMode(B_UP_PIN,INPUT);
  pinMode(SERVO_FET_PIN,OUTPUT);
  
  digitalWrite(SERVO_FET_PIN, HIGH);

  set_temp(temp_set);
  temp_meas_raw = analogRead(TEMP_PIN);
  read_temp();

  b_down_state = digitalRead(B_DOWN_PIN);  
  b_up_state = digitalRead(B_UP_PIN);
  
  Serial.begin(115200);


}

void loop() {
 
  
  if ((int)p_mmapData->set_temp != (int)temp_set) {
    system("echo change set temp > /dev/ttyGS0");
    Serial.println((int)temp_set);
    temp_change_flag = 1;
    servo_disable_counter = 0;
    temp_set = p_mmapData->set_temp;
  }
  if (abs(p_mmapData->cur_temp - read_temp()) > .1) {
    system("echo current temp change > /dev/ttyGS0");
    p_mmapData->cur_temp = read_temp();  
  }  
  
   b_down_state = digitalRead(B_DOWN_PIN);  
   b_up_state = digitalRead(B_UP_PIN);
  
  if (b_down_state && !b_up_state) {
    temp_set -= 1;
    temp_change_flag = 1;
    servo_disable_counter = 0;
  }
  else if (!b_down_state && b_up_state) {
    temp_set += 1;
    temp_change_flag = 1;
    servo_disable_counter = 0;
  }
  else if (!b_down_state && !b_up_state) {
    servo_disable_counter++;
  }
  if (servo_disable_counter > SERVO_DISABLE_COUNTS) {digitalWrite(SERVO_FET_PIN, LOW);}  
  
  if (temp_set < MIN_TEMP) {temp_set = MIN_TEMP;}
  if (temp_set > MAX_TEMP) {temp_set = MAX_TEMP;}
  
  if (temp_change_flag) {
    set_temp(temp_set);
    temp_change_flag = 0;
  }
  
  read_temp();
  delay(50);  
}

void set_temp(char temp) {
  digitalWrite(SERVO_FET_PIN, HIGH); 
  p_mmapData->set_temp = temp;
  therm_servo.write(map(temp, MIN_TEMP, MAX_TEMP, SERVO_MIN_ANG, SERVO_MAX_ANG));
  Serial.println((int)temp);
}

float read_temp() {
  temp_meas_raw = analogRead(TEMP_PIN);
  temp_meas = BETA*(float)temp_meas + (1.-BETA)*((((500. * (temp_meas_raw-0)) / 1023.0) - 273.15) * (9./5.) + 32.);
  return temp_meas;
}
void debug(void) {
  delay(1000);
//  Serial.println(analogRead(TEMP_PIN));
//  Serial.println(temp_meas, 1);
//  Serial.println((int)temp_set);
  Serial.println((int)p_mmapData->cur_temp);
}
