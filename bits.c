
  int Funct(arg1, arg2, ...) {
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
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
  // EVIL TRICK: long long
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
  unsigned sign_exp = uf & 0xFF800000;
  unsigned frac;
  unsigned mask = 0x7FFFFF;
  unsigned shift = 1;

  switch (sign_exp) {
    case 0x7F800000:
    case 0xFF800000:
      return uf;
    case 0:
    case 0x80000000:
      break;
    default:
      switch (sign_exp -= 0x800000) {
        case 0:
        case 0x80000000:
          mask = 0xFFFFFF;
          break;
        default:
          shift = 0;
      }
  }

  frac = uf & mask;
  frac = frac >> shift;

  switch (uf & 3) {
    case 3:
      frac += shift;
  }

  return sign_exp | frac;
}

int float_f2i(unsigned uf) {

  unsigned sign = uf & 0x80000000;
  unsigned exp = (uf >> 23) & 0xFF;
  unsigned frac = uf & 0x7FFFFF;

  if (exp < 127)
    return 0;
  else if (exp > 158)
    return 0x80000000;

  frac = (frac | 0x800000) >> (150 - exp);
  return sign ? -frac : frac;
}
