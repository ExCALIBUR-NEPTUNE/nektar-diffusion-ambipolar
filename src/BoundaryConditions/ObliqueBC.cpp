#include "ObliqueBC.h"

using namespace std;

namespace Nektar
{

std::string ObliqueBC::className =
    GetDiffBndCondFactory().RegisterCreatorFunction(
        "Oblique", ObliqueBC::create, "Oblique boundary condition.");

ObliqueBC::ObliqueBC(const LibUtilities::SessionReaderSharedPtr &pSession,
                     const Array<OneD, MultiRegions::ExpListSharedPtr> &pFields,
                     const Array<OneD, Array<OneD, NekDouble>> &pTraceNormals,
                     const Array<OneD, Array<OneD, NekDouble>> &pObliqueField,
                     const int pSpaceDim, const int bcRegion, const int cnt)
    : DiffBndCond(pSession, pFields, pTraceNormals, pObliqueField, pSpaceDim,
                  bcRegion, cnt)
{
}

void ObliqueBC::v_Apply(Array<OneD, Array<OneD, NekDouble>> &Fwd,
                        Array<OneD, Array<OneD, NekDouble>> &FwdOblique,
                        Array<OneD, Array<OneD, NekDouble>> &physarray,
                        [[maybe_unused]] const NekDouble &time)
{
    int i;
    int nVariables = physarray.size();

    const Array<OneD, const int> &traceBndMap = m_fields[0]->GetTraceBndMap();

    // Adjust the physical values of the trace to take
    // user defined boundaries into account
    int e, id1, id2, nBCEdgePts, eMax;

    eMax = m_fields[0]->GetBndCondExpansions()[m_bcRegion]->GetExpSize();

    for (e = 0; e < eMax; ++e)
    {
        nBCEdgePts = m_fields[0]
                         ->GetBndCondExpansions()[m_bcRegion]
                         ->GetExp(e)
                         ->GetTotPoints();
        id1 =
            m_fields[0]->GetBndCondExpansions()[m_bcRegion]->GetPhys_Offset(e);
        id2 =
            m_fields[0]->GetTrace()->GetPhys_Offset(traceBndMap[m_offset + e]);

        // Calculate -(B.n) in tmp
        Array<OneD, NekDouble> tmp(nBCEdgePts, 0.0);

        for (i = 0; i < m_spacedim; ++i)
        {
            Vmath::Vvtvp(nBCEdgePts, &FwdOblique[i][id2], 1,
                         &m_traceNormals[i][id2], 1, &tmp[0], 1, &tmp[0], 1);
        }
        Vmath::Smul(nBCEdgePts, -1.0, &tmp[0], 1, &tmp[0], 1);

        // Calculate B_par = B - (B.n)n
        Array<OneD, NekDouble> B_par(m_spacedim * nBCEdgePts, 0.0);
        for (i = 0; i < m_spacedim; ++i)
        {
            Vmath::Vvtvp(nBCEdgePts, &tmp[0], 1, &m_traceNormals[i][id2], 1,
                         &FwdOblique[i][id2], 1, &B_par[i * nBCEdgePts +
                         id2], 1);
        }

        // Calculate grad(Fwd).B_par
        for (i = 0; i < nVariables; ++i)
        {
            Array<OneD, NekDouble> FwdDeriv(nBCEdgePts);
            m_fields[0]
                ->GetBndCondExpansions()[m_bcRegion]
                ->GetExp(e)
                ->PhysDirectionalDeriv(B_par, Fwd[i], FwdDeriv);

            // Copy boundary adjusted values into the boundary expansion

            Vmath::Vcopy(nBCEdgePts, &FwdDeriv[id2], 1,
                         &(m_fields[i]
                               ->GetBndCondExpansions()[m_bcRegion]
                               ->UpdatePhys())[id1],
                         1);
        }
    }
}

} // namespace Nektar
