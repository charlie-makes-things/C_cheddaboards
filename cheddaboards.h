
#ifndef CHEDDA_C_IMPLEMENTATION
#define CHEDDA_C_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

//initialise cheddaboards with your api key and game id
//you must call this before submitting scores etc.
extern int chedda_init(char *api_key,char* game_id);

//free cheddaboards data. call this once you're done.
extern int chedda_free();

//create a new user
extern void *chedda_create_user(char *nickname);
//create a user from existing data - i.e. loaded from a save file.
extern void *chedda_create_existing_user(char *UID,char *nickname);
//free user data
extern void chedda_user_free(void *user);
//returns the chedda user id, useful for saving to file
extern char *chedda_get_user_id(void *user);
//returns the user nickname as show in scoreboards. useful for saving to file
extern char *chedda_get_user_nickname(void *user);

//get scores for the specified board. the boardID must match the one you set in
//the cheddaboards developer console.
extern int chedda_get_scores( char *boardID, int count, char **returnData);
//submit score to the global board. i.e. any non-targeted board
extern int chedda_submit_score_global(void *user,long score, int streak,char **returnData);
//submit score to a targeted scoreboard. the boardID must match the one you set 
//in the cheddaboards developer console.
extern int chedda_submit_score_targeted(void *user,char *boardID,long score, int streak,char **returnData);


#endif //CHEDDA_C_IMPLEMENTATION