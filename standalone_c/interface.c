#include "sweeper.h"

int main()
{
   start_game(24, 30, 100);
   display_as_client();
   display_as_server();
   reveal(10, 10);
   display_as_client();
   display_as_server();
   quit();
   return 0;
}

