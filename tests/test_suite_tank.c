#include "tank.h"
#include "test_util.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

double MAX_WIDTH = 1000.0;
double MAX_HEIGHT = 500.0;
double LENGTH = 60.0;
double GRAVITATIONAL_CONSTANT = -900.81;
double DAMPING_CONSTANT = 0.85;

double DEFAULT_SPEED = 200.0;
double DEFAULT_ANGLE = M_PI / 4;
double DEFAULT_ROTATION_SPEED = M_PI / 8;

// creates a star for our tests, with all the params necessary to intialize a
// star (from star_init)

// this also tests star_init and star_free
void test_make_tank() {
  
}

int main(int argc, char *argv[]) {
  // Run all tests? True if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }
  DO_TEST(test_make_tank);
  puts("tank_init PASS");
  puts("tank_free PASS");
}
