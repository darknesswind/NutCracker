
#pragma once

#include <istream>
#include <vector>
#include <memory>

#include "BinaryReader.h"
#include "SqObject.h"

extern bool g_DebugMode;

// ****************************************************************************************************************************
class NutFunction
{
public:
	struct Instruction
	{
		union
		{
			int arg1;
			float arg1_float;
		};

		char op;
		char arg0, arg2, arg3;
	};

private:
	struct OuterValueInfo
	{
		int type;
		SqObject src;
		SqObject name;
	};

	struct LocalVarInfo
	{
		std::string name;
		int start_op;
		int end_op;
		int pos;
		bool foreachLoopState;
	};

	struct LineInfo
	{
		int line;
		int op;
	};

	int m_FunctionIndex;
	std::string m_SourceName;
	std::string m_Name;

	int m_StackSize;
	bool m_IsGenerator;
	bool m_GotVarParams;

	std::vector<SqObject> m_Literals;
	std::vector<std::string> m_Parameters;
	std::vector<OuterValueInfo> m_OuterValues;
	std::vector<LocalVarInfo> m_Locals;
	std::vector<LineInfo> m_LineInfos;
	std::vector<int> m_DefaultParams;
	std::vector<Instruction> m_Instructions;
	std::vector<NutFunction> m_Functions;

	friend class VMState;

	void DecompileStatement( VMState& state ) const;
	void DecompileJumpZeroInstruction( VMState& state, int arg0, int arg1 ) const;
	void DecompileJumpInstruction( VMState& state, int arg1 ) const;
	void DecompileDoWhileLoop( VMState& state, int jumpAddress ) const;
	void DecompileSwitchBlock( VMState& state ) const;
	void PrintOpcode( std::ostream& out, int pos, const Instruction& op ) const;

public:
	NutFunction()
	{
		m_FunctionIndex = -1;
	}

	void SetIndex( int index )
	{
		m_FunctionIndex = index;
	}

	void Load( BinaryReader& reader );

	void GenerateFunctionSource( int n, std::ostream& out, const std::string& name, const std::vector< std::string >& defaults ) const;
	void GenerateBodySource( int n, std::ostream& out ) const;

	void GenerateFunctionSource( int n, std::ostream& out ) const
	{	//disasemble a function on the fly
		std::vector< std::string > dummy;
		GenerateFunctionSource(n, out, m_Name, dummy);
	}

	bool DoCompare( const NutFunction& other, const std::string parentName, std::ostream& out ) const;

	const NutFunction* FindFunction( const std::string& name ) const;
	const NutFunction& GetFunction( int i ) const;
};


// ****************************************************************************************************************************
class NutScript
{
	NutFunction m_main;
	

public:
	void LoadFromFile( const char* fileName );
	void LoadFromStream( std::istream& in );

	const NutFunction& GetMain( void ) const	{ return m_main;	}
};
