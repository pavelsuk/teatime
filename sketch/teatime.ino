/* ---------------------------------------------------------------                                                              
    TeaTime is a project based on sparkcore (spark.io)  
    that helps you to pick a teabag from a tea (even remotely)

    In order to run this code, you need to 
    1. Copy/paste this code into your spar.io/build
    2. Attach a servo to A0
    3. Test it:
    
        // first manually: 
        curl https://api.spark.io/v1/devices/<your_deviceID>/teaup -d access_token=<your_accesstoken>
        curl https://api.spark.io/v1/devices/<your_deviceID>/teadown -d access_token=<your_accesstoken>
    
        // and now - start the timer (10secs only - just for test)    
        curl https://api.spark.io/v1/devices/<your_deviceID>/startTimer -k -d access_token=<your_accesstoken> -d params=10  
        curl https://api.spark.io/v1/devices/<your_deviceID>/getTimer -k -d access_token=<your_accesstoken>

    
    Copyright Pavel Suk, Krystof Suk, 2014
    Source: https://github.com/pavelsuk

*/


const int default_tmInterval = 180;     // default time for tea 3 minutes
const int TEA_INIT = 90;                // servo initial position (90 degrees) 
const int TEA_UP = 85;                  // servo position, when teabag is up 
const int TEA_DOWN = 115;               // servo position, when teabag is down 
const int SERVO_STEP = 3;               // how much servo should move in one step (3 degrees)
const int SERVO_DELAY = 50;             // wait 50ms between steps


int tmInterval = 0; // time interval in sec 
int origTime = 0;  

int servoPos = TEA_INIT;    // variable to store the servo position

Servo myservo;  // create servo object to control a servo


void setup() {
    //Register Timer functions
    Spark.function("startTimer", startTimer);
    Spark.function("stopTimer", stopTimer);
    Spark.function("getTimer", getTimer);

    //Register Tea functions
    Spark.function("teaup", teaUp);
    Spark.function("teadown", teaDown);

    //Register general functions for manipulating servo - just for debugging
    Spark.function("setpos", setPosition);
    Spark.variable("getpos", &servoPos, INT);

    initServo();

}

void initServo()
{
    myservo.attach(A0);  // attaches the servo on the A0 pin to the servo object
    teaUp(""); // always start with teabag up
}


void loop() {

// basic timer
    if (isTeaReady()) {
        teaUp("");
        tmInterval=0;
    }
    delay(500);

}

boolean isTeaReady() {
    if (tmInterval>0) {
        return (origTime+tmInterval-Time.now()<0);  
    } else return false; // tea was not started yet or is finished already
    
}


// ---------------------------------------------------------------
// Timer (these functions should be called from outside 

int startTimer(String tminsecStr)
{
    int tminsec = tminsecStr.toInt();
    origTime = Time.now();
    if (tminsec<=0) {
      tmInterval= default_tmInterval; 
    } 
    else {
        tmInterval= tminsec;
    }
    teaDown("");
    return tmInterval; 
}

int stopTimer(String tminsecStr)
{
    tmInterval= 0;
    teaUp("");
    return tmInterval; 
}

int getTimer(String noParam)
{
    return origTime+ tmInterval-Time.now();
}


// ---------------------------------------------------------------
// Servo manipulation

int teaUp(String posValue) {
    setPositionInt(TEA_UP);
    return 200;
}    

int teaDown(String posValue) {
    setPositionInt(TEA_DOWN);
    return 200;  
}

int setPositionInt(int posValue) {
    int i= servoPos;
    
    servoPos;
    if (posValue == servoPos) return 0;
    if (i<posValue) 
    {
        for(i; i<=posValue; i+=SERVO_STEP) {
            myservo.write(i);
            delay(SERVO_DELAY);
        } 
    } else 
    {
        for(i; i>=  posValue; i-=SERVO_STEP) {
            myservo.write(i);
            delay(SERVO_DELAY);
        }
    }    
    
    myservo.write(posValue);    // since last step might go slightly over, return it back to wanted position
    servoPos = posValue;
    return 0;
}


// ---------------------------------------------------------------
// Callable function from outside to set specific position (in degrees)
int setPosition(String posValue) {
    return setPositionInt(posValue.toInt());
}
