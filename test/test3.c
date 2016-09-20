#include "test.h"

int f(int a, int b, int c) {
	int d, e, f;
}
int a = 100, b = 200, c = 300;
int main() {
	print(a); space(); print(b);
	endl();
	int a;
	{
		int b;
		a = 100 * b = 100 * 100;
		print(a); space(); print(b);
	}
}