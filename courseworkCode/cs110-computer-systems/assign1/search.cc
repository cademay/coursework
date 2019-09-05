/*
 * Author: Cade May
 * Date: April 10, 2017
 * Description: this file searches through a database of actors and movies
 * and uses breadth first search to identify connections between two actors
 * through series of other movies and other actors
 */

#include "imdb.h"
#include <string.h>
#include <set>
#include <list>
#define Path vector<node>

static const int CANCEL_LENGTH = 7;
static const int kWrongArgumentCount = 1;
static const int kDatabaseNotFound = 2;

using namespace std;

/*
 *this struct is used so that as paths connecting actors are formed, once a path
 * is found and needs to be printed, the necessary corresponding information will be readily available
 */
struct node {
	string actor;
	string movie;
	int year;
};

//this method allows the struct node to be placed into data structures
//an < operator function is required when structs are used in this manner
bool operator < ( const node & a, const node & b ) {
	return a.movie < b.movie ||  a.year < b.year ||  a.actor < b.actor;
}

/*
 *this method takes an actor and an empty set of nodes as input, and uses imdb methods
 *to compile a list of all of the given actor's costars by first getting all of the actors
 *featured movies, and then finding all of the actors who were in those films
 *then it remotely fills the given referenced set with the costar nodes
 */
void getAllCostars(const imdb &db, string currActor, set<node> &costars, set<struct film> &alreadySeenMovie, set<string> &alreadySeenActor) {

	vector<film> movies;	
	if (!db.getCredits(currActor, movies) || movies.size() == 0) {
		return;
	}

	for (unsigned int i = 0; i < movies.size(); i++) {
		const film& curMovie = movies[i];
		if (alreadySeenMovie.count(curMovie) != 0) continue;

		vector<string> cast;
		db.getCast(curMovie, cast);	
		for (unsigned int j = 0; j < cast.size(); j++) {
			if (alreadySeenActor.count(cast[j]) != 0) continue;
			if (cast[j].compare(currActor) != 0) {
				costars.insert({cast[j], curMovie.title, curMovie.year});
			}
		}
	}
}

/*
 *this is an implementation of breadth first search, which uses a while loop to
 *build paths between actor nodes. once it finds a path where the last name
 *equals the name it was given to search for, it sets the referenced path vector and then returns
 */
void search(const imdb& db, char* st, char* nd, Path &link) {
	string start = st;
	string end = nd;

	list<Path> pathList;
	set<struct film> alreadySeenMovie;
	set<string> alreadySeenActor;
	Path firstPath;
	Path correctPath;
	firstPath.push_back({start});
	pathList.push_back(firstPath);

	while (!pathList.empty()) {
		Path currPath = pathList.front();
		pathList.pop_front();
		int size  = currPath.size();
		node currActor = currPath[size - 1];

		if ((currActor.actor).compare(end) == 0) {
			correctPath = currPath;
			break;
		}

		if (size >= CANCEL_LENGTH) break;

		if (alreadySeenActor.count(currActor.actor) == 0 || alreadySeenMovie.count({currActor.movie,currActor.year}) == 0) {
			alreadySeenActor.insert(currActor.actor);
			alreadySeenMovie.insert({currActor.movie,currActor.year});

			set<node> costars;
			getAllCostars(db, currActor.actor, costars, alreadySeenMovie, alreadySeenActor);

			for (node neighbor : costars) {
				Path newPath = currPath;
				newPath.push_back(neighbor);
				if (neighbor.actor.compare(end) == 0) {
					link = newPath;
					return;
				}
				pathList.push_back(newPath);
			}
		}

	}	

	link = correctPath;
}

/*
 *
 *this main method does some simple error checking on the inputs
 *and then calls the search method to populate the path vector
 *then it prints out the contents of the resulting path
 * in a sensible way
 */
int main(int argc, char *argv[]) {

	if (argc != 3) return kWrongArgumentCount;

	imdb db(kIMDBDataDirectory);
	if (!db.good()) {
		cerr << "Data directory not found!  Aborting..." << endl;
		return kDatabaseNotFound;
	}

	if (strcmp(argv[1], argv[2]) != 0) {
		Path link;
		search(db, argv[1], argv[2], link);

		for (unsigned int i = 0; i+1 < link.size(); i++) {
			cout << link[i].actor << " was in " << "\"" << link[i + 1].movie << "\"" << " ("  << link[i + 1].year << ")" << " with " << link[i + 1].actor << "."  << endl;
		}
	} else {
		cerr << "You entered the same actor twice. Aborting..." << endl;
	}
	return 0;
}
