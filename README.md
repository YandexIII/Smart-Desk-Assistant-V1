# Smart desk clock (V1 Prototype)

## What's Working Right Now (V1)
This prototype runs on an Arduino Mega and integrates several components:
*   **Time & Temp:** A DS3231 Real-Time Clock (RTC) and a temperature/humidity sensor. 
*   **User Interface:** A 16x2 LCD screen and a physical push-button to cycle through menus.
*   **RFID Login:** An MFRC522 RFID reader. I have a Blue tag and a White tag. When I scan one, it "logs me in" on the screen and sets a specific alarm (7 AM or 10 PM). Scanning again logs me out.
*   **Smart Lighting:** An analog light sensor that turns on an LED automatically when it gets dark.

## Flaws
*   The Osoyoo IoT Wi-Fi shield that came with my Mega is practically dead so i couldnt implement wifi and resigned to using an rtc module.
*   My Ir receiver isnt working so im using a simple push button for now but planning on adding a brand new ir sensor and using a remote to make the controls and alarm setup more modular:
*   I am currently using an lcd screen with 16 characters but planning on switching to an oled .96inch screen to improve the UI and readability

## Future upgrades
1.   Switching to an esp32 and ditching the rtc to fetch the exact time and weather.
2.   Swap the LCD for a 0.96" OLED.
3.   Use an old TV remote to control the menus instead of a single push-button.
4.   Upgrade the code to support an array of multiple alarms.
5.   Try to implement a way to link the clock to some sort of google calendar to make it more usefull than a simple clock.
6.   3d printing a casing using black and translucent pla(for the led)

If you want to build this, check the `.ino` file. The RFID reader uses SPI (SS=48, RST=49). The LCD and sensors use I2C. 
