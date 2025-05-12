
# Hangar Base

The **Hangar Base** project is interactive display system inspired by mecha hangars, designed primarily for showcasing **Gunpla** (Gundam plastic models). It detects the presence of a Gunpla placed on the hangar and provides visual and/or audio feedback depending on the detection status. It combines basic sensor input with display output to create a dynamic user experience.

## 🚀 Project Description

This project is to simulate a hangar base using arduino components. The base will:

* **Display "Hangar Empty"** when no object (e.g., Gunpla) is detected.
* **Show basic stats or information** (like unit name, model type, serial number, etc.) when a Gunpla is detected on the base.
* **Play sound effects or background music**.
* **Read object-specific data** using RFID (in the future).

The system uses distance sensing to determine if an object is present, and can optionally identify specific models using RFID tags.

Notes: Originally, I attempt to use RFID (MFRC522) to identify specific Gunpla units and display unique stats. However, due to technical issues—despite correct wiring and working code—the RFID module failed to detect any tags. As a result, the project was refactored to rely entirely on a distance sensor (HC-SR04) for presence detection.

---

## 🧩 Required Components

| Component                            | Description                                            |
| ------------------------------------ | ------------------------------------------------------ |
| **OLED SSD1306 (128x64 Mono)**       | Displays visual information when Gunpla is present     |
| **LCD I2C 16x2**                     | Used for showing status text or static data            |
| **HC-SR04 Distance Sensor**          | Detects if something (Gunpla) is present in the hangar |
| **MFRC522 RFID Reader** *(Optional)* | Used to detect specific Gunpla via RFID tags           |
| **DFPlayer Mini**                    | Plays MP3 audio files such as startup sounds or themes |
| **Speaker**                          | Outputs audio from the DFPlayer Mini                   |
| **Hangar Base**                      | A physical diorama or platform for placing the Gunpla  |
| **Gunpla Model** *(or test object)*  | The model that activates the display and sound         |

---

## 🔧 How It Works

1. The **HC-SR04** distance sensor monitors the presence of a model in the hangar.
2. When **no object is detected**, the system shows "Hangar Empty" on the displays.
3. When a **Gunpla is detected**, the system:

   * Displays predefined stats (e.g., unit name, serial code) on the **OLED or LCD**.
   * (Optionally) Identifies the specific model via **RFID** and shows unique stats.
   * (Optionally) Plays audio using **DFPlayer Mini** and a **Speaker**.
4. All feedback (visual/audio) is updated in real-time based on presence detection.

## 📸 Showcase
No Gunpla detected
![WhatsApp Image 2025-05-12 at 10 39 22_3847b437](https://github.com/user-attachments/assets/c4728794-cb24-48ae-a406-5433c99d47ee)

Gunpla Detected
![WhatsApp Image 2025-05-12 at 10 39 20_9eb172c8](https://github.com/user-attachments/assets/a719c0c5-9458-4272-a72d-9818a7e95b32)

---
## Wiring and PCB
I only put the wiring diagram and PCB design for using the MFRC522 RFID Reader
WIRING
![Hangar Base PCB_bb](https://github.com/user-attachments/assets/4c3246b9-67d4-400d-83a5-00317a50e117)


PCB
![image](https://github.com/user-attachments/assets/51999105-3888-43dd-8bea-1b0c5ede8d59)
