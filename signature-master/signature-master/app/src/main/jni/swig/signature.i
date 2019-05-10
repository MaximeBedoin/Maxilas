/* File : signature.i */
%module sign

%{
#include "signature.h"
#include "Obs.h"
#include "gmm.h"
#include "hmm.h"
%}

/* Let's just grab the original header file here */
%include "Obs.h"
%include "signature.h"
%include "gmm.h"
%include "hmm.h"
