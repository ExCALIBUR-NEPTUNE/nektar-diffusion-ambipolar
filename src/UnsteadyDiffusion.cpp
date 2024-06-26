///////////////////////////////////////////////////////////////////////////////
//
// File: UnsteadyDiffusion.cpp
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
// Description: Unsteady diffusion solve routines
//
///////////////////////////////////////////////////////////////////////////////

#include "UnsteadyDiffusion.h"
#include <LibUtilities/TimeIntegration/TimeIntegrationScheme.h>
#include <iomanip>
#include <iostream>
#include <tinyxml.h>

#include <boost/core/ignore_unused.hpp>

using namespace std;

namespace Nektar
{
string UnsteadyDiffusion::className =
    GetEquationSystemFactory().RegisterCreatorFunction(
        "UnsteadyDiffusion", UnsteadyDiffusion::create);

UnsteadyDiffusion::UnsteadyDiffusion(
    const LibUtilities::SessionReaderSharedPtr &pSession,
    const SpatialDomains::MeshGraphSharedPtr &pGraph)
    : UnsteadySystem(pSession, pGraph)
{
}

/**
 * @brief Initialisation object for the unsteady diffusion problem.
 */
void UnsteadyDiffusion::v_InitObject(bool DeclareField)
{
    UnsteadySystem::v_InitObject(DeclareField);

    m_session->MatchSolverInfo("SpectralVanishingViscosity", "True",
                               m_useSpecVanVisc, false);

    if (m_useSpecVanVisc)
    {
        m_session->LoadParameter("SVVCutoffRatio", m_sVVCutoffRatio, 0.75);
        m_session->LoadParameter("SVVDiffCoeff", m_sVVDiffCoeff, 0.1);
    }

    int npoints = m_fields[0]->GetNpoints();

    m_session->LoadParameter("k_par", m_kpar, 100.0);
    m_session->LoadParameter("k_perp", m_kperp, 1.0);
    m_session->LoadParameter("theta", m_theta, 0.0);
    m_session->LoadParameter("n", m_n, 1e18);
    m_session->LoadParameter("epsilon", m_epsilon, 1.0);

    // Convert to radians.
    m_theta *= -M_PI / 180.0;

    Array<OneD, NekDouble> xc(npoints), yc(npoints);
    m_fields[0]->GetCoords(xc, yc);

    if (m_useSpecVanVisc)
    {
        m_factors[StdRegions::eFactorSVVCutoffRatio] = m_sVVCutoffRatio;
        m_factors[StdRegions::eFactorSVVDiffCoeff] = m_sVVDiffCoeff / m_epsilon;
    }

    NekDouble ct = cos(m_theta), st = sin(m_theta);
    NekDouble d00 =
        (2.0 / (3.0 * m_n)) * ((m_kpar - m_kperp) * ct * ct + m_kperp);
    NekDouble d01 = (2.0 / (3.0 * m_n)) * ((m_kpar - m_kperp) * ct * st);
    NekDouble d11 =
        (2.0 / (3.0 * m_n)) * ((m_kpar - m_kperp) * st * st + m_kperp);

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

    // Override previous setup if a magnetic field is defined
    if (m_session->DefinesFunction("MagneticField"))
    {
        Array<OneD, Array<OneD, NekDouble>> dummy;
        dummy = Array<OneD, Array<OneD, NekDouble>>(3);

        Array<OneD, NekDouble> d00(npoints, 1.0);
        Array<OneD, NekDouble> d11(npoints, 1.0);
        Array<OneD, NekDouble> d01(npoints, 0.0);

        std::vector<std::string> B;
        B.push_back("Bx");
        B.push_back("By");
        B.push_back("Bz");
        B.resize(3);
        
        GetFunction("MagneticField")->Evaluate(B, dummy);
        for (int k = 0; k < npoints ; k++)
        {
            d00[k] = (m_kpar - m_kperp) * dummy[0][k]*dummy[0][k] + m_kperp;
            d01[k] = (m_kpar - m_kperp) * dummy[0][k]*dummy[1][k];
            d11[k] = (m_kpar - m_kperp) * dummy[1][k]*dummy[1][k] + m_kperp;
        }
        m_varcoeff[StdRegions::eVarCoeffD00] = d00;
        m_varcoeff[StdRegions::eVarCoeffD01] = d01;
        m_varcoeff[StdRegions::eVarCoeffD11] = d11;
    }

    m_source = Array<OneD, NekDouble>(npoints, 0.0);
    // Define source term if any
    if (m_session->DefinesFunction("Source"))
    {
        Array<OneD, NekDouble> zc(npoints, 0.0);
        LibUtilities::EquationSharedPtr source =
            m_session->GetFunction("Source", 0);
        source->Evaluate(xc, yc, zc, m_source);
    }

    ASSERTL0(m_projectionType == MultiRegions::eGalerkin,
             "Only continuous Galerkin discretisation supported.");

    if (m_session->MatchSolverInfo("TimeIntegrationMethod", "IMEXOrder3"))
    {
        m_ode.DefineOdeRhs(&UnsteadyDiffusion::DoOdeRhs, this);
        m_ode.DefineProjection(&UnsteadyDiffusion::DoOdeProjection, this);
    }
    m_ode.DefineImplicitSolve(&UnsteadyDiffusion::DoImplicitSolve, this);
}

/**
 * @brief Unsteady diffusion problem destructor.
 */
UnsteadyDiffusion::~UnsteadyDiffusion()
{
}

void UnsteadyDiffusion::v_GenerateSummary(SummaryList &s)
{
    UnsteadySystem::v_GenerateSummary(s);
    if (m_useSpecVanVisc)
    {
        stringstream ss;
        ss << "SVV (cut off = " << m_sVVCutoffRatio
           << ", coeff = " << m_sVVDiffCoeff << ")";
        AddSummaryItem(s, "Smoothing", ss.str());
    }
}

/**
 * @brief Compute the right-hand side for the unsteady diffusion
 * problem.
 *
 * @param inarray    Given fields.
 * @param outarray   Calculated solution.
 * @param time       Time.
 */
void UnsteadyDiffusion::DoOdeRhs(
    const Array<OneD, const Array<OneD, NekDouble>> &inarray,
    Array<OneD, Array<OneD, NekDouble>> &outarray, const NekDouble time)
{
    int nq = m_fields[0]->GetNpoints();

    // RHS should be set to zero.
    for (int i = 0; i < outarray.size(); ++i)
    {
        Vmath::Zero(nq, &outarray[i][0], 1);
    }

    // Add source term S to N.
    Vmath::Vadd(nq, m_source, 1, outarray[0], 1, outarray[0], 1);
}

/**
 * @brief Compute the projection for the unsteady diffusion problem.
 *
 * @param inarray    Given fields.
 * @param outarray   Calculated solution.
 * @param time       Time.
 */
void UnsteadyDiffusion::DoOdeProjection(
    const Array<OneD, const Array<OneD, NekDouble>> &inarray,
    Array<OneD, Array<OneD, NekDouble>> &outarray, const NekDouble time)
{
    int i;
    int nvariables = inarray.size();
    SetBoundaryConditions(time);

    Array<OneD, NekDouble> coeffs(m_fields[0]->GetNcoeffs());

    for (i = 0; i < nvariables; ++i)
    {
        m_fields[i]->FwdTrans(inarray[i], coeffs);
        m_fields[i]->BwdTrans(coeffs, outarray[i]);
    }
}

/**
 * @brief Implicit solution of the unsteady diffusion problem.
 */
void UnsteadyDiffusion::DoImplicitSolve(
    const Array<OneD, const Array<OneD, NekDouble>> &inarray,
    Array<OneD, Array<OneD, NekDouble>> &outarray, const NekDouble time,
    const NekDouble lambda)
{
    boost::ignore_unused(time);

    int nvariables                       = inarray.size();
    int npoints                          = m_fields[0]->GetNpoints();
    m_factors[StdRegions::eFactorLambda] = 1.0 / lambda / m_epsilon;

    if (m_useSpecVanVisc)
    {
        m_factors[StdRegions::eFactorSVVCutoffRatio] = m_sVVCutoffRatio;
        m_factors[StdRegions::eFactorSVVDiffCoeff]   = m_sVVDiffCoeff / m_epsilon;
    }

    // We solve ( \nabla^2 - HHlambda ) Y[i] = rhs [i]
    // inarray = input: \hat{rhs} -> output: \hat{Y}
    // outarray = output: nabla^2 \hat{Y}
    // where \hat = modal coeffs
    for (int i = 0; i < nvariables; ++i)
    {
        // Multiply 1.0/timestep/lambda
        Vmath::Smul(npoints, -m_factors[StdRegions::eFactorLambda], inarray[i],
                    1, outarray[i], 1);

        // Solve a system of equations with Helmholtz solver
        m_fields[i]->HelmSolve(outarray[i], m_fields[i]->UpdateCoeffs(),
                               m_factors, m_varcoeff);

        m_fields[i]->BwdTrans(m_fields[i]->GetCoeffs(), outarray[i]);

        m_fields[i]->SetPhysState(false);
    }
}
} // namespace Nektar
