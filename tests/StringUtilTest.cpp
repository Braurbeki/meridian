#include "TestMain.h"

#include "util/StringUtil.h"

using namespace mer::util;

MER_TEST(trimsWhitespace)
{
    MER_CHECK_EQ(trim("  hello  "), std::string("hello"));
    MER_CHECK_EQ(trim("\t\n"), std::string(""));
    MER_CHECK_EQ(trim("none"), std::string("none"));
}

MER_TEST(splitsAndJoins)
{
    const auto parts = split("a,b,c", ',');
    MER_CHECK_EQ(parts.size(), std::size_t(3));
    MER_CHECK_EQ(join(parts, "-"), std::string("a-b-c"));
}

MER_TEST(extractsPathComponents)
{
    MER_CHECK_EQ(baseName("/vol/media/A001_C012.mxf"), std::string("A001_C012.mxf"));
    MER_CHECK_EQ(stemName("/vol/media/A001_C012.mxf"), std::string("A001_C012"));
    MER_CHECK_EQ(extension("/vol/media/A001_C012.mxf"), std::string("mxf"));
    MER_CHECK_EQ(parentPath("/vol/media/A001_C012.mxf"), std::string("/vol/media"));
}

MER_TEST(normalisesWindowsSeparators)
{
    MER_CHECK_EQ(baseName("D:\\media\\shot.mov"), std::string("shot.mov"));
}
