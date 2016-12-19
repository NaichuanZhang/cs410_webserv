/** \file
 * Combine multiple demos for the Two Sigma / Blinklabs EightByEight badge.
 *
 * The button is used to cycle between the different demos.
 * 
 * The pixels one is not really a separate demo; we switch to it
 * automatically if it tells us that it has received a packet.
 * If we go more than ten seconds without a packet, the current demo
 * is restarted.
 * 
 * If there are three button clicks that are 500ms apart from each other, 
 * we will toggle whether the "Pixels" demo runs (which connects to the
 * wifi network and shows the frame requested).
 */
#include "Badge.h"
#include "Pov.h"
#include "Sketch.h"
#include "TextScroll.h"
#include "Pixels.h"
#include <string.h>

Badge badge;
Pixels pixels;
TextScroll textscroll;
Sketch sketch;
Pov pov;

Demo * demos[] = {
  &sketch, // rain
  &textscroll,
};

const unsigned num_demos = sizeof(demos) / sizeof(*demos);
static unsigned demo_num = 0;
static Demo * demo;
static unsigned clicks = 0;
static char mac_buf[6*3+1];
static uint32_t last_draw_millis;
static uint32_t last_video_millis;
static uint32_t last_click_millis;
static bool draw_video;
static bool ignore_pixels;
const unsigned brightnessDivisor = 4;
static unsigned brightness = 128 * brightnessDivisor - 1;

static int temp_num = 0;
String temp_string;
const char * cc;

void nudgeBrightness() {
  //slowly growing brightness with some catch at the apex to easily stop
  unsigned x =  ((brightness++ / brightnessDivisor) % 288);
  badge.matrix.setBrightness(_min(255, x)); // for some reason min is undefined. Seems to be A Thing (https://github.com/esp8266/Arduino/issues/263)
}

void setup()
{
	badge.begin();
	badge.matrix.clear();
	badge.matrix.show();
	nudgeBrightness();

	WiFi.persistent(false);

	// do not join any wifi networks if the button is held down
	// during startup
	if (!badge.button())
	{
#if 1
		WiFi.mode(WIFI_STA);
		WiFi.begin("twosigma-blinky", "blinkblinkblink");
#else
		WiFi.mode(WIFI_AP);
		WiFi.softAP("mypocket", "BUBBLEmonkey");
		WiFi.config(IPAddress(192,168,1,4), IPAddress(0,0,0,0), IPAddress(255,255,255,0));
#endif
	}

	uint8_t mac[6];
	WiFi.macAddress(mac);
	sprintf(mac_buf, "%02x:%02x:%02x:%02x:%02x:%02x",
		mac[0],
		mac[1],
		mac[2],
		mac[3],
		mac[4],
		mac[5]
	);


	pixels.begin();
	pov.begin();


	// Initialize all of the demos and start at 0
	for(int i = 0 ; i < num_demos ; i++)
		demos[i]->begin();

	demo_num = 0;
	demo = demos[demo_num];
}


void loop()
{

  String new_str; 
  
  while (Serial.available()) {
    delay(3);  //delay to allow buffer to fill 
    
    if (Serial.available() >0) {
      
      char c = Serial.read();  //gets one byte from serial buffer
      new_str += c; //makes the string readString
    
    } 
  }

  if (new_str.length() > 1) {
      
      Serial.println(new_str); //see what was received
    
      temp_num = (int) new_str.charAt(0);

      if(new_str.length() == 2){

        temp_string = (String) new_str.charAt(1);
        
        temp_string = temp_string + "C";
        
        Serial.print(temp_string);
      }

      else if(new_str.length() == 3){

        temp_string = (String) new_str.charAt(1);
        
        String temp2 = (String) new_str.charAt(2);
        
        temp_string = temp_string + temp2+ " C" ;
        
        Serial.print(temp_string);
        
      }
      
     
  }
 
	
	const uint32_t now = millis();

	if (badge.button_edge())
	{
		// should cycle to the next demo
			demo_num = (demo_num + 1) % num_demos;
			demo = demos[demo_num];
		  last_click_millis = now;
	}

  demo->set(temp_num, temp_string);
  
	bool do_draw = demo->step(badge.ax, badge.ay, badge.az);

	// only draw the LEDs at 30Hz
	if (!do_draw && now - last_draw_millis < 30)
		
		return;
	
	last_draw_millis = now;
	
	
	demo->draw(badge.matrix);

	badge.matrix.show();
	delay(2);
 
}
