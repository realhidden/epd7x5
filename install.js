const os = require('os');
const spawn = require('cross-spawn');

if (os.arch() === 'arm') {
    spawn.sync('npm', ['run', 'native_build'], {
        input: 'ARM architecture detected. Going to build native module.',
        stdio: 'inherit'
    });
}