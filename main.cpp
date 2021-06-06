#include <mbed.h>
#include <TextLCD.h>
#include <Keypad.h>
#include <HCSR04.h>

// initialization
TextLCD lcd(PTE29, PTE30, PTE20, PTE21, PTE22, PTE23, TextLCD::LCD16x2);
Keypad pad(PTB11, PTB10, PTB9, PTB8, PTE5, PTE4, PTE3, PTE2);
HCSR04 sensor(PTA1, PTA2);

//PWM output pins
PwmOut led1(PTD2);//red
PwmOut led2(PTD0);//green
PwmOut ledGreen(LED_GREEN);
PwmOut ledRed(LED_RED);
PwmOut ledBlue(LED_BLUE);


char read_char(void);
void display_distance(int distance);
         

int main()
{
    char distance;
    //turn-off all LEDs at powerup
    led1.period(0.01f);  // 4 second period
    led1.write(0.0f);  // 50% duty cycle 

    led2.period(0.01f);  // 4 second period
    led2.write(0.0f);  // 50% duty cycle 
    
    ledBlue = 1;
    ledGreen = 1;
    ledRed = 1;
    
    char key1, key2, key3;
    lcd.cls();
    lcd.printf("Enter a distance:");
    
    //wait for distance
    key1 = read_char();
    wait(0.5);
    lcd.printf("%c", key1);
    key2 = read_char();
    wait(0.5);
    lcd.printf("%c", key2);
    key3 = read_char();
    wait(0.5);
    lcd.printf("%c", key3);

    distance = (key1 - 48) * 100 + (key2 - 48) * 10 + (key3 - 48);//input
    wait(2);
    lcd.cls();
    
    int dist_sensor;
    int dist2;
    //program code
    while(true)
    {
        dist_sensor = sensor.distance(CM);
        display_distance(dist_sensor);
        wait(0.3);
        dist2 =  sensor.distance(CM);
        if (dist2 > dist_sensor)
        {
            //RGD ---> blue
            ledBlue = 0;
            ledGreen = 1;
            ledRed = 1;
        }
        else if (dist2 < dist_sensor)
        {
            //RGB ---> yellow
            ledBlue = 1;
            ledGreen = 0;
            ledRed = 0;
        }
        else
        {
            //RGB off
            ledBlue = 1;
            ledGreen = 1;
            ledRed = 1;
        }
        
        if (distance < dist_sensor)
        {
            //both LEDs off
            led2.write(0.0f); 
            led1.write(0.0f);   
        }
        else
        {
            if ((3 * dist_sensor > distance) && (3 * dist_sensor <= 2 * distance))
            {
                //discretly dim the red LED
                if (dist_sensor >= (distance * 5)/15 && dist_sensor < (distance * 6)/15)
                {
                    led1.write(1.0f);
                    led2.write(0.0f);
                }
                else if (dist_sensor >= (distance * 6)/15 && dist_sensor < (distance * 7)/15)
                {
                    led1.write(0.8f);
                    led2.write(0.0f);
                }
                else if (dist_sensor >= (distance * 7)/15 && dist_sensor < (distance * 8)/15)
                {
                    led1.write(0.6f);
                    led2.write(0.0f);
                }
                else if (dist_sensor >= (distance * 8)/15 && dist_sensor < (distance * 9)/15)
                {
                    led1.write(0.4f);
                    led2.write(0.0f);
                }
                else
                {
                    led1.write(0.2f);
                    led2.write(0.0f);
                }
                
            } 
            else if (3 * dist_sensor <= distance)  
            {
                //red LED on in case of danger
                led1.write(1.0f);
                led2.write(0.0f);
            }
            else 
            {
                //green led on if no danger
                led1.write(0.0f);
                led2.write(1.0f);
            }
        }
        lcd.cls();
    } 
}

/*This function waits for a key and 
returns the ASCII when a key pressed*/
char read_char(void)
{
    int released = 1;
    char key;
    while(1)
    {
        key = pad.ReadKey();               

        if(key == '\0')
            released = 1;                       

        if((key != '\0') && (released == 1)) 
            return key;            
    }   
}

/*This function displays the distance on the LCD*/
void display_distance(int distance)
{
    int first_digit = distance % 10;
    int second_digit = (distance / 10) % 10;
    int third_digit = distance / 100;
    if (third_digit != 0)
    {
        lcd.putc(third_digit + '0');
        lcd.putc(second_digit + '0');
    }
    else if (second_digit != 0)
    {
         lcd.putc(second_digit + '0');
    }
    lcd.putc(first_digit + '0');
}
