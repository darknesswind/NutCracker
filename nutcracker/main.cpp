#include "NutScript.h"
#include <iostream>
#include <fstream>

const char* version = "0.02";
const char* nutVersion = "2.2.4";

void Usage( void )
{
	std::cout << "NutCracker Squirrel script decompiler, ver " << version << std::endl;
	std::cout << "for binary nut file version " << nutVersion << std::endl;
	std::cout << std::endl;
	std::cout << "  Usage:" << std::endl;
	std::cout << "    nutcracker [options] <file to decompile>" << std::endl;
	std::cout << "    nutcracker -cmp <file1> <file2>" << std::endl;
	std::cout << std::endl;
	std::cout << "  Options:" << std::endl;
	std::cout << "   -h         Display usage info" << std::endl;
	std::cout << "   -cmp       Compare two binary files" << std::endl;
	std::cout << "   -d <name>  Display debug decompilation for function" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
}


int Compare( const char* file1, const char* file2, bool general )
{
	try
	{
		NutScript s1, s2;
		s1.LoadFromFile(file1);
		s2.LoadFromFile(file2);

		if (general)
		{
			QTextStream stream;
			bool result = s1.GetMain().DoCompare(s2.GetMain(), "", stream);

			if (result)
				std::cout << "[         ]";
			else
				std::cout << "[! ERROR !]";

			std::cout << " : " << file1 << std::endl;

			return result ? 0 : -1;
		}
		else
		{
			QTextStream stream(stdout);
			bool result = s1.GetMain().DoCompare(s2.GetMain(), "", stream);
			std::cout << std::endl << "Result: " << (result ? "Ok" : "ERROR") << std::endl;

			return result ? 0 : -1;
		}
	}
	catch( std::exception& ex )
	{
		std::cout << "Error: " << ex.what() << std::endl;
		return -1;
	}
}

static QString g_textBuff;
void DebugFunctionPrint( const NutFunction& function )
{
	g_DebugMode = true;
	g_textBuff.clear();
	QTextStream stream(&g_textBuff);
	stream.setCodec("UTF-8");
	function.GenerateFunctionSource(0, stream);
	std::cout << g_textBuff.toLocal8Bit().data();
}

int Decompile( const char* file, const char* debugFunction )
{
	try
	{
		NutScript script;
		script.LoadFromFile(QString::fromLocal8Bit(file));

		if (debugFunction)
		{
			if (0 == strcmp(debugFunction, "main"))
			{
				DebugFunctionPrint(script.GetMain());
				return 0;
			}
			else
			{
				const NutFunction* func = script.GetMain().FindFunction(debugFunction);
				if (!func)
				{
					std::cout << "Unable to find function \"" << debugFunction << "\"." << std::endl;
					return -2;
				}

				DebugFunctionPrint(*func);
				return 0;
			}
		}

		g_textBuff.clear();
		QTextStream stream(&g_textBuff);
		script.GetMain().GenerateBodySource(0, stream);
		std::cout << g_textBuff.toLocal8Bit().data();
	}
	catch( std::exception& ex )
	{
		std::cout << g_textBuff.toLocal8Bit().data();
		std::cout << "Error: " << ex.what() << std::endl;
		return -1;
	}
	return 0;
}


int main( int argc, char* argv[] )
{
	const char* debugFunction = NULL;

	for( int i = 1; i < argc; ++i)
	{
		if (0 == _stricmp(argv[i], "-h"))
		{
			Usage();
			return 0;
		}
		else if (0 == _stricmp(argv[i], "-d"))
		{
			if ((argc - i) < 2)
			{
				Usage();
				return -1;
			}
			debugFunction = argv[i + 1];
			i += 1;
		}
		else if (0 == _stricmp(argv[i], "-cmp"))
		{
			if ((argc - i) < 3)
			{
				Usage();
				return -1;
			}
			return Compare(argv[i + 1], argv[i + 2], false);
		}
		else if (0 == _stricmp(argv[i], "-cmpg"))
		{
			if ((argc - i) < 3)
			{
				Usage();
				return -1;
			}
			return Compare(argv[i + 1], argv[i + 2], true);
		}
		else
		{
			int res = Decompile(argv[i], debugFunction);
			system("pause");
			return res;
		}
	}


	Usage();
	return -1;
}
