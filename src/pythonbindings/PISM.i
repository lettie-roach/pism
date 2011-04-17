// Copyright (C) 2011 David Maxwell
//
// This file is part of PISM.
//
// PISM is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// PISM is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License
// along with PISM; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

%module cpp

%{
// The material in this section is included verbatim in the C++ source code generated by SWIG.
// The necessary header files required to compile must be included.
// This list is NOT the whole set of headers being wrapped; it is just the list of includes that 
// draws in all the other needed includes as well. See the end of this file for the list
// of PISM headers being wrapped.

#include "PISMIO.hh"
#include "Timeseries.hh"
#include "exactTestsIJ.h"
#include "stressbalance/SSAFEM.hh"
#include "stressbalance/SSAFD.hh"
#include "pism_python.hh"
#include "iceModel.hh"
#include "SNESProblem.hh"
%}

// SWIG doesn't know about __atribute__ (used, e.g. in pism_const.hh) so we make it ignore it
#define __attribute__(x) 

// Include petsc4py.i so that we get support for automatic handling of PetscErrorCode return values
%include "petsc4py/petsc4py.i"

%include "pism_exception.i"

// Automatic conversions between std::string and python string arguments and return values
%include std_string.i
// Conversions between python lists and certain std::vector's
%include std_vector.i
// Conversions between python lists and certain std::vector's
%include std_set.i



namespace std {
   %template(IntVector) vector<int>;
   %template(DoubleVector) vector<double>;
   %template(StringVector) vector<string>;
   %template(StringSet) set<string>;
}
// Tell SWIG that PetscReal and a double are the same thing so that we can reuse
// the DoubleVector template declared above.  I don't know why SWIG doesn't figure this out.
// Without the typedef below, if I add %template(PestsRealVector) vector<PetscReal>; then
// the SWIG wrappers contain two attempts to wrap a vector<double>.  If I leave out the 
// PetscRealVector template and the typedef, then SWIG doesn't do the type conversion between
// Python lists and vector<PetscReal> arguments.   The typedef below is the WRONG thing to do
// because the actual definition of a PetscReal (from petscmath.h) depends on a compiler flag.
// So this needs to get fixed properly at some point.  Do I have access to the Petsc compiler flags?
// If so, maybe just copy the code block from petscmath.h here for the PetscReal typedefs.
typedef double PetscReal; // YUCK.
typedef int PetscInt; // YUCK.


// Why did I include this?
%include "cstring.i"

// Lots of Pism objects are returned by passing a reference to a pointer. Use 
//
// %Pism_pointer_reference_typemaps(MyType)
// 
// to declare typemaps so that an argument MyType *&OUTPUT should be treated
// as  an output (so no input variable shows up on the Python side, and
// the output shows up as an output variable).  To apply the typemap
// to all arguments of this type, use 
// %apply MyType *& OUTPUT { MyType *&}
// or use %Pism_pointer_reference_is_always_ouput(MyType) in the first place
%define %Pism_pointer_reference_typemaps(TYPE)
%typemap(in, numinputs=0,noblock=1) TYPE *& OUTPUT (TYPE *temp) {
    $1 = &temp;
}
%typemap(argout,noblock=1) TYPE *& OUTPUT
{
    %append_output(SWIG_NewPointerObj(%as_voidptr(*$1), $*descriptor, 0 | %newpointer_flags));
};
%enddef
%define %Pism_pointer_reference_is_always_output(TYPE)
%Pism_pointer_reference_typemaps(TYPE);
%apply TYPE *& OUTPUT { TYPE *&}
%enddef


%define %Pism_reference_output_typemaps(TYPE)
%typemap(in, numinputs=0,noblock=1) TYPE & OUTPUT (TYPE temp) {
    $1 = &temp;
}
%typemap(argout,noblock=1) TYPE & OUTPUT
{
    %append_output(SWIG_NewPointerObj(%as_voidptr($1), $descriptor, 0 | %newpointer_flags));
};
%enddef

%define %Pism_reference_is_always_output(TYPE)
%Pism_reference_output_typemaps(TYPE);
%apply TYPE & OUTPUT { TYPE &}
%enddef


%typemap(in, numinputs=0,noblock=1) bool & OUTPUT (bool temp = false) {
    $1 = &temp;
}
%typemap(argout,noblock=1) bool & OUTPUT
{
    %append_output(SWIG_From(bool)(*$1));
};


%typemap(in, numinputs=0,noblock=1) PETScInt & OUTPUT (PETScInt temp) {
    $1 = &temp;
}
%typemap(argout,noblock=1) PETScInt & OUTPUT
{
    %append_output(SWIG_From(int)(*$1));
};

%typemap(in, numinputs=0,noblock=1) string& result (string temp) {
    $1 = &temp;
}

%typemap(in, numinputs=0,noblock=1) std::string& OUTPUT (std::string temp) {
    $1 = &temp;
}
%typemap(argout,noblock=1) std::string & OUTPUT
{
    %append_output(SWIG_FromCharPtr((*$1).c_str()));
}
%apply std::string &OUTPUT { std::string &result}

