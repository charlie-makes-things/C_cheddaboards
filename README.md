# C_cheddaboards
a simple library for communicating with cheddabords using C - see https://cheddaboards.com/ and https://github.com/cheddatech/CheddaBoards-Godot for more info.

# Building the library

You can use the buildlibrary-static.sh script to build a static library. Alternatively you can use the buildlibrary-dynamic.sh script to build a dynamic library. It is likely you will need to change the permissions on these scripts to execute them i.e. chmod +x buildlibrary-xxxxx.sh

The library will be output into either the build/static or build/dylib folder, move whichever on of these you wish to use onto your linker path to use the library. I'd recommend using the static library, as the library is very small.

# How To:

Basic usage:

1: create and account and set up a game and boards over at https://cheddaboards.com/ and have a read of the REST api docs here: https://github.com/cheddatech/CheddaBoards-Godot/blob/main/docs/quickstart-api.md

2: install libcurl, either from your distros package manager or from https://curl.se/libcurl/

3: include cheddaboards.h in you source file and point your linker at the library.

4: initialize the library by calling chedda_init(api_key,game_id);

5: Set or get some scores! use the chedda_get_scores() and chedda_submit_score_global() or chedda_submit_scores_targetted() functions for this. If they return successfully, you can parse the json data to check for errors from the server and populate your scoreboards.

6: Free memory. Remember to free userdata returned by chedda_create_user()and chedda_create_existing_user() with chedda_user_free(). The returnData parameter in chedda_get_scores() and chedda_submit_score() also needs to be freed if it's not NULL after the function returns. Also remember to use chedda_free() when you finish using the library, or at exit. See the example in test.c.

# Example

See test.c for a usage example. You can use the buildrun-xxx.sh to compile both the library and the test file either statically or dynamically. You will likely need to use 'chmod +x buildrun-xxx.sh' first.

# License

This is provided for free with no license, do with it as you wish though it would be nice if you let me know if you use the code.

# Notes

This is a pretty barebones example. I have no plans to expand the code beyond what is here, though if you expand it I'd be happy to include your additions here.

libcurl functions can take a long time to complete, so you will likely want to call the chedda_get_scores() and chedda_submit_score() functions from their own threads. This is beyond the scope of this example.