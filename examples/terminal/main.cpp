#include <cstdlib>
#include <unistd.h>

int main() {
    execl("/home/blame/projects/terminally-hacked/terminally-hacked",
          "terminally-hacked", static_cast<char*>(nullptr));
    return EXIT_FAILURE;
}