%typemap(in, numinputs=0,noblock=1) vector<PetscInt> & OUTPUT (vector<PetscInt> temp) {
    $1 = &temp;
}
%typemap(argout,noblock=1) vector<PetscInt> & OUTPUT
{
    int len;
    len = $1->size();
    $result = PyList_New(len);
     int i;
     for(i=0; i<len; i++)
     {
         PyList_SetItem($result, i, PyInt_FromLong((*$1)[i]));
     }
}

%typemap(in, numinputs=0,noblock=1) vector<PetscReal> & OUTPUT (vector<PetscReal> temp) {
    $1 = &temp;
}
%typemap(argout,noblock=1) vector<PetscReal> & OUTPUT
{
    int len;
    len = $1->size();
    $result = PyList_New(len);
     int i;
     for(i=0; i<len; i++)
     {
         PyList_SetItem($result, i, PyFloat_FromDouble((*$1)[i]));
     }
}


%apply vector<PetscInt> & OUTPUT {vector<PetscInt> &result};
%apply vector<PetscReal> & OUTPUT {vector<PetscReal> &result};
%apply vector<string> & OUTPUT {vector<string> & result};
 
%apply int &OUTPUT {int &result}

%apply PetscInt & OUTPUT {PetscInt & result};
%apply PetscReal & OUTPUT {PetscReal & result};
%apply bool & OUTPUT {bool & is_set, bool & result, bool & flag};


%Pism_pointer_reference_is_always_output(IceModelVec2S)
%Pism_pointer_reference_is_always_output(IceModelVec2V)


// These methods are called from PISM.options.
%rename PISMOptionsInt _optionsInt;
%rename PISMOptionsReal _optionsReal;
%rename PISMOptionsString _optionsString;
%rename PISMOptionsIntArray _optionsIntArray;
%rename PISMOptionsRealArray _optionsRealArray;
%rename PISMOptionsStringArray _optionsStringArray;
%rename PISMOptionsList _optionsList;
%rename PISMOptionsIsSet optionsIsSet;

// The varargs to verbPrintf aren't making it through from python.  But that's ok: we'd like
// to extend the printf features of verbPrintf to include python's formatting for objects.
// So we rename verbPrintf here and call it (without any varargs) from a python verbPrintf.
%rename verbPrintf _verbPrintf;


// Shenanigans to allow python indexing to get at IceModelVec entries.  I couldn't figure out a more
// elegant solution.
%extend IceModelVec2S
{
    PetscReal getitem(int i, int j)
    {
        return (*($self))(i,j);
    }

    void setitem(int i, int j, PetscReal val)
    {
        (*($self))(i,j) = val;
    }

    %pythoncode {
    def __getitem__(self,*args):
        return self.getitem(args[0][0],args[0][1])
    def __setitem__(self,*args):
        if(len(args)==2):
            self.setitem(args[0][0],args[0][1],args[1])
        else:
            raise ValueError("__setitem__ requires 2 arguments; received %d" % len(args));
    }
};

%extend IceModelVec2V
{
    PISMVector2 &getitem(int i, int j)
    {
        return (*($self))(i,j);
    }

    void setitem(int i, int j, PISMVector2 val)
    {
        (*($self))(i,j) = val;
    }

    void setitem(int i, int j, double u, double v)
    {
        (*($self))(i,j).u = u;
        (*($self))(i,j).v = v;
    }

    %pythoncode {
    def __getitem__(self,*args):
        return self.getitem(args[0][0],args[0][1])
    def __setitem__(self,*args):
        if(len(args)==2):
            i=args[0][0]; j=args[0][1]
            val = args[1];
            if(isinstance(val,list) and len(val)==2):
                self.setitem(i,j,val[0],val[1])
            else:
                self.setitem(i,j,val)
        else:
            raise ValueError("__setitem__ requires 2 arguments; received %d" % len(args));
    }
};

%extend Timeseries
{
    %ignore operator[];
    double getitem(unsigned int i)
    {
        return (*$self)[i];
    }
    
    %pythoncode {
    def __getitem__(self,*args):
        return self.getitem(*args)
    }
};


%extend IceGrid
{
    %pythoncode {
    def points(self):
        for i in xrange(self.xs,self.xs+self.xm):
            for j in xrange(self.ys,self.ys+self.ym):
                yield (i,j)
    def points_with_ghosts(self,nGhosts=0):
        for i in xrange(self.xs-nGhosts,self.xs+self.xm+nGhosts):
            for j in xrange(self.ys-nGhosts,self.ys+self.ym+nGhosts):
                yield (i,j)
    }

}

// FIXME: the the following code blocks there are explicit calls to Py????_Check.  There seems to 
// be a more elegant solution using SWIG_From(int) and so forth that I'm not familiar with.  The
// following works for now.

