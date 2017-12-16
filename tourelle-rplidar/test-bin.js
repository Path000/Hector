const hexToBinaryString = (hex) => {
	return (hex >>> 0)
		.toString(2)
		.padStart(4, '0');
}

for (let index = 0; index < 16; index++) {
	let buff = Buffer.from([index]);
	let hex = index.toString(16);
	let bin = hexToBinaryString(buff[0]);
	let mask = 0x03;
	let maskBin = hexToBinaryString(mask);
	let extract = (index | 0x03) >>> 0;
	console.log(`${index} ${hex} ${bin} | ${maskBin} -> ${extract}`);
}
/*
0 0 0000 & 0011 -> 0
1 1 0001 & 0011 -> 1
2 2 0010 & 0011 -> 2
3 3 0011 & 0011 -> 3
4 4 0100 & 0011 -> 0
5 5 0101 & 0011 -> 1
6 6 0110 & 0011 -> 2
7 7 0111 & 0011 -> 3
8 8 1000 & 0011 -> 0
9 9 1001 & 0011 -> 1
10 a 1010 & 0011 -> 2
11 b 1011 & 0011 -> 3
12 c 1100 & 0011 -> 0
13 d 1101 & 0011 -> 1
14 e 1110 & 0011 -> 2
15 f 1111 & 0011 -> 3
*/

console.log(hexToBinaryString((9 << 4) | 9));
//10011001

console.log((9 << 4) | 9);
//153

const HEALTH_STATUSES = ['Good', 'Warning', 'Error'];
for (let index = 0; index < 3; index++) {
	let buff = Buffer.from([index]);
	console.log(HEALTH_STATUSES[buff[0]]);
}
/*
Good
Warning
Error
*/

let buff = Buffer.from([0x01]);
console.log(buff);
console.log(buff[0]);
console.log(HEALTH_STATUSES[buff[0]]);
/*
<Buffer 01>
1
Warning
*/

for (let index = 0; index < 16; index++) {
	console.log(index * 5 + 4);
	console.log(index * 5 + 9);
}