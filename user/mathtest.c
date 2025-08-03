#include <math.h>
#include <stdio.h>

int main() {
	printf("Math library test program\n");
	printf("========================\n\n");

	// Test basic math functions
	double x = 2.0;
	double y = 3.0;

	printf("Testing with x=%.1f, y=%.1f\n\n", x, y);

	// Power functions
	printf("pow(%.1f, %.1f) = %.3f\n", x, y, pow(x, y));
	printf("sqrt(%.1f) = %.3f\n", x * x, sqrt(x * x));
	printf("exp(1.0) = %.6f (should be ~2.718282)\n", exp(1.0));
	printf("log(%.6f) = %.3f (should be ~1.0)\n", exp(1.0), log(exp(1.0)));

	printf("\n");

	// Trigonometric functions
	double pi = 4.0 * atan(1.0);
	printf("pi = 4*atan(1) = %.6f\n", pi);
	printf("sin(pi/2) = %.3f (should be 1.0)\n", sin(pi / 2));
	printf("cos(pi) = %.3f (should be -1.0)\n", cos(pi));
	printf("tan(pi/4) = %.3f (should be 1.0)\n", tan(pi / 4));

	printf("\n");

	// Other functions
	printf("ceil(2.3) = %.1f\n", ceil(2.3));
	printf("floor(2.8) = %.1f\n", floor(2.8));
	printf("fabs(-5.5) = %.1f\n", fabs(-5.5));

	printf("\nMath library test completed successfully!\n");

	return 0;
}
