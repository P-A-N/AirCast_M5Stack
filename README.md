# CoSpace_M5Stack


## dependency

- [arduinojson](https://arduinojson.org/v6/doc/installation/)
- [M5Stack](https://github.com/m5stack/M5Stack)

## how to install and run
日本語の解説は[こちら](https://raspberrypi.mongonta.com/howto-start-m5stack-arduinoide/)

- Donwload ArduinoIDE from [here](https://www.arduino.cc/en/Main/Software) and install it in your computer
- Set below url to **Additional Boards Manager URL** in *File / Preferences* 
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

- Install **esp32** by boardmanager (*tool/board/boardmanager*) 

- Install **M5Stack** and **arduinojson** library by LibraryManager (*Sketch/Include Libraries/Manage Libraries*)

- Select Serial Port (*Tool/Serial Port*)

- Finally you can install this program and run by Arduino IDE.