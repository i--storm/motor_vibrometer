#include "I2Cdev.h"
#include "MPU6050.h"
#include <servo.h>
#include <math.h>
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
	#include "Wire.h"
#endif


#define LED_PIN 13
#define CALIBRATION_TIME 5 //Calibration time. Keep sensors still
#define MESUREMENT_TIME 5 //Time we measuring values
#define MOTOR_PIN 9 //Motor pin
#define MOTOR_STOP 1000 //Stops motor and initiates ESC
#define MOTOR_SPIN 1200 //Choose safe rotations speed. Do not let motor to pull copter into the air
#define MOTOR_SPIN_UP_TIME 1 //Delay before measurements to let motor spin up

/****************************************************************/
/*                END OF COFIGURABLE PARAMS                     */
/****************************************************************/

#define Z_ACC_CONST 16384

void serialPrintSix(int16_t lax,int16_t lay,int16_t laz,int16_t lgx,int16_t lgy,int16_t lgz,bool lln=false);

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

//Sums for average calc
float sax, say, saz;
float sgx, sgy, sgz;

//Calibrations
int16_t cax, cay, caz;
int16_t cgx, cgy, cgz;

//max values
int16_t m_ax, m_ay, m_az;
int16_t m_gx, m_gy, m_gz;

//average values
float av_ax, av_ay, av_az;
float av_gx, av_gy, av_gz;

int16_t rax, ray, raz;
int16_t rgx, rgy, rgz;

long cnt,m_cnt;

bool is_calibration=true;
bool is_measurement=false;
bool dmpReady = false;  
bool blinkState = false;

unsigned long time=0;	

Servo esc;

String serial_content = "";
char serial_character;

int motor_pwm;

void setup() {
    
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    
    Serial.begin(115200);
		
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
	
}

void loop() {
	
	// if programming failed, don't try to do anything
	if (!dmpReady) return;
		

	if(is_calibration){
		calibrate();
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
			if(abs(az-Z_ACC_CONST)>m_az) m_az=abs(az-Z_ACC_CONST);
			if(abs(gx)>m_gx) m_gx=abs(gx);
			if(abs(gy)>m_gy) m_gy=abs(gy);
			if(abs(gz)>m_gz) m_gz=abs(gz);
			
			
			
			av_ax=(float)(av_ax*m_cnt+abs(ax))/(m_cnt+1);
			av_ay=(float)(av_ay*m_cnt+abs(ay))/(m_cnt+1);
			av_az=(float)(av_az*m_cnt+abs(az-Z_ACC_CONST))/(m_cnt+1);
			av_gx=(float)(av_gx*m_cnt+abs(gx))/(m_cnt+1);
			av_gy=(float)(av_gy*m_cnt+abs(gy))/(m_cnt+1);
			av_gz=(float)(av_gz*m_cnt+abs(gz))/(m_cnt+1);
			
			
			m_cnt++;
			
			if(m_cnt%200==0){
				Serial.print(".");
			}
			
			if(micros()-time>MESUREMENT_TIME*1000000){
				is_measurement=false;
			}

		}

		esc.writeMicroseconds(MOTOR_STOP);

		Serial.println("");
		Serial.println("\tax\tay\taz\tgx\tgy\tgz");
		Serial.print("Max:\t");
		serialPrintSix(m_ax,m_ay,m_az,m_gx,m_gy,m_gz,true);
		Serial.print("Avg:\t");
		serialPrintSix(av_ax,av_ay,av_az,av_gx,av_gy,av_gz,true);
		
		Serial.print("Measurements count: ");
		Serial.println(m_cnt);

	}
	
	    
}

void startMeasurement(){
	Serial.println(F("\nSend motor PWM number or .(dot) to start measurement: "));
	serial_content="";
	while (Serial.available() && Serial.read()); // empty buffer
	while (!Serial.available());                 // wait for data
	while (Serial.available()){
		serial_character = Serial.read();
		serial_content.concat(serial_character);
	}
	if(serial_content!=""){
		if(serial_content!="."){
			Serial.print("Starting motor at: ");
			Serial.println(serial_content);
			motor_pwm=serial_content.toInt();
		}else{
			Serial.print("Starting motor at: ");
			Serial.println(MOTOR_SPIN);
			motor_pwm=MOTOR_SPIN;
		}
	}

	esc.writeMicroseconds(motor_pwm);

	Serial.println("Spin up...");
	delay(MOTOR_SPIN_UP_TIME*1000);
	

	m_ax=0;
	m_ay=0;
	m_az=0;
	m_gx=0;
	m_gy=0;
	m_gz=0;

	m_cnt=0;

	is_measurement=true;
	
	Serial.print("Measuring ");
	Serial.print(MESUREMENT_TIME);
	Serial.println(" seconds...");

}

void serialPrintSix(int16_t lax,int16_t lay,int16_t laz,int16_t lgx,int16_t lgy,int16_t lgz,bool lln){

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

void calibrate(){
	time=micros();
	
	Serial.println("");
	Serial.print("Calibrating");
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
	
	
	cax=-1*sax/cnt/8.71;
	cay=-1*say/cnt/9.03;
	caz=(Z_ACC_CONST-saz/cnt)/8.52;
	cgx=-1*sgx/cnt/3.34;
	cgy=-1*sgy/cnt/4.11;
	cgz=-1*sgz/cnt/4.05;
	
	Serial.println("");
	Serial.println("Calibrations:");
	serialPrintSix(cax,cay,caz,cgx,cgy,cgz,true);
	
	accelgyro.setXAccelOffset(cax);
	accelgyro.setYAccelOffset(cay);
	accelgyro.setZAccelOffset(caz);
	accelgyro.setXGyroOffset(cgx);
	accelgyro.setYGyroOffset(cgy);
	accelgyro.setZGyroOffset(cgz);	
}