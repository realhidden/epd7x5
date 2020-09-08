const epd7x5 = require('../build/Release/epd7x5');

export const RES_WIDTH = 880;
export const RES_HEIGHT = 528;

const RES_PIXEL_COUNT = RES_WIDTH * RES_HEIGHT;
const TARGET_COLOR_BUFFER_SIZE = RES_PIXEL_COUNT >> 3;
const SOURCE_COLOR_BUFFER_SIZE = RES_PIXEL_COUNT << 2;

export function displayImageBuffer(buffer: Buffer) {
	if (buffer.length !== SOURCE_COLOR_BUFFER_SIZE) {
		throw new Error('Invalid image buffer size. This module expects BGRA color buffer.');
	}

	// Black/white buffer
	const bwBuffer = Buffer.alloc(TARGET_COLOR_BUFFER_SIZE, 0xFF); // White pixels by default

	// Red buffer
	const redBuffer = Buffer.alloc(TARGET_COLOR_BUFFER_SIZE, 0);

	for (let i = 0; i < RES_PIXEL_COUNT; ++i) {
		const red = buffer[(i << 2) + 2];
		const blue = buffer[i << 2];

		if (red === 0) {
			bwBuffer[i >> 3] &= ~(0x80 >> (i % RES_WIDTH % 8));
		} else if (blue === 0 && red > 0) {
			redBuffer[i >> 3] |= 0x80 >> (i % RES_WIDTH % 8);
		}
	}

	epd7x5.displayFrame(bwBuffer, redBuffer);
}

export const init: () => boolean = epd7x5.init;
export const deepSleep: () => void = epd7x5.deepSleep;

