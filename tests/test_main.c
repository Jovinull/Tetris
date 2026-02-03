#include <stdio.h>
#include <stdlib.h>

int test_board(void);
int test_srs(void);
int test_randomizer(void);
int test_scoring(void);

#define RUN_TEST(fn) do { \
  int r = (fn)(); \
  if (r != 0) { printf("[FAIL] %s\n", #fn); return r; } \
  printf("[ OK ] %s\n", #fn); \
} while(0)

int main(void) {
  RUN_TEST(test_board);
  RUN_TEST(test_srs);
  RUN_TEST(test_randomizer);
  RUN_TEST(test_scoring);
  printf("All tests passed.\n");
  return 0;
}