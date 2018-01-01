const print = (val)=> {
	console.log(`${val.toString(2)} : ${val.toString(16)} : ${val.toString(10)}`);
}

let angle = 34;
print(angle);


angle = ~angle+1;

print(angle);
