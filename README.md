# C_cheddaboards
a simple example for communicating with cheddabords using C - see https://cheddaboards.com/ and https://github.com/cheddatech/CheddaBoards-Godot for more info.

# How To:

Basic usage:

1: create and account and set up a game and boards over at https://cheddaboards.com/

2: copy and paste your API Key and game id into the chedda_API_key and chedda_game_ID strings in chedda.c

3: create a user or load user details from a file. Use the functions chedda_create_user() or chedda_create_existing_user() for this.

4: Set or get some scores! use the chedda_get_scores() and chedda_submit_score() functions for this. If they return successfully, you can parse the json data to check for errors from the server and populate your scoreboards.

5: Free memory. Remember to free userdata returned by chedda_create_user()and chedda_create_existing_user() with chedda_user_free(). The returnData parameter in chedda_get_scores() and chedda_submit_score() also needs to be freed if it's not NULL after the function returns. See the example.

# Example

See the main() function in chedda.c for a usage example. If you have libcurl installed you can use the include buildrun.sh script to compile and execute the example. You will likely need to use 'chmod +x buildrun.sh' first.

# License

This is provided for free with no license, do with it as you wish though it would be nice if you let me know if you use the code.

# Notes

This is a pretty barebones example but it does work. I have no plans to expand the code beyond what is here, though if you expand it I'd be happy to include our additions here.

libcurl functions can take a long time to complete, so you will likely want to call the chedda_get_scores() and chedda_submit_score() functions from their own threads. This is beyond the scope of this example.