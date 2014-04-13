#include "I2Cdev.h"
#include "MPU6050.h"
#include <servo.h>


#define LED_PIN 13
#define CALIB_CNT 1000
#define MEASUREMENT_CNT 1000
#define CALIBRATION_TIME 5
#define MESUREMENT_TIME 5
#define MOTOR_PIN 9
#define MOTOR_STOP 1000


// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;


//Sums for average calc
long sax, say, saz;
long sgx, sgy, sgz;

//Calibrations
int16_t cax, cay, caz;
int16_t cgx, cgy, cgz;

//max values
int16_t m_ax, m_ay, m_az;
int16_t m_gx, m_gy, m_gz;

int16_t rax, ray, raz;
int16_t rgx, rgy, rgz;
long cnt,m_cnt;

bool is_calibration=true;
bool is_measurement=false;
bool dmpReady = false;  // set true if DMP init was successful

unsigned long time=0;	

Servo esc;
String serial_content = "";
char serial_character;
int motor_pwm;



bool blinkState = false;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(115200);
	//while (!Serial); // wait for Leonardo enumeration, others continue immediately
	
	Serial.println(F("\nSend any character to begin calibration: "));
	while (Serial.available() && Serial.read()); // empty buffer
	while (!Serial.available());                 // wait for data
	while (Serial.available() && Serial.read());

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
		
	dmpReady = true;   
	
	/*accelgyro.setXAccelOffset(-1256);
	accelgyro.setYAccelOffset(-1191);
	accelgyro.setZAccelOffset(1125);
	
	accelgyro.setXGyroOffset(12);
	accelgyro.setYGyroOffset(28);
	accelgyro.setZGyroOffset(-11);*/
	
	accelgyro.setXAccelOffset(0);
	accelgyro.setYAccelOffset(0);
	accelgyro.setZAccelOffset(0);
	
	accelgyro.setXGyroOffset(0);
	accelgyro.setYGyroOffset(0);
	accelgyro.setZGyroOffset(0);
	
	

    // configure Arduino LED for
    pinMode(LED_PIN, OUTPUT);
	
	esc.attach(MOTOR_PIN);

	esc.writeMicroseconds(MOTOR_STOP);
	
	rax=0;
	ray=0;
	raz=0;
	rgx=0;
	rgy=0;
	rgz=0;
	cnt=0;
	m_cnt=0;
	
	Serial.println("Calibrating");
}

void loop() {
	
	
	
	// if programming failed, don't try to do anything
	if (!dmpReady) return;
		

	if(is_calibration){
		time=micros();
		cnt=0;
		while(is_calibration){

			// read raw accel/gyro measurements from device
			accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

			sax=sax+ax;
			say=say+ay;
			saz=saz+az;
			sgx=sgx+gx;
			sgy=sgy+gy;
			sgz=sgz+gz;

			if(cnt%200==0){
				Serial.print(".");
			}	
			
		
			if(micros()-time>CALIBRATION_TIME*1000000){
				is_calibration=false;
			}
			cnt++;
		}
		
		/*Serial.println("");
		Serial.println("Summs:");
		
		Serial.print(sax); Serial.print("\t");
		Serial.print(say); Serial.print("\t");
		Serial.print(saz); Serial.print("\t");
		Serial.print(sgx); Serial.print("\t");
		Serial.print(sgy); Serial.print("\t");
		Serial.println(sgz);*/
		
		Serial.println("");
		Serial.println("Calibrations:");
		
		cax=-1*sax/cnt/8.71;
		cay=-1*say/cnt/9.03;
		caz=(16384-saz/cnt)/8.52;
		
		cgx=-1*sgx/cnt/3.34;
		cgy=-1*sgy/cnt/4.11;
		cgz=-1*sgz/cnt/4.05;
		
		
		Serial.print(cax); Serial.print("\t");
		Serial.print(cay); Serial.print("\t");
		Serial.print(caz); Serial.print("\t");
		Serial.print(cgx); Serial.print("\t");
		Serial.print(cgy); Serial.print("\t");
		Serial.println(cgz);
		
		accelgyro.setXAccelOffset(cax);
		accelgyro.setYAccelOffset(cay);
		accelgyro.setZAccelOffset(caz);
		
		accelgyro.setXGyroOffset(cgx);
		accelgyro.setYGyroOffset(cgy);
		accelgyro.setZGyroOffset(cgz);
		
		cnt++;
		
	}

	startMeasurement();

	if(is_measurement){

		time=micros();
		m_cnt=0;

		while(is_measurement){

			// read raw accel/gyro measurements from device
			accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

			if(abs(ax)>m_ax) m_ax=abs(ax);
			if(abs(ay)>m_ay) m_ay=abs(ay);
			if(abs(az-16384)>m_az) m_az=abs(az-16384);
			if(abs(gx)>m_gx) m_gx=abs(gx);
			if(abs(gy)>m_gy) m_gy=abs(gy);
			if(abs(gz)>m_gz) m_gz=abs(gz);

			
			if(micros()-time>MESUREMENT_TIME*1000000){
				is_measurement=false;
			}

		}

		esc.writeMicroseconds(MOTOR_STOP);

		Serial.println("");
		Serial.println("Max:");
		Serial.print(m_ax); Serial.print("\t");
		Serial.print(m_ay); Serial.print("\t");
		Serial.print(m_az); Serial.print("\t");
		Serial.print(m_gx); Serial.print("\t");
		Serial.print(m_gy); Serial.print("\t");
		Serial.print(m_gy); Serial.print("\t");
		Serial.println("");

		

	}
	
	    
}

void startMeasurement(){
	Serial.println(F("\nSend motor PWM number to start measurement: "));
	serial_content="";
	while (Serial.available() && Serial.read()); // empty buffer
	while (!Serial.available());                 // wait for data
	while (Serial.available()){
		serial_character = Serial.read();
		serial_content.concat(serial_character);
	}
	if(serial_content!=""){
		Serial.println(serial_content);
		motor_pwm=serial_content.toInt();
	}

	esc.writeMicroseconds(motor_pwm);

	delay(3000);

	m_ax=0;
	m_ay=0;
	m_az=0;
	m_gx=0;
	m_gy=0;
	m_gz=0;

	m_cnt=0;

	is_measurement=true;

}

void serialPrintSix(int16_t lax,int16_t lay,int16_t laz,int16_t lgx,int16_t lgy,int16_t lgz,bool lln=false){

	Serial.print(lax); Serial.print("\t");
	Serial.print(lay); Serial.print("\t");
	Serial.print(laz); Serial.print("\t");
	Serial.print(lgx); Serial.print("\t");
	Serial.print(lgy); Serial.print("\t");
	Serial.print(lgz); Serial.print("\t");
	if(lln){
		Serial.println("");
	}

}



void countRounds(){
	
	if(cnt==0){
		rax=ax;
	}else{		
		rax=rax+(ax-rax)/100;
	}
	
	
	
	if(cnt==0){
		ray=ay;
	}else{
		ray=ray+(ay-ray)/100;
	}
	
	if(cnt==0){
		raz=az;
	}else{
		raz=raz+(az-raz)/100;
	}
	
	
	if(cnt==0){
		rgx=gx;
	}else{
		rgx=rgx+(gx-rgx)/100;
	}
	
	if(cnt==0){
		rgy=gy;
	}else{
		rgy=rgy+(gy-rgy)/100;
	}
	
	if(cnt==0){
		rgz=gz;
	}else{
		rgz=rgz+(gz-rgz)/100;
	}
	
	
	
	
}