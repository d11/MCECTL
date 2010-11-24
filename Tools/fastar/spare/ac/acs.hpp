/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef ACS_HPP
#define ACS_HPP
#define IN_ACS_HPP

#include "pm-ac.hpp"
#include "acmopt.hpp"
#include "acmfail.hpp"
#include "acmkmpfl.hpp"

// A couple of Aho-Corasick pattern matchers.
typedef PMAC<ACMachineOpt> PMACOpt;
typedef PMAC<ACMachineFail> PMACFail;
typedef PMAC<ACMachineKMPFail> PMACKMPFail;

#undef IN_ACS_HPP
#endif

