#include "sweeper.h"

// Linked list structure for playing field positions
typedef struct node
{
   unsigned char* position;
   unsigned char x;
   unsigned char y;
   struct node* next;
} Node;

// Game variables which are shared between functions
static bool first_reveal_setup_performed; // Mines won't be placed until the player has performed a reveal.
static short mines_not_flagged; // Number of mines which haven't been flagged.  Can be negative.
static unsigned short unmined_positions_remaining; // Number of positions which the player must reveal to win.
static unsigned short current_mines; // Number of mines on the playing field after setup.
static unsigned char current_height; // Current height of the playing field.
static unsigned char current_width; // Current width of the playing field.
static unsigned char* field_begin; // Pointer to first position char in field.
static Node* nm_head; // Not-mined list used to track available positions for random mine placement.
static Node* nm_tail; // Not-mined list used to track available positions for random mine placement.
static Node* mbla_head; // Modified by last action list used to return specific results of actions to client.
static Node* mbla_tail; // Modified by last action list used to return specific results of actions to client.
static unsigned char game_status; // Status code indicating current game state.
static struct timespec time_started; // Time elapsed between thread start and when the game was started.

// Internal getters and setters for position data
static unsigned char get_adjacent(unsigned char *position)
{
   return *position & BIT_ADJACENTS;
}
static void set_adjacent(unsigned char* position, unsigned char adjacent_mines)
{
   *position |= adjacent_mines;
}
static bool is_mined(unsigned char* position)
{
   return (*position & BIT_MINED) == BIT_MINED;
}
static void set_mined(unsigned char* position, bool value)
{
   if(value) *position |= BIT_MINED;
   else *position &= ~(BIT_MINED);
}
static bool is_flagged(unsigned char* position)
{
   return (*position & BIT_FLAGGED) == BIT_FLAGGED;
}
static void set_flagged(unsigned char* position, bool value)
{
   if(value) *position |= BIT_FLAGGED;
   else *position &= ~(BIT_FLAGGED);
}
static bool is_revealed(unsigned char* position)
{
   return (*position & BIT_REVEALED) == BIT_REVEALED;
}
static void set_revealed(unsigned char* position, bool value)
{
   if(value) *position |= BIT_REVEALED;
   else *position &= ~(BIT_REVEALED);
}

// Calculate adjacency counts for all of the positions on the playing field which are adjacent to mines and place
// the detected sum into the leading data bits for each position.
static void calculate_adjacency_data()
{
   for(unsigned short y = 0; y < current_height; y++)
   {
      for(unsigned short x = 0; x < current_width; x++)
      {
         unsigned char* position = field_begin + current_width * y + x;
         if(!is_mined(position))
         {
            unsigned char adjacent_mines = 0;
            if(x > 0 && y > 0)
            {
               unsigned char* above_left = field_begin + current_width * (y - 1) + x - 1;
               if(is_mined(above_left)) adjacent_mines++;
            }
            if(y > 0)
            {
               unsigned char* above = field_begin + current_width * (y - 1) + x;
               if(is_mined(above)) adjacent_mines++;
            }
            if(x < current_width - 1 && y > 0)
            {
               unsigned char* above_right = field_begin + current_width * (y - 1) + x + 1;
               if(is_mined(above_right)) adjacent_mines++;
            }
            if(x > 0)
            {
               unsigned char* left = field_begin + current_width * y + x - 1;
               if(is_mined(left)) adjacent_mines++;
            }
            if(x < current_width - 1)
            {
               unsigned char* right = field_begin + current_width * y + x + 1;
               if(is_mined(right)) adjacent_mines++;
            }
            if(x > 0 && y < current_height - 1)
            {
               unsigned char* below_left = field_begin + current_width * (y + 1) + x - 1;
               if(is_mined(below_left)) adjacent_mines++;
            }
            if(y < current_height - 1)
            {
               unsigned char* below = field_begin + current_width * (y + 1) + x;
               if(is_mined(below)) adjacent_mines++;
            }
            if(x < current_width - 1 && y < current_height - 1)
            {
               unsigned char* below_right = field_begin + current_width * (y + 1) + x + 1;
               if(is_mined(below_right)) adjacent_mines++;
            }
            set_adjacent(position, adjacent_mines);
         }
      }
   }
}

