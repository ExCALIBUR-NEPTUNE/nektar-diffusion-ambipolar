#ifndef NEKTAR_SOLVERS_DIFFUSIONSOLVER_BNDCOND_DIFFBNDCOND
#define NEKTAR_SOLVERS_DIFFUSIONSOLVER_BNDCOND_DIFFBNDCOND

#include <string>

#include <LibUtilities/BasicUtils/NekFactory.hpp>
#include <LibUtilities/BasicUtils/SharedArray.hpp>
#include <MultiRegions/ExpList.h>

namespace Nektar
{
class DiffBndCond;

/// A shared pointer to a boundary condition object
typedef std::shared_ptr<DiffBndCond> DiffBndCondSharedPtr;

/// Declaration of the boundary condition factory
typedef LibUtilities::NekFactory<
    std::string, DiffBndCond, const LibUtilities::SessionReaderSharedPtr &,
    const Array<OneD, MultiRegions::ExpListSharedPtr> &,
    const Array<OneD, Array<OneD, NekDouble>> &,
    const Array<OneD, Array<OneD, NekDouble>> &, const int, const int,
    const int>
    DiffBndCondFactory;

/// Declaration of the boundary condition factory singleton
DiffBndCondFactory &GetDiffBndCondFactory();

/**
 * @class DiffBndCond
 * @brief Encapsulates the user-defined boundary conditions for the
 *        diffusion solver.
 */
class DiffBndCond
{
public:
    virtual ~DiffBndCond()
    {
    }

    /// Apply the boundary condition
    void Apply(Array<OneD, Array<OneD, NekDouble>> &Fwd,
               Array<OneD, Array<OneD, NekDouble>> &FwdOblique,
               Array<OneD, Array<OneD, NekDouble>> &physarray,
               const NekDouble &time = 0);

    /// Apply the Weight of boundary condition
    void ApplyBwdWeight()
    {
        v_ApplyBwdWeight();
    }

protected:
    /// Session reader
    LibUtilities::SessionReaderSharedPtr m_session;
    /// Array of fields
    Array<OneD, MultiRegions::ExpListSharedPtr> m_fields;
    /// Trace normals
    Array<OneD, Array<OneD, NekDouble>> m_traceNormals;
    /// Oblique Field
    Array<OneD, Array<OneD, NekDouble>> m_obliqueField;
    /// Space dimension
    int m_spacedim;
    /// Weight for average calculation of diffusion term
    NekDouble m_diffusionAveWeight;

    Array<OneD, NekDouble> m_velInf;

    /// Id of the boundary region
    int m_bcRegion;
    /// Offset
    int m_offset;

    /// Constructor
    DiffBndCond(const LibUtilities::SessionReaderSharedPtr &pSession,
                const Array<OneD, MultiRegions::ExpListSharedPtr> &pFields,
                const Array<OneD, Array<OneD, NekDouble>> &pTraceNormals,
                const Array<OneD, Array<OneD, NekDouble>> &pObliqueField,
                const int pSpaceDim, const int bcRegion, const int cnt);

    virtual void v_Apply(Array<OneD, Array<OneD, NekDouble>> &Fwd,
                         Array<OneD, Array<OneD, NekDouble>> &FwdOblique,
                         Array<OneD, Array<OneD, NekDouble>> &physarray,
                         const NekDouble &time) = 0;

    virtual void v_ApplyBwdWeight();
};
} // namespace Nektar
#endif