#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include "imdb.h"
#include <string.h>
using namespace std;

#define YEAR_SHIFT 1900

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory) {
	const string actorFileName = directory + "/" + kActorFileName;
	const string movieFileName = directory + "/" + kMovieFileName;  
	actorFile = acquireFileMap(actorFileName, actorInfo);
	movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const {
	return !( (actorInfo.fd == -1) || 
			(movieInfo.fd == -1) ); 
}

imdb::~imdb() {
	releaseFileMap(actorInfo);
	releaseFileMap(movieInfo);
}

//input: actor and vector of films
//output: success bool, remotely fills vector with actor's associated films
bool imdb::getCredits(const string& player, vector<film>& films) const { 

	int* actorOffsetsBeg = (int*)( (char*)actorFile + 4);
	int* actorOffsetsEnd = (int*)( (char*)actorFile + 4 + ( *(int*)actorFile * sizeof(int) ) );

	int* spot = lower_bound(actorOffsetsBeg, actorOffsetsEnd, player, [this](int offset, const string& key) {
		char* actor = (char*)actorFile + offset;
		string firstArg = actor;
		return firstArg < key;
	});

	if (spot == actorOffsetsEnd) {
		films.clear();
		return false;
	}

	//get all movies that found actor is featured in
	void *actorSpot = (char*)actorFile + *spot;
	char* actorName = (char*)actorSpot;
	string name = actorName;
	if (name != player) return false;
	int filmCountShift = strlen(actorName) + sizeof(char);
	
	if ( strlen(actorName) % 2 == 0 ) {
		//even actor name, so odd w/ nullterm, so add 1 so even
		filmCountShift++;
	}
	
	short filmCount = *(short*)((char*)actorSpot + filmCountShift);
	int filmArrayShift = filmCountShift + sizeof(short);
	
	if ( filmArrayShift % 4 != 0 ) {
		//actor name zize + num_films size total have even bytes, but not mult 4, so fix that 
		filmArrayShift += 2;
	}
	
	for (int i = 0; i < filmCount; i++) {
		int curShift = *(int*)( (char*)actorName + filmArrayShift + (i * sizeof(int)) );
		char* curMovieName = (char*)movieFile + curShift;
		char* curMovieYear = (char*)curMovieName + strlen(curMovieName) + 1;
		int year = (int)(*curMovieYear) + YEAR_SHIFT;
		struct film curFilm;
		string name = curMovieName;
		curFilm.title = name;
		curFilm.year = year;
		films.push_back(curFilm);
	}	
	return true;
}

//input: movie, vector of actors
//output: success bool. remotely populates vector with associated actors
bool imdb::getCast(const film& movie, vector<string>& players) const { 

	int *movieOffsetsBeg = (int*)((char*)movieFile + 4);
	int *movieOffsetsEnd = (int*)((char*)movieFile + 4 + ( *(int*)movieFile * sizeof(int) ));
	int* spot = lower_bound(movieOffsetsBeg, movieOffsetsEnd, movie, [this](int offset, const film& key) {

			struct film curFilm;
			curFilm.title = (char*)movieFile + offset;

			char * yearBufferSpot = (char*)movieFile + offset + strlen( ((char*)movieFile + offset) ) + 1;
			curFilm.year = (int)(*yearBufferSpot) + YEAR_SHIFT;
		
			return (curFilm <  key);
		});

	if (spot == movieOffsetsEnd) {
		players.clear();
		return false;
	}	  

	//get all actors out of correctFilm
	int correctOffset = *spot;
	void* correctMovieBlock = (char*)movieFile + correctOffset;
	char* movieName = (char*)correctMovieBlock;
	int movieNameLength = strlen(movieName);
	int castCountSpotShift = movieNameLength + (2 * sizeof(char) );
	if ( movieNameLength % 2 != 0 ) {
		//odd, so extra null term, because odd + '\0' (1byte) + 1byte for year = odd + 2 = odd
		castCountSpotShift++;
	}
	short castCount = *(short*)((char*)correctMovieBlock + castCountSpotShift);
	int castArrayShift = castCountSpotShift + sizeof(short);
	
	if (castArrayShift % 4 != 0) {
		castArrayShift += 2;
	}
	int* castArrayStart = (int*)((char*)correctMovieBlock + castArrayShift);

	for (int i = 0; i < castCount; i++) {
		int curActorShift = *(int*)( (char*)castArrayStart + (i * sizeof(int)) );
		char* curActorName = (char*)actorFile + curActorShift;
		string actor = curActorName;
		players.push_back(actor);
	}

	return true;
}

const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info) {
	struct stat stats;
	stat(fileName.c_str(), &stats);
	info.fileSize = stats.st_size;
	info.fd = open(fileName.c_str(), O_RDONLY);
	return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info) {
	if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
	if (info.fd != -1) close(info.fd);
}
