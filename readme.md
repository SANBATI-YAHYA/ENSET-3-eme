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

### 1️⃣ QTR-8A Sensor (Analog)
Capteur infrarouge 8 canaux pour détection de ligne

| Canal | Broche ESP32 | ADC Channel | Fonction |
|-------|-------------|------------|----------|
| S0 | **GPIO36** | ADC1_CH0 | Capteur 1 |
| S1 | **GPIO39** | ADC1_CH3 | Capteur 2 |
| S2 | **GPIO34** | ADC1_CH6 | Capteur 3 |
| S3 | **GPIO35** | ADC1_CH7 | Capteur 4 |
| S4 | **GPIO32** | ADC1_CH4 | Capteur 5 |
| S5 | **GPIO33** | ADC1_CH5 | Capteur 6 |
| S6 | **GPIO25** | ADC1_CH8 | Capteur 7 |
| S7 | **GPIO26** | ADC1_CH9 | Capteur 8 |

**Alimentation QTR-8A:**
- VCC → +5V
- GND → GND

---

### 2️⃣ L298N Motor Driver
Pilote moteur pour deux moteurs DC

#### Moteur Gauche (Left Motor)
| Fonction | Broche ESP32 | Type |
|----------|-------------|------|
| EN_L (Enable) | **GPIO18** | PWM |
| IN1_L | **GPIO19** | Digital |
| IN2_L | **GPIO23** | Digital |

#### Moteur Droit (Right Motor)
| Fonction | Broche ESP32 | Type |
|----------|-------------|------|
| EN_R (Enable) | **GPIO5** | PWM |
| IN1_R | **GPIO17** | Digital |
| IN2_R | **GPIO16** | Digital |

**Alimentation L298N:**
- GND → GND
- +5V → VSS (si disponible)
- +12V → +12V (Alimentation moteurs, optionnel)

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
| **QTR-8A** | ✅ | ❌ | ✅ |
| **L298N** | ⚠️* | ✅ | ✅ |
| **Moteurs** | ❌ | ✅ | ✅ |

*⚠️ L298N: Le VSS peut être connecté à +5V optionnellement

---

## 🎮 Commandes Moteurs / Motor Commands

### PWM (Vitesse) - Broches Enable
- **GPIO18** (EN_L) - Contrôle vitesse moteur gauche (0-255)
- **GPIO5** (EN_R) - Contrôle vitesse moteur droit (0-255)

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
GPIO36  → ADC1_CH0    (QTR-8A S0)
GPIO39  → ADC1_CH3    (QTR-8A S1)
GPIO34  → ADC1_CH6    (QTR-8A S2)
GPIO35  → ADC1_CH7    (QTR-8A S3)
GPIO32  → ADC1_CH4    (QTR-8A S4)
GPIO33  → ADC1_CH5    (QTR-8A S5)
GPIO25  → ADC1_CH8    (QTR-8A S6)
GPIO26  → ADC1_CH9    (QTR-8A S7)

GPIO18  → L298N EN_L  (PWM)
GPIO19  → L298N IN1_L (Digital)
GPIO23  → L298N IN2_L (Digital)

GPIO5   → L298N EN_R  (PWM)
GPIO17  → L298N IN1_R (Digital)
GPIO16  → L298N IN2_R (Digital)
```

**Total: 20 broches utilisées**

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
