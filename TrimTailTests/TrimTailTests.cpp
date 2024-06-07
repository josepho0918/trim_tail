#include "pch.h"
#include "CppUnitTest.h"
#include <filesystem>
#include <fstream>
#include "TrimTail.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace std::filesystem;

namespace TrimTailTests
{
	TEST_CLASS(TrimTailTests)
	{
	public:

        const path TEST_FILE_PATH = "test.txt";
		
        TEST_METHOD(TestHasTrailingBlanks)
        {
            ofstream(TEST_FILE_PATH) << "    " << endl;
            Assert::IsTrue(HasTrailingBlanks(TEST_FILE_PATH));
            remove(TEST_FILE_PATH);

            ofstream(TEST_FILE_PATH) << "Hello World    " << endl;
            Assert::IsTrue(HasTrailingBlanks(TEST_FILE_PATH));
            remove(TEST_FILE_PATH);

            ofstream(TEST_FILE_PATH) << "Hello World" << endl;
            Assert::IsFalse(HasTrailingBlanks(TEST_FILE_PATH));
            remove(TEST_FILE_PATH);
        }

        TEST_METHOD(TestGetCleanLine)
        {

            ofstream(TEST_FILE_PATH) << "Hello World    " << endl;
            Assert::AreEqual("Hello World"s, *GetCleanLine(*make_unique<fstream>(TEST_FILE_PATH, ios::in)));
            remove(TEST_FILE_PATH);

            ofstream(TEST_FILE_PATH) << "Hello World\t\t" << endl;
            Assert::AreEqual("Hello World"s, *GetCleanLine(*make_unique<fstream>(TEST_FILE_PATH, ios::in)));
            remove(TEST_FILE_PATH);
        }

        TEST_METHOD(TestRemoveTrailingBlanks)
        {
            stringstream buffer;

            ofstream(TEST_FILE_PATH) << "Hello World    ";
            RemoveTrailingBlanks(TEST_FILE_PATH);
            buffer.str("");
            buffer << ifstream(TEST_FILE_PATH).rdbuf();
            Assert::AreEqual("Hello World"s, buffer.str());
            remove(TEST_FILE_PATH);

            ofstream(TEST_FILE_PATH) << "Hello World    " << endl;
            RemoveTrailingBlanks(TEST_FILE_PATH);
            buffer.str("");
            buffer << ifstream(TEST_FILE_PATH).rdbuf();
            Assert::AreEqual("Hello World\n"s, buffer.str());
            remove(TEST_FILE_PATH);
        }
	};
}
