# DriftRiff Mini - ESP32 Beat Sequencer

A handheld lo-fi beat sequencer inspired by the Elektron Model:Samples, built with ESP32 and Adafruit ILI9341 touchscreen.

## Features

- **6 audio tracks** with individual step sequences
- **16-step sequencer** with visual grid interface
- **PWM audio output** via ESP32 internal DAC (GPIO25)
- **Resistive touchscreen control** with stylus support
- **microSD card sample loading** (8-bit unsigned mono .raw files)
- **Real-time BPM control** (60-200 BPM)
- **Minimalist black/red UI** design
- **Modular code architecture** for easy expansion

## Hardware Requirements

### Core Components
- **ESP32 Development Board**
- **Adafruit ILI9341 2.8" TFT + Resistive Touchscreen**
- **microSD Card** (formatted FAT32)
- **Breadboard/PCB** for connections
- **Jumper wires**
- **Stylus** for touchscreen input

### Audio Output
- **3.5mm audio jack** connected to GPIO25
- **10μF capacitor** (for DC blocking)
- **1kΩ resistor** (current limiting)

## Wiring Diagram

### TFT Display (SPI)
```
ESP32     | ILI9341
----------|----------
GPIO23    | MOSI
GPIO18    | CLK
GPIO19    | MISO
GPIO5     | CS
GPIO2     | DC
GPIO4     | RST
3.3V      | VCC
GND       | GND
```

### Resistive Touchscreen
```
ESP32     | Touch
----------|----------
A2 (YP)   | Y+
A3 (XM)   | X-
GPIO14    | Y-
GPIO27    | X+
```

### SD Card
```
ESP32     | SD Card
----------|----------
GPIO23    | MOSI
GPIO18    | CLK
GPIO19    | MISO
GPIO15    | CS
3.3V      | VCC
GND       | GND
```

### Audio Output
```
ESP32     | Audio
----------|----------
GPIO25    | Signal → [1kΩ] → [10μF] → 3.5mm Jack Tip
GND       | 3.5mm Jack Sleeve
```

## Software Setup

### Arduino IDE Configuration
1. Install **ESP32 board package** in Arduino IDE
2. Install required libraries:
   - `Adafruit GFX Library`
   - `Adafruit ILI9341`
   - `TouchScreen`
   - `SD`

### Project Structure
```
DriftRiffMini/
├── main.cpp           # Main application loop
├── sequencer.h/cpp    # Sequencer logic and step management
├── ui.h/cpp          # User interface and display handling
├── audioengine.h/cpp # PWM audio output and sample playback
├── sdloader.h/cpp    # SD card sample loading
└── touchscreen.h/cpp # Touch input processing
```

### SD Card Setup
Create the following folder structure on your SD card:
```
/samples/
├── kick.raw
├── snare.raw
├── hihat.raw
├── perc.raw
├── bass.raw
└── lead.raw
```

### Sample Format
- **Format**: 8-bit unsigned mono
- **Sample Rate**: 22,050 Hz
- **File Extension**: .raw
- **Max Size**: 32KB per sample

#### Converting Audio to Raw Format
Using **Audacity**:
1. Import your audio file
2. Convert to **Mono** (Tracks → Mix → Mix and Render)
3. Set **Project Rate** to 22050 Hz
4. Export → **Export Other** → **Raw (header-less)**
5. Choose **Unsigned 8-bit PCM**

## Usage

### Basic Operation
1. **Power on** - Device initializes and loads samples
2. **Touch grid squares** to toggle steps on/off
3. **Red squares** = active steps
4. **White outline** = current playing step
5. **Touch +/-** buttons to adjust BPM
6. **Touch PLAY/PAUSE** to control playback

### Default Pattern
The sequencer starts with a basic demo pattern:
- **Track 0 (KICK)**: Steps 1, 5, 9, 13
- **Track 1 (SNARE)**: Steps 5, 13
- **Track 2 (HIHAT)**: Every other step

### Track Assignment
- **Track 0**: KICK - kick.raw
- **Track 1**: SNARE - snare.raw
- **Track 2**: HIHAT - hihat.raw
- **Track 3**: PERC - perc.raw
- **Track 4**: BASS - bass.raw
- **Track 5**: LEAD - lead.raw

## Customization

### Adding New Samples
1. Convert audio to 8-bit unsigned mono .raw format
2. Copy to `/samples/` folder on SD card
3. Rename to match expected filenames
4. Restart device to reload samples

### Modifying UI Colors
Edit color definitions in `ui.h`:
```cpp
#define COLOR_BG        ILI9341_BLACK
#define COLOR_STEP_ON   ILI9341_RED
#define COLOR_CURRENT   ILI9341_WHITE
```

### Adjusting Audio Output
Modify audio parameters in `audioengine.h`:
```cpp
#define SAMPLE_RATE         22050
#define MAX_CONCURRENT_SAMPLES 4
```

## Troubleshooting

### SD Card Issues
- Ensure card is formatted as **FAT32**
- Check `/samples/` folder exists
- Verify sample files are named correctly
- Check wiring connections

### Touch Not Responding
- Verify touchscreen wiring
- Adjust calibration values in `touchscreen.h`:
```cpp
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
```

### No Audio Output
- Check GPIO25 connection
- Verify capacitor polarity
- Ensure samples are loading (check Serial monitor)
- Test with headphones (line-level output)

### Memory Issues
- Reduce `MAX_SAMPLE_SIZE` in `sdloader.h`
- Use shorter sample files
- Monitor memory usage in Serial output

## Future Enhancements

### Planned Features
- **Pattern chaining** and song mode
- **Real-time effects** (bitcrush, delay, reverb)
- **Sample recording** via I2S microphone
- **MIDI sync** input/output
- **Parameter automation**
- **Multiple pattern banks**

### Hardware Expansions
- **Rotary encoders** for parameter control
- **Hardware buttons** for track mute/solo
- **LED indicators** for visual feedback
- **External audio input** for sampling
- **Battery power** with charging circuit

## License

Open source hardware/software project. Feel free to modify and share!

## Credits

Inspired by Elektron Model:Samples workflow and designed for lo-fi beat making enthusiasts.

---

**Happy beat making! 🎵**
