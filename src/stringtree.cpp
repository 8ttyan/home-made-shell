
#include "stringtree.h"
#include <cstring>

#define D_SEPARATOR " | "

StringTree::StringTree()
: mMaxStringSize(0)
{
}
StringTree::StringTree(const char* pCharacter)
: mMaxStringSize(0)
{
	mString = pCharacter;
}
StringTree::StringTree(const string& pString)
: mString(pString)
, mMaxStringSize(0)
{
}
StringTree& StringTree::push_back(const StringTree& pTree)
{
	mChildren.push_back(pTree);
	return mChildren.back();
}
StringTree& StringTree::push_back(const char* pCharacter)
{
	mChildren.push_back( StringTree(pCharacter) );
	return mChildren.back();
}
void StringTree::operator = (const string& pString)
{
	mString = pString;
}
string& StringTree::operator * ()
{
	return mString;
}
string* StringTree::operator -> ()
{
	return &mString;
}
size_t StringTree::size() const
{
	return mChildren.size();
}
StringTree& StringTree::front()
{
	return mChildren.front();
}
StringTree& StringTree::back()
{
	return mChildren.back();
}
void StringTree::print()
{
	updateMaxStringSize();
	size_t maxDepth=depth();
	for (size_t i=maxDepth; i>=1; i--) {
		printAt(i);
		printf("\n");
	}
}
void print_centerd(const char* str, int width) {
	int len=strlen(str);
	int padding=(width-len)/2;
	if ( padding>0 ) {
		printf("%*s%s", padding,"",str);
	} else {
		printf("%s",str);
	}
}
void StringTree::printAt(size_t pDepth) const
{
	pDepth--;
	if ( pDepth==0 ) {
		print_centerd(mString.c_str(), maxStringSize());
		return;
	}
	bool first=true;
	for (const auto& child : mChildren) {
		if ( pDepth==1 && first==false ) {
			printf(D_SEPARATOR);
		}
		first = false;
		child.printAt(pDepth);
	}
}
size_t StringTree::depth() const
{
	size_t maxDepth=0;
	for (auto& child : mChildren) {
		size_t myDepth = child.depth();
		if ( myDepth>maxDepth ) maxDepth=myDepth;
	}
	return maxDepth + 1;
}
int StringTree::maxStringSize() const
{
	return mMaxStringSize;
}
int StringTree::updateMaxStringSize()
{
	int total=0;
	for (auto& child : mChildren) {
		total += child.updateMaxStringSize();
	}
	if ( mChildren.size()>0 ) {
		total += (sizeof(D_SEPARATOR)-1) * (mChildren.size() - 1);
	}
	if ( total > mString.size() ) {
		mMaxStringSize = total;
	} else {
		mMaxStringSize = mString.size();
	}
	return mMaxStringSize;
}
