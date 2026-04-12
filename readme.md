# 🤖 Robot Suiveur de Ligne - Line Following Robot
## ESP32 + QTR-8A + L298N

---

## 📋 Vue d'ensemble / Overview

Ce robot est un **suiveur de ligne** (line follower) basé sur:
- **ESP32** microcontrôleur
- **QTR-8A** capteur infrarouge 8 canaux (analog)
- **L298N** pilote moteur dual

---

## ⚠️ IMPORTANT - WiFi Compatibility

**ℹ️ NOTES IMPORTANTES / IMPORTANT NOTES:**
- ✅ Utilise **ADC1 uniquement** pour éviter les conflits avec WiFi
- ❌ **N'utilisez PAS ADC2** (réservé pour WiFi)
- 🔌 Les broches GPIO32-GPIO39 appartiennent à ADC1

---

## 📍 Configuration des Broches / Pin Configuration

### 1️⃣ QTR Sensors (Analog - 6 canaux)
Capteurs infrarouges pour détection de ligne

| Capteur | Broche ESP32 | ADC Channel | Fonction |
|---------|-------------|------------|----------|
| QTR 1 | **GPIO36** | ADC1_CH0 | Sensor 1 |
| QTR 2 | **GPIO39** | ADC1_CH3 | Sensor 2 |
| QTR 3 | **GPIO34** | ADC1_CH6 | Sensor 3 |
| QTR 4 | **GPIO35** | ADC1_CH7 | Sensor 4 |
| QTR 5 | **GPIO32** | ADC1_CH4 | Sensor 5 |
| QTR 6 | **GPIO33** | ADC1_CH5 | Sensor 6 |

**Alimentation QTR:**
- VCC → +5V
- GND → GND

---

### 2️⃣ L298N Motor Driver - DC Motors (2 moteurs)
Pilote moteur pour deux moteurs DC

#### Moteur Gauche (Left Motor)
| Fonction | Broche ESP32 | Type |
|----------|-------------|------|
| IN1 | **GPIO18** | Digital |
| IN2 | **GPIO19** | Digital |
| ENA (PWM) | **GPIO23** | PWM |

#### Moteur Droit (Right Motor)
| Fonction | Broche ESP32 | Type |
|----------|-------------|------|
| IN3 | **GPIO21** | Digital |
| IN4 | **GPIO22** | Digital |
| ENB (PWM) | **GPIO5** | PWM |

**Alimentation L298N:**
- GND → GND
- +5V → VSS (optionnel)
- +12V → +12V (Alimentation moteurs)

---

### 3️⃣ Servo Motors (2 servos)
Servomoteurs pour contrôle de direction

| Servo | Broche ESP32 | Type | Fonction |
|-------|-------------|------|----------|
| Servo 1 | **GPIO27** | PWM | Servo 1 |
| Servo 2 | **GPIO14** | PWM | Servo 2 |

**Alimentation Servos:**
- VCC → +5V
- GND → GND
- Signal → GPIO (comme indiqué ci-dessus)

---

### 4️⃣ Ultrasonic Sensors (2 capteurs)
Capteurs ultrason pour détection de distance

#### Ultrasonic 1
| Fonction | Broche ESP32 |
|----------|-------------|
| TRIG | **GPIO4** |
| ECHO | **GPIO13** |

#### Ultrasonic 2
| Fonction | Broche ESP32 |
|----------|-------------|
| TRIG | **GPIO16** |
| ECHO | **GPIO17** |

**Alimentation Ultrasonic:**
- VCC → +5V
- GND → GND

---

## 🔋 Schéma d'Alimentation / Power Supply

```
┌─────────────────────────────────────────┐
│        BLOC ALIMENTATION / PSU          │
├─────────────────────────────────────────┤
│  GND ──────────────────┬─────────────┐  │
│  +5V ───────────────┬──┼─────────┐   │  │
│  +12V ──┬───────────┼──┤         │   │  │
└────────┼───────────┼──┼─────────┼───┘  │
         │           │  │         │       │
    ┌────▼───┐   ┌───▼──┴─┐   ┌──▼──┐   │
    │ L298N  │   │ESP32   │   │QTR-8A   │
    │        │   │        │   │        │
    │+12V    │   │+5V     │   │+5V    │
    │GND     │   │GND     │   │GND    │
    └────────┘   └────────┘   └────────┘
```

