///////////////////////////////////////////////////////////////////////////////
//
// File SteadyDiffusionVaryingB.cpp
//
// For more information, please see: http://www.nektar.info
//
// The MIT License
//
// Copyright (c) 2006 Division of Applied Mathematics, Brown University (USA),
// Department of Aeronautics, Imperial College London (UK), and Scientific
// Computing and Imaging Institute, University of Utah (USA).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// Description: Steady diffusion with a non-constant magnetic field solve routines
//
///////////////////////////////////////////////////////////////////////////////

#include "SteadyDiffusionVaryingB.h"
#include <LibUtilities/TimeIntegration/TimeIntegrationScheme.h>
#include <LibUtilities/BasicUtils/SessionReader.h>
#include <iostream>
#include <iomanip>

#include <boost/core/ignore_unused.hpp>

using namespace std;

namespace Nektar
{

string SteadyDiffusionVaryingB::className = GetEquationSystemFactory().
    RegisterCreatorFunction("SteadyDiffusionVaryingB", SteadyDiffusionVaryingB::create);

SteadyDiffusionVaryingB::SteadyDiffusionVaryingB(
    const LibUtilities::SessionReaderSharedPtr& pSession,
    const SpatialDomains::MeshGraphSharedPtr& pGraph)
    : EquationSystem(pSession, pGraph)
{
}

/**
 * @brief Initialisation object for the steady diffusion problem.
 */
void SteadyDiffusionVaryingB::v_InitObject()
{
    EquationSystem::v_InitObject();

    int npoints = m_fields[0]->GetNpoints();
    Array<OneD,NekDouble>  xc0,xc1,xc2;

    Array<OneD, NekDouble> xc(npoints), yc(npoints);
    m_fields[0]->GetCoords(xc, yc);

    int nq, coordim;
    nq = m_fields[0]->GetNpoints();

    // Set up variable coefficients
    //GetFunction("d00")->Evaluate(m_session->GetVariables(), m_fields);
    //GetFunction("d01")->Evaluate(m_session->GetVariables(), m_fields);
    //GetFunction("d11")->Evaluate(m_session->GetVariables(), m_fields);

    //----------------------------------------------
    // Set up variable coefficients if defined

    coordim = m_fields[0]->GetCoordim(0);
    xc0 = Array<OneD,NekDouble>(nq,0.0);
    xc1 = Array<OneD,NekDouble>(nq,0.0);
    xc2 = Array<OneD,NekDouble>(nq,0.0);

    switch(coordim)
    {
    case 2:
        m_fields[0]->GetCoords(xc0,xc1);
        break;
    case 3:
        m_fields[0]->GetCoords(xc0,xc1,xc2);
        break;
    default:
        ASSERTL0(false,"Coordim not valid");
        break;
    }

    if (m_session->DefinesFunction("d00"))
    {
        Array<OneD, NekDouble> d00(nq,0.0);
        LibUtilities::EquationSharedPtr d00func = m_session->GetFunction("d00",0);
        d00func->Evaluate(xc0, xc1, xc2, d00);
        m_varcoeff[StdRegions::eVarCoeffD00] = d00;
    }
    if (m_session->DefinesFunction("d01"))
    {
        Array<OneD, NekDouble> d01(nq,0.0);
        LibUtilities::EquationSharedPtr d01func = m_session->GetFunction("d01",0);
        d01func->Evaluate(xc0, xc1, xc2, d01);
        m_varcoeff[StdRegions::eVarCoeffD01] = d01;
    }
    if (m_session->DefinesFunction("d11"))
    {
        Array<OneD, NekDouble> d11(nq,0.0);
        LibUtilities::EquationSharedPtr d11func = m_session->GetFunction("d11",0);
        d11func->Evaluate(xc0, xc1, xc2, d11);
        m_varcoeff[StdRegions::eVarCoeffD11] = d11;
    }
    //----------------------------------------------

    ASSERTL0(m_projectionType == MultiRegions::eGalerkin,
             "Only continuous Galerkin discretisation supported.");
}

/**
 * @brief Steady diffusion problem destructor.
 */
SteadyDiffusionVaryingB::~SteadyDiffusionVaryingB()
{
}

void SteadyDiffusionVaryingB::v_GenerateSummary(SummaryList& s)
{
    EquationSystem::v_GenerateSummary(s);
}

/**
 * @brief Implicit solution of the steady diffusion problem.
 */
void SteadyDiffusionVaryingB::v_DoSolve()
{
    StdRegions::ConstFactorMap factors;
    factors[StdRegions::eFactorLambda] = 0.0;

    for (int i = 0; i < m_fields.size(); ++i)
    {
        Vmath::Zero(m_fields[i]->GetNcoeffs(), m_fields[i]->UpdateCoeffs(), 1);
        Vmath::Zero(m_fields[i]->GetNpoints(), m_fields[i]->UpdatePhys(), 1);

        // Solve a system of equations with Helmholtz solver
        m_fields[i]->HelmSolve(m_fields[i]->GetPhys(),
                               m_fields[i]->UpdateCoeffs(),
                               factors,
                               m_varcoeff);
        m_fields[i]->BwdTrans(m_fields[i]->GetCoeffs(),
                              m_fields[i]->UpdatePhys());
    }
}
}
