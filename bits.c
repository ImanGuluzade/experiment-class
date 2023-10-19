#include <stdio.h>

// Function prototypes
int bitXor(int x, int y);
int bitParity(int x);
int anyOddBit(int x);
int logicalShift(int x, int n);
int rotateRight(int x, int n);
int bang(int x);
int tmax(void);
int absVal(int x);
int divpwr2(int x, int n);
int ezFiveEighths(int x);
int isPositive(int x);
int isLessOrEqual(int x, int y);
int ilog2(int x);
unsigned float_half(unsigned uf);
int float_f2i(unsigned uf);

int main() {
    // Sample usage of the functions:

    // Example of calling the bitXor function:
    int result1 = bitXor(5, 3);
    printf("bitXor result: %d\n", result1);

    // Example of calling the bitParity function:
    int result2 = bitParity(7);
    printf("bitParity result: %d\n", result2);

    // Example of calling the anyOddBit function:
    int result3 = anyOddBit(42);
    printf("anyOddBit result: %d\n", result3);

    // Example of calling the logicalShift function:
    int result4 = logicalShift(8, 2);
    printf("logicalShift result: %d\n", result4);

    // Example of calling the rotateRight function:
    int result5 = rotateRight(16, 2);
    printf("rotateRight result: %d\n", result5);

    // Example of calling the bang function:
    int result6 = bang(0);
    printf("bang result: %d\n", result6);

    // Example of calling the tmax function:
    int result7 = tmax();
    printf("tmax result: %d\n", result7);

    // Example of calling the absVal function:
    int result8 = absVal(-10);
    printf("absVal result: %d\n", result8);

    // Example of calling the divpwr2 function:
    int result9 = divpwr2(16, 2);
    printf("divpwr2 result: %d\n", result9);

    // Example of calling the ezFiveEighths function:
    int result10 = ezFiveEighths(20);
    printf("ezFiveEighths result: %d\n", result10);

    // Example of calling the isPositive function:
    int result11 = isPositive(-5);
    printf("isPositive result: %d\n", result11);

    // Example of calling the isLessOrEqual function:
    int result12 = isLessOrEqual(10, 5);
    printf("isLessOrEqual result: %d\n", result12);

    // Example of calling the ilog2 function:
    int result13 = ilog2(32);
    printf("ilog2 result: %d\n", result13);

    // Example of calling the float_half function:
    unsigned uf = 0x40490FDB;
    unsigned result14 = float_half(uf);
    printf("float_half result: 0x%08X\n", result14);

    // Example of calling the float_f2i function:
    unsigned uf2 = 0xC0597800;
    int result15 = float_f2i(uf2);
    printf("float_f2i result: %d\n", result15);

    return 0;
}

// Function implementations (You provided these earlier)
int bitXor(int x, int y) {
    return ~(x & y) & ~(~x & ~y);
}

int bitParity(int x) {
    x = (x >> 1) ^ x;
    x = (x >> 2) ^ x;
    x = (x >> 4) ^ x;
    x = (x >> 8) ^ x;
    x = (x >> 16) ^ x;
    return x & 1;
}

int anyOddBit(int x) {
    int t = 0xAA;
    t = t | (t << 8);
    t = t | (t << 16);
    return !!(x & t);
}

int logicalShift(int x, int n) {
    int t = 1 << (31 ^ n);
    return ((x >> n) + t) ^ t;
}

int rotateRight(int x, int n) {
    return (x >> n) ^ ((x ^ (x >> 31)) << (31 ^ n) << 1);
}

int bang(int x) {
    return ((x | ((~x) + 1)) >> 31) + 1;
}

int tmax(void) {
    return ~(1 << 31);
}

int absVal(int x) {
    int t = x >> 31;
    return (t + x) ^ t;
}

int divpwr2(int x, int n) {
    int t = x >> 31;
    return (x + (t ^ (t << n))) >> n;
}

int ezFiveEighths(int x) {
    int y = (x << 2) + x;
    return (y + ((y >> 31) & 7)) >> 3;
}

int isPositive(int x) {
    return !(((x >> 31) | (!x)));
}

int isLessOrEqual(int x, int y) {
    return ((x + ~(((x ^ y) >> 31) | y)) >> 31) & 1;
}

int ilog2(int x) {
    int ans = (!(x >> 16)) << 4;
    ans ^= (!(x << ans >> 24)) << 3;
    ans ^= 28;
    ans ^= (!(x >> ans)) << 2;
    x = x >> ans;
    ans ^= ((~0x5B) >> (x & 30)) & 3;
    return ans;
}

unsigned float_half(unsigned uf) {
    // Implement this function if needed for your assignment.
    // Return a placeholder value for now.
    return 0;
}

int float_f2i(unsigned uf) {
    // Implement this function if needed for your assignment.
    // Return a placeholder value for now.
    return 0;
}
