//**************************************************************************
//**
//**	##   ##    ##    ##   ##   ####     ####   ###     ###
//**	##   ##  ##  ##  ##   ##  ##  ##   ##  ##  ####   ####
//**	 ## ##  ##    ##  ## ##  ##    ## ##    ## ## ## ## ##
//**	 ## ##  ########  ## ##  ##    ## ##    ## ##  ###  ##
//**	  ###   ##    ##   ###    ##  ##   ##  ##  ##       ##
//**	   #    ##    ##    #      ####     ####   ##       ##
//**
//**	$Id: vcc.h 4212 2010-04-03 20:13:36Z dj_jl $
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

#ifndef __VCC_H__
#define __VCC_H__

// HEADER FILES ------------------------------------------------------------

#define OPCODE_STATS

#include "core.h"
#include "common.h"
#include "../../engine/vc_location.h"
#include "../../engine/vc_type.h"
#include "../../engine/vc_member.h"
#include "../../engine/progdefs.h"
#include "../../engine/vc_field.h"
#include "../../engine/vc_property.h"
#include "../../engine/vc_method.h"
#include "../../engine/vc_constant.h"
#include "../../engine/vc_struct.h"
#include "../../engine/vc_state.h"
#include "../../engine/vc_class.h"
#include "../../engine/vc_package.h"
#include "../../engine/vc_emit_context.h"
#include "../../engine/vc_expr_base.h"
#include "../../engine/vc_expr_literal.h"
#include "../../engine/vc_expr_unary_binary.h"
#include "../../engine/vc_expr_cast.h"
#include "../../engine/vc_expr_type.h"
#include "../../engine/vc_expr_field.h"
#include "../../engine/vc_expr_array.h"
#include "../../engine/vc_expr_invoke.h"
#include "../../engine/vc_expr_assign.h"
#include "../../engine/vc_expr_local.h"
#include "../../engine/vc_expr_misc.h"
#include "../../engine/vc_statement.h"
#include "../../engine/vc_error.h"
#include "../../engine/vc_lexer.h"
#include "../../engine/vc_modifiers.h"
#include "../../engine/vc_parser.h"

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

int dprintf(const char *text, ...);

char *va(const char *text, ...) __attribute__ ((format(printf, 1, 2)));

VStream* OpenFile(const VStr& Name);

// PUBLIC DATA DECLARATIONS ------------------------------------------------

extern bool						GBigEndian;

#endif
