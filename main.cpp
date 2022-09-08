#include<stdio.h>
#include<bits/stdc++.h>
#include<sys/wait.h>
#include<pwd.h>
#include<grp.h>
#include<limits.h>
#include<dirent.h>
#include<signal.h> 
#include<stdlib.h>
#include<iostream>
#include<string.h>
#include<sys/ioctl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<vector>
#include<termios.h>
#include<stack>
#include<algorithm>
#include<ctype.h> 
#include<errno.h>
#include<time.h>
#include<fcntl.h>
#include<fstream>
using namespace std;

void COMMAND_MODE();
void ENABLE_SCROLLING();
void enableRawMode();
void disableRawMode();
double GetFileSize(const char*);
void PRINT_ALL_DETAILS(const char*);
void PRINT_ALL(vector<pair<string,bool>>,int,int);
void NORMAL_MODE(const char*);
int start1,endlast;
int row,column;
int x,y;
int flag=1;
int size_allfileanddir=0;
bool tflag=true;
bool normal_flag=true;
struct termios orig_termios;
struct winsize windowsize;
string currentdirectory();
string permissions(const char *);
string gethomepath();
void gotoxy(int x,int y);
string real_resolvepath(string);
string resolvepath(string);
string global_for_search_path="";
stack<string> leftpath;
stack<string> rightpath;
vector<pair<string,bool>> allfileanddir;
string maindirectory=currentdirectory();


string gethomepath(){								//get home path of user.
	string HOMEEEE;									//eg. /home/piyush_singh.
	uid_t userid=getuid();
	struct passwd *pw=getpwuid(userid);				//getpwuid-search user database for a user ID.
	HOMEEEE=pw->pw_dir;								//pw_dir-User's login directory.
	return HOMEEEE;

}


string currentdirectory(){
	char tmp[512];									//get current working directory
	getcwd(tmp,512);								//the path of directory in which we (initially,the code) is present.
	string maindirectory=tmp;
	return maindirectory;

}


void disableRawMode(){								//noncanonical to canonical
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

}


void gotoxy(int x,int y){
	printf("%c[%d;%dH",27,x,y);						//move cursor to (x,y).

}


void enableRawMode(){								//canonical to non anonical 
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(disableRawMode);
	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

}


string real_resolvepath(string handle){					//used to get absolute path 
	char * fullpath;									
	fullpath=realpath(handle.c_str(),NULL);
	if(fullpath==NULL) {
		cout<<"INVALID PATH !!"<<endl;
		return "0";
	}
	else{
		string strpath(fullpath);
		return strpath;
	}

}


string resolvepath(string path){						//relative path to absoute path
	string temptildehandle=path;
	if(temptildehandle[0]=='~'){			//w.r.t home/user
		string tilde=gethomepath();
		if(path.size()==1){
			return tilde;
		}
		else{
			temptildehandle.erase(0,1);
			temptildehandle=tilde+temptildehandle;
			string checkpath=real_resolvepath(temptildehandle);
			if(checkpath=="0"){
				return "0";
			}
			else{
				return checkpath;
			}

		}

	}
	else if(temptildehandle[0]=='.'){			//considers both cases . and ..
		string temppathwithdot=maindirectory+"/"+temptildehandle;
		string absolutepathwithoutdot=real_resolvepath(temppathwithdot);
		if(absolutepathwithoutdot=="0"){
			return "0";
		}
		else{
			return absolutepathwithoutdot;
		}

	}
	else if(temptildehandle[0]=='/'){			//user gives absolute path.
		return temptildehandle;
	}
	else{
		return (maindirectory+"/"+temptildehandle);		//path in current directory.
	}

}


double GetFileSize(const char* file){
	struct stat buff;							//get file size
	int rc=stat(file,&buff);					//The stat() system call actually returns file attributes,
	if(rc==0){									//type of file,modified date,path,ect..
		return buff.st_size;
	}
	else{
		return -1;
	}

}


string permissions(const char *file){
	struct stat st;
	string permval="";							//get permissions
	if(stat(file,&st) == 0){
        // mode_t perm=st.st_mode;
		permval+=(st.st_mode & S_ISDIR(st.st_mode))?'d':'-';
		permval+=(st.st_mode & S_IWUSR)?'w':'-';
		permval+=(st.st_mode & S_IXUSR)?'x':'-';
		permval+=(st.st_mode & S_IRGRP)?'r':'-';
		permval+=(st.st_mode & S_IWGRP)?'w':'-';
		permval+=(st.st_mode & S_IXGRP)?'x':'-';
		permval+=(st.st_mode & S_IROTH)?'r':'-';
		permval+=(st.st_mode & S_IWOTH)?'w':'-';
		permval+=(st.st_mode & S_IXOTH)?'x':'-';
        // modeval+='\0';
		return permval;     
	}
	else{
		return "0";
	}   

}


