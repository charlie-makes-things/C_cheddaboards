//simple C interface for using the cheddaboards web API to set and get
//scores.
//by charlie - https://charliemakesthings.com/
//code - https://github.com/charlie-makes-things/C_cheddaboards
//
//uses the libcurl library to communicate with the cheddaboards server
//to compile, try 'gcc chedda.c -o chedda -lcurl' then './chedda'
//
//note that you will need to call these functions from a separate thread
//in your game or the program will pause while it awaits a response from the
//server. This is beyond the scope of this example.
//
//make sure to enter your API key and game name in the variables immediately
//below

//set these to you api key and game id.
const char *chedda_API_key = "your_api_key_goes_here";
const char *chedda_game_ID = "your_game_id_goes_here";


#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>


//user id struct
typedef struct cheddaUser{
	char *UID;
	char *nickname;
}cheddaUser;

//struct to contain deta from chedda boards
struct __chedda_response {
  char *memory;
  size_t size;
};



//callback function to write recieved data
static size_t __chedda_response_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct __chedda_response *mem = (struct __chedda_response *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

//generate a new user id. you are responsible for freeing this memory
//this function is used by chedda_create_user and chedda_create_existing_user
//so you likely won need to use this function directly
char *__chedda_create_UID(){
		//unix timestamp
		time_t ltime;
    time(&ltime);
    //a random number
    long rnd=rand() % RAND_MAX;
    //allocate memory for string
    char *out=malloc(sizeof(char)*35);
    //put data into string as per chedda boards SDK format 
    snprintf(out,35,"dev_%li_%lx",ltime,rnd);
    return out;
}

//create a new cheddaboards user. 
//you are responsible for freeing memory created by this function
cheddaUser *chedda_create_user(char *nickname){
	cheddaUser *user=(cheddaUser*)malloc(sizeof(cheddaUser));
	user->UID=__chedda_create_UID();
	user->nickname=(char*)malloc(sizeof(char)*strlen(nickname));
	strcpy(user->nickname,nickname);
	return user;
}

//create a user from existing data - for example user data read from a savefile
//you are responsible for freeing memory created by this function
cheddaUser *chedda_create_existing_user(char *UID,char *nickname){
	cheddaUser *user=(cheddaUser*)malloc(sizeof(cheddaUser));
	user->UID=(char*)malloc(sizeof(char)*strlen(UID));
	strcpy(user->UID,UID);
	user->nickname=(char*)malloc(sizeof(char)*strlen(nickname));
	strcpy(user->nickname,nickname);
	return user;
}

//free a chedda user struct created by chedda_create_user() 
//or chedda_create_existing_user()
void chedda_user_free(cheddaUser *user){
	free(user->UID);
	free(user->nickname);
	free(user);
}




//get scores:
//	-WARNING! 	- no error checking! Make sur eyou have correctly set the
//				- chedda_API_key and chedda_game_ID variables.
//				- you will need to free returnData yourself				
//			
//	-boardID 	- the name of your scoreboard as set on the cheddaboards website.
//  -count		- the number of entries to recieve
//  -returnData - a char buffer set to NULL. If the function is successful
//				  this will contain the response from cheddaboards.
//
//  returns - an int code provided by libcurl. see: https://curl.se/libcurl/c/libcurl-errors.html
//			  you should be testing for CURLE_OK before using the data set in the returnData param
int chedda_get_scores( char *boardID, int count, char **returnData){

  CURLcode ret;
  CURL *hnd;
  struct __chedda_response chunk = {.memory = malloc(0),
                           			.size = 0};
  struct curl_slist *slist1;
 
  char apiString[128];
  strcpy(apiString,"X-API-Key: ");
  strcat(apiString,chedda_API_key);

  char cnt[4];
  snprintf(cnt,4,"%d",count);

  char urlString[1024];
  strcpy(urlString,"https://api.cheddaboards.com/games/");
  strcat(urlString,chedda_game_ID);
  strcat(urlString,"/scoreboards/");
  strcat(urlString,boardID);
  strcat(urlString,"\?sort=score&limit=");
  strcat(urlString,cnt);

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "X-API-Key: cb_sos_109755744");

  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(hnd, CURLOPT_URL, urlString);
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.18.0");
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_TLSv1_2);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, __chedda_response_cb);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA,(void*)&chunk);

  ret = curl_easy_perform(hnd);

  curl_easy_cleanup(hnd);
  hnd = NULL;
  curl_slist_free_all(slist1);
  slist1 = NULL;

  *returnData=strdup(chunk.memory);
  free(chunk.memory);

  return (int)ret;

}


