const os = require('os');
const spawn = require('cross-spawn');

if (os.arch() === 'arm') {
  console.log('Building native module...');
  spawn.sync('npm', ['run', 'native_build'], { stdio: 'inherit' });
} else {
  console.log('Non ARM architecture detected. Skipping native build.');
}