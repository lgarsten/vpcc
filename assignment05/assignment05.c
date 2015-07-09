// test function declarations 

int b;

int addtwonumbers(int number, int number_two) {
	
	return number + number_two;
}

int *testmalloc() {
	int *mal;
	mal = malloc(4 * 2);
	return mal;
}

int main() {
	int *tmp;
	int *m;

	b = addtwonumbers(5,16);	// b = 0x15
	
	b = 0;

	tmp = testmalloc();

	m = tmp;
	*tmp = 2;
	tmp = tmp + 1;
	*tmp = 5;
	b = b + *tmp;
	tmp = tmp -1;
	b = b + *tmp;		// b = 0x7

	tmp = malloc(4 *1);

	b = 0;
	*tmp = 15;
	b = b + *tmp + *m;	// b = 0x11
	m = m +1;		
	b = b + *m;		// b = 0x16
}