// Secondary setup function which is triggered when the player performs their first reveal action.
static void first_reveal_setup(unsigned char x, unsigned char y)
{
   // Remove the position to reveal from the non-mined list so that we don't consider it for mine placement
   // during generation.  This prevents the player from unfairly blowing up on their first reveal.
   if(x == 0 && y == 0)
   {
      Node* nm_free = nm_head;
      nm_head = nm_head->next;
      free(nm_free);
      nm_free = NULL;
   }
   else
   {
      Node* nm_index = nm_head;
      unsigned short positions_left_to_traverse = current_width * y + x;
      while(positions_left_to_traverse > 1)
      {
         nm_index = nm_index->next;
         positions_left_to_traverse--;
      }
      Node* nm_free = nm_index->next;
      nm_index->next = nm_index->next->next;
      free(nm_free);
      nm_free = NULL;
   }

   // Initialize random number generator.  This will make a system call to dev/urandom to retrieve entropy bits
   // for a random seed which are generated by the system from process IDs, temperature sensors, network packet
   // timings, and other quasi-random entities.  This is a GNU/Linux specific feature which serves as a more
   // secure alternative to the common practice of using the current time as a random seed.  This seed will be
   // used in conjunction with a Mersenne Twister pseudo-random number generator to generate random positions
   // for mine placements on the playing field.  This PRNG is purportedly faster and less predictable than the
   // default implementation of the standard 'rand' function.
   long urand;
   syscall(SYS_getrandom, &urand, sizeof(long), 0);
   //srand(urand);
   seed_mt19937(urand);
   mines_not_flagged = current_mines;
   unsigned short mines_placed = 0;
   while(mines_placed < current_mines)
   {
      // The most straight-forward approach here would be to generate random numbers for the x and y coordinates
      // of the next mine placement.  If those coordinates happened to be mined already we would simply generate
      // a new set of random coordinates.  Unfortunately, as more mines are placed it would become more and more
      // probable that we'd run into these sorts of collision scenarios.  Random number generation, while
      // generally operating in O(1) time, is computationally expensive and there is no guaranteed upper limit
      // on the number of collisions (and therefore recalculations) which we'd have to perform.  Instead, I've
      // decided to utilize a safer and more performant approach which uses a linked list of remaining unmined
      // locations.  A random position in this list will be chosen for each new mine placement.  We'll traverse
      // the list to that position, place the mine, and then remove the position from the list.  While this is
      // technically an O(N) operation each traverse will take less and less time since the unmined list will
      // continue to shorten as more mines are placed.  This is also guaranteed to work without collisions, and
      // the bounds on N are fairly trivial since the playing field has relatively small maximum dimensions.
      //unsigned short random_number = (unsigned short)rand();
      unsigned short random_number = (unsigned short)extract_mt19937();
      unsigned short positions_left_to_traverse = random_number % (unmined_positions_remaining - 1);
      if(positions_left_to_traverse == 0)
      {
         set_mined(nm_head->position, true);
         Node* nm_free = nm_head;
         nm_head = nm_head->next;
         free(nm_free);
         nm_free = NULL;
      }
      else
      {
         Node* nm_index = nm_head;
         while(positions_left_to_traverse > 1)
         {
            nm_index = nm_index->next;
            positions_left_to_traverse--;
         }
         set_mined(nm_index->next->position, true);
         Node* nm_free = nm_index->next;
         nm_index->next = nm_index->next->next;
         free(nm_free);
         nm_free = NULL;
      }
      mines_placed++;
      unmined_positions_remaining--;
   }

   // Now that the mines have all been placed we can calculate the adjacency data for unmined positions;
   calculate_adjacency_data();
   
   // Start the timer
   clock_gettime(CLOCK_MONOTONIC, &time_started);

   // Return to reveal function
   first_reveal_setup_performed = true;
}

