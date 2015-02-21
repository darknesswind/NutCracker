
// #include <iostream>
// #include <sstream>
// #include <fstream>
 #include <QFile>

#include "Errors.h"
#include "NutScript.h"
#include "BinaryReader.h"

bool g_DebugMode = false;

// ***************************************************************************************************************
const NutFunction* NutFunction::FindFunction( const QString& name ) const
{
	QString localName, subName;
	int p = name.indexOf("::");
	if (p < 0)
	{
		localName = name;
	}
	else
	{
		localName = name.mid(0, p);
		subName = name.mid(p + 2);
	}

	if (localName.isEmpty())
		return NULL;

	int pos = -1;

	if (localName[0] >= '0' && localName[0] <= '9')
	{
		pos = localName.toInt();
	}
	else
	{
		for(size_t i = 0; i < m_Functions.size(); ++i)
			if (m_Functions[i].m_Name == name)
			{
				pos = (int)i;
				break;
			}
	}

	if (pos < 0 || pos >= (int)m_Functions.size())
		return NULL;

	if (subName.isEmpty())
		return &m_Functions[pos];
	else
		return m_Functions[pos].FindFunction(subName);
}


// ***************************************************************************************************************
const NutFunction& NutFunction::GetFunction( int i ) const
{
	if (i < 0 || i >= (int)m_Functions.size())
		throw Error("Invalid index in GetFunction.");

	return m_Functions[i];
}


// ***************************************************************************************************************
void NutFunction::Load( BinaryReader& reader )
{
	reader.ConfirmOnPart();

	reader.ReadSQStringObject(m_SourceName);
	reader.ReadSQStringObject(m_Name);

	reader.ConfirmOnPart();
	
	int nLiterals = reader.ReadInt32();
	int nParameters = reader.ReadInt32();
	int nOuterValues = reader.ReadInt32();
	int nLocalVarInfos = reader.ReadInt32();
	int nLineInfos = reader.ReadInt32();
	int nDefaultParams = reader.ReadInt32();
	int nInstructions = reader.ReadInt32();
	int nFunctions = reader.ReadInt32();
	
	reader.ConfirmOnPart();

	m_Literals.resize(nLiterals);	// 字面值、常量
	for(int i = 0; i < nLiterals; ++i)
		m_Literals[i].Load(reader);

	reader.ConfirmOnPart();
	
	m_Parameters.resize(nParameters);	
	for(int i = 0; i < nParameters; ++i)
		reader.ReadSQStringObject(m_Parameters[i]);

	reader.ConfirmOnPart();

	m_OuterValues.resize(nOuterValues);
	for(int i = 0; i < nOuterValues; ++i)
	{
		m_OuterValues[i].type = reader.ReadInt32();
		m_OuterValues[i].src.Load(reader);
		m_OuterValues[i].name.Load(reader);
	}

	reader.ConfirmOnPart();

	m_Locals.resize(nLocalVarInfos);
	for(int i = 0; i < nLocalVarInfos; ++i)
	{
		reader.ReadSQStringObject(m_Locals[i].name);
		m_Locals[i].pos = reader.ReadInt32();
		m_Locals[i].start_op = reader.ReadInt32();
		m_Locals[i].end_op = reader.ReadInt32();
		m_Locals[i].foreachLoopState = false;
	}

	reader.ConfirmOnPart();

	m_LineInfos.resize(nLineInfos);
	reader.Read(&(m_LineInfos.front()), nLineInfos * sizeof(LineInfo));

	reader.ConfirmOnPart();
	
	m_DefaultParams.resize(nDefaultParams);
	if (nDefaultParams)
	{
		reader.Read(&(m_DefaultParams.at(0)), nDefaultParams * sizeof(int));
	}

	reader.ConfirmOnPart();

	m_Instructions.resize(nInstructions);
	if (nInstructions)
	{
		reader.Read(&(m_Instructions.at(0)), nInstructions * sizeof(Instruction));
	}

	reader.ConfirmOnPart();

	m_Functions.resize(nFunctions);
	for(int i = 0; i < nFunctions; ++i)
	{
		m_Functions[i].Load(reader);
		m_Functions[i].SetIndex(i);
	}

	m_StackSize = reader.ReadInt32();
	m_IsGenerator = reader.ReadBool();
	m_VarParams = reader.ReadInt32();

	// Preprocess local variables
	int f = 0;

	for (LocalVarInfos::iterator i = m_Locals.begin(); i != m_Locals.end(); ++i)
	{
		if (i->name == "@ITERATOR@")
		{
			// Hack - sq will push three local variables for every foreach loop, last one is named @ITERATOR@,
			// we are searching for that triples and mark, because their scopes are badly set

			i->foreachLoopState = true;
			f = 2;
		}
		else if (f > 0)
		{
			i->foreachLoopState = true;
			--f;
		}
	}
}


// ***************************************************************************************************************
void NutScript::LoadFromFile( const QString& fileName )
{
	QFile file(fileName);

	if (!file.open(QFile::ReadOnly))
		throw Error("Unable to open file: \"%s\"", file.errorString().toLocal8Bit().data());

	try
	{
		LoadFromStream(&file);
	}
	catch(...)
	{
		file.close();
		throw;
	}

	file.close();
}


