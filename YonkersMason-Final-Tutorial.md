# Flora Blink Shoe

A wearable shoe that responds to movement with accelerating LED blinks! Semi-inspired by [Skechers light up shoes](https://www.skechers.com/technologies/features/lights/?start=0&sz=24)

---

## Table of Contents

- [Overview](#overview)
- [Parts List](#parts-list)
- [Code Walkthrough](#code-walkthrough)
- [Let's Get Sewing](#lets-get-sewing)
- [Troubleshooting](#troubleshooting)
- [Calibrating](#calibration--testing)
- [Done!](#done)

---

## Overview

This guide will walk you through how to create a motion activated light up shoe that is useful for:
- Nighttime visibility
- Practicing your sewing skills
- Recreating the cool shoes you had as a kid!

I created this shoe with the intention of modifying my running shoes to make me easier to see at night. The more frequently the wearer moves, the faster the LEDs blink. This guide should be usable for most shoes, but in this case I am working with a pair of Nike shoes covered in a consistent fabric material across most of the shoe.

---

## Parts List

- 1x [Adafruit flora](https://www.adafruit.com/product/659)
- The following from the [Lilypad Protosnap Plus](https://www.sparkfun.com/lilypad-protosnap-plus.html):
  - 3x LED (any color works!)
  - 1x Switch
- 1x [tilt sensor](https://www.sparkfun.com/tilt-sensor-at407.html?gQT=1)
- 1x [LiPo battery](https://www.amazon.com/EEMB-Rechargeable-Connector-Wireless-Polarity/dp/B0B7N2T1TD)
- 1x Shoe (any)
- 1x Spool of thread (any)
- 1x [Leather needle](https://www.amazon.com/dp/B08H86BQJB)

![Lay out!](images/01_parts_list.jpg)
---

## Code Walkthrough

The code required for this project is relatively simple! For in-depth information see the [full code](blink_shoe.ino). I will explain the helper functions here.

### handleMovement
```
void handleMovement() {
  unsigned long currentTime = millis();

  // Only increase activity level on the "active" tilt state
  if (currentTiltState == ACTIVE_STATE) {
    if (ledFlashing) {
      // Already flashing - increase activity level
      if (activityLevel < maxActivityLevel) {
        activityLevel++;
      }
      // Reset the waiting period
      waitingToStop = false;
    } else {
      // Start flashing
      ledFlashing = true;
      lastFlashTime = currentTime;
      ledState = HIGH;
      digitalWrite(LED_PIN_10, ledState);
      digitalWrite(LED_PIN_12, ledState);
      activityLevel++;
      Serial.println("Started");
    }

    updateFlashRate();
    lastActivityTime = currentTime;
  }
}
```
This function determines if the wearer is moving, and makes the decision to up the "activity level". The activiy level is what determines how fast the shoe should be blinking. It starts by grabbing the current time (something that this program does a lot!) using the `millis()` function. This function gets the time in milliseconds since the Flora started up. This gets used frequently to determine the time since something has happened! It is also why many of the global variables used are unsigned longs, which are meant to be compared to values returned by `millis()`.

After getting the time, the current tilted state is checked. The tilt sensor I used for this project has two states, tilted and untilted. Since I am not exactly measuring how tilted the shoe is, but whether it is moving, I only care about one of the two possible states and whether or not that state has changed and then returned back.

If the LEDs are already flashing then the activity level is increased, this means the wearer is in motion and has remained in motion! Otherwise the wearer has just started moving, the LEDs must begin to flash and the activity level is incremented.

The flash speed is then adjusted and the time is recorded. This time is used to decrement the activity level once the wearer stops moving.

### updateFlashRate
```
void updateFlashRate() {
  if (activityLevel == 0) {
    currentFlashInterval = baseInterval;
  } else {
    // Calculate blink rate linearly
    float minHz = 1000.0 / baseInterval;
    float maxHz = 1000.0 / fastestInterval;
    float hzStep = (maxHz - minHz) / maxActivityLevel;

    float targetHz = minHz + (hzStep * activityLevel);
    currentFlashInterval = (unsigned long)(1000.0 / targetHz);
  }
}
```
This is the other important helper function! This function is responsible for calculating and updating the rate at which the LEDs blink. The math behind the calculation works by finding the max and min Hz relative to the slowest and fastest time intervals, then finding a linear step size based on the number of possible activity levels. This step size is then multiplied by the activity level to arrive that the rate the LEDs will flash at each level.

---

## Let's Get Sewing

### Getting started
I am going to have three LEDs. One on the front, outer side, and rear of the shoe. I'll begin by attaching conductive thread to the negative connectors of each LED:
![Negative LED hookups](images/02_staged_thread.jpg)
Make sure that you loop through at least a few times before tying off and to use more thread than you think you'll need! The last thing we want is a loose connection somewhere that will be hard to [troubleshoot](#troubleshooting).
- From here on out I will not go in-depth on connecting conductive thread to connectors on sensors and actuators. The method that I use goes as follows:
  - Loop through the connector once.
  - Tie two safety knots (simple over under half knots).
  - Loop through 3-5 additional times, ensuring that the loops are tight.

### Ground Circuit
Now that the LEDs have their negative connectors wired up, let's get them attached to the shoe! The wiring setup this project uses involves a ground (negative) circuit that goes around the base of the shoe. This ensures that things stay simple as the LEDs will share the same ground.

![First LED](images/03_led.jpg)
Here we have our first LED, the front LED, sewn onto the shoe. We are going about this one LED at a time and then tying them together to connect the ground circuit to ensure that each LED is placed exactly where we want it!

![First LED tail](images/04_led.jpg)
Go at least halfway in each direction to where you want to put the next LED, that way the wires can meet in the middle and be tied together.
![First LED tail](images/05_led.jpg)
Here is the other end! I'll go into less detail about the other two LEDs as the same process is reapeated!

![Second LED](images/06_led.jpg)
![Third LED](images/07_led.jpg)
Here we have the second and third LEDs sewn in! Do not worry about the positive connections yet, we do not want to get started on those until the Flora is installed!

![Switch](images/08_switch.jpg)
I decided to place the switch on the outer side of the shoe, so I insatlled it between where the front and side LED's negative threads meet. This switch functions as an analog way to connect and disconnect the ground!

![Attaching the Flora](images/09_flora.jpg)
Up next is attaching the Flora, the most important piece of this project! It needs to be in place before wiring the positive threads from the LEDs or connecting the other end of the switch. If it is not in place, you'll wind up with loose threads once you attach it! I decided to choose two adjacent pinouts that I do not intend to use and used regular thread to keep it in place. The conductive threads that will connect to the pinouts I intend to use will help keep the Flora more secure than how it is now.
- Make sure that it is already somewhat snug before moving on! We do not want it shifting around too much once we begin connecting everything together!

![Connecting the ground](images/10_ground.jpg)
Now it is time to connect the ground circuit to the Flora! You can attach anywhere on the ground circuit (with the exception of the part betweent the switch and the Flora) to any pinout that reads GND.

![Ground closeup](images/11_ground.jpg)
Here is a closeup of how overboard I go in connecting threads to the flora. You can probably get away with less loops around the connector, but I like to play it safe!

![Connecting switch](images/12_switch.jpg)
Here is the last piece of the ground circuit! Connect the other end of the switch to the 3.3V pinout on the Flora.

### Positive Connections & Test
Next up is attaching the positive ends of our LEDs to the Flora. You may use the same pin for each LED, but for convenience I decided to use a total of two pins, one with one LED & the other with two LEDs.

![First positive](images/13_test.jpg)
Similar to what we have been doing, connect the positive end of an LED to a pinout on the Flora. Here I am using pin 10.

It is a good idea to test that our ground circuit actually works, since this is the first point at which that is possible. Load up the Flora with a simple program such as:
```
const int LED_PIN = 10;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  // Do nothing - LEDs stay on
}
```
This program turns the LED on pin 10 on and keeps it on, great for testing!

![Second positive](images/14_test.jpg)
Here is my second positive connection. This time the side LED. If you hook it up to a different pinout, make sure to update the test program!

![Current state](images/15_zoom_out.jpg)
Here you can see that I am testing both LEDs at once, always a good idea to make sure the ground was wired right this way!

### Tilt Sensor
Up next is installing the tilt sensor. This is the first part of this project that did not initially go my way!

An important note:
- The tilt sensor must be "upright", meaning that the side the connectors comes out of must be facing down.
- I overlooked this detail when planning this project and had to move the sensor from inside the shoe to the outside.
- Those holes you can see inside the shoe would have been perfect otherwise!

![Tilt sensor](images/16_tilt.jpg)
Start by bending the connectors that come out of the sensor like so, it is going on the side of the shoe with the connectors piercing the side to help keep it in place. Note that it is upside-down in this image!

![Tilt sensor installed](images/17_tilt.jpg)
Here you can see the sensor installed after multiple failed attempts to place it inside the shoe. Every shoe is different, so try what you feel works best to secure it with regular thread. Like myself your heart will guide you in the right direction eventually!

![Tilt sensor connectors](images/18_tilt.jpg)
The next step is attaching the tilt sensor to the rest of the circuit. I did this by tying and wrapping conductive threads to both connectors and then routing to the appropriate location. Here the negative connector is wired into the rest of the negative circuit and the positive is wired onto the Flora at pin 6.

I had to cross some paths for the connections here, so make sure that you do not allow them to touch! Sewing on the opposite side as the thread you are crossing helps.

### Battery
Next up is installing the battery, almost done! With the shoe I am using this was the easiest part, other shoes may not offer this convenience. The inside of my shoe is lined with a loose, flexible material that was extremely easy to cut and slip the battery into. I cannot even notice it when wearing the shoe!

![Battery](images/19_battery.jpg)
Here you can see that I made a small cut to the inner lining of the shoe. The battery I am using is fairly bulky, but was easy to insert inside. It is also easy to remove for replacement down the line!

Other shoes may require more sophisticated battery hiding. My backup plan if this did not work was to cut a portion of the insole out to place the battery in. It is my understanding that this is where "smart shoes" that contain bluetooth capabilities store their batteries and other components.

---

## Troubleshooting

Ensuring that things are working correctly with the physical aspect of this project is fairly straightforward. I will cover the areas that I had to spend the most time experimenting with.

### LEDs lighting up
If you are experiencing issues with the LEDs not lighting up correctly try the following:
- Shoes move around, ensure that you did not sew any of the connections too tight causing them to fray or snap.
- Ensure that the knots you tied between LEDs are solid, I mistakenly tied a false knot that came undone when I was not paying attention.

If you are experiencing issues with the tilt sensor try the following:
- Ensure that it is upright, the connector side should be facing the ground.
- If you had to cross any threads, ensure they are not touching.
- Ensure that it is mounted close to perpendicular to the ground. The motion we are recording depends on this.

---

## Calibration & Testing

Testing the shoe is very straightforward:
- Connect the Flora to your pc via a sufficiently long usb cable.
- Monitor the serial ouput on the arduino IDE.
- Move the shoe or take a step, the activity level should increase by 1.
  - If you are seeing that the activity level is increasing by more than 1, increase the debounce delay value.
  - If you are not seeing the activity level increase on steps, decrease the debounce delay value.

Most of the testing on the software side will relate to the debounce delay. It was implemented to prevent false positives as the tilt sensor has only two states, small movements may cause rapid fluctuations in this state and the debounce delay ensures that the state is maintained for at least some time before it is counted as a movement.

Here I will detail what values to adjust in the code when testing the device post construction.
- If you used different LED pinouts, check the pin definiton variables.
- To make the shoe more/less sensitive, adjust the debounce delay. This value increases sensitivity at the cost of false positives.
- To increase the flash-rate, decrease the base interval & fastest interval.
- To decrease the flash-rate, increase the base interval & fastest interval.

---

## Conclusion
This guide followed the steps that I took creating my own DIY light up shoe out of basic arduino parts and a running shoe. Your milage may vary with other types of shoes or design choices, but this guide should serve as a reference and starting point for similar projects! Thank you for reading.