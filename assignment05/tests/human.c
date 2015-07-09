int result_a;
int result_b;

int countHeight(int *humanArr, int height_limit);
int countMan(int *humanArr);
int *newHuman(int age, int gender, int height);

int *newHuman(int age, int gender, int height) {
	int *human;	// 0 = age, 1 = gender, 2 = height
	int *humancursor;

	human = malloc(4 * 4);
	humancursor = human;
	
	*humancursor = age;
	humancursor = human + 1;
	*humancursor = gender;
	humancursor = human + 2;
	*humancursor = height;
	humancursor = human + 3;
	*humancursor = 0;
		      
	return human;
}

int main() {
	int *humanArr;
	int *humanArrcursor;
	
	humanArr = malloc(5 * 4);
	humanArrcursor = humanArr;

	humanArrcursor = humanArr + 4;
	*humanArrcursor = 0;	// escape int

	// insert example data into array
	humanArrcursor = humanArr;
	*humanArrcursor = newHuman(12,1,130);
	humanArrcursor = humanArr +1;
	*humanArrcursor = newHuman(13,1,140);
	humanArrcursor = humanArr +2;
	*humanArrcursor = newHuman(20,1,165);
	humanArrcursor = humanArr +3;
	*humanArrcursor = newHuman(17,0,170);

	// count sum of male ppl in array
	result_a = countMan(humanArr);
	
	// count sum of ppl higher 150
	result_b = countHeight(humanArr, 150);

	result_a = result_a + 0;	// 0x1
	result_b = result_b + 0;	// 0x2
}

int countHeight(int *humanArr, int height_limit) {
	int *humanArrcursor;	
	int count;
	int *tmpHuman;
		
	count = 0;
	
	humanArrcursor = humanArr;

	while (*humanArrcursor != 0) {
		tmpHuman = *humanArrcursor;
		tmpHuman = tmpHuman + 2;
		if (*tmpHuman >= height_limit)
			count = count +1;
		
		humanArrcursor = humanArrcursor + 1;
	}
	
	return count;
}

int countMan(int *humanArr) {
	int *humanArrcursor;	
	int m;
	int w;
	int count;
	int *tmpHuman;

	m = 0;
	w = 1;	
	count = 0;
	
	humanArrcursor = humanArr;

	while (*humanArrcursor != 0) {
		tmpHuman = *humanArrcursor;
		tmpHuman = tmpHuman + 1;
		if (*tmpHuman == m)
			count = count +1;
		
		humanArrcursor = humanArrcursor + 1;
	}
	
	return count;
}

