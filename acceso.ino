#define ALARM_DEACTIVATION_HOUR 13
#define ALARM_ACTIVATION_HOUR   20
#define ALARM_DURATION          5 

#define DOOR_SENSOR_PIN     D0
#define SYSTEM_ARMED_LED    D1
#define SYSTEM_ALARM        A0
#define ON                  LOW
#define OFF                 HIGH
#define ACTIVATED           HIGH
#define DEACTIVATED         LOW


#define ALARM_DURATION_mSECS    (ALARM_DURATION * 1000 * 60)

int alarm = 0;
bool bSystemArmed=FALSE;
bool bAlarmON=FALSE;

int State=0;
int CurrentState=0;
int i = 0;





Timer ApagarAlarma(ALARM_DURATION_mSECS,fnApagarAlarma,TRUE);


/*****************************************************************************************************/
void fnApagarAlarma()
{
    digitalWrite(SYSTEM_ALARM,DEACTIVATED);
   // ApagarAlarma.stop();
    Serial.println("Alarma Desactivada");

}


/*****************************************************************************************************/
void tweeterCallback(const char *event, const char *data)
{
    i++;
    Serial.print(i);
    Serial.print(event);
    Serial.print(", data: ");
    if (data)
    Serial.println(data);
    else
    Serial.println("NULL");
    
    digitalWrite(D1,LOW);
    delay(6000);
    digitalWrite(D1,HIGH);

}


/*****************************************************************************************************/
void myHandler(const char *event, const char *data)
{
    /*
    i++;
    Serial.print(i);
    Serial.print(event);
    Serial.print(", data: ");
    if (data)
    Serial.println(data);
    else
    Serial.println("NULL");
    */
    Serial.println("Alarma Encendida");
    delay(4000);
    Serial.println("Alarma Apagada");
}




/*****************************************************************************************************/
void mySysArming(const char *event, const char *data)
{
    if(strcmp("Armed",data))
    {
        bSystemArmed=FALSE;    
        digitalWrite(SYSTEM_ARMED_LED,OFF);
        Serial.println("System Disarmed");
    }
    else
    {
        bSystemArmed=TRUE;    
        digitalWrite(SYSTEM_ARMED_LED,ON);
        Serial.println("System Armed");
    }
}


/*****************************************************************************************************/
void setup() 
{
    /* Global Variables Init (just in case) */
    bSystemArmed=FALSE;
    
    
    /* Peripheral Configuration */
    Serial.begin(9600);                         // Serial Port Configuration
    pinMode(DOOR_SENSOR_PIN,INPUT);             // Door Sensor
    pinMode(SYSTEM_ARMED_LED,OUTPUT);           // System Armed Indicator
    pinMode(SYSTEM_ALARM,OUTPUT);               // System Alarm

    /* Suscriptions configuration */   
    Particle.subscribe("Alarm", myHandler,MY_DEVICES);
    Particle.subscribe("SystemArmed", mySysArming,MY_DEVICES);
    Particle.subscribe("pptweeter", tweeterCallback,MY_DEVICES);

    /* Peripheral initial state */
    digitalWrite(SYSTEM_ARMED_LED,ON);
    digitalWrite(SYSTEM_ALARM,DEACTIVATED);


    Time.zone(-6);
    
    Serial.print("Day: ");
    Serial.println(Time.weekday());
    Serial.print("Hour: ");
    Serial.println(Time.hour()); // 1400647897

    if(Time.weekday()>1)
    {
        // Monday - Saturday
        if((Time.hour() < ALARM_DEACTIVATION_HOUR) ||  (Time.hour() > ALARM_ACTIVATION_HOUR))
        {
            bSystemArmed=TRUE;
        }
    }
    else
    {
        // Sunday
        bSystemArmed=TRUE;
    }

    if(bSystemArmed == TRUE)
    {
        Serial.print("System Armed on Boot");
        digitalWrite(SYSTEM_ARMED_LED,ON);

    }
    else
    {
        Serial.print("System Disarmed on Boot");
        digitalWrite(SYSTEM_ARMED_LED,OFF);
    }
}


/*****************************************************************************************************/
void loop() 
{

    if(digitalRead(DOOR_SENSOR_PIN))
    {
        CurrentState=1;
        //digitalWrite(D1,HIGH);
    }
    else
    {
        CurrentState=0;
        //digitalWrite(D1,LOW);
    }
    /*
    Serial.print("Sus: ");
    Serial.print(CurrentState,DEC);
    Serial.print(State,DEC);
    Serial.print("\n\r");
    */
    //Serial.print("\r\n Time: "); // 1400647897
    
    
    if(CurrentState!=State)
    {
        if(CurrentState==0)
        {
            State=0;
            if(bSystemArmed==TRUE)
            {
                Particle.publish("Door","Closed");
            }
            Serial.println("Puerta Cerrada");
            //digitalWrite(SYSTEM_ARMED_LED,ON);
        }
        else
        {
            State=1;
            if(bSystemArmed==TRUE)
            {
                Particle.publish("Door","Open");
                Serial.println("Alarma Activada");
                digitalWrite(SYSTEM_ALARM,ACTIVATED);
                bAlarmON=TRUE;
                ApagarAlarma.start();
                
            }
            Serial.println("Puerta Abierta");

            //digitalWrite(SYSTEM_ARMED_LED,OFF);
            //digitalWrite(SYSTEM_ALARM,OFF);

        }
        /*
        Serial.print("New: ");
        Serial.print(CurrentState,DEC);
        Serial.print(State,DEC);
        Serial.print("\n\r");
        */
    }

    /* Check if Alarm has been turned ON */


//    Serial.print(alarm,DEC);
    delay(2000);
}