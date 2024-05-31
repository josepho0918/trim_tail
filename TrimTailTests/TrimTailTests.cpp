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
            ofstream test_file;
            ifstream file;

            test_file.open(test_file_path);
            test_file << "    " << endl;
            test_file.close();

            file.open(test_file_path);
            Assert::IsTrue(HasTrailingBlanks(file));
            file.close();

            test_file.open(test_file_path);
            test_file << "Hello World    " << endl;
            test_file.close();

            file.open(test_file_path);
            Assert::IsTrue(HasTrailingBlanks(file));
            file.close();

            remove(test_file_path);

            test_file.open(test_file_path);
            test_file << "Hello World" << endl;
            test_file.close();

            file.open(test_file_path);
            Assert::IsFalse(HasTrailingBlanks(file));
            file.close();

            remove(test_file_path);
        }

        TEST_METHOD(TestGetCleanLine)
        {
            ofstream test_file;
            ifstream file;
            optional<string> line;

            test_file.open(test_file_path);
            test_file << "Hello World    " << endl;
            test_file.close();

            file.open(test_file_path);
            line = GetCleanLine(file);
            Assert::IsTrue(line.has_value());
            Assert::AreEqual("Hello World"s, line.value());
            file.close();

            remove(test_file_path);

            test_file.open(test_file_path);
            test_file << "Hello World\t\t" << endl;
            test_file.close();

            file.open(test_file_path);
            line = GetCleanLine(file);
            Assert::IsTrue(line.has_value());
            Assert::AreEqual("Hello World"s, line.value());
            file.close();

            remove(test_file_path);
        }

        TEST_METHOD(TestRemoveTrailingBlanks)
        {
            ofstream test_file;
            ifstream file;
            stringstream buffer;

            test_file.open(test_file_path);
            test_file << "Hello World    ";
            test_file.close();

            RemoveTrailingBlanks(test_file_path);

            file.open(test_file_path);
            buffer.str("");
            buffer << file.rdbuf();
            Assert::AreEqual("Hello World"s, buffer.str());
            file.close();

            remove(test_file_path);

            test_file.open(test_file_path);
            test_file << "Hello World    " << endl;
            test_file.close();

            RemoveTrailingBlanks(test_file_path);

            file.open(test_file_path);
            buffer.str("");
            buffer << file.rdbuf();
            Assert::AreEqual("Hello World\n"s, buffer.str());
            file.close();

            remove(test_file_path);
        }
	};
}