void ENABLE_SCROLLING(){
	gotoxy(1,1);
	x=1,y=1;
	int tracker=0;
	while(flag==1){
		char c;
		c=getchar();
		if(c==static_cast<char>(27)){
			c=getchar();
			if(c==static_cast<char>(91)){
				c=getchar();
				if(c==static_cast<char>(65)){					//up
					if(x>tracker && x>1){		
						x--;
						tracker--;
						gotoxy(x,y);
					}
					
				}

				else if(c==static_cast<char>(66)){				//DOWN
					if(x<endlast){		
						x++;
						tracker++;		
						gotoxy(x,y);
					}	
				}

				else if(c==static_cast<char>(67)){  			//right
					if(rightpath.empty()!=1){
						leftpath.push(maindirectory);
						maindirectory=rightpath.top();
						rightpath.pop();
						NORMAL_MODE(maindirectory.c_str());
					}	
				}

				else if(c==static_cast<char>(68)){				//left			
					if(leftpath.empty()!=1){
						rightpath.push(maindirectory);
						maindirectory=leftpath.top();
						leftpath.pop();
						NORMAL_MODE(maindirectory.c_str());
					}
				}
			}
		}


		else if(c==static_cast<char>(127)){	
			string temppath3=maindirectory;			//backspace
			if(temppath3!="/home"){
				leftpath.push(maindirectory);		//usefull when we press left arrow key.
				int i=temppath3.size()-1;
				while(temppath3[i]!='/'){			
					i--;
				}
				string ans="";
				for(int j=0;j<i;j++){
					ans+=temppath3[j];
				}
				maindirectory=ans;
				while(!rightpath.empty()){			//as when backspace is pressed all paths leading to right key press should be deleted.
					rightpath.pop();
				}
				NORMAL_MODE(maindirectory.c_str());		//start normal mode.
			}
		}


		else if(c==static_cast<char>(10)){		//ENTER
			if(allfileanddir[tracker].second==0){			
				//file
				string tmppathy=maindirectory+'/'+allfileanddir[tracker].first;
				if(!fork()){
					execlp("xdg-open","xdg-open",tmppathy.c_str(),(char*)0);
					exit(0);
				}
				wait(NULL);
			}

			else if(allfileanddir[tracker].second==1){		
				//directory
				maindirectory=maindirectory+'/'+allfileanddir[tracker].first;
				leftpath.push(maindirectory);
				NORMAL_MODE(leftpath.top().c_str());
			}
		}


		else if(c=='h' || c=='H'){				//home key pressed.
			leftpath.push(maindirectory);				//usefull when left key is pressed
			maindirectory=gethomepath();
			while(!rightpath.empty()){
				rightpath.pop();
			}	
			NORMAL_MODE(maindirectory.c_str());
		}


		else if(c==':'){
			normal_flag=0;			//switch to command mode.
			flag=0;
			COMMAND_MODE();
		}


		else if(c=='q'){
			flag=0;
			tflag=0;
			normal_flag=0;
			printf("\033[H\033[2J\033[3J");		//clear
			printf("\033[0;0f");				//cursot_to_00

			exit(0);
		}
	}

}


void PRINT_ALL_DETAILS(const char* veci,string nameofi){
	char c='\0';
	struct stat ids;
	stat(veci,&ids); 
	struct passwd *pw=getpwuid(ids.st_uid);		//getting username.
	struct group  *gr=getgrgid(ids.st_gid);		//getting groupname.
	double size=(GetFileSize(veci)/1024);
	string permq=permissions(veci);
	if(permq=="0"){
		cout<<"ERROR WHILE READING PERMISSIONS ."<<"\n";
		return;
	}
	printf("%s\t  %s\t  %s\t  %10.3fkB\t",permq.c_str(),pw->pw_name,gr->gr_name,size); 
	printf("  %-60s\t",nameofi.c_str());
	printf("%s",ctime(&ids.st_mtime));

}


