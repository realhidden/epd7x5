# epd7x5-v2
A Node.js package for the [7.5inch e-Paper HAT(B) V2 (12.2019) Waveshare display](https://www.waveshare.com/wiki/7.5inch_e-Paper_HAT_(B\)) on a Raspberry Pi 2/3/zero.

It can consume BGRA image color buffer which may be produced by using for instance [node-canvas](https://github.com/Automattic/node-canvas) library.

## Dependencies
WiringPi for GPIO access of Raspberry Pi

## Installation
1. Enable the SPI interface on Raspberry Pi: `sudo raspi-config` 
2. WiringPi: follow installation on [wiringpi.com](http://wiringpi.com/download-and-install/)
3. (optional) Install [node-canvas](https://github.com/Automattic/node-canvas): `npm i canvas`
4. epd7x5-v2: `npm i epd7x5-v2`

## Example

This example requires [node-canvas](https://github.com/Automattic/node-canvas) library installed.

```typescript
import * as epd from 'epd7x5-v2';
import { createCanvas, registerFont } from 'canvas';

// Initialize canvas
registerFont('/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf', { family: 'DejaVu' });
const canvas = createCanvas(epd.RES_WIDTH, epd.RES_HEIGHT);
const ctx = canvas.getContext('2d');

// Draw something
ctx.antialias = 'none';
ctx.fillStyle = 'white';
ctx.fillRect(0, 0, epd.RES_WIDTH, epd.RES_HEIGHT);
ctx.font = '65px DejaVu';
ctx.textBaseline = 'top';
ctx.fillStyle = 'black';
ctx.fillText('Hello World!', 5, 5);
ctx.fillStyle = 'red';
ctx.fillText('Hello World!', 5, 105);
ctx.fillRect(5, 205, 500, 69);
ctx.fillStyle = 'white';
ctx.fillText('Hello World!', 5, 205);

// Send image to the HAT and sleep
epd.init();
epd.displayImageBuffer(canvas.toBuffer('raw'));
epd.deepSleep();
```

## Roadmap

* Rewrite to async code