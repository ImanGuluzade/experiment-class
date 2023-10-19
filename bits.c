#include <stdio.h>
#include <limits.h>

int Funct(int arg1, int arg2, ...) {
    int var1 = arg1;

    int varM = arg2;

    int varJ =  0; 
  
    int varN =  0; 

    return 0; 
}

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
    int t = 1 << (31 - n);
    return ((x >> n) + t) ^ t;
}

int rotateRight(int x, int n) {
    return (x >> n) | ((x & ((1 << n) - 1)) << (32 - n));
}

int bang(int x) {
    return ((x | (~x + 1)) >> 31) + 1;
}

int tmax(void) {
    return INT_MAX;
}

int absVal(int x) {
    int t = x >> 31;
    return (t ^ (x + t)) - t;
}

int divpwr2(int x, int n) {
    int t = x >> 31;
    return (x + (t & ((1 << n) - 1))) >> n;
}

int ezFiveEighths(int x) {
    int y = (x << 2) + x;
    return (y + ((y >> 31) & 7)) >> 3;
}

int isPositive(int x) {
    return !((x >> 31) | !x);
}

int isLessOrEqual(int x, int y) {
    return !(((x ^ y) >> 31) | (x + ~y + 1) >> 31);
}

int ilog2(int x) {
    int ans = (!(x >> 16)) << 4;
    ans ^= (!(x >> (ans + 8))) << 3;
    ans ^= 24;
    ans ^= (!(x >> (ans + 4))) << 2;
    x = x >> ans;
    ans ^= ((~0x3B) >> (x & 30)) & 3;
    return ans;
}

int main() {
  
    return 0;
}