//set scores:
//	-WARNING! 	- no error checking! Make sur eyou have correctly set the
//				- chedda_API_key and chedda_game_ID variables.
//				- you will need to free returnData yourself				
//			
//	-user		- a cheddaUser struct with name and UID set.
//  -score		- the score to set
//	-streak		- the streak to set
//  -returnData - a char buffer set to NULL. If the function is successful
//				  this will contain the response from cheddaboards.
//
//  returns - an int code provided by libcurl. see: https://curl.se/libcurl/c/libcurl-errors.html
//			  you should be testing for CURLE_OK before using the data set in the returnData param
int chedda_submit_score(cheddaUser *user,long score, int streak,char **returnData){
	CURLcode ret;
	CURL *hnd;
	struct __chedda_response chunk = {.memory = malloc(0),
                           			.size = 0};
	struct curl_slist *slist1;

	char apiString[128];
  	strcpy(apiString,"X-API-Key: ");
  	strcat(apiString,chedda_API_key);

  	char gameIDString[128];
  	strcpy(gameIDString,"X-Game-ID: ");
  	strcat(gameIDString,chedda_game_ID);

  	char scoreStr[25];
  	snprintf(scoreStr,25,"%ld",score);

  	char streakStr[25];
  	snprintf(streakStr,25,"%d",streak);

  	char *postString=(char*)malloc(sizeof(char)*2048);;
		strcpy(postString,"{\"playerId\": \"");
		strcat(postString,user->UID);
		strcat(postString,"\", \"gameId\": \"");
		strcat(postString,chedda_game_ID);
		strcat(postString,"\", \"nickname\": \"");
		strcat(postString,user->nickname);
		strcat(postString,"\", \"score\": ");
		strcat(postString,scoreStr);
		strcat(postString,", \"streak\": ");
		strcat(postString,streakStr);
		strcat(postString,"}");

		slist1 = NULL;
		slist1 = curl_slist_append(slist1, apiString);
		slist1 = curl_slist_append(slist1, chedda_game_ID);
		slist1 = curl_slist_append(slist1, "Content-Type: application/json");

		hnd = curl_easy_init();
		curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
		curl_easy_setopt(hnd, CURLOPT_URL, "https://api.cheddaboards.com/scores");
		curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
		//make sure to get the length of the string correct, or cheddaboards
		//complains about invalid json strings.
		curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, strlen(postString));
		curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postString);
		curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
		curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.18.0");
		curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
		curl_easy_setopt(hnd, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_TLSv1_2);
		curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
		curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, __chedda_response_cb);
		curl_easy_setopt(hnd, CURLOPT_WRITEDATA,(void*)&chunk);

		ret = curl_easy_perform(hnd);

		curl_easy_cleanup(hnd);
		hnd = NULL;
		curl_slist_free_all(slist1);
		slist1 = NULL;
		free(postString);

		*returnData=strdup(chunk.memory);
		  free(chunk.memory);

		return (int)ret;
}


//test/example program
int main(int argc, char *argv[])
{
	//seed the random number generator
	srand(time(NULL));

	//create a new userID to store locally.
	cheddaUser *user=chedda_create_user("charlie");
	printf("user %s\nUID %s\n",user->nickname,user->UID );	
	
	//or...	
	//if you have loaded the user details from a save file, use this function
	//to create the cheddaUser struct
	// char *your_loaded_uid="dev_1781519789_4d6e35f6";
	// char *your_loaded_nickname="Mr. Horse";
	// cheddaUser *user=chedda_create_existing_user(your_loaded_uid,your_loaded_nickname);
	// printf("user %s\nUID %s\n",user->nickname,user->UID );

	
	
	//boardName should match the name of the board you entered on cheddaboards
	//e.g. daily, weekly, all time etc.
	char *boardName="flippyflip";
	char *data=NULL;	
	//send a request for scores on the board boardName. 	
	int ret=chedda_get_scores(boardName,100,&data);
	if(ret==CURLE_OK){
		printf("recieved scores:\n");
		printf("%s\n",data );
		printf("make sure to parse for errors. looks like chedda boards will say \"ok:false\" if there is something wrong\n");
		printf("you can now parse the json string and add the results to your game! woohoo!\n");
	}else{
		printf("oops! there was an error recieving scores.\n");
		printf("error: %s\n",(char*)curl_easy_strerror(ret) );
		
	};	

	//set a score
	char *result=NULL;
	int set=chedda_submit_score(user,1234567,3,&result);
	if(set==CURLE_OK){
		printf("score sent!\n");
		printf("%s\n", result);
		printf("remember to check the result string in case there is an error returned by cheddaboards\n");
	}else{
		printf("oops! there was an error sending the score :(\n");
		printf("error: %s\n",(char*)curl_easy_strerror(set) );

	}

	//free memory assigned to the cheddaUser
	chedda_user_free(user);
	//free memory assigned by set/get score funtions
	if(data!=NULL)
		free(data);

	if(result!=NULL)
		free(result);

	return 0;

}
/**** End of sample code ****/
