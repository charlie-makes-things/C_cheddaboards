

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

char *g_chedda_API_key=NULL;
char *g_chedda_game_ID=NULL;

extern int chedda_init(char *api_key,char* game_id);
extern int chedda_free();

extern void *chedda_create_user(char *nickname);
extern void *chedda_create_existing_user(char *UID,char *nickname);
extern void chedda_user_free(void *user);

extern int chedda_get_scores( char *boardID, int count, char **returnData);
extern int chedda_submit_score_global(void *user,long score, int streak,char **returnData);
extern int chedda_submit_score_targeted(void *user,char *boardID,long score, int streak,char **returnData);

extern int chedda_init(char *api_key,char *game_id){
	
	int apilen=strlen(api_key);
	int gidlen=strlen(game_id);
	
	g_chedda_game_ID=NULL;
	g_chedda_API_key=NULL;
	
	g_chedda_game_ID=(char*)malloc(sizeof(char)*gidlen);
	g_chedda_API_key=(char*)malloc(sizeof(char)*apilen);
	
	strcpy(g_chedda_game_ID,game_id);
	strcpy(g_chedda_API_key,api_key);
	
	return 0;
}

extern int chedda_free(){
	if(g_chedda_game_ID!=NULL){
		free(g_chedda_game_ID);
	}
	if(g_chedda_API_key!=NULL){
		free(g_chedda_API_key);
	}
	return 0;
}


//user id struct
typedef struct cheddaUser{
	char *UID;
	char *nickname;
}cheddaUser;

//temp struct to contain data from chedda boards. used internally
struct __chedda_response {
  char *memory;
  size_t size;
};

//callback function to write recieved data. used internally
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

//generate a new user id. you are responsible for freeing this memory.
//this function is used by chedda_create_user and chedda_create_existing_user
//so you likely won't need to use this function directly
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
void *chedda_create_user(char *nickname){
	cheddaUser *user=(cheddaUser*)malloc(sizeof(cheddaUser));
	user->UID=__chedda_create_UID();
	user->nickname=(char*)malloc(sizeof(char)*strlen(nickname));
	strcpy(user->nickname,nickname);
	return (void*)user;
}

//create a user from existing data - for example user data read from a savefile
//you are responsible for freeing memory created by this function
void *chedda_create_existing_user(char *UID,char *nickname){
	cheddaUser *user=(cheddaUser*)malloc(sizeof(cheddaUser));
	user->UID=(char*)malloc(sizeof(char)*strlen(UID));
	strcpy(user->UID,UID);
	user->nickname=(char*)malloc(sizeof(char)*strlen(nickname));
	strcpy(user->nickname,nickname);
	return (void*)user;
}

//free a chedda user struct created by chedda_create_user() 
//or chedda_create_existing_user()
void chedda_user_free(void *user){
	struct cheddaUser *use=(struct cheddaUser*)user;
	free(use->UID);
	free(use->nickname);
	free(use);
}


extern char *chedda_get_user_id(void *user){
	cheddaUser *usr=(cheddaUser*)user;
	return usr->UID;
}

extern char *chedda_get_user_nickname(void *user){
	cheddaUser *usr=(cheddaUser*)user;
	return usr->nickname;
}

