#include <map>

int main() {
  std::map<int, int> numbers;

  int test_data[] = {
#include "numbers.txt"
  };
  std::pair<int, int> pr(0, 1);
  for (int i = 0; i < sizeof test_data / sizeof test_data[0]; i++) {
    pr.first = test_data[i];
    numbers.insert(pr);
  }
  return 0;
}
