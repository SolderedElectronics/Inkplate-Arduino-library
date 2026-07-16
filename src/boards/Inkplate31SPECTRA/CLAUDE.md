# Inkplate 31SPECTRA

31.5" 6-color E Ink Spectra 6 panel (E Ink part EL315TW1/EL315TW5, ED2208-QBA/QBE),
2560x1440, driven by 8 source-driver ICs over quad-SPI, with an IST9201 PMIC
generating the panel bias voltages. First-pass implementation, ported directly
from E Ink's vendor sample code and app notes - no hardware has run this yet.

## Reference material this was built from

- `~/Downloads/Documentation/sample code/SampleCodeFor315_20251216_v1/` -
  `EL315TW1.c/.h` (panel register sequencing), `pmic.c/.h` (IST9201 I2C
  sequencing), `comm.c/.h` (SPI/I2C/GPIO glue, shows the intended ESP-IDF
  QSPI transaction shape), `pindefine.h`, `main.c` (call order).
- `EL315TW1_Partial_Window_Update_20240606_v1.pdf` - the only place the
  8-chip physical layout and PTLW math are spelled out with worked examples.
- `P-511-1172-V1_ED2208-QBE (EL315TW5) Product Spec` - confirms 2560x1440 and
  the "L-CS0..3 / R-CS0..3" (left half / right half) chip grouping.
- `31.5 inch E6 (EL315TW1) introduction_V6_20231102_IST9201.pdf` - confirms
  ESP32-S3-WROOM-1U-N16R8 + IST9201 as the reference platform.

All register command bytes and magic init values in `pins.h` and
`IST9201.h` are copied verbatim from the vendor source - not re-derived.
Where the vendor code and the app-note PDF text disagreed on a numeric
detail (see "Partial window alignment" below), the `.c` source was treated
as authoritative over the PDF prose.

## Pin mapping

All of this came directly from the user, not from a schematic file read by
this session - if real hardware disagrees, this is the first place to
check.

