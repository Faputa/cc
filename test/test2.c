#include "test.h"

int add(int a, int b) {
	return a + b;
}
int sub(int a, int b) {
	return a - b;
}
int mul(int a, int b) {
	return a * b;
}
int div(int a, int b) {
	return a / b;
}
int main() {
	int a = scan();
	int b = scan();
	space(); print(add(a, b)); endl();
	space(); print(sub(a, b)); endl();
	space(); print(mul(a, b)); endl();
	space(); print(div(a, b)); endl();
}