// ***************************************************************************************************************
void NutScript::LoadFromStream( QIODevice* in )
{
	BinaryReader reader(in);

	// Magic
	if (reader.ReadUInt16() != 0xFAFA) 
		throw BadFormatError();

	if (reader.ReadInt32() != 'SQIR') 
		throw BadFormatError();

	if (reader.ReadInt32() != sizeof(char))
		throw Error("NUT file compiled for different size of char that expected.");

	if (reader.ReadInt32() != sizeof(int))
		throw Error("NUT file compiled for different size of char that expected.");

	if (reader.ReadInt32() != sizeof(float))
		throw Error("NUT file compiled for different size of char that expected.");

	m_main.Load(reader);

	if (reader.ReadInt32() != 'TAIL') 
		throw BadFormatError();
}

bool Eq( const NutFunction::Instruction& a, const NutFunction::Instruction& b )
{
	if (a.op != b.op)
		return false;

	if (a.op == 3)	// OP_LOADFLOAT
	{
		return a.arg0 == b.arg0 && (std::abs(a.arg1_float - b.arg1_float) < 0.00001) && a.arg2 == b.arg2 && a.arg3 == b.arg3;
	}
	else
	{
		return a.arg0 == b.arg0 && a.arg1 == b.arg1 && a.arg2 == b.arg2 && a.arg3 == b.arg3;
	}
}

// ***************************************************************************************************************
bool NutFunction::DoCompare( const NutFunction& other, const QString& parentName, QTextStream& out ) const
{
	bool functionsOk = true;
	bool literalsOk = true;
	bool parametersOk = true;
	bool outerValuesOk = true;
	bool instructionsOk = true;


	QString name;
	
	if (!parentName.isEmpty())
		name.append(parentName).append("::");

	if (!m_Name.isEmpty())
		name.append(m_Name);
	else
		name.append('[').append(m_FunctionIndex).append(']');

	if (m_Functions.size() != other.m_Functions.size())
	{
		out << name << ':' << endl;
		out << "    - different number of subfunctions: " << m_Functions.size() << " to " << other.m_Functions.size() << endl;
		functionsOk = false;
	}
	else
	{
		for(size_t i = 0; i < m_Functions.size(); ++i)
			if (!m_Functions[i].DoCompare(other.m_Functions[i], name, out))
				functionsOk = false;

		out << name << ':' << endl;
	}

	if (m_Literals.size() != other.m_Literals.size())
	{
		out << "    - different number of literals: " << m_Literals.size() << " to " << other.m_Literals.size() << endl;
		literalsOk = false;
	}
	else
	{
		for(size_t i = 0; i < m_Literals.size(); ++i)
			if (m_Literals[i] != other.m_Literals[i])
			{
				out << "    - different literals @ " << i << ": \"" << m_Literals[i] << "\" and \"" << other.m_Literals[i] << "\"" << endl;
				literalsOk = false;
			}
	}

	if (m_Parameters.size() != other.m_Parameters.size())
	{
		out << "    - different number of parameters: " << m_Parameters.size() << " to " << other.m_Parameters.size() << endl;
		parametersOk = false;
	}

	if (m_OuterValues.size() != other.m_OuterValues.size())
	{
		out << "    - different number of outer values: " << m_OuterValues.size() << " to " << other.m_OuterValues.size() << endl;
		outerValuesOk = false;
	}
	else
	{
		for(size_t i = 0; i < m_OuterValues.size(); ++i)
			if (m_OuterValues[i].src != other.m_OuterValues[i].src)
			{
				out << "    - different outer value source @ " << i << ": " << m_OuterValues[i].src << " and " << other.m_OuterValues[i].src << endl;
				outerValuesOk = false;
			}
	}

	if (m_Instructions.size() != other.m_Instructions.size())
	{
		out << "    - different number of instructions: " << m_Instructions.size() << " to " << other.m_Instructions.size() << endl;
		instructionsOk = false;
	}
	
	
	for(size_t i = 0, j = 0; i < m_Instructions.size() && j < other.m_Instructions.size(); ++i, ++j)
	{
		const Instruction& a = m_Instructions[i];
		const Instruction& b = other.m_Instructions[j];

		if (!Eq(a, b))
		{
			instructionsOk = false;

			if ((i + 1) < m_Instructions.size() && Eq(m_Instructions[i + 1], b))
			{
				out << "    - instruction missing in second @ [" << i  << "]<->[" << j << "]:" << endl;
				out << "          ";
				PrintOpcode(out, i, a);
				out << endl;
				--j;
			}
			else if ((j + 1) < other.m_Instructions.size() && Eq(other.m_Instructions[j + 1], a))
			{
				out << "    - instruction missing in first @ [" << i  << "]<->[" << j << "]:" << endl;
				out << "          ";
				other.PrintOpcode(out, i, b);
				out << endl;
				--i;
			}
			else
			{
				out << "    - different instructions @ [" << i  << "]<->[" << j << "]:" << endl;
				
				out << "          ";
				PrintOpcode(out, i, a);
				out << endl;

				out << "          ";
				other.PrintOpcode(out, i, b);
				out << endl;

				if (a.op != b.op)
					break;
			}
		}
	}

	return functionsOk && literalsOk && parametersOk && outerValuesOk && instructionsOk;
}