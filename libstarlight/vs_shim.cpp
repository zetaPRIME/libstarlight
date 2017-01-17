// various stuff to force Visual Studio to play nice
// not used in compilation, thankfully

#undef __cplusplus
#define __cplusplus 201103L

// whee, silly macro bullshit
#define _GLIBCXX_VISIBILITY(a)
#define _GLIBCXX_BEGIN_NAMESPACE_CONTAINER
#define _GLIBCXX_END_NAMESPACE_CONTAINER