int result;

int factorial(int n);

int main () {
	int number;
	number = 10;	
	
	result = factorial(number);	//0x375f00
}

int factorial(int n) {

    if (n == 0)
        return 1;
    return n * factorial(n - 1);
}
