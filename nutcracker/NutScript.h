#pragma once
#include "SqObject.h"
#include "Expressions.h"
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

		Opcode op;
		char arg0, arg2, arg3;
	};

private:
	struct OuterValueInfo
	{
		enum SQOuterType
		{
			otLOCAL = 0,
			otOUTER = 1
		};
		SQOuterType type;
		SqObject src;
		SqObject name;
	};

	struct LocalVarInfo
	{
		LString name;
		int start_op;
		int end_op;
		int pos;
		bool foreachLoopState;
	};
	typedef std::vector<LocalVarInfo> LocalVarInfos;

	struct LineInfo
	{
		int line;
		int op;
	};

	int m_FunctionIndex;
	LString m_SourceName;
	LString m_Name;

	int m_StackSize;
	bool m_IsGenerator;
	int m_VarParams;

	std::vector<SqObject> m_Literals;
	std::vector<LString> m_Parameters;
	std::vector<OuterValueInfo> m_OuterValues;
	LocalVarInfos m_Locals;
	std::vector<LineInfo> m_LineInfos;
	std::vector<int> m_DefaultParams;
	std::vector<Instruction> m_Instructions;
	std::vector<NutFunction> m_Functions;

	friend class VMState;

	void DecompileStatement( VMState& state ) const;
	void DecompileJumpZeroInstruction( VMState& state, int arg0, int arg1 ) const;
	bool DecompileLoopJumpInstruction(VMState& state, ExpressionPtr condPtr, int offset) const;
	void DecompileJumpInstruction( VMState& state, int arg1 ) const;
	void DecompileDoWhileLoop( VMState& state, int jumpAddress ) const;
	void DecompileSwitchBlock( VMState& state ) const;
	void DecompileAppendArray( VMState& state, int arg0, int arg1, AppendArrayType arg2, int arg3) const;
	void DecompileJCMP( VMState& state, int end, int offsetIp, int begin, int cmpOp) const;

	void PrintOpcode( std::wostream& out, int pos, const Instruction& op ) const;

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

	void GenerateFunctionSource( int n, std::wostream& out, const LString& name, const std::vector< LString >& defaults ) const;
	void GenerateBodySource( int n, std::wostream& out ) const;

	void GenerateFunctionSource( int n, std::wostream& out ) const
{	//disasemble a function on the fly
		std::vector< LString > dummy;
		GenerateFunctionSource(n, out, m_Name, dummy);
	}

	bool DoCompare( const NutFunction& other, const LString& parentName, std::wostream& out ) const;

	const NutFunction* FindFunction( const LString& name ) const;
	const NutFunction& GetFunction( int i ) const;
};


// ****************************************************************************************************************************
class NutScript
{
	NutFunction m_main;
	

public:
	void LoadFromFile( const char* );
	void LoadFromStream( LFile& in );

	const NutFunction& GetMain( void ) const	{ return m_main;	}
};
