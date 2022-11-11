///////////////////////////////////////////////////////////////////////////////
//
// File: SteadyDiffusion.cpp
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
// Description: Steady diffusion solve routines
//
///////////////////////////////////////////////////////////////////////////////

#include "SteadyDiffusion.h"
#include <LibUtilities/TimeIntegration/TimeIntegrationScheme.h>
#include <iomanip>
#include <iostream>
#include <tinyxml.h>

#include <boost/core/ignore_unused.hpp>

using namespace std;

namespace Nektar
{
string SteadyDiffusion::className =
    GetEquationSystemFactory().RegisterCreatorFunction("SteadyDiffusion",
                                                       SteadyDiffusion::create);

SteadyDiffusion::SteadyDiffusion(
    const LibUtilities::SessionReaderSharedPtr &pSession,
    const SpatialDomains::MeshGraphSharedPtr &pGraph)
    : EquationSystem(pSession, pGraph)
{
}

/**
 * @brief Initialisation object for the steady diffusion problem.
 */
void SteadyDiffusion::v_InitObject(bool DeclareField)
{
    EquationSystem::v_InitObject(DeclareField);

    m_session->LoadParameter("k_par", m_kpar, 100.0);
    m_session->LoadParameter("k_perp", m_kperp, 1.0);
    m_session->LoadParameter("theta", m_theta, 5.0);

    // Convert to radians.
    m_theta *= -M_PI / 180.0;

    m_factors[StdRegions::eFactorLambda] = 0.0;

    NekDouble ct = cos(m_theta), st = sin(m_theta);
    NekDouble d00 = (m_kpar - m_kperp) * ct * ct + m_kperp;
    NekDouble d01 = (m_kpar - m_kperp) * ct * st;
    NekDouble d11 = (m_kpar - m_kperp) * st * st + m_kperp;

    TiXmlDocument &doc = m_session->GetDocument();
    TiXmlHandle docHandle(&doc);
    TiXmlElement *master = docHandle.FirstChildElement("NEKTAR").Element();
    TiXmlElement *xmlCol = master->FirstChildElement("COLLECTIONS");

    // Check if user has specified some options
    if (xmlCol)
    {
        const char *defaultImpl    = xmlCol->Attribute("DEFAULT");
        const std::string collinfo = defaultImpl ? string(defaultImpl) : "";
        if (collinfo != "MatrixFree")
        {
            int nq = m_fields[0]->GetNpoints();
            // Set up variable coefficients
            m_varcoeff[StdRegions::eVarCoeffD00] =
                Array<OneD, NekDouble>(nq, d00);
            m_varcoeff[StdRegions::eVarCoeffD01] =
                Array<OneD, NekDouble>(nq, d01);
            m_varcoeff[StdRegions::eVarCoeffD11] =
                Array<OneD, NekDouble>(nq, d11);
        }
        else
        {
            // Set up constant coefficients
            m_factors[StdRegions::eFactorCoeffD00] = d00;
            m_factors[StdRegions::eFactorCoeffD01] = d01;
            m_factors[StdRegions::eFactorCoeffD11] = d11;
        }
    }
    else
    {
        int nq = m_fields[0]->GetNpoints();
        // Set up variable coefficients
        m_varcoeff[StdRegions::eVarCoeffD00] = Array<OneD, NekDouble>(nq, d00);
        m_varcoeff[StdRegions::eVarCoeffD01] = Array<OneD, NekDouble>(nq, d01);
        m_varcoeff[StdRegions::eVarCoeffD11] = Array<OneD, NekDouble>(nq, d11);
    }

    ASSERTL0(m_projectionType == MultiRegions::eGalerkin,
             "Only continuous Galerkin discretisation supported.");
}

/**
 * @brief Steady diffusion problem destructor.
 */
SteadyDiffusion::~SteadyDiffusion()
{
}

void SteadyDiffusion::v_GenerateSummary(SummaryList &s)
{
    EquationSystem::v_GenerateSummary(s);
}

/**
 * @brief Implicit solution of the steady diffusion problem.
 */
void SteadyDiffusion::v_DoSolve()
{
    for (int i = 0; i < m_fields.size(); ++i)
    {
        Vmath::Zero(m_fields[i]->GetNcoeffs(), m_fields[i]->UpdateCoeffs(), 1);
        Vmath::Zero(m_fields[i]->GetNpoints(), m_fields[i]->UpdatePhys(), 1);

        // Solve a system of equations with Helmholtz solver
        m_fields[i]->HelmSolve(m_fields[i]->GetPhys(),
                               m_fields[i]->UpdateCoeffs(), m_factors,
                               m_varcoeff);
        m_fields[i]->BwdTrans(m_fields[i]->GetCoeffs(),
                              m_fields[i]->UpdatePhys());
    }
}
} // namespace Nektar
