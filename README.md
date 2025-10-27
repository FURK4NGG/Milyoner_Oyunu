<!-- Proje-Resmi -->

## 👀 Milyoner_Oyunu Overview  
<h1 align="center">AI based 28 Questions Game</h1>  


## 🚀 Features  
<h1 align="center">This is an AI-driven game designed to accurately guess your selection through a process of progressively eliminating options with increasingly specific questions, all based on your responses.<br><br>•It's powered by Google Gemini<br>•It asks a different question every time</h1>  


## 🔎 Preparation
<details>
<summary>1. Components</summary>
'1' ESP-8266
</details>

2. `Find your wifi's name and password`
3. `Go to the`[`Google AI Studio`](https://aistudio.google.com/app/api-keys)`and create your own api key`
> **Not Required:**Go to the[`Google AI Studio`](https://aistudio.google.com/app/api-keys)>F12>Network>F5>Click on any request>Security>Copy the SHA-1 fingerprint
   


## 📦 Setup 
1. `Refer to the circuit diagram`
2. `Install the 'Arduino IDE' software and open 'milyoner.ino' file`
3. `Paste your wifi, api and fingerprint inf.`
4. `File>Preferences>Additional Boards Manager URLs:(Click the double window button)`
>Paste this code  
```bash
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
5. `Click 'OK'`  
6. `Tools>Board>Boards Manager...`  
7. `Search 'esp8266' and install it` 
> ⚠️ **Warning:** Make sure you have installed the correct USB driver (CH340, CH341 or CP2102) before connecting the ESP8266 to your computer.
8. `Plug the ESP8266 into your computer`
9. `Tools>Port>'Select the esp8266's port'`
10. `Tools>Manage Libraries...>Install the libraries used in the code`
11. `Click the 'upload ➡️' button`  
✅ **To make sure it has been uploaded successfully, you should see the message 'Done uploading'**  


## 🎉 Run  
1. `Reset the esp8266`
2. `Connect to the IP address shown on the ESP’s screen or serial monitor using a device that is connected to the same network as the ESP8266.`


## 🔒 License  
<h1 align="center">📜 GPL-3.0 License</h1>  
