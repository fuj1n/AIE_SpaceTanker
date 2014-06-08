#include "ScoreTable.hpp"
#include <direct.h>

//Since there is no more encryption, no point keeping the key secret
static const char* KEY = "\u00A7CD";

ScoreTable::ScoreTable() {
	scoreMap = new SimplifiedHashmap<std::string, std::string>();
}

void ScoreTable::load() {
	scoreMap->clear();
	FileIO::read("data/scores.data", scoreMap);
	//scoreMap = Crypt::encryptDecryptMap<std::string, std::string>(KEY, scoreMap);

	if(scoreMap->isEmpty()) {
		defaults();
		save();
	}
}

void ScoreTable::save() {
	if(!scoreMap->isEmpty()) {
		if(_mkdir("data") == NULL && false)
			return;

		//SimplifiedHashmap<std::string, std::string>* cryptMap = Crypt::encryptDecryptMap<std::string, std::string>(KEY, scoreMap);

		FileIO::write("data/scores.data", scoreMap);
		load();
		sort();
	}
}

void ScoreTable::defaults() {
	scoreMap->clear();

	putScore("OKU", 10000);
	putScore("ADU", 8000);
	putScore("DW ", 6000);
	putScore("MIK", 5000);
	putScore("AMN", 4000);
	putScore("FOR", 4500);
	putScore("FUJ", 4000);
	putScore("CH ", 3500);
	putScore("MYR", 3000);
	putScore("ABS", 2500);
}

bool ScoreTable::isHighScore(unsigned long long score) {
	if(scoreMap->size() < 10) {
		return true;
	}

	unsigned long long lowestScore = (unsigned long long) - 1;
	for(unsigned int i = 0; i < scoreMap->size(); i++) {
		std::string::size_type s_size = 0;
		unsigned long long ll_score = std::stoull(scoreMap->getValues()->at(i), &s_size, 10);

		if(ll_score < lowestScore) {
			lowestScore = ll_score;
		}
	}

	if(score > lowestScore) {
		return true;
	}

	return false;
}

void ScoreTable::putScore(char name[4], unsigned long long score) {
	if(isHighScore(score)) {
		//Ensures the name is not going to be overridden unless actually a higher score
		if(scoreMap->containsKey(std::string(name))) {
			std::string::size_type s_size = 0;
			unsigned long long ll_score = std::stoull(scoreMap->get(std::string(name)), &s_size, 10);
			if(ll_score >= score) {
				return;
			}
		}
		scoreMap->put(std::string(name), std::to_string(score));
		sort();
	}
}

void ScoreTable::removeScore(char name[4]) {
	scoreMap->remove(std::string(name));
	sort();
}

int ScoreTable::findScore(std::string str) {
	return scoreMap->getElementID(str);
}

/*Requirements for sort:
- Sort by value
- Remove extra elements(limit to 10)
*/
void ScoreTable::sort() {
	if(scoreMap->isEmpty()) {
		return;
	}

	SimplifiedHashmap<std::string, std::string>* sortedHash = new SimplifiedHashmap<std::string, std::string>();

	while(!scoreMap->isEmpty()) {
		unsigned long long highestScore = 0;
		std::string highestScoreKey;

		for(unsigned int i = 0; i < scoreMap->size(); i++) {
			std::string::size_type s_size = 0;
			unsigned long long ll_score = std::stoull(scoreMap->getValues()->at(i), &s_size, 10);

			if(ll_score > highestScore) {
				highestScore = ll_score;
				highestScoreKey = scoreMap->getKeys()->at(i);
			}
		}

		sortedHash->put(highestScoreKey, std::to_string(highestScore));
		scoreMap->remove(highestScoreKey);
	}

	if(sortedHash->size() > 10) {
		while(sortedHash->size() != 10) {
			sortedHash->remove(sortedHash->getKeys()->at(sortedHash->size() - 1));
		}
	}

	delete scoreMap;
	scoreMap = sortedHash;
}