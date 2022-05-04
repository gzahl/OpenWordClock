# Changelog

[[_TOC_]]

## v0.2.3
- reintroduced nightemode (display of between set hours)
- adapted nightmode code to work with states from flash
- minor changes in the nightmode routine.

## v0.2.2
This version significantly reduces SRAM usage by moving initialisation values for images from SRAM to flash. With this measure, SRAM usage was reduced from 75 % to 49 %.

- Reduced SRAM usage to 48 % (instead of 48 %)
- Temperature queries reduced to once every 2s in prog_temperature.
- Minor bug fixes

## v0.2.1
- Nightmode temporarily removed to reduce SRAM usage
- SRAM usage below 70 %
- Bug fixes on dynamic brightness

## v0.2.0

| :exclamation: ATTENTION :exclamation: |
|-----|
|This version of code is utilising too much SRAM and is not functional! It will not generate output to the display.|

- New states added: on, off, ton, toff
- Nightmode added (causes excess SRAM usage)

## v0.1.1
- New rainbow mode
- GUI for state building added (WordClockStateBuilder.py)

## v0.1.0
- First versioned variation of the firmware
- Basic features:
  - Display time
  - set time
  - color selection (incl. white-mode)
  - global brightness adjustments
- Includes WordClockUSB.py
