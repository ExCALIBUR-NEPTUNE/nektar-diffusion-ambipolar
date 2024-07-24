#include "DiffBndCond.h"

using namespace std;

namespace Nektar
{
DiffBndCondFactory &GetDiffBndCondFactory()
{
    static DiffBndCondFactory instance;
    return instance;
}

DiffBndCond::DiffBndCond(
    const LibUtilities::SessionReaderSharedPtr &pSession,
    const Array<OneD, MultiRegions::ExpListSharedPtr> &pFields,
    const Array<OneD, Array<OneD, NekDouble>> &pTraceNormals,
    const Array<OneD, Array<OneD, NekDouble>> &pObliqueField,
    const int pSpaceDim, const int bcRegion, const int cnt)
    : m_session(pSession), m_fields(pFields), m_traceNormals(pTraceNormals),
      m_obliqueField(pObliqueField), m_spacedim(pSpaceDim),
      m_bcRegion(bcRegion), m_offset(cnt)
{
    m_velInf = Array<OneD, NekDouble>(m_spacedim, 0.0);

    if (m_spacedim >= 2)
    {
        m_session->LoadParameter("vInf", m_velInf[1], 0.0);
    }
    if (m_spacedim == 3)
    {
        m_session->LoadParameter("wInf", m_velInf[2], 0.0);
    }

    m_diffusionAveWeight = 1.0;
}

/**
 * @param   bcRegion      id of the boundary region
 * @param   cnt
 * @param   Fwd
 * @param   physarray
 * @param   time
 */
void DiffBndCond::Apply(Array<OneD, Array<OneD, NekDouble>> &Fwd,
                        Array<OneD, Array<OneD, NekDouble>> &FwdOblique,
                        Array<OneD, Array<OneD, NekDouble>> &physarray,
                        const NekDouble &time)
{
    v_Apply(Fwd, FwdOblique, physarray, time);
}

/**
 * @ brief Newly added bc should specify this virtual function
 * if the Bwd/value in m_bndCondExpansions is the target value like Direchlet
 * bc weight should be 1.0.
 * if some average Fwd and Bwd/value in m_bndCondExpansions
 * is the target value like WallViscousBC weight should be 0.5.
 */
void DiffBndCond::v_ApplyBwdWeight()
{
    size_t nVariables = m_fields.size();
    for (int i = 0; i < nVariables; ++i)
    {
        m_fields[i]->SetBndCondBwdWeight(m_bcRegion, m_diffusionAveWeight);
    }
}

} // namespace Nektar
