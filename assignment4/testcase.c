int a;
int b;
int c;
int d;

int main() {
	b = 10;
	a = 0;
	c = 0;
	d = 0;
	//a = b + 6 + (a -1 +(b +20));

	while (a < b) {
			
		while (c < b) {
			c = c +1;
		}	
		d = d + c;	
		a = a +1;
		c = 0;
	}
	a = a + 0;	// = 10
	d = d + 0;	// = 100	
}	