unsigned char start_game(unsigned char height, unsigned char width, unsigned short mines)
{
   // Error handling
   if(height < NEW_GAME_MIN_HEIGHT) return START_GAME_HEIGHT_TOO_SMALL;
   if(height > NEW_GAME_MAX_HEIGHT) return START_GAME_HEIGHT_TOO_LARGE;
   if(width < NEW_GAME_MIN_WIDTH) return START_GAME_WIDTH_TOO_SMALL;
   if(width > NEW_GAME_MAX_WIDTH) return START_GAME_WIDTH_TOO_LARGE;
   if(mines < NEW_GAME_MIN_MINES) return START_GAME_NOT_ENOUGH_MINES;
   if(mines > (height - 1) * (width - 1)) return START_GAME_TOO_MANY_MINES;

   // Initialize shared variables
   first_reveal_setup_performed = false;
   mines_not_flagged = mines;
   unmined_positions_remaining = height * width;
   current_mines = mines;
   current_height = height;
   current_width = width;
   mbla_head = NULL;
   mbla_tail = NULL;

   // Allocate memory for playing field.  Note that each position on the field is represented by a single
   // character where the individual bits in that character are utilized for an adjacent mine count, mined flag,
   // revealed flag, and flagged flag.  Coordinate and dimension values are not stored in the field memory itself,
   // so separate variables must be provided for these in order to calculate the specific memory location needed
   // to access a particular position character within the playing field.  A pointer to a position character is
   // not sufficient to determine where that position is on the playing field without also knowing the memory
   // address of the start of the playing field and the width of the playing field.
   field_begin = (unsigned char*)calloc(height * width, sizeof(unsigned char));

   // Initialize the non-mined non-revealed linked list.  At this point every position on the playing field belongs
   // in this list since nothing has been revealed and no mines have been placed yet.
   nm_head = (Node*)malloc(sizeof(Node));
   *nm_head = (Node){0};
   nm_tail = nm_head;
   for(int y = 0; y < height; y++)
   {
      for(int x = 0; x < width; x++)
      {
         unsigned char* position = field_begin + width * y + x;
         nm_tail->next = (Node*)malloc(sizeof(Node));
         *(nm_tail->next) = (Node){position, x, y, NULL};
         nm_tail = nm_tail->next;
      }
   }
   Node* nm_free = nm_head;
   nm_head = nm_head->next;
   free(nm_free);
   nm_free = NULL;

   // Timer will not be started until the player performs their first reveal
   memset(&time_started, 0, sizeof(struct timespec));

   // Game is ready to play!
   game_status = GAME_STATUS_IN_PROGRESS;
   return GENERAL_NO_ERROR;
}