void PRINT_ALL(vector<pair<string,bool>> name,int start,int end){
	printf("\033[H\033[2J\033[3J");		//clear
	printf("\033[0;0f");				//cursot_to_00
	while(start<end){

		string temppath2=maindirectory+'/'+name[start].first;

		PRINT_ALL_DETAILS(temppath2.c_str(),name[start].first);

		start++;
	}

	ioctl(STDIN_FILENO, TIOCGWINSZ, &windowsize);		//getting window size.
	row=windowsize.ws_row-6;
	column=windowsize.ws_col+5;
	gotoxy(row,column);
	cout<<"---------------------------------- STATUS_BAR --------------------------------- "<<endl;
	cout<<"CURRENT_PATH --> "<<maindirectory<<endl;
	
}


void GET_ALL_INFO(const char* rasta){
	struct dirent *d;									//dirent:is used to read each file in directory opened by DIR.
	DIR *dh=opendir(rasta);
	if(dh==NULL){
		cout<<"NOT ABLE TO OPEN DIRECTORY."<<endl;		//DIR is used to open directory.
		return;
	}
	
	d=readdir(dh);
	while(d != NULL){
		if(d->d_type == DT_DIR){						//directory
			allfileanddir.push_back({d->d_name,1});
		}
		else if(d->d_type == DT_REG){					//file
			allfileanddir.push_back({d->d_name,0});
		}
		else{
			continue;
		}
		d=readdir(dh);

	}
	closedir(dh);

}



void NORMAL_MODE(const char* rasta){
	
	allfileanddir.clear();
	
	GET_ALL_INFO(rasta);
	
	sort(allfileanddir.begin(),allfileanddir.end()); 

	size_allfileanddir=allfileanddir.size();
	
	start1=0;

	if(size_allfileanddir<40){						//if allfileanddir_size is greater then particular size then show upto certain limit(here 46). 
		endlast=size_allfileanddir;
		PRINT_ALL(allfileanddir,0,size_allfileanddir);
		
	}
	else{
		endlast=40;
		PRINT_ALL(allfileanddir,0,endlast);
	}

	if(normal_flag){							//if normal_flag is enabled then only start scrolling ,we will be calling normal_mode from command mode,
		ENABLE_SCROLLING();						//then normal_flag would be inset and then we will only print all details of directory and not enable scrolling,
	}											//KIND OF REFRESH SCREEN TO DISPLAY THE EFFECTS OF COMMAND MODE commands.

}


/////////////////////////////////////////////////////////////////////////////////COMMAND_MODE///////////////////////////////////////////////////////////////////////////


void createfile(string pathfile){
	int fd=open(pathfile.c_str(),O_RDWR|O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO);

}


void createfolder(string pathfolder){
	const char *temppathfolder=pathfolder.c_str();
	mkdir(temppathfolder,0777);

}


void copyfile(const char *SRC, const char* DEST){
	ifstream src(SRC, ios::binary);
	ofstream dest(DEST, ios::binary);
	dest << src.rdbuf();

}


void first_copyfile(string custompath,string s1,string s2){			//made this first_copyfile for generic use in move and other commands
	string absolutepath1=custompath+"/"+s1;
	createfile(absolutepath1);
	string absolutepath2=resolvepath(s2);
	if(absolutepath2=="0"){
		return;
	}
	else{
		absolutepath2=absolutepath2+"/"+s1;
		copyfile(absolutepath1.c_str(),absolutepath2.c_str());
		struct stat st2;
		stat(absolutepath2.c_str(),&st2);							//changing file permission to their original state.
		chmod(absolutepath2.c_str(),st2.st_mode);
	}

}


void copyfolder(string curpath,string name,string cfpath){
	string cfpathupdated=cfpath+"/"+name;
	createfolder(cfpathupdated);
	string curpathupdated=curpath+"/"+name;
	const char* temp=curpathupdated.c_str();
	int iserror=chdir(temp);
	if(iserror==-1){			//error occured			
		cout<<"NOT ABLE TO OPEN FILE IN copyfolder"<<"\n";
		return;
	}
	char tmp2[512];
	getcwd(tmp2,512);

	struct dirent *entity;										 

	DIR *dh2=opendir(curpathupdated.c_str());
	if(dh2==NULL){
		cout<<"NOT ABLE TO OPEN FILE IN copyfolder/DIR"<<endl;
		return;
	}

	mode_t filemode;

	struct stat searchstat;										//stat:This function return information about a file, in the buffer pointed to by searchstat.
																//Eg.file-name,file-mode,file-type,user-id,group-id,etc...
	entity=readdir(dh2);

	while(entity!=NULL){
		if(entity->d_name[0]=='.'){
			entity=readdir(dh2);
		}
		else{
			stat(entity->d_name,&searchstat);

			if(entity->d_type==DT_DIR && searchstat.st_mode){
				string searchindirectory=curpathupdated;
				copyfolder(searchindirectory,entity->d_name,cfpathupdated);
			}
			else{
				createfile(cfpathupdated+"/"+entity->d_name);
				first_copyfile(curpathupdated.c_str(),entity->d_name,cfpathupdated.c_str());

			}
			entity=readdir(dh2);
		}
	}
	return;

}


