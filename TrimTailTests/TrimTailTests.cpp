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

        const path test_file_path = "test.txt";
		
        TEST_METHOD(TestHasTrailingBlanks)
        {
            ofstream(test_file_path) << "    " << endl;
            Assert::IsTrue(HasTrailingBlanks(test_file_path));
            remove(test_file_path);

            ofstream(test_file_path) << "Hello World    " << endl;
            Assert::IsTrue(HasTrailingBlanks(test_file_path));
            remove(test_file_path);

            ofstream(test_file_path) << "Hello World" << endl;
            Assert::IsFalse(HasTrailingBlanks(test_file_path));
            remove(test_file_path);
        }

        TEST_METHOD(TestGetCleanLine)
        {

            ofstream(test_file_path) << "Hello World    " << endl;
            Assert::AreEqual("Hello World"s, *GetCleanLine(*make_unique<ifstream>(test_file_path)));
            remove(test_file_path);

            ofstream(test_file_path) << "Hello World\t\t" << endl;
            Assert::AreEqual("Hello World"s, *GetCleanLine(*make_unique<ifstream>(test_file_path)));
            remove(test_file_path);
        }

        TEST_METHOD(TestRemoveTrailingBlanks)
        {
            stringstream buffer;

            ofstream(test_file_path) << "Hello World    ";
            RemoveTrailingBlanks(test_file_path);
            buffer.str("");
            buffer << ifstream(test_file_path).rdbuf();
            Assert::AreEqual("Hello World"s, buffer.str());
            remove(test_file_path);

            ofstream(test_file_path) << "Hello World    " << endl;
            RemoveTrailingBlanks(test_file_path);
            buffer.str("");
            buffer << ifstream(test_file_path).rdbuf();
            Assert::AreEqual("Hello World\n"s, buffer.str());
            remove(test_file_path);
        }
	};
}
