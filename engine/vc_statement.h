//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: vc_statement.h 3810 2008-10-12 20:29:47Z dj_jl $
//**
//**	Copyright (C) 1999-2006 Jānis Legzdiņš
//**
//**	This program is free software; you can redistribute it and/or
//**  modify it under the terms of the GNU General Public License
//**  as published by the Free Software Foundation; either version 2
//**  of the License, or (at your option) any later version.
//**
//**	This program is distributed in the hope that it will be useful,
//**  but WITHOUT ANY WARRANTY; without even the implied warranty of
//**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//**  GNU General Public License for more details.
//**
//**************************************************************************

class VStatement
{
public:
	TLocation		Loc;

	VStatement(const TLocation&);
	virtual ~VStatement();
	virtual bool Resolve(VEmitContext&) = 0;
	virtual void DoEmit(VEmitContext&) = 0;
	void Emit(VEmitContext&);
};

class VEmptyStatement : public VStatement
{
public:
	VEmptyStatement(const TLocation&);
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VIf : public VStatement
{
public:
	VExpression*	Expr;
	VStatement*		TrueStatement;
	VStatement*		FalseStatement;

	VIf(VExpression*, VStatement*, const TLocation&);
	VIf(VExpression*, VStatement*, VStatement*, const TLocation&);
	~VIf();
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VWhile : public VStatement
{
public:
	VExpression*		Expr;
	VStatement*			Statement;

	VWhile(VExpression*, VStatement*, const TLocation&);
	~VWhile();
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VDo : public VStatement
{
public:
	VExpression*		Expr;
	VStatement*			Statement;

	VDo(VExpression*, VStatement*, const TLocation&);
	~VDo();
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VFor : public VStatement
{
public:
	TArray<VExpression*>	InitExpr;
	VExpression*			CondExpr;
	TArray<VExpression*>	LoopExpr;
	VStatement*				Statement;

	VFor(const TLocation&);
	~VFor();
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VForeach : public VStatement
{
public:
	VExpression*		Expr;
	VStatement*			Statement;

	VForeach(VExpression*, VStatement*, const TLocation&);
	~VForeach();
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VSwitch : public VStatement
{
public:
	struct VCaseInfo
	{
		vint32			Value;
		VLabel			Address;
	};

	VExpression*		Expr;
	TArray<VCaseInfo>	CaseInfo;
	VLabel				DefaultAddress;
	TArray<VStatement*>	Statements;
	bool				HaveDefault;

	VSwitch(const TLocation&);
	~VSwitch();
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VSwitchCase : public VStatement
{
public:
	VSwitch*		Switch;
	VExpression*	Expr;
	vint32			Value;
	vint32			Index;

	VSwitchCase(VSwitch*, VExpression*, const TLocation&);
	~VSwitchCase();
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VSwitchDefault : public VStatement
{
public:
	VSwitch*		Switch;

	VSwitchDefault(VSwitch*, const TLocation&);
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VBreak : public VStatement
{
public:
	VBreak(const TLocation&);
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VContinue : public VStatement
{
public:
	VContinue(const TLocation&);
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VReturn : public VStatement
{
public:
	VExpression*		Expr;
	int					NumLocalsToClear;

	VReturn(VExpression*, const TLocation&);
	~VReturn();
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VExpressionStatement : public VStatement
{
public:
	VExpression*		Expr;

	VExpressionStatement(VExpression*);
	~VExpressionStatement();
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VLocalVarStatement : public VStatement
{
public:
	VLocalDecl*		Decl;

	VLocalVarStatement(VLocalDecl*);
	~VLocalVarStatement();
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

class VCompound : public VStatement
{
public:
	TArray<VStatement*>		Statements;

	VCompound(const TLocation&);
	~VCompound();
	bool Resolve(VEmitContext&);
	void DoEmit(VEmitContext&);
};

