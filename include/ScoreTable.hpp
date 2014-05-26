#include "Utils.cpp"

class ScoreTable {
private:
public:
	//Warning: not fool proof
	SimplifiedHashmap<std::string, std::string>* scoreMap;

	ScoreTable();
	void load();
	void save();
	void defaults();

	bool isHighScore(unsigned long long score);
	void putScore(char name[4], unsigned long long score);
	void removeScore(char name[4]);
	int findScore(std::string str);
	void sort();
};