// The SWIG builtin typecheck for a const char [] (used, e.g., with overloaded methods) checks that 
// the string is zero length. So we have this bug fix from SWIG developer William Fulton here.
%typemap(typecheck,noblock=1,precedence=SWIG_TYPECHECK_STRING, fragment="SWIG_AsCharPtrAndSize") const char[] {
 int res = SWIG_AsCharPtrAndSize($input, 0, NULL, 0);
 $1 = SWIG_CheckState(res);
}
// Apparently petsc4py doesn't make any typemaps for MPIInts, which PISM uses repeatedly.
%apply int {PetscMPIInt};


// Support for nc_types (e.g. NC_BYTE, etc).  In NetCDF3, an nc_type is an enum, and in 
// NetCDF4 it is typedef'ed to be an int. The enums pose a small problem in C++ because
// you can't assign an arbitrary integer to an enum without a cast, and you can't assume
// even in C that an enum is an int, so you have to be careful about pointers to enums
// versus pointers to ints.  Moreover, I don't know how to grab the definitions from
// netcdf.h here without wrapping everything in the file.
//
// So: we assume that nc_type is an enum.  On input, we force the python input to be an int,
// use pointers to the int variable where needed, and then do a static cast to shove the int
// into an nc_type.  This procedure works correctly if nc_type is an int instead of an enum.
// As for the allowed values, we copy the defines from (NetCDF4) netcdf.h.  No typechecking
// is being done to ensure that a python int on input is a valid nc_type, which isn't good.
// In particular, the allowed values are different in NetCDF4 vs. NetCDF3 (there are more of them.)
// A constraint check to the minimal set of NetCDF3 types would be the right thing to do. (FIXME)
%typemap(in) nc_type (int tmp){
    SWIG_AsVal(int)($input,&tmp);
    $1 = static_cast<nc_type>(tmp);
}
%typemap(typecheck) nc_type {
    $1 = PyInt_Check($input);
}
// Copied straight from netcdf.h  I wonder if there is a more elegant way to do this, without dragging in everything
// else from netcdf.h
#define	NC_NAT 	        0	/* NAT = 'Not A Type' (c.f. NaN) */
#define	NC_BYTE         1	/* signed 1 byte integer */
#define	NC_CHAR 	2	/* ISO/ASCII character */
#define	NC_SHORT 	3	/* signed 2 byte integer */
#define	NC_INT 	        4	/* signed 4 byte integer */
#define NC_LONG         NC_INT  /* deprecated, but required for backward compatibility. */
#define	NC_FLOAT 	5	/* single precision floating point number */
#define	NC_DOUBLE 	6	/* double precision floating point number */
#define	NC_UBYTE 	7	/* unsigned 1 byte int */
#define	NC_USHORT 	8	/* unsigned 2-byte int */
#define	NC_UINT 	9	/* unsigned 4-byte int */
#define	NC_INT64 	10	/* signed 8-byte int */
#define	NC_UINT64 	11	/* unsigned 8-byte int */
#define	NC_STRING 	12	/* string */



// Tell SWIG that the following variables are truly constant
%immutable PISM_Revision;
%immutable PISM_DefaultConfigFile;

// Deal with 'print' being a python keyword
%extend NCVariable
{
    %rename(printinfo) print;
}

%extend grid_info
{
    %rename(printinfo) print;
}


%include "stressbalance/SNESProblem.hh"
%template(SNESScalarProblem) SNESProblem<1,PetscScalar>;
%template(SNESVectorProblem) SNESProblem<2,PISMVector2>;

// Now the header files for the PISM source code we want to wrap.
// By default, SWIG does not wrap stuff included from an include file,
// (which is good!) so we need to list every file containing a class
// we want to wrap, including base classes if we want access to base class
// methods
%include "grid.hh"
%include "NCVariable.hh"
%include "pism_const.hh"
%include "Timeseries.hh"
%include "iceModelVec.hh"
%include "PISMVars.hh"
%include "NCTool.hh"
%include "PISMIO.hh"
%include "PISMDiagnostic.hh"
%include "PISMComponent.hh"
%include "materials.hh"
%include "LocalInterpCtx.hh"
%include "rheology/flowlaws.hh"
%include "EnthalpyConverter.hh"
%include "stressbalance/ShallowStressBalance.hh"

%include "iceModel.hh"

// The template used in SSA.hh needs to be instantiated in SWIG before
// it is used.
%template(PISMDiag_SSA) PISMDiag<SSA>;
%include "stressbalance/SSA.hh"

%include "stressbalance/SSAFEM.hh"
%include "stressbalance/SSAFD.hh"
%include "pism_python.hh"




// Tell SWIG that input arguments of type double * are to be treated as return values,
// and that int return values are to be error checked as per a PetscErrorCode.
%apply double *OUTPUT  {double *};
%typemap(out,noblock=1) int {
PyPetsc_ChkErrQ($1); %set_output(VOID_Object);
}
%include "exactTestsIJ.h"
// FIXME! I don't know how to undo the output typemap.
// %typemap(out,noblock=1) int = PREVIOUS;
%clear double *;
