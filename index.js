const epd7x5 = require('./build/Release/epd7x5');
const gd = require('node-gd');

const RES_WIDTH = 800;
const RES_HEIGHT = 480;
const COLOR_BUFFER_SIZE = (RES_WIDTH * RES_HEIGHT) >> 3;

const COLOR_WHITE = 32;
const COLOR_RED = 128;
const COLOR_BLACK = 224;

function getImageBuffer() {
	const img = gd.createSync(RES_WIDTH, RES_HEIGHT);
	for (let i=0; i<256; i++) img.colorAllocate(i, i, i);
	return img;
}

function displayImageBuffer(img) {
	const buf = Buffer.alloc(COLOR_BUFFER_SIZE, 0xFF);
	const bufRed = Buffer.alloc(COLOR_BUFFER_SIZE, 0);
	for (let y = 0; y < RES_HEIGHT; y++) {
		for (let x = 0; x < RES_WIDTH; x++) {
			switch (img.getPixel(x, y)) {
				case COLOR_BLACK:
					buf[((y * RES_WIDTH) + x) >> 3] &= ~(0x80 >> (x % 8));
					break;
				case COLOR_RED:
					bufRed[((y * RES_WIDTH) + x) >> 3] |= 0x80 >> (x % 8);
					break;
			}
		}
	}

	epd7x5.displayFrame(buf, bufRed);
	img.destroy();
}

exports.getImageBuffer = getImageBuffer;
exports.displayImageBuffer = displayImageBuffer;
exports.init = epd7x5.init;
exports.deepSleep = epd7x5.deepSleep;
exports.white = COLOR_WHITE;
exports.red = COLOR_RED;
exports.black = COLOR_BLACK;
exports.width = RES_WIDTH;
exports.height = RES_HEIGHT;
exports.gd = gd;

