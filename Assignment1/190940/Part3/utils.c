#include "wc.h"


extern struct team teams[NUM_TEAMS];
extern int test;
extern int finalTeam1;
extern int finalTeam2;

int processType = HOST;
const char *team_names[] = {
  "India", "Australia", "New Zealand", "Sri Lanka",   // Group A
  "Pakistan", "South Africa", "England", "Bangladesh" // Group B
};


void teamPlay(void)
{
  char* input_file_path=(char*)malloc(30);
  sprintf(input_file_path,"./test/%d/inp/%s",test,team_names[processType]);
  int fd=open(input_file_path,O_RDONLY);
  if(fd<0){
    printf("fd<0\n");
    exit(-1);
  }
  char digit;
  char msg;
  while(1){
    //printf("Hey while loop of teamPlay\n");
    if(read(fd,&digit,1)<0){
      printf("Unable to read\n");
      exit(-1);
    }
    if(write(teams[processType].matchpipe[1],&digit,1)<0){
      printf("Unable to write\n");
      exit(-1);
    }
    if(read(teams[processType].commpipe[0],&msg,1)<0){
      printf("Unable to read from commpipe in teamPlay\n");
      exit(0);
    }
    if(msg=='c'){
      continue;
    }
    else{
      //printf("I am dying\n");
      exit(-1);
    }
  }
}

void endTeam(int teamID)
{
  if(write(teams[teamID].commpipe[1],"t",1)<0){
    printf("Unable to write in endTeam");
    exit(-1);
  }
}

int match(int team1, int team2)
{
  // Get the output directory
  char* input_file_path=(char*)malloc(30);
  sprintf(input_file_path,"./test/%d/inp",test);
  char* output_file_path=(char*)malloc(100);
  sprintf(output_file_path,"./test/%d/out/",test);

  //toss decide
  //printf("match: line\n");
  char toss1,toss2;
  int bat_team;
  int bowl_team;
  if(write(teams[team1].commpipe[1],"c",1)<0){
    printf("hey\n");
    exit(0);
  }
  if(write(teams[team2].commpipe[1],"c",1)<0){
    printf("hey\n");
    exit(0);
  }
  if(read(teams[team1].matchpipe[0],&toss1,1)<0){
    printf("hey\n");
    exit(0);
  }
  if(read(teams[team2].matchpipe[0],&toss2,1)<0){
    printf("hey\n");
    exit(0);
  }

  if((toss1-toss2)%2==0){
    bat_team=team2;
    bowl_team=team1;
  }
  else {
    bat_team=team1;
    bowl_team=team2;
  }

  // Get the file name and open it
  int fd;
  if((team1<4 && team2<4)||(team1>=4 && team2>=4)){
    if(bat_team==team1){
      strcat(output_file_path,team_names[team1]);
      strcat(output_file_path,"v");
      strcat(output_file_path,team_names[team2]);
    }
    else{
      strcat(output_file_path,team_names[team2]);
      strcat(output_file_path,"v");
      strcat(output_file_path,team_names[team1]);
    }
  }
  else{
    if(bat_team==team1){
      strcat(output_file_path,team_names[team1]);
      strcat(output_file_path,"v");
      strcat(output_file_path,team_names[team2]);
    }
    else{
      strcat(output_file_path,team_names[team2]);
      strcat(output_file_path,"v");
      strcat(output_file_path,team_names[team1]);
    }
    strcat(output_file_path,"-Final");
  }
  //printf("%s\n",output_file_path);
  fd=open(output_file_path,O_RDWR|O_CREAT,0644);
  if(fd<0){
    printf("match: fd<0\n");
    exit(-1);
  }

  // Print the 1st line
  char* line1=(char*)malloc(30);
  sprintf(line1,"Innings1: %s bats\n",team_names[bat_team]);
  write(fd,line1,strlen(line1));

  //print the 1st innings individual score and final score
  int score1=0;
  int wickets=0;
  int presbatscore=0;
  char digbat,digbowl;
  int digbat1,digbowl1;
  char* line=(char*)malloc(50);
  for(int i=1;i<=120;i++){
    if(wickets==10)break;
    if(write(teams[team1].commpipe[1],"c",1)<0){
      printf("Unable to write to commpipe\n");
      exit(-1);
    }
    if(write(teams[team2].commpipe[1],"c",1)<0){
      printf("Unable to write to commpipe\n");
      exit(-1);
    }
    if(read(teams[bat_team].matchpipe[0],&digbat,1)<0){
      printf("Unable to read from matchpipe\n");
      exit(-1);
    }
    if(read(teams[bowl_team].matchpipe[0],&digbowl,1)<0){
      printf("Unable to read from matchpipe\n");
      exit(-1);
    }
    digbowl1=digbowl-'0';
    digbat1=digbat-'0';
    if(digbat1==digbowl1){
      wickets++;
      sprintf(line,"%d:%d\n",wickets,presbatscore);
      write(fd,line,strlen(line));
      presbatscore=0;
    }
    else {
      presbatscore+=digbat1;
      score1+=digbat1;
    }
    //sleep(0.5);
  }
  if(wickets!=10){
    sprintf(line,"%d:%d*\n",wickets+1,presbatscore);
    write(fd,line,strlen(line));
  }
  sprintf(line,"%s Total: %d\n",team_names[bat_team],score1);
  write(fd,line,strlen(line));

  // Innings 2
  sprintf(line1,"\nInnings2: %s bats\n",team_names[bowl_team]);
  write(fd,line1,strlen(line1));

  //Innings 2 running
  int score2=0;
  wickets=0;
  presbatscore=0;
  int win=0;// 2 for tie
  for(int i=1;i<=120;i++){
    if(score2>score1){
      win=1;
      break;
    }
    if(wickets==10)break;
    write(teams[team1].commpipe[1],"c",1);
    write(teams[team2].commpipe[1],"c",1);
    read(teams[bowl_team].matchpipe[0],&digbat,1);
    read(teams[bat_team].matchpipe[0],&digbowl,1);
    digbowl1=digbowl-'0';
    digbat1=digbat-'0';
    if(digbat1==digbowl1){
      wickets++;
      sprintf(line,"%d:%d\n",wickets,presbatscore);
      write(fd,line,strlen(line));
      presbatscore=0;
    }
    else {
      presbatscore+=digbat1;
      score2+=digbat1;
    }
  }
  if(wickets!=10){
    sprintf(line,"%d:%d*\n",wickets+1,presbatscore);
    write(fd,line,strlen(line));
  }
  if(score1==score2)win=2;
  sprintf(line,"%s Total: %d\n",team_names[bowl_team],score2);
  write(fd,line,strlen(line));

  // Comparisons
  if(win==2){
    sprintf(line,"TIE: %s beats %s",team_names[team1],team_names[team2]);
    write(fd,line,strlen(line));
    return team1;
  }
  else if(win==1){
    sprintf(line,"%s beats %s by %d wickets",team_names[bowl_team],team_names[bat_team],10-wickets);
    write(fd,line,strlen(line));
    return bowl_team;
  }
  else{
    sprintf(line,"%s beats %s by %d runs",team_names[bat_team],team_names[bowl_team],score1-score2);
    write(fd,line,strlen(line));
    return bat_team;
  }
}

