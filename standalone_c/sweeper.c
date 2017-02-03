#include "sweeper.h"

// Getters and setters for position data
unsigned char get_adjacent(unsigned char *position)
{
   return *position & BIT_ADJACENTS;
}
void set_adjacent(unsigned char* position, unsigned char adjacent_mines)
{
   *position |= adjacent_mines;
}
bool is_checked(unsigned char* position)
{
   return (*position & BIT_CHECKED) == BIT_CHECKED;
}
void set_checked(unsigned char* position, bool value)
{
   if(value) *position |= BIT_CHECKED;
   else *position &= ~(BIT_CHECKED);
}
bool is_mined(unsigned char* position)
{
   return (*position & BIT_MINED) == BIT_MINED;
}
void set_mined(unsigned char* position, bool value)
{
   if(value) *position |= BIT_MINED;
   else *position &= ~(BIT_MINED);
}
bool is_flagged(unsigned char* position)
{
   return (*position & BIT_FLAGGED) == BIT_FLAGGED;
}
void set_flagged(unsigned char* position, bool value)
{
   if(value) *position |= BIT_FLAGGED;
   else *position &= ~(BIT_FLAGGED);
}
bool is_revealed(unsigned char* position)
{
   return (*position & BIT_REVEALED) == BIT_REVEALED;
}
void set_revealed(unsigned char* position, bool value)
{
   if(value) *position |= BIT_REVEALED;
   else *position &= ~(BIT_REVEALED);
}

// Entry point for 'start_game' calls
unsigned char start_game(unsigned char height, unsigned char width, unsigned short mines)
{
   // Variables to set once
   unsigned short size; // Total number of positions on the field.
   unsigned char* field_begin; // Pointer to first position char in field.
   unsigned long urand; // dev/urandom number used to seed srand for random number generation for mine placements.

   // Variables which change 
   unsigned short mines_max; // Maximum number of allowed mines given the current playing field dimensions.
   unsigned short mines_placed; // Number of mines randomly placed on field.
   unsigned char* position; // Pointer into current field position.

   // Validate game parameters
   if(height < NEW_GAME_MIN_HEIGHT) return START_GAME_HEIGHT_TOO_SMALL;
   if(height > NEW_GAME_MAX_HEIGHT) return START_GAME_HEIGHT_TOO_LARGE;
   if(width < NEW_GAME_MIN_WIDTH) return START_GAME_WIDTH_TOO_SMALL;
   if(width > NEW_GAME_MAX_WIDTH) return START_GAME_WIDTH_TOO_LARGE;
   if(mines < NEW_GAME_MIN_MINES) return START_GAME_NOT_ENOUGH_MINES;
   mines_max = (height - 1) * (width - 1);
   if(mines > mines_max) return START_GAME_TOO_MANY_MINES;

   // Initialize field
   size = width * height;
   field_begin = (unsigned char*)calloc(size, sizeof(unsigned char));

   // Initialize mines
   mines_placed = 0;
   syscall(SYS_getrandom, &urand, sizeof(unsigned short), 0);
   srand(urand);
   while(mines_placed < mines)
   {
      // At this point we view the field as a linked list of unmined positions.  This means that there are only
      // (size - mines_placed) positions in the list at any given time.  We choose a random number which
      // corresponds to one of the remaining positions.
      unsigned short positions_until_random = rand() % (size - mines_placed);
      position = field_begin;

      // We will now traverse this "list" of unmined positions.  For the sake of performance we'll just traverse
      // the existing positions and not count the mined ones against the positions_until_random variable.  This is
      // computationally less expensive than maintaining an actual linked list and updating references between the
      // nodes/positions.
      while(positions_until_random > 0)
      {
         if(!is_mined(position))
         {
            // Position is unmined, so it counts as a traverse towards the final randomly chosen position.  Mined
            // positions do not.
            positions_until_random--;
         }

         // Move to the next position, even if the current one did not count as one of the traversed locations in
         // the list because it was mined.
         position++;
      }

      // We have now performed all of the traverses of unmined locations which we needed to, but we must manually
      // check the remaining locations to see if they are mined positions which would not have been "caught" by the
      // traverse, since they don't count against it.  In other words: these are the nodes that would have been
      // removed from the end of our conceptual linked list after being mined.
      while(is_mined(position))
      {
         position++;
      }
      set_mined(position, true);
      mines_placed++; 
   }

   // Calculate adjacency info for all unmined positions
   for(unsigned short y = 0; y < height; y++)
   {
      for(unsigned short x = 0; x < width; x++)
      {
         position = field_begin + width * y + x;
         if(!is_mined(position))
         {
            unsigned char adjacent_mines = 0;
            if(x > 0 && y > 0)
            {
               unsigned char* above_left = field_begin + width * (y - 1) + x - 1;
               if(is_mined(above_left)) adjacent_mines++;
            }
            if(y > 0)
            {
               unsigned char* above = field_begin + width * (y - 1) + x;
               if(is_mined(above)) adjacent_mines++;
            }
            if(x < width - 1 && y > 0)
            {
               unsigned char* above_right = field_begin + width * (y - 1) + x + 1;
               if(is_mined(above_right)) adjacent_mines++;
            }
            if(x > 0)
            {
               unsigned char* left = field_begin + width * y + x - 1;
               if(is_mined(left)) adjacent_mines++;
            }
            if(x < width - 1)
            {
               unsigned char* right = field_begin + width * y + x + 1;
               if(is_mined(right)) adjacent_mines++;
            }
            if(x > 0 && y < height - 1)
            {
               unsigned char* below_left = field_begin + width * (y + 1) + x - 1;
               if(is_mined(below_left)) adjacent_mines++;
            }
            if(y < height - 1)
            {
               unsigned char* below = field_begin + width * (y + 1) + x;
               if(is_mined(below)) adjacent_mines++;
            }
            if(x < width - 1 && y < height - 1)
            {
               unsigned char* below_right = field_begin + width * (y + 1) + x + 1;
               if(is_mined(below_right)) adjacent_mines++;
            }
            set_adjacent(position, adjacent_mines);
         }
      }
   }

   // Bolting on rudimentary output for testing purposes.  Will replace later.
   for(unsigned short y = 0; y < height; y++)
   {
      for(unsigned short x = 0; x < width; x++)
      {
         position = field_begin + y * width + x;
         if(is_mined(position))
         {
            printf("X");
         }
         else
         {
            printf("%d", get_adjacent(position));
         }
      }
      printf("\n");
   }

   // Clear the playing field
   free(field_begin);
   return GENERAL_NO_ERROR;
}

