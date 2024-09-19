#include "../include/fibonacci.h"

uint64_t fibonacci(unsigned int n) {
  switch (n) {
    case 0:
      return 0;
      break;
    case 1:
      return 1;
      break;
    default:
      return (fibonacci(n - 1) + fibonacci(n - 2));
  }
}
