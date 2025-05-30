#include <gtest/gtest.h>
#include <Utility/Utility.h>

TEST(Utility_Tests, CheckAddUniqueToVector)
{
	std::vector<int> expectedVector{ 1, 2 };

	std::vector<int> vector;

	int a = 1;
	AddUniqueToVector(vector, a);
	AddUniqueToVector(vector, a);

	int b = 2;
	AddUniqueToVector(vector, b);

	//Check for only one 1 added and not two
	EXPECT_TRUE(expectedVector == vector);

	//Check that the item got removed
	vector.erase(vector.begin(), vector.begin() + 1);
	EXPECT_TRUE(expectedVector != vector);

	//Check so the item got added a single time again
	AddUniqueToVector(vector, a);
	AddUniqueToVector(vector, a);
	AddUniqueToVector(vector, a);
	expectedVector = { 2, 1 };
	EXPECT_TRUE(expectedVector == vector);
}

struct DummyItem
{
	int a = 1;

};

bool operator==(const DummyItem& left, const DummyItem& right)
{
	return left.a == right.a;
}

TEST(Utility_Tests, CheckEraseItemFromVector)
{
	DummyItem a{ 1 };
	DummyItem b{ 2 };
	DummyItem c{ 3 };
	DummyItem d{ 4 };
	std::vector<DummyItem> expectedVector{ b,c,d };

	std::vector<DummyItem> vector{ a,b,c,d };

	//Make sure (a) was removed and nothing else
	EraseItemFromVector(vector, a);
	EXPECT_TRUE(expectedVector == vector);
}

TEST(Utility_Tests, CheckStringConversion)
{
	//Check normal one step conversion
	std::string out = WStringToString(L"Hej Hej wstring");
	std::wstring outW = StringToWString("Hej Hej string");
	EXPECT_TRUE(out == "Hej Hej wstring");
	EXPECT_TRUE(outW == L"Hej Hej string");

	//Check multi step conversion
	EXPECT_TRUE(WStringToString(StringToWString(WStringToString(L"Correct"))) == "Correct");
}