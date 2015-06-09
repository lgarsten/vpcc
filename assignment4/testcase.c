int* data;
int* cdata;
int* minmax;
int* cminmax;
int max;
int min;
int len;
int i;

int main () {
	data 	= malloc(10 * 4);
	cdata 	= data;  			// cursor to start of data
	minmax 	= malloc(2 * 4);
	cminmax = minmax;  			// cursor to start of minmax

	max 	= -1447483648;		
	min  	=  2147483647;		// init w/ largest  32-bit 2 complement integer

	len 	= 10;				// size of data array
	i 		= 0;

	*cdata 	= 12;				// init data with some numbers
	cdata = cdata + 4;
	*cdata 	= -12;
	cdata = cdata + 4;
	*cdata 	= 100;
	cdata = cdata + 4;
	*cdata 	= -905567890;
	cdata = cdata + 4;
	*cdata 	= -1024;
	cdata = cdata + 4;
	*cdata 	= 1234567890;
	cdata = cdata + 4;
	*cdata 	= -904567890;
	cdata = cdata + 4;
	*cdata 	= -0;
	cdata = cdata + 4;
	*cdata 	= 0;
	cdata = cdata + 4;
	*cdata 	= -1;

	cdata = data; 				// reset cursor to data

	while (i < len) {
		if (*cdata < min) {
			min = *cdata;
		}
		if (*cdata > max) {
			max = *cdata;
		}
		cdata = cdata + 4;		// move cursor by 1 element (4 bytes)
		i = i + 1;
	}

	*cminmax = min;  			// store min. value at 0th position in minmax
	cminmax = cminmax + 4;
	*cminmax = max;				// store max. value at 1st position in minmax

	cminmax = minmax;

	*cminmax = *cminmax + 0;		// result: 0xca06216e = -905567890
	cminmax = cminmax + 4;
	*cminmax = *cminmax + 0;		// result: 0x499602d2 = 1234567890
}