void spawnTeams(void)
{
  for(int i=0;i<NUM_TEAMS;i++){
    strcpy(teams[i].name,team_names[i]);
    if(pipe(teams[i].matchpipe)<0){
      printf("ERROR IN PIPING\n");
      exit(-1);
    }
    if(pipe(teams[i].commpipe)<0){
      printf("ERROR IN PIPING\n");
      exit(-1);
    }
    int pid=fork();
    if(!pid){
      //close(teams[i].matchpipe[0]);
      //close(teams[i].commpipe[1]);
      processType=i;
      //printf("1\n");
      teamPlay();
      exit(-1);
    }
    else{
      //close(teams[i].matchpipe[0]);
      //close(teams[i].commpipe[1]);
    }
  }
	
}

void conductGroupMatches(void)
{
  int fd[2];
  if(pipe(fd)<0){
      printf("ERROR IN PIPING\n");
      exit(-1);
  }
  pid_t pid=fork();
  if(!pid){
    // Group process
    //close(fd[0]);
    int wins[NUM_TEAMS/2]={0};
    int w;
    w=match(0,1);
    wins[w]++;
    w=match(0,2);
    wins[w]++;
    w=match(0,3);
    wins[w]++;
    w=match(1,2);
    wins[w]++;
    w=match(1,3);
    wins[w]++;
    w=match(2,3);
    wins[w]++;
    int maxwins=wins[0];
    int ind=0;
    for(int i=1;i<NUM_TEAMS/2;i++){
      if(maxwins<wins[i]){
        maxwins=wins[i];
        ind=i;
      }
      //printf("%d %d\n",i,wins[i]);
    }
    char index='0'+ind;
    write(fd[1],&index,1);
    for(int i=0;i<4;i++){
      if(ind!=(i)){
        //printf("%d is killed\n",i);
        endTeam(i);
      }
    }
    exit(0);
  }

  // Group 2
  int fd1[2];
  if(pipe(fd1)<0){
      printf("ERROR IN PIPING\n");
      exit(-1);
  }
  int pid1=fork();
  if(!pid1){
    // Group process
    close(fd1[0]);
    int wins[NUM_TEAMS/2]={0};
    int w;
    w=match(4,5);
    wins[w-4]++;
    w=match(4,6);
    wins[w-4]++;
    w=match(4,7);
    wins[w-4]++;
    w=match(5,6);
    wins[w-4]++;
    w=match(5,7);
    wins[w-4]++;
    w=match(6,7);
    wins[w-4]++;
    int maxwins=wins[0];
    int ind=0;
    for(int i=1;i<4;i++){
      if(maxwins<wins[i]){
        maxwins=wins[i];
        ind=i;
      }
    }
    ind+=4;
    char index=ind+'0';
    write(fd1[1],&index,1);
    for(int i=0;i<4;i++){
      if(ind!=(i+4)){
        endTeam(i+4);
      }
    }
    exit(0);
  }
  //close(fd1[1]);
  int stat;
  waitpid(pid,&stat,0);
  char b;
  read(fd[0],&b,1);
  //printf("%c\n",b);
  finalTeam1=b-'0';
  //close(fd[1]);
  waitpid(pid1,&stat,0);
  read(fd1[0],&b,1);
  finalTeam2=b-'0';
  //printf("Yo conductGroups is over yo and finals are %d %d\n",finalTeam1,finalTeam2);
}
