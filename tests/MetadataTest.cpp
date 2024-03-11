#include "TestMain.h"

#include "core/MetadataDict.h"

using namespace mer::core;

MER_TEST(storesAndReadsBack)
{
    MetadataDict dict;
    dict.setString("Title", "Interview Take 3");
    MER_CHECK_EQ(dict.get("title").asString(), std::string("Interview Take 3"));
    MER_CHECK(dict.contains("TITLE"));
}

MER_TEST(missingKeyIsUnset)
{
    MetadataDict dict;
    const MetadataValue v = dict.get("absent");
    MER_CHECK(!v.isSet());
    MER_CHECK(!v.hasContent());
}

MER_TEST(presenceAndContentDiffer)
{
    // A tag that exists but carries no text is present, not useful.
    const MetadataValue empty = MetadataValue::fromString("");
    MER_CHECK(empty.isSet());
    MER_CHECK(!empty.hasContent());

    const MetadataValue filled = MetadataValue::fromString("Reel 4");
    MER_CHECK(filled.isSet());
    MER_CHECK(filled.hasContent());
}

MER_TEST(coercesTypes)
{
    MER_CHECK_EQ(MetadataValue::fromString("42").asInt(), std::int64_t(42));
    MER_CHECK_EQ(MetadataValue::fromString("nope").asInt(7), std::int64_t(7));
    MER_CHECK(MetadataValue::fromString("yes").asBool());
    MER_CHECK_EQ(MetadataValue::fromInt(9).asString(), std::string("9"));
}

MER_TEST(mergeOverwrites)
{
    MetadataDict a;
    a.setString("scene", "12");
    MetadataDict b;
    b.setString("scene", "13");
    b.setString("take", "2");
    a.merge(b);
    MER_CHECK_EQ(a.get("scene").asString(), std::string("13"));
    MER_CHECK_EQ(a.size(), std::size_t(2));
}
