#include "pch.h"
#include "CppUnitTest.h"
#include <filesystem>
#include <fstream>
#include "TrimTail.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
namespace fs = std::filesystem;

namespace TrimTailTests
{
	TEST_CLASS(TrimTailTests)
	{
	public:

        const fs::path TEST_FILE_PATH = "test.txt";
		
        TEST_METHOD(TestHasTrailingBlanks)
        {
            ofstream(TEST_FILE_PATH) << "    " << endl;
            Assert::IsTrue(HasTrailingBlanks(TEST_FILE_PATH));
            remove(TEST_FILE_PATH);

            ofstream(TEST_FILE_PATH) << "Hello " << endl << "World " << endl;
            Assert::IsTrue(HasTrailingBlanks(TEST_FILE_PATH));
            remove(TEST_FILE_PATH);

            ofstream(TEST_FILE_PATH) << "Hello\t" << endl << "World\t" << endl;
            Assert::IsTrue(HasTrailingBlanks(TEST_FILE_PATH));
            remove(TEST_FILE_PATH);

            ofstream(TEST_FILE_PATH) << "Hello" << endl << "World" << endl;
            Assert::IsFalse(HasTrailingBlanks(TEST_FILE_PATH));
            remove(TEST_FILE_PATH);
        }

        TEST_METHOD(TestSingleLine)
        {
            stringstream buffer;

            ofstream(TEST_FILE_PATH) << "Hello World \t ";
            RemoveTrailingBlanks(TEST_FILE_PATH);
            buffer.str("");
            buffer << ifstream(TEST_FILE_PATH).rdbuf();
            Assert::AreEqual("Hello World"s, buffer.str());
            remove(TEST_FILE_PATH);

            ofstream(TEST_FILE_PATH) << "Hello World \t " << endl;
            RemoveTrailingBlanks(TEST_FILE_PATH);
            buffer.str("");
            buffer << ifstream(TEST_FILE_PATH).rdbuf();
            Assert::AreEqual("Hello World\n"s, buffer.str());
            remove(TEST_FILE_PATH);
        }

        TEST_METHOD(TestMultipleLines)
        {
            stringstream buffer;

            ofstream(TEST_FILE_PATH) << "Hello \t " << endl << "World \t ";
            RemoveTrailingBlanks(TEST_FILE_PATH);
            buffer.str("");
            buffer << ifstream(TEST_FILE_PATH).rdbuf();
            Assert::AreEqual("Hello\nWorld"s, buffer.str());
            remove(TEST_FILE_PATH);

            ofstream(TEST_FILE_PATH) << "Hello \t " << endl << "World \t " << endl;
            RemoveTrailingBlanks(TEST_FILE_PATH);
            buffer.str("");
            buffer << ifstream(TEST_FILE_PATH).rdbuf();
            Assert::AreEqual("Hello\nWorld\n"s, buffer.str());
            remove(TEST_FILE_PATH);
        }
	};
}
