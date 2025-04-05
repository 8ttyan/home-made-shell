
#include "stringtree.h"
#include <cstring>

#define D_SEPARATOR "|"

StringTree::StringTree()
: mChildrenStringSize(0)
{
}
StringTree::StringTree(const char* pCharacter)
: mChildrenStringSize(0)
{
	mString = pCharacter;
}
StringTree::StringTree(const string& pString)
: mString(pString)
, mChildrenStringSize(0)
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
	updateChildrenStringSize();
	size_t maxDepth=depth();
	for (size_t i=maxDepth; i>=1; i--) {
		printf(D_SEPARATOR);
		printAt(i);
		printf(D_SEPARATOR);
		printf("\n");
	}
}
void print_centerd(const char* str, int width) {
	int len=strlen(str);
	int leftSpace=(width-len)/2;
	if ( leftSpace>0 ) {
		int rightSpace=width-len-leftSpace;
		printf("%*s%s%*s", leftSpace,"",str,rightSpace,"");
	} else {
		printf("%s",str);
	}
}
void StringTree::printAt(size_t pDepth) const
{
	pDepth--;
	if ( pDepth==0 ) {
		print_centerd(mString.c_str(), maxStringSize());
		//printf("%d",mChildrenStringSize);
		return;
	}
	if ( mChildren.size()==0 ) {
		print_centerd("",maxStringSize());
		return;
	}
	bool first=true;
	for (const auto& child : mChildren) {
		if ( first==false ) {
			printf(D_SEPARATOR);
		}
		first = false;
		child.printAt(pDepth);
	}
	int diff = mString.size()-mChildrenStringSize;
	if ( diff>0 ) {
		printf("%*s",diff,"");
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
	if ( mString.size() > mChildrenStringSize ) {
		return (int)mString.size();
	}
	return mChildrenStringSize;
}
int StringTree::updateChildrenStringSize()
{
	mChildrenStringSize=0;
	for (auto& child : mChildren) {
		mChildrenStringSize += child.updateChildrenStringSize();
	}
	if ( mChildren.size()>0 ) {
		mChildrenStringSize += mChildren.size() - 1;
	}
	return maxStringSize();
}
