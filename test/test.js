const fs = require('fs');
const execSync = require('child_process').execSync;

const file = 'test';
let result;

console.log(`compiling ${file}.wast`);
result = execSync(`wasm-as ${file}.wast`) + '';
console.log(result);

console.log(`running ${file}.wasm`);
const buf = new Uint8Array(fs.readFileSync(`./${file}.wasm`));
WebAssembly.instantiate(buf, {}).then(prog => {
	console.log(prog.instance.exports.foo());
});