**Expander1** (PCAL6416A, I2C addr `0x20`):
```
bit 0..7  = CS3,CS2,CS1,CS0,CS7,CS6,CS5,CS4  (reversed-nibble PCB routing)
bit 9     = V_BAT_MOS       (battery ADC divider MOSFET, same convention as Inkplate13SPECTRA)
bit 10    = SD_ENABLE        (SD card power MOSFET, same bit as Inkplate13SPECTRA's SD_PMOS_PIN)
bit 11    = PMIC_PWR_EN      (enables an external 12V buck/boost, TPS61288-class, feeding the IST9201's input rail)
bit 12    = IST9201_EN       (the IST9201's own chip-enable - this is the vendor sample's "PMIC_EN")
bit 13    = VDDP_EN
bit 14    = VDDN_EN
bit 15    = VCNP_EN
```
`csExpanderBit[8]` in `pins.h` translates logical CS index (0..7, matching
the panel's own left-to-right chip order) to the expander bit above.

**Expander2** (PCAL6416A, I2C addr `0x21`): present on the board, `begin()`-called
so it isn't left floating, but **no pins on it are used by this driver**.
The user didn't have a pinout for it ready yet - if/when one exists, it
plugs in the same way expander1 did.

**ESP32-S3 direct GPIO** - see the top of `pins.h` for the full table.
One ambiguity worth flagging: the user's pin list names both `IO4/IO5`
("SPI1_D_CX"/"SPI2_D_CX") and `IO9/IO11/IO13/IO14` ("SI3"/"SI0"/"SI1"/"SI2")
in a way that reads like the same quad-SPI data signals labeled twice. This
driver uses `IO11/IO13/IO14/IO9` as `SI0..SI3` (they match the panel
datasheet's own signal names directly) and clock on `IO12`. If hardware
bring-up shows `IO4/IO5` are actually the correct data lines, only the four
`SPECTRA315_SPI_D*` defines in `pins.h` need to change - nothing else
depends on which physical pins these are.

Pin 17 is shared between the expander1 interrupt line and the WAKE_BTN
signal per the user - wired as the expander's interrupt pin
(`IOExpander::setIntPin`); the wake button rides the same line.

## Why quad-SPI needed the ESP-IDF driver, not Arduino SPIClass

The panel is wired for 4-wire quad SPI (SI0-SI3) so the ~2.3MB full-frame
transfer isn't painfully slow. Arduino's `SPIClass` on this core doesn't
expose quad/QIO transactions - only the raw ESP-IDF `driver/spi_master.h`
API does (`SPI_TRANS_MODE_QIO`, `spi_transaction_ext_t`). This is a new
pattern for this codebase (every other board's panel SPI goes through
`SPIClass`); the SD card on this board still uses `SPIClass(1)` (`spi1`)
exactly like Inkplate13SPECTRA - only the panel bus bypasses it.

`qspiInit()` (`Inkplate31SPECTRADriver.cpp`) puts the panel bus on
`SPI3_HOST` specifically to avoid clashing with whatever host
arduino-esp32 assigns to `SPIClass(1)` for the SD card (typically
`SPI2_HOST`) - **this pairing needs to be confirmed on real hardware**,
along with the QSPI clock speed (currently a conservative `10 MHz`
placeholder) and the exact `SPI_TRANS_VARIABLE_CMD`/`command_bits`
override semantics for the command-then-quad-burst sequence (mirrors the
commented-out reference in `comm.c`, but that reference was never actually
compiled/run either).

CS is fully software-managed (`devCfg.spics_io_num = -1`): all 8 chips'
CS lines sit on expander1, so each SPI transaction is bracketed by
`csSelect()`/`csDeselect()` calls that toggle the expander over I2C. This
happens per-chip-per-command (max ~10-20 times per full refresh), not
per-byte, so the I2C overhead is negligible against e-paper waveform
timing.

## Panel geometry and the per-chip-strip buffer layout

The 2560x1440 panel is 8 driver ICs in a **single row**, left to right
(confirmed by the partial-window app note's worked example - not a
vertical 4x2 or 2x4 grid). Per-chip *visible* pixel widths:
`400, 400, 400, 80, 400, 400, 400, 80` (cumulative: `chipXOffset` in
`pins.h` = `0,400,800,1200,1280,1680,2080,2480,2560`). Every chip's
addressable RAM is a fixed `400 x 1440` (4bpp, 288000 bytes) **regardless**
of visible width - chips 3 and 7 only drive their first 80 RAM columns to
real pixels; the other 320 columns exist but aren't wired to anything
visible. This is why the vendor's own `main.c` sends the full 288000-byte
buffer to every chip including the narrow ones.

`DMemory4Bit` is therefore **8 contiguous 288000-byte strips**
(2304000 bytes total, `ps_malloc`'d), not a naive
`E_INK_WIDTH*E_INK_HEIGHT/2` packed buffer like other boards use. This
means `display()` can blast each chip's data as one contiguous QSPI burst
with no per-row interleaving loop (unlike Inkplate13SPECTRA, which
interleaves 2 equal-width chips row by row). `writePixelInternal()` uses
three small lookup tables (`_colChipByteBase`/`_colByteInRow`/`_colNibble`,
built once in `initializeFramebuffers()`) to go from a global column
straight to (chip, byte, nibble) in O(1).

`clearDisplay()` is a single `memset` over the whole 2304000-byte buffer -
this also correctly blanks the unused padding columns in chips 3/7, since
`writePixelInternal` can never address them (every valid on-screen x maps
to a real visible column by construction).

## Landscape-native orientation (differs from Inkplate13SPECTRA/Inkplate2)

Inkplate13SPECTRA and Inkplate2 are **portrait-native** panels
(`E_INK_WIDTH < E_INK_HEIGHT`) that get rotated 90° at the driver level
(`setRotation(1)` in `initDriver()`) to present landscape to the user. The
31SPECTRA panel is landscape-native (`2560 x 1440`), so it follows
Inkplate6COLOR's convention instead: no default rotation call, and
`ImageColor`'s dither-buffer axis-swap gate
(`ImageColor.h:133`/`ImageColor.cpp:64`, `ARDUINO_INKPLATE2 ||
ARDUINO_INKPLATE13SPECTRA`) deliberately does **not** include
`ARDUINO_INKPLATE31SPECTRA` - it must fall into the unswapped `else`
branch. Everywhere else that gates on "needs `ImageColor`"
(`ImageColor.h:21` and 7 mirrored sites) or "exclude the monochrome
`Image`/`Gif` path" (9 sites in `graphics/Image` and `graphics/Gif`) does
get `ARDUINO_INKPLATE31SPECTRA` added.

## PMIC power sequencing (`src/features/IST9201/`)

The vendor sample only has one PMIC enable pin (`PMIC_EN`). This board has
two: `PMIC_PWR_EN` (expander1 bit 11) gates an **external 12V buck/boost**
(TPS61288-class, per the user) that feeds the IST9201's own input supply;
`IST9201_EN` (bit 12) is the IST9201's own chip enable, equivalent to the
vendor's `PMIC_EN`. This driver enables `PMIC_PWR_EN` first (with a short
settling delay) before anything else touches the IST9201, and only
disables it as part of the full `railsOff()` teardown - never on every
per-refresh `powerDown()`. This two-stage sequencing was confirmed with
the user but not verified against a schematic in this session.

Also worth noting: the vendor sample's `enablePmic()`/`disablePmic()`
(called every `epdDisplay()`) and `powerSwitchDisable()` (called only once,
at the very end of the whole session in `main.c`) are **not symmetric** -
`disablePmic()` only waits for `PMIC_PG` to fall, it does NOT turn off
VDDN/VDDP/VNCP. Those stay biased across successive quick refreshes and
are only fully torn down before a long sleep. `IST9201::powerDown()`
mirrors the lightweight per-refresh path; `IST9201::railsOff()` mirrors
the heavy one-time teardown (with the vendor's documented 5s/5s/3s
discharge delays) - the board driver doesn't call `railsOff()` anywhere
yet, since nothing here implements deep-sleep prep for this board.

`IST9201::setVcomFromPanel()` (ports the vendor's `setPwrToPmic()`/
`voltageToRegisterData()`) is implemented but **not wired into any
`display()` path**. It's the vendor's optional per-panel VCOM calibration
step (`setEpdPower()` in the sample reads PWR/VCOM registers back from the
panel and recomputes the PMIC ladder from them) - available for a future
auto-calibration pass once the fixed `initPmicData` table has been
validated to produce a good image on real hardware.

## Partial window alignment - a source discrepancy

The app-note PDF's bullet list says `xPixel – xStart > 32 and xStart +
xPixel <= 400`, which doesn't parse consistently with the worked example
or the `.c` source's actual check
(`(HRED - HRST + 1 < 32) | (HRED + 1 > 800)`, i.e. roughly `xPixel >= 16`
in host units) - likely an OCR/transcription slip in the PDF. This driver
does **not** hard-enforce a minimum window size (same permissive,
best-effort-clip approach Inkplate13SPECTRA's own `displayPartial` already
takes) rather than guessing at which source is right.

## Not done / explicitly out of scope

- **`boards.txt` / variant files** live in a separate sibling repo
  (`~/Desktop/Inkplate-Board-Definitions-for-Arduino-IDE/`), not here. A
  new `Inkplate31SPECTRA.*` board entry with `build.board=INKPLATE31SPECTRA`
  is needed there before `ARDUINO_INKPLATE31SPECTRA` is ever actually
  defined by the Arduino core - none of this compiles end-to-end without
  that.
- Only one example ported so far (`Basic/..._Hello_World`), not the full
  set of ~40 examples Inkplate13SPECTRA has.
- No real hardware has run any of this. The riskiest untested areas, in
  rough order of "most likely to need fixing first": the QSPI bus/host
  setup and clock speed in `qspiInit()`, the SI0-3 vs IO4/IO5 pin
  ambiguity noted above, the exact `PMIC_PWR_EN`→`IST9201_EN` timing, and
  whether the fixed `initPmicData` VCOM/bias table (copied as-is from the
  vendor) needs per-unit calibration via `setVcomFromPanel()`.
