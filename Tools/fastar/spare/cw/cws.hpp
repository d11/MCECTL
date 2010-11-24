/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWS_HPP
#define CWS_HPP
#define IN_CWS_HPP

#include "pm-cw.hpp"
#include "cwshnla.hpp"
#include "cwshnorm.hpp"
#include "cwshwbm.hpp"
#include "cwshopt.hpp"
#include "cwshrla.hpp"
#include "cwshnaiv.hpp"

// A couple of Commentz-Walter pattern matchers.
typedef PMCW<CWShiftRLA> PMCWRLA;
typedef PMCW<CWShiftOpt> PMCWOpt;
typedef PMCW<CWShiftNorm> PMCWNorm;
typedef PMCW<CWShiftWBM> PMCWWBM;
typedef PMCW<CWShiftNLA> PMCWNLA;
typedef PMCW<CWShiftNaive> PMCWNaive;

#undef IN_CWS_HPP
#endif

