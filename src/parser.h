
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
	bool run(LexicalTokenizer&, Shell*, bool pPrintTree=false);
private:
	bool Line(LexicalTokenizer&, Shell*, StringTree*);
	bool _Shell(LexicalTokenizer&, Shell*, StringTree*);
	bool _Sentence(LexicalTokenizer&, Shell*, StringTree*);
	bool _ProcessGroup(LexicalTokenizer&, Sentence*, StringTree*);
	bool _Process(LexicalTokenizer&, ProcessGroup*, StringTree*);
	bool _Command(LexicalTokenizer&, Process*, StringTree*);
private:
};