bool search(string keyword,const char* path){
	int iserror=chdir(path);

	if(iserror==-1){			//error occured
		cout<<"NOT ABLE TO OPEN FILE IN search"<<"\n";
		return 0;
	}
	char tmp2[512];
	getcwd(tmp2,512);

	struct dirent *entity;

	DIR *dh2=opendir(path);
	if(dh2==NULL){
		cout<<"NOT ABLE TO OPEN FILE IN search/DIR"<<endl;
		return 0;
	}

	mode_t filemode;

	struct stat searchstat;
	
	entity=readdir(dh2);

	while(entity!=NULL){
		if(entity->d_name[0]=='.'){
			entity=readdir(dh2);
		}
		else if(entity->d_name==keyword){
			global_for_search_path=path;
			return 1;
		}
		else{
			stat(entity->d_name,&searchstat);

			if(entity->d_type==DT_DIR && searchstat.st_mode){
				string searchindirectory=tmp2;
				searchindirectory.append("/");
				searchindirectory.append(entity->d_name);
				if(search(keyword,searchindirectory.c_str())){
					return 1;
				}

			}
			entity=readdir(dh2);
		}
	}
	return 0;

}


void deletefile(const char* str1){
	remove(str1);

}


void deletefolder(string dpath){
	int iserror=chdir(dpath.c_str());
	if(iserror==-1){			//error occured			
		cout<<"NOT ABLE TO OPEN FILE IN deletefolder"<<"\n";
		return;
	}
	char tmp2[512];
	getcwd(tmp2,512);
	struct dirent *entity;

	DIR *dh2=opendir(dpath.c_str());
	if(dh2==NULL){
		cout<<"NOT ABLE TO OPEN FILE IN deletefolder in DIR"<<endl;
		return;
	}
	mode_t filemode;

	struct stat searchstat;
	
	entity=readdir(dh2);

	while(entity!=NULL){
		if(entity->d_name[0]=='.'){
			entity=readdir(dh2);
			continue;
		}
		else{
			stat(entity->d_name,&searchstat);

			if(entity->d_type==DT_DIR){
				string temppath=dpath+"/"+entity->d_name;
				deletefolder(temppath);
			}
			else{
				string temppath2=dpath+"/"+entity->d_name;
				deletefile(temppath2.c_str());
			}
			entity=readdir(dh2);
		}
	}
	closedir(dh2);
	rmdir(dpath.c_str());
	return;

}



void movefile(string curpath,string name,string mfpath){
	first_copyfile(curpath,name,mfpath);
	string tempmfpath=mfpath+"/"+name;
	deletefile((curpath+"/"+name).c_str());

}


void movefolder(string src,string name,string dst){
	copyfolder(src,name,dst);
	string tempmovefolder=src+"/"+name;
	deletefolder(tempmovefolder);

}


