
#pragma once

#include <vector>
#include <memory>
#include <QTextStream>
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

		Opcode op;
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
		QString name;
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
	QString m_SourceName;
	QString m_Name;

	int m_StackSize;
	bool m_IsGenerator;
	int m_VarParams;

	std::vector<SqObject> m_Literals;
	std::vector<QString> m_Parameters;
	std::vector<OuterValueInfo> m_OuterValues;
	LocalVarInfos m_Locals;
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
	void PrintOpcode( QTextStream& out, int pos, const Instruction& op ) const;

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

	void GenerateFunctionSource( int n, QTextStream& out, const QString& name, const std::vector< QString >& defaults ) const;
	void GenerateBodySource( int n, QTextStream& out ) const;

	void GenerateFunctionSource( int n, QTextStream& out ) const
{	//disasemble a function on the fly
		std::vector< QString > dummy;
		GenerateFunctionSource(n, out, m_Name, dummy);
	}

	bool DoCompare( const NutFunction& other, const QString& parentName, QTextStream& out ) const;

	const NutFunction* FindFunction( const QString& name ) const;
	const NutFunction& GetFunction( int i ) const;
};


// ****************************************************************************************************************************
class NutScript
{
	NutFunction m_main;
	

public:
	void LoadFromFile( const QString& );
	void LoadFromStream( QIODevice* in );

	const NutFunction& GetMain( void ) const	{ return m_main;	}
};