//get scores:
//	-WARNING! 	- no error checking! Make sure you have correctly set the
//							- g_chedda_API_key and g_chedda_game_ID variables.
//							- you will need to free returnData yourself				
//			
//	-boardID 		- the name of your scoreboard as set on the cheddaboards website.
//  -count			- the number of entries to recieve
//  -returnData - a char buffer set to NULL. If the function is successful
//				  			this will contain the response from cheddaboards.
//
//  returns 		- an int code provided by libcurl. see: https://curl.se/libcurl/c/libcurl-errors.html
//			  				you should be testing for CURLE_OK before using the data set in the returnData param
int chedda_get_scores( char *boardID, int count, char **returnData){

  CURLcode ret;
  CURL *hnd;
  struct __chedda_response chunk = {.memory = malloc(0),
                           			.size = 0};
  struct curl_slist *slist1;
 
  char apiString[128];
  strcpy(apiString,"X-API-Key: ");
  strcat(apiString,g_chedda_API_key);

  char cnt[4];
  snprintf(cnt,4,"%d",count);

  char urlString[1024];
  strcpy(urlString,"https://api.cheddaboards.com/games/");
  strcat(urlString,g_chedda_game_ID);
  strcat(urlString,"/scoreboards/");
  strcat(urlString,boardID);
  strcat(urlString,"\?sort=score&limit=");
  strcat(urlString,cnt);

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, apiString);

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
//	-WARNING! 	- no error checking! Make sure you have correctly set the
//							- g_chedda_API_key and g_chedda_game_ID variables.
//							- you will need to free returnData yourself				
//			
//	-user				- a cheddaUser struct with name and UID set.
//  -score			- the score to set
//	-streak			- the streak to set
//  -returnData - a char buffer set to NULL. If the function is successful
//				  			this will contain the response from cheddaboards.
//
//  returns 		- an int code provided by libcurl. see: https://curl.se/libcurl/c/libcurl-errors.html
//			  				you should be testing for CURLE_OK before using the data set in the returnData param
int chedda_submit_score_global(void *user,long score, int streak,char **returnData){
	
	struct cheddaUser *usr=(struct cheddaUser*)user;

	CURLcode ret;
	CURL *hnd;
	struct __chedda_response chunk = {.memory = malloc(0),
                           			.size = 0};
	struct curl_slist *slist1;

	char apiString[128];
  	strcpy(apiString,"X-API-Key: ");
  	strcat(apiString,g_chedda_API_key);

  	char gameIDString[128];
  	strcpy(gameIDString,"X-Game-ID: ");
  	strcat(gameIDString,g_chedda_game_ID);

  	char scoreStr[25];
  	snprintf(scoreStr,25,"%ld",score);

  	char streakStr[25];
  	snprintf(streakStr,25,"%d",streak);

  	char *postString=(char*)malloc(sizeof(char)*2048);;
		strcpy(postString,"{\"playerId\": \"");
		strcat(postString,usr->UID);
		strcat(postString,"\", \"gameId\": \"");
		strcat(postString,g_chedda_game_ID);
		strcat(postString,"\", \"nickname\": \"");
		strcat(postString,usr->nickname);
		strcat(postString,"\", \"score\": ");
		strcat(postString,scoreStr);
		strcat(postString,", \"streak\": ");
		strcat(postString,streakStr);
		strcat(postString,"}");

		slist1 = NULL;
		slist1 = curl_slist_append(slist1, apiString);
		slist1 = curl_slist_append(slist1, g_chedda_game_ID);
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

//set scores on specific board:
//	-WARNING! 	- no error checking! Make sure you have correctly set the
//							- g_chedda_API_key and g_chedda_game_ID variables.
//							- you will need to free returnData yourself				
//	-boardID			- the boardid set in cheddaboards
//	-user				- a cheddaUser struct with name and UID set.
//  -score			- the score to set
//	-streak			- the streak to set
//  -returnData - a char buffer set to NULL. If the function is successful
//				  			this will contain the response from cheddaboards.
//
//  returns 		- an int code provided by libcurl. see: https://curl.se/libcurl/c/libcurl-errors.html
//			  				you should be testing for CURLE_OK before using the data set in the returnData param
int chedda_submit_score_targeted(void *user,char *boardID,long score, int streak,char **returnData){
	
	struct cheddaUser *usr=(struct cheddaUser*)user;

	CURLcode ret;
	CURL *hnd;
	struct __chedda_response chunk = {.memory = malloc(0),
                           			.size = 0};
	struct curl_slist *slist1;

	char apiString[128];
  	strcpy(apiString,"X-API-Key: ");
  	strcat(apiString,g_chedda_API_key);

  	char gameIDString[128];
  	strcpy(gameIDString,"X-Game-ID: ");
  	strcat(gameIDString,g_chedda_game_ID);

  	char scoreStr[25];
  	snprintf(scoreStr,25,"%ld",score);

  	char streakStr[25];
  	snprintf(streakStr,25,"%d",streak);

  	char *postString=(char*)malloc(sizeof(char)*2048);;
		strcpy(postString,"{\"playerId\": \"");
		strcat(postString,usr->UID);
		strcat(postString,"\", \"scoreboardId\": \"");
		strcat(postString,boardID);
		strcat(postString,"\", \"gameId\": \"");
		strcat(postString,g_chedda_game_ID);
		strcat(postString,"\", \"nickname\": \"");
		strcat(postString,usr->nickname);
		strcat(postString,"\", \"score\": ");
		strcat(postString,scoreStr);
		strcat(postString,", \"streak\": ");
		strcat(postString,streakStr);
		strcat(postString,"}");

		//printf("%s\n",postString );

		slist1 = NULL;
		slist1 = curl_slist_append(slist1, apiString);
		slist1 = curl_slist_append(slist1, g_chedda_game_ID);
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