void COMMAND_MODE(){

	gotoxy(windowsize.ws_row-2,windowsize.ws_col+2);
	cout<<"<<<<<<<<<<-----------------COMMAND_MODE----------------->>>>>>>>>>"<<"\n";
	while(1){
		char c;
		string commands;
		string partcommand;
		vector<string> fullcommands;
		while((c=getchar())!=27 && c!=10){		//27-ESC,10-Enter,127-backspace
			if(c==127){
				printf("\33[2K\r");		//clear line
				int commands_len = commands.length();
				commands= commands.substr(0,commands_len-1);
				cout<<commands;
				continue;

			}
			commands +=c;
			cout<<c;

		}
		printf("\n");
		
		if(c==10){										//Enter
			for(int i=0;i<commands.length();i++){
				if(commands[i]==' '){
					fullcommands.push_back(partcommand);
					partcommand.clear();
				}
				else{
					partcommand+=commands[i];
				}
			}

		}

		fullcommands.push_back(partcommand);

		if(fullcommands[0]=="search"){
			NORMAL_MODE(maindirectory.c_str());
			if(fullcommands.size()!=2){
				printf("---->  INVALID COMMAND,ENTER FILENAME TO SEARCH.\n");

				break;

			}
			else if(search(fullcommands[1],maindirectory.c_str())){
				cout<<"---->  Present,at Path : "<<global_for_search_path<<"\n";
			}
			else{
				cout<<"---->  Not Present"<<"\n";
			}

		}



		else if(fullcommands[0]=="create_file"){
			NORMAL_MODE(maindirectory.c_str());
			if(fullcommands.size()<3){
				printf("---->  INVALID COMMAND IN create_file\n");
				break;
			}
			if((fullcommands.size()%2) == 0){			//matlab pair me nahi hai filename and destination.
				printf("----> MISSING FILENAME OR DESTINATION,RE-ENTER IN create_file/size is even\n");
			}
			else{
				for(int i=1;i<fullcommands.size();i+=2){
					string createfilename=fullcommands[i];
					string absolutecreatefilepathe=resolvepath(fullcommands[i+1]);
					if(absolutecreatefilepathe=="0"){
						printf("ENTERED PATH IS INCORRECT,RE-ENTER IN create_file/normal_mode\n");

					}
					else{
						createfile(absolutecreatefilepathe+"/"+createfilename);
						cout<<"SUCCESSFULLY CREATED FILE(S)."<<"\n";
					}
				}
			}

		}



		else if(fullcommands[0]=="create_dir"){
			NORMAL_MODE(maindirectory.c_str());
			if(fullcommands.size()<3){
				printf("---->  INVALID COMMAND IN create_dir");
				break;
			}
			if((fullcommands.size()%2) == 0){			//matlab pair me nahi hai foldername and destination.
				printf("----> MISSING FOLDERNAME OR DESTINATION,RE-ENTER IN create_dir\n");
				
			}
			else{
				for(int i=1;i<fullcommands.size();i+=2){
					string absolutecreatefolderpathe=resolvepath(fullcommands[i+1]);
					string createfoldername=fullcommands[i];
					if(absolutecreatefolderpathe=="0"){
						printf("ENTERED PATH IS INCORRECT,RE-ENTER IN create_dir/normal_mode\n");

					}
					else{
						createfolder(absolutecreatefolderpathe+"/"+createfoldername);
						cout<<"SUCCESSFULLY CREATED DIRECTORY."<<"\n";
					}
				}
			}

		}



	    else if(fullcommands[0]=="copy"){
	    	NORMAL_MODE(maindirectory.c_str());
	    	if(fullcommands.size()<3){
				printf("---->  INVALID COMMAND IN copy");
				break;
			}

			int fullcommands_size=fullcommands.size();
			string checkpath_in_copy=resolvepath(fullcommands[fullcommands_size-1]);
			if(checkpath_in_copy=="0"){
				printf("ENTERED PATH IS INCORRECT,RE-ENTER IN copy\n");
			}

			for(int i=1;i<fullcommands_size-1;i++){
				string name_in_copy="";
				string path_in_copy="";
				string temp_checkpath_in_copy=resolvepath(fullcommands[i]);
				if(temp_checkpath_in_copy=="0" || checkpath_in_copy=="0"){
					printf("ENTERED PATH IS INCORRECT,RE-ENTER IN copy\n");
				}
				else{
					int k=temp_checkpath_in_copy.size()-1;
					while(temp_checkpath_in_copy[k]!='/'){
						name_in_copy=temp_checkpath_in_copy[k]+name_in_copy;
						k--;
					}

					for(int j=0;j<k;j++){
						path_in_copy=path_in_copy+temp_checkpath_in_copy[j];
					}

					struct stat st3;
					char is_directory;
					stat(temp_checkpath_in_copy.c_str(),&st3);
					is_directory=(st3.st_mode & S_ISDIR(st3.st_mode))?'d':'f';
					if(is_directory=='d'){
						copyfolder(path_in_copy,name_in_copy,checkpath_in_copy);	
						printf("---->  COPIED FOLDER SUCCESSFULLY.\n");

					}
					else{
						first_copyfile(path_in_copy,name_in_copy,checkpath_in_copy);	
						printf("---->  COPIED FILE SUCCESSFULLY.\n");

					}

				}

			}

	    }

		// else if(fullcommands[0]=="copy_file"){											sole copy_file function
		// 	NORMAL_MODE(maindirectory.c_str());
		// 	if(fullcommands.size()<3){
		// 		printf("INVALID COMMAND IN copy_file");
		// 		break;
		// 	}
		// 	string name_copyfile="";
		// 	string path_copyfile="";
		// 	string temp_checkpath_copyfile=resolvepath(fullcommands[1]);
		// 	string checkpath_copyfile=resolvepath(fullcommands[2]);
		// 	if(temp_checkpath_copyfile!="0" && checkpath_copyfile!="0"){
		// 		int i=temp_checkpath_copyfile.size()-1;
		// 		while(temp_checkpath_copyfile[i]!='/'){
		// 			name_copyfile=temp_checkpath_copyfile[i]+name_copyfile;
		// 			i--;
		// 		}
		// 		for(int j=0;j<i;j++){
		// 			path_copyfile=path_copyfile+temp_checkpath_copyfile[j];
		// 		}
		// 		first_copyfile(path_copyfile,name_copyfile,checkpath_copyfile);	
		// 		printf("---->  COPIED FILE SUCCESSFULLY.\n");
				
		// 	}
		// 	else{
		// 		printf("ENTERED PATH IS INCORRECT,RE-ENTER IN copy_file\n");
		// 	}
		// }

		// else if(fullcommands[0]=="copy_folder"){										//sole copy_folder function
		// 	NORMAL_MODE(maindirectory.c_str());
		// 	if(fullcommands.size()<3){
		// 		printf("---->  INVALID COMMAND IN copy_folder");
		// 		break;
		// 	}

		// 	string name_copyfolder="";
		// 	string path_copyfolder="";								
		// 	string temp_checkpath_copyfolder=resolvepath(fullcommands[1]);
		// 	string checkpath_copyfolder=resolvepath(fullcommands[2]);
		// 	if(temp_checkpath_copyfolder!="0" && checkpath_copyfolder!="0"){
		// 		int i=temp_checkpath_copyfolder.size()-1;
		// 		while(temp_checkpath_copyfolder[i]!='/'){
		// 			name_copyfolder=temp_checkpath_copyfolder[i]+name_copyfolder;
		// 			i--;
		// 		}
		// 		for(int j=0;j<i;j++){
		// 			path_copyfolder=path_copyfolder+temp_checkpath_copyfolder[j];
		// 		}
		// 		copyfolder(path_copyfolder,name_copyfolder,checkpath_copyfolder);	
		// 		printf("---->  COPIED FOLDER SUCCESSFULLY.\n");
		// 	}
		// 	else{
		// 		printf("ENTERED PATH IS INCORRECT,RE-ENTER IN copy_folder\n");
		// 	}
		// }

		

	    else if(fullcommands[0]=="move"){
			NORMAL_MODE(maindirectory.c_str());
			if(fullcommands.size()<3){
				printf("---->  INVALID COMMAND IN move");
				break;
			}

			int fullcommands_size=fullcommands.size();
			string checkpath_in_move=resolvepath(fullcommands[fullcommands_size-1]);
			if(checkpath_in_move=="0"){
				printf("ENTERED PATH IS INCORRECT,RE-ENTER IN move\n");

			}

			for(int i=1;i<fullcommands_size-1;i++){
				string name_in_move="";
				string path_in_move="";
				string temp_checkpath_in_move=resolvepath(fullcommands[i]);
				if(temp_checkpath_in_move=="0" || checkpath_in_move=="0"){
					printf("ENTERED PATH IS INCORRECT,RE-ENTER IN move\n");
				}
				else{
					int k=temp_checkpath_in_move.size()-1;
					while(temp_checkpath_in_move[k]!='/'){
						name_in_move=temp_checkpath_in_move[k]+name_in_move;
						k--;
					}

					for(int j=0;j<k;j++){
						path_in_move=path_in_move+temp_checkpath_in_move[j];
					}

					struct stat st3;
					char is_directory;
					stat(temp_checkpath_in_move.c_str(),&st3);
					is_directory=(st3.st_mode & S_ISDIR(st3.st_mode))?'d':'f';
					if(is_directory=='d'){
						movefolder(path_in_move,name_in_move,checkpath_in_move);
						printf("---->  MOVED DIRECTORY SUCCESSFULLY.\n");

					}
					else{
						movefile(path_in_move,name_in_move,checkpath_in_move);	
						printf("---->  MOVED FILE SUCCESSFULLY.\n");

					}

				}

			}

		}



		else if(fullcommands[0]=="delete_file"){
			NORMAL_MODE(maindirectory.c_str());
			if(fullcommands.size()<2){
	    		printf("---->  INVALID COMMAND");
	    		break;
	    	}
	    	string checkpath_in_deletefile=resolvepath(fullcommands[1]);
	    	if(checkpath_in_deletefile=="0"){
				printf("ENTERED PATH IS INCORRECT,RE-ENTER IN delete_file\n");

			}
			else{
				deletefile(checkpath_in_deletefile.c_str());
				NORMAL_MODE(maindirectory.c_str());
				printf("---->  DELETED FILE SUCCESSFULLY.\n");

			}

		}



		else if(fullcommands[0]=="delete_dir"){
			NORMAL_MODE(maindirectory.c_str());
			if(fullcommands.size()<2){
	    		printf("---->  INVALID COMMAND");
	    		break;
	    	}
	    	string checkpath_in_deletefolder=resolvepath(fullcommands[1]);
	    	if(checkpath_in_deletefolder=="0"){
				printf("ENTERED PATH IS INCORRECT,RE-ENTER IN delete_folder\n");	

			}
			else{
				deletefolder(checkpath_in_deletefolder.c_str());
				NORMAL_MODE(maindirectory.c_str());							//normalmode is called in between to just refresh the screen so that updated contents are visible there itself.
				printf("---->  DELETED DIRECTORY SUCCESSFULLY.\n");

			}

		}



	    else if(fullcommands[0]=="goto"){		//goto
	    	if(fullcommands.size()<2){
	    		printf("---->  INVALID COMMAND");
	    		break;
	    	}
	    	leftpath.push(maindirectory);	

	    	string gotopath=resolvepath(fullcommands[1]);
	    	if(gotopath=="0"){
	    		printf("ENTERED PATH IS INCORRECT,RE-ENTER.\n");
	    	}
	    	struct stat s2;
	    	if(stat(gotopath.c_str(),&s2)==0  && (s2.st_mode & S_IFDIR)){
			    //it's a valid directory
	    		maindirectory=gotopath;
				NORMAL_MODE(maindirectory.c_str());							//normalmode is called in between to just refresh the screen so that updated  contents are visible there itself.
	    	}																
	    	else{
				//not a valid directory
	    		printf("ENTERED PATH IS INCORRECT,RE-ENTER.\n");
	    		leftpath.pop();
	    	}

	    }



	    else if(fullcommands[0]=="rename"){
	    	NORMAL_MODE(maindirectory.c_str());
	    	if(fullcommands.size()<3){
	    		printf("---->  INVALID COMMAND");
				printf("\33[2K\r");  		//clear line
				break;
			}
			string temps1=resolvepath(fullcommands[1]);
			string temps2=resolvepath(fullcommands[2]);
			if(temps1=="0" || temps2=="0"){
				printf("ENTERED PATH IS INCORRECT,RE-ENTER.\n");
			}
			else{
				rename(temps1.c_str(),temps2.c_str());
				NORMAL_MODE(maindirectory.c_str());
				cout<<"---->  RENAMED SUCCESSFULLY"<<"\n";
			}

		}



	    else if(c==27){	
	    	normal_flag=1;		//escape
	    	flag=1;				//back to normal mode.
	    	NORMAL_MODE(maindirectory.c_str());		
	    	break;

	    }



	    else if(fullcommands[0]=="quit"){
	    	printf("\033[H\033[2J\033[3J");		//clear the screen
			printf("\033[0;0f");			//cursor_0_0
			tflag=0;
			exit(0);

		}



		else{
			NORMAL_MODE(maindirectory.c_str());				
			cout<<"---->  INVALID COMMAND,RE-ENTER."<<"\n";

		}

	}

}


int main(){

	leftpath.push(maindirectory);				//push current path in left stack.

	enableRawMode();
	while(tflag){
		if(normal_flag){						//denotes we are in normal mode.
			NORMAL_MODE(leftpath.top().c_str());	//start normal mode with path present in left stack.
		}
		else{
			COMMAND_MODE();						//start command mode.
		}
	}
	return 0;

}
