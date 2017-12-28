int a = 9;
int b = 9;

int x() {
	return 7;
}

int main(int argc, char **argv) {
	int p = x();
	return a + p + x();
}
