
#include <string>
using namespace std;
#include "lexicaltokenizer.h"
class StringTree;
class Shell;
class Sentence;
class ProcessGroup;
class Process;

class Parser
{
public:
	Parser();
	void run(LexicalTokenizer&, Shell*, bool pPrintTree=false);
private:
	void Line(LexicalTokenizer&, Shell*, StringTree*);
	void _Shell(LexicalTokenizer&, Shell*, StringTree*);
	void _Sentence(LexicalTokenizer&, Shell*, StringTree*);
	void _ProcessGroup(LexicalTokenizer&, Sentence*, StringTree*);
	void _Process(LexicalTokenizer&, ProcessGroup*, StringTree*);
	void _Command(LexicalTokenizer&, Process*, StringTree*);
private:
};

