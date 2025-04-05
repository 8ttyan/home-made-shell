
#include <string>
using namespace std;
#include "lexicaltokenizer.h"
class StringTree;

class Parser
{
public:
	Parser();
	void run(LexicalTokenizer&);
private:
	void Line(LexicalTokenizer&, StringTree*);
	void Shell(LexicalTokenizer&, StringTree*);
	void Sentence(LexicalTokenizer&, StringTree*);
	void ProcessGroup(LexicalTokenizer&, StringTree*);
	void Process(LexicalTokenizer&, StringTree*);
	void Command(LexicalTokenizer&, StringTree*);
};

