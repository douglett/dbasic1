const fs = require('fs');

const src = fs.readFileSync('./test.wasm');
const bytes = new Uint8Array(src);

WebAssembly
	.instantiate(bytes, {
		env: {
			memory: new WebAssembly.Memory({ initial: 256 })
		}
	})
	.then(result => {
		console.log(result);
		console.log(result.instance.exports);

		console.log('running f:', result.instance.exports.f());
	})
	.catch(e => console.error(e));