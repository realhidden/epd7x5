import * as epd from '../lib';
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