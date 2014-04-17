#include "Utils.cpp"

class ScoreTable {
private:
public:
	//Warning: not fool proof
	SimplifiedHashmap<std::string, std::string>* scoreMap;

	ScoreTable();
	void load();
	void save();

	bool isHighScore(unsigned long long score);
	void putScore(char name[3], unsigned long long score);
	void removeScore(char name[3]);
	void sort();
};