### Distribution d'Alimentation / Power Distribution

| Composant | +5V | +12V | GND |
|-----------|-----|------|-----|
| **ESP32** | ✅ | ❌ | ✅ |
| **QTR Sensors** | ✅ | ❌ | ✅ |
| **L298N** | ⚠️* | ✅ | ✅ |
| **DC Motors** | ❌ | ✅ | ✅ |
| **Servo Motors** | ✅ | ❌ | ✅ |
| **Ultrasonic Sensors** | ✅ | ❌ | ✅ |

*⚠️ L298N: Le VSS peut être connecté à +5V optionnellement

---

## 🎮 Commandes Moteurs / Motor Commands

### PWM (Vitesse) - Broches Enable
- **GPIO23** (ENA) - Contrôle vitesse moteur gauche (0-255)
- **GPIO5** (ENB) - Contrôle vitesse moteur droit (0-255)

### Direction - Broches Input
| Direction | IN1 | IN2 |
|-----------|-----|-----|
| **Avant** | HIGH | LOW |
| **Arrière** | LOW | HIGH |
| **Arrêt** | LOW | LOW |
| **Freinage** | HIGH | HIGH |

---

## 📊 Utilisation Totale des Broches

```
QTR SENSORS (Analog):
GPIO36  → ADC1_CH0    (QTR 1)
GPIO39  → ADC1_CH3    (QTR 2)
GPIO34  → ADC1_CH6    (QTR 3)
GPIO35  → ADC1_CH7    (QTR 4)
GPIO32  → ADC1_CH4    (QTR 5)
GPIO33  → ADC1_CH5    (QTR 6)

L298N MOTOR DRIVER:
GPIO18  → L298N IN1 (Motor Left)
GPIO19  → L298N IN2 (Motor Left)
GPIO23  → L298N ENA (Motor Left - PWM)

GPIO21  → L298N IN3 (Motor Right)
GPIO22  → L298N IN4 (Motor Right)
GPIO5   → L298N ENB (Motor Right - PWM)

SERVO MOTORS (PWM):
GPIO27  → Servo 1
GPIO14  → Servo 2

ULTRASONIC SENSORS:
GPIO4   → Ultrasonic 1 TRIG
GPIO13  → Ultrasonic 1 ECHO
GPIO16  → Ultrasonic 2 TRIG
GPIO17  → Ultrasonic 2 ECHO
```

**Total: 22 broches utilisées**

---

## ✅ Avantages de cette Configuration

✔️ **ADC1 uniquement** → Pas de conflits WiFi
✔️ **Broches PWM rapides** → Contrôle moteur fluide
✔️ **Distribution logique** → Capteurs à droite, moteurs au centre
✔️ **Flexibilité** → Upgrade facile du firmware

---

## 📦 Matériel Requis / Bill of Materials

- 1x ESP32 DevKit
- 1x QTR-8A Sensor
- 1x L298N Motor Driver
- 2x Moteurs DC 3-12V
- 1x Source d'alimentation 5V + 12V
- Câbles de connexion
- Breadboard/Platine

---

## 🔗 Ressources Utiles

- [ESP32 Pinout](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)
- [ADC Channels Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html)
- [QTR-8A Sensor Guide](https://www.pololu.com/product/961)
- [L298N Motor Driver](https://www.alldatasheet.com/datasheet-pdf/pdf/1133430/ST/L298N.html)

---

## 📝 Notes de Programmation / Programming Notes

```cpp
// ADC1 Initialization
#define QTR_S0 36
#define QTR_S1 39
#define QTR_S2 34
#define QTR_S3 35
#define QTR_S4 32
#define QTR_S5 33
#define QTR_S6 25
#define QTR_S7 26

// Motor Control
#define EN_L 18
#define IN1_L 19
#define IN2_L 23

#define EN_R 5
#define IN1_R 17
#define IN2_R 16
```

---

**Dernière mise à jour / Last Update:** Avril 2026 / April 2026
