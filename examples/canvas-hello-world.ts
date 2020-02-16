import * as epd from '../lib';
import { createCanvas } from 'canvas';

// Initialize canvas
const canvas = createCanvas(epd.RES_WIDTH, epd.RES_HEIGHT);
const ctx = canvas.getContext('2d', { pixelFormat: 'A8' });

// Draw something
ctx.fillStyle = 'white';
ctx.fillRect(0, 0, epd.RES_WIDTH, epd.RES_HEIGHT);
ctx.font = '65px';
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