unsigned char reveal(unsigned char x, unsigned char y)
{
   // Error handling
   if(game_status != GAME_STATUS_IN_PROGRESS) return REVEAL_GAME_NOT_IN_PROGRESS;
   if(game_status > GAME_STATUS_IN_PROGRESS) return REVEAL_GAME_ALREADY_FINISHED;
   if(x >= current_width) return REVEAL_X_COORDINATE_TOO_HIGH;
   if(y >= current_height) return REVEAL_Y_COORDINATE_TOO_HIGH;
   unsigned char* position = field_begin + current_width * y + x;
   if(is_revealed(position)) return REVEAL_ALREADY_REVEALED;

   // Setup playing field if this is the first time the player has performed a reveal.
   if(!first_reveal_setup_performed)
   {
      first_reveal_setup(x, y);
   }

   // Perform reveal
   set_revealed(position, true);
   if(is_mined(position))
   {
      // Boom!
      game_status = GAME_STATUS_LOST;
      return GENERAL_NO_ERROR;
   }
   unmined_positions_remaining--;
 
   // Populate the reveal queue starting with the first revealed position
   Node* rq_head = (Node*)malloc(sizeof(Node));
   *rq_head = (Node){position, x, y, NULL};
   Node* rq_tail = rq_head;

   // Begin revealing additional positions and adding surrounding unmined+unrevealed+unadjacent positions to the
   // queue.  Revealing is performed first as a method of marking which positions have already been added to the
   // queue (in order to prevent duplicate entries).
   while(rq_head != NULL)
   {
      if(rq_head->x > 0 && rq_head->y > 0)
      {
         unsigned char* above_left = field_begin + current_width * (rq_head->y - 1) + rq_head->x - 1;
         if(!is_mined(above_left) && !is_revealed(above_left))
         {
            set_revealed(above_left, true);
            unmined_positions_remaining--;
            if(get_adjacent(above_left) == 0)
            {
               rq_tail->next = (Node*)malloc(sizeof(Node));
               *rq_tail->next = (Node){above_left, rq_head->x - 1, rq_head->y - 1, NULL};
               rq_tail = rq_tail->next;
            }
         }
      }
      if(rq_head->y > 0)
      {
         unsigned char* above = field_begin + current_width * (rq_head->y - 1) + rq_head->x;
         if(!is_mined(above) && !is_revealed(above))
         {
            set_revealed(above, true);
            unmined_positions_remaining--;
            if(get_adjacent(above) == 0)
            {
               rq_tail->next = (Node*)malloc(sizeof(Node));
               *rq_tail->next = (Node){above, rq_head->x, rq_head->y - 1, NULL};
               rq_tail = rq_tail->next;
            }
         }
      }
      if(rq_head->x < current_width - 1 && rq_head->y > 0)
      {
         unsigned char* above_right = field_begin + current_width * (rq_head->y - 1) + rq_head->x + 1;
         if(!is_mined(above_right) && !is_revealed(above_right))
         {
            set_revealed(above_right, true);
            unmined_positions_remaining--;
            if(get_adjacent(above_right) == 0)
            {
               rq_tail->next = (Node*)malloc(sizeof(Node));
               *rq_tail->next = (Node){above_right, rq_head->x + 1, rq_head->y - 1, NULL};
               rq_tail = rq_tail->next;
            }
         }
      }
      if(rq_head->x > 0)
      {
         unsigned char* left = field_begin + current_width * rq_head->y + rq_head->x - 1;
         if(!is_mined(left) && !is_revealed(left))
         {
            set_revealed(left, true);
            unmined_positions_remaining--;
            if(get_adjacent(left) == 0)
            {
               rq_tail->next = (Node*)malloc(sizeof(Node));
               *rq_tail->next = (Node){left, rq_head->x - 1, rq_head->y, NULL};
               rq_tail = rq_tail->next;
            }
         }
      }
      if(rq_head->x < current_width - 1)
      {
         unsigned char* right = field_begin + current_width * rq_head->y + rq_head->x + 1;
         if(!is_mined(right) && !is_revealed(right))
         {
            set_revealed(right, true);
            unmined_positions_remaining--;
            if(get_adjacent(right) == 0)
            {
               rq_tail->next = (Node*)malloc(sizeof(Node));
               *rq_tail->next = (Node){right, rq_head->x + 1, rq_head->y, NULL};
               rq_tail = rq_tail->next;
            }
         }
      }
      if(rq_head->x > 0 && rq_head->y < current_height - 1)
      {
         unsigned char* below_left = field_begin + current_width * (rq_head->y + 1) + rq_head->x - 1;
         if(!is_mined(below_left) && !is_revealed(below_left))
         {
            set_revealed(below_left, true);
            unmined_positions_remaining--;
            if(get_adjacent(below_left) == 0)
            {
               rq_tail->next = (Node*)malloc(sizeof(Node));
               *rq_tail->next = (Node){below_left, rq_head->x - 1, rq_head->y + 1, NULL};
               rq_tail = rq_tail->next;
            }
         }
      } 
      if(rq_head->y < current_height - 1)
      {
         unsigned char* below = field_begin + current_width * (rq_head->y + 1) + rq_head->x;
         if(!is_mined(below) && !is_revealed(below))
         {
            set_revealed(below, true);
            unmined_positions_remaining--;
            if(get_adjacent(below) == 0)
            {
               rq_tail->next = (Node*)malloc(sizeof(Node));
               *rq_tail->next = (Node){below, rq_head->x, rq_head->y + 1, NULL};
               rq_tail = rq_tail->next;
            }
         }
      }
      if(rq_head->x < current_width - 1 && rq_head->y < current_height - 1)
      {
         unsigned char* below_right = field_begin + current_width * (rq_head->y + 1) + rq_head->x + 1;
         if(!is_mined(below_right) && !is_revealed(below_right))
         {
            set_revealed(below_right, true);
            unmined_positions_remaining--;
            if(get_adjacent(below_right) == 0)
            {
               rq_tail->next = (Node*)malloc(sizeof(Node));
               *rq_tail->next = (Node){below_right, rq_head->x + 1, rq_head->y + 1, NULL};
               rq_tail = rq_tail->next;
            }
         }
      }

      // Chop off the reveal queue head and advance to the next position
      Node* rq_free = rq_head;
      rq_head = rq_head->next;
      free(rq_free);
      rq_free = NULL;
   }
   rq_tail = NULL;

   // Detect win condition
   if(unmined_positions_remaining == 0) game_status = GAME_STATUS_WON;
   return GENERAL_NO_ERROR;
}

unsigned char flag(unsigned char x, unsigned char y)
{
   // Error handling
   if(game_status != GAME_STATUS_IN_PROGRESS) return FLAG_GAME_NOT_IN_PROGRESS;
   if(game_status > GAME_STATUS_IN_PROGRESS) return FLAG_GAME_ALREADY_FINISHED;
   if(x >= current_width) return FLAG_X_COORDINATE_TOO_HIGH;
   if(y >= current_height) return FLAG_Y_COORDINATE_TOO_HIGH;
   unsigned char* position = field_begin + current_width * y + x;
   if(is_revealed(position)) return FLAG_REVEALED_CANNOT_FLAG;
   if(is_flagged(position)) return FLAG_ALREADY_FLAGGED;

   // Set flag
   set_flagged(position, true);
   mines_not_flagged--;

   // Update modified positions list to reflect this action
   mbla_tail = NULL;
   while(mbla_head != NULL)
   {
      Node* mbla_free = mbla_head;
      mbla_head = mbla_head->next;
      free(mbla_free);
      mbla_free = NULL;
   }
   mbla_head = (Node*)malloc(sizeof(Node));
   *mbla_head = (Node){position, x, y, NULL};
   mbla_tail = mbla_head;
   return GENERAL_NO_ERROR;
}

unsigned char unflag(unsigned char x, unsigned char y)
{
   // Error handling
   if(game_status != GAME_STATUS_IN_PROGRESS) return UNFLAG_GAME_NOT_IN_PROGRESS;
   if(game_status > GAME_STATUS_IN_PROGRESS) return UNFLAG_GAME_ALREADY_FINISHED;
   if(x >= current_width) return UNFLAG_X_COORDINATE_TOO_HIGH;
   if(y >= current_height) return UNFLAG_Y_COORDINATE_TOO_HIGH;
   unsigned char* position = field_begin + current_width * y + x;
   if(is_revealed(position)) return UNFLAG_REVEALED_CANNOT_UNFLAG;
   if(!is_flagged(position)) return UNFLAG_NOT_FLAGGED;

   // Clear flag
   set_flagged(position, false);
   mines_not_flagged++;

   // Update modified positions list to reflect this action
   mbla_tail = NULL;
   while(mbla_head != NULL)
   {
      Node* mbla_free = mbla_head;
      mbla_head = mbla_head->next;
      free(mbla_free);
      mbla_free = NULL;
   }
   mbla_head = (Node*)malloc(sizeof(Node));
   *mbla_head = (Node){position, x, y, NULL};
   mbla_tail = mbla_head; 
   return GENERAL_NO_ERROR;
}

unsigned char quit()
{
   // Error handling
   if(game_status == GAME_STATUS_NOT_IN_PROGRESS) return QUIT_GAME_NOT_IN_PROGRESS;

   // Reset all shared variables and lists
   first_reveal_setup_performed = false;
   mines_not_flagged = 0;
   unmined_positions_remaining = 0;
   current_height = 0;
   current_width = 0;
   free(field_begin);
   field_begin = NULL;
   while(nm_head != NULL)
   {
      Node* nm_free = nm_head;
      nm_head = nm_head->next;
      free(nm_free);
      nm_free = NULL;
   }
   nm_tail = NULL;
   while(mbla_head != NULL)
   {
      Node* mbla_free = mbla_head;
      mbla_head = mbla_head->next;
      free(mbla_free);
      mbla_free = NULL;
   }
   mbla_tail = NULL;
   game_status = GAME_STATUS_NOT_IN_PROGRESS;
   memset(&time_started, 0, sizeof(struct timespec));
   return GENERAL_NO_ERROR;
}

// Temporary
void display_as_client()
{
   printf("Field from client perspective:\n");
   for(unsigned short y = 0; y < current_height; y++)
   {
      for(unsigned short x = 0; x < current_width; x++)
      {
         unsigned char* position = field_begin + current_width * y + x;
         if(!is_revealed(position))
         {
            printf("?");
         }
         else
         {
            if(is_mined(position))
            {
               printf("X");
            }
            else
            {
               unsigned char adjacent_mines = get_adjacent(position);
               if(adjacent_mines != 0) printf("%d", adjacent_mines);
               else printf("_");
            } 
         } 
      }
      printf("\n");
   }
   printf("\n");
}

// Temporary
void display_as_server()
{
   printf("Field from server perspective:\n");
   for(unsigned short y = 0; y < current_height; y++)
   {
      for(unsigned short x = 0; x < current_width; x++)
      {
         unsigned char* position = field_begin + current_width * y + x;
         if(is_mined(position))
         {
            printf("X");
         }
         else
         {
            unsigned char adjacent_mines = get_adjacent(position);
            if(adjacent_mines != 0) printf("%d", adjacent_mines);
            else printf("_");
         }
      }
      printf("\n");
   }
   printf("\n");
}

