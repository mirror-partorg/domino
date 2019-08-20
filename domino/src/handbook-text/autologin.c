#include <unistd.h>

int main()
{
  execlp( "login", "login", "-f", "handbook", NULL);
  return 0;
}
