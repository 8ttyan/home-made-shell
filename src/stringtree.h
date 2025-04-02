
#include <string>
#include <list>
using namespace std;

class StringTree
{
public:
	StringTree();
	StringTree(const char*);
	StringTree(const string&);
	StringTree& push_back(const StringTree&);
	StringTree& push_back(const char*);
	void operator = (const string&);
	string& operator * ();
	string* operator -> ();
	size_t size() const;
	StringTree& front();
	StringTree& back();
	void print();
	size_t depth() const;
	int maxStringSize() const;
	int updateMaxStringSize();
private:
	void printAt(size_t pDepth) const;

private:
	string mString;
	list<StringTree> mChildren;
	int mMaxStringSize;
};

