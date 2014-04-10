#include "ScoreTable.h"
#include <direct.h>

static const char* KEY = "NOT_REVEALED";

ScoreTable::ScoreTable(){
	scoreMap = new SimplifiedHashmap<std::string, std::string>();
}

void ScoreTable::load(){
	scoreMap->clear();
	FileIO::read("data/scores.data", scoreMap);
	scoreMap = Crypt::encryptDecryptMap<std::string, std::string>(KEY, scoreMap);
}

void ScoreTable::save(){
	if(!scoreMap->isEmpty()){
		_mkdir("data");

		SimplifiedHashmap<std::string, std::string>* cryptMap = Crypt::encryptDecryptMap<std::string, std::string>(KEY, scoreMap);

		FileIO::write("data/scores.data", cryptMap);
		load();
		sort();
	}
}

bool ScoreTable::isHighScore(unsigned long long score){
	if(scoreMap->size() < 10){
		return true;
	}

	unsigned long long lowestScore = -1;
	for(unsigned int i = 0; i < scoreMap->size(); i++){
		std::string::size_type s_size = 0;
		unsigned long long ll_score = std::stoull(scoreMap->getValues()->at(i), &s_size, 10);
		
		if(ll_score < lowestScore){
			lowestScore = ll_score;
		}
	}

	if(score > lowestScore){
		return true;
	}

	return false;
}

void ScoreTable::putScore(char name[3], unsigned long long score){
	if(isHighScore(score)){
		//Ensures the name is not going to be overridden unless actually a higher score
		if(scoreMap->containsKey(std::string(name))){
			std::string::size_type s_size = 0;
			unsigned long long ll_score = std::stoull(scoreMap->get(std::string(name)), &s_size, 10);
			if(ll_score >= score){
				return;
			}
		}
		scoreMap->put(std::string(name), std::to_string(score));
		sort();
	}
}

void ScoreTable::removeScore(char name[3]){
	scoreMap->remove(std::string(name));
	sort();
}

/*Requirements for sort:
- Sort by value
- Remove extra elements(limit to 10)
*/
void ScoreTable::sort(){
	if(scoreMap->isEmpty()){
		return;
	}

	SimplifiedHashmap<std::string, std::string>* sortedHash = new SimplifiedHashmap<std::string, std::string>();

	while(!scoreMap->isEmpty()){
		unsigned long long highestScore = 0;
		std::string highestScoreKey;

		for(unsigned int i = 0; i < scoreMap->size(); i++){
			std::string::size_type s_size = 0;
			unsigned long long ll_score = std::stoull(scoreMap->getValues()->at(i), &s_size, 10);

			if(ll_score > highestScore){
				highestScore = ll_score;
				highestScoreKey = scoreMap->getKeys()->at(i);
			}
		}

		sortedHash->put(highestScoreKey, std::to_string(highestScore));
		scoreMap->remove(highestScoreKey);
	}

	if(sortedHash->size() > 10){
		while(sortedHash->size() != 10){
			sortedHash->remove(sortedHash->getKeys()->at(sortedHash->size() - 1));
		}
	}

	delete scoreMap;
	scoreMap = sortedHash;
}