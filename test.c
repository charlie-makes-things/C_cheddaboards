
#include "cheddaboards.h"

int main(int argc, char const *argv[])
{
	/* code */


	printf("cheddaboard library test\n");

	printf("initialise library...\n");

	char *api_key="your_api_key_here";
	char *game_id="your_game_id_here";

	chedda_init(api_key,game_id);

	printf("create user...\n");	
	void *user=chedda_create_user("Ronald");
	if(user!=NULL){
		printf("user created\n");
	}else{
		printf("user not created. error\n");
		return 1;
	}

	printf("creating existing user\n");
	// char *your_loaded_uid="dev_1781519789_4d6e35f6";
	// char *your_loaded_nickname="Mr. Horse";
	// void *existing_user=chedda_create_existing_user(your_loaded_uid,your_loaded_nickname);
	// if(existing_user!=NULL){
	// 	printf("existing_user created\n");
	// }else{
	// 	printf("existing_user not created. error\n");
	// 	return 1;
	// }

	char *boardName="your-board-id-here";
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
	if(data!=NULL)
		free(data);

	//set a score
	data=NULL;
	int set=chedda_submit_score_global(user,1234567,3,&data);
	if(set==CURLE_OK){
		printf("score sent!\n");
		printf("%s\n", data);
		printf("remember to check the result string in case there is an error returned by cheddaboards\n");
	}else{
		printf("oops! there was an error sending the score :(\n");
		printf("error: %s\n",(char*)curl_easy_strerror(set) );

	}
	if(data!=NULL)
		free(data);

	char *targetBoard="your-board-id-here";
	data=NULL;
	int settrgt=chedda_submit_score_targeted(user,targetBoard,1245,4,&data);
	if(settrgt==CURLE_OK){
		printf("targeted score sent!\n");
		printf("%s\n", data);
		printf("remember to check the result string in case there is an error returned by cheddaboards\n");
	}else{
		printf("oops! there was an error sending the targeted score :(\n");
		printf("error: %s\n",(char*)curl_easy_strerror(settrgt) );

	}
	if(data!=NULL)
		free(data);


	printf("free user\n");
	if(user!=NULL)
		chedda_user_free(user);
	printf("free library\n");
	chedda_free();


	